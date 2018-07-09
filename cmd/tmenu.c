// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018 Ondrej Jirman <megous@megous.com>
 */
#include <common.h>
#include <command.h>
#include <cli_hush.h>
#include <video.h>
#include <video_font.h>
#include <dm/uclass.h>
#include <dm/device.h>
#include <touchpanel.h>

// first some generic drawing primitives

struct painter {
	u8* fb;
	u8* fb_end;
	u8* cur;
	u32 line_length;
	u32 bpp;
	u32 rows;
	u32 cols;
};

static void painter_set_xy(struct painter* p, uint x, uint y)
{
	p->cur = p->fb + min(y, p->rows - 1) * p->line_length + min(x, p->cols - 1) * p->bpp;
}

static void painter_move_dxy(struct painter* p, int dx, int dy)
{
	p->cur += dy * p->line_length + dx * p->bpp;

	if (p->cur >= p->fb_end)
		p->cur = p->fb_end - 1;

	if (p->cur < p->fb)
		p->cur = p->fb;
}

static void painter_rect_fill(struct painter* p, uint w, uint h, u32 color)
{
	int x, y;
	u32* cur;

	for (y = 0; y < h; y++) {
		cur = (u32*)(p->cur + p->line_length * y);

		for (x = 0; x < w; x++)
			*(cur++) = color;
	}
}

static void painter_line_h(struct painter* p, int dx, u32 color)
{
	if (dx < 0) {
		painter_move_dxy(p, 0, dx);
		painter_rect_fill(p, 1, -dx, color);
	} else {
		painter_rect_fill(p, 1, dx, color);
		painter_move_dxy(p, 0, dx);
	}
}

static void painter_line_v(struct painter* p, int dy, u32 color)
{
	if (dy < 0) {
		painter_move_dxy(p, 0, dy);
		painter_rect_fill(p, 1, -dy, color);
	} else {
		painter_rect_fill(p, 1, dy, color);
		painter_move_dxy(p, 0, dy);
	}
}

static void painter_bigchar(struct painter* p, char ch, u32 color)
{
	int i, row;
	void *line = p->cur;

	for (row = 0; row < VIDEO_FONT_HEIGHT * 2; row++) {
		uchar bits = video_fontdata[ch * VIDEO_FONT_HEIGHT + row / 2];
		uint32_t *dst = line;

		for (i = 0; i < VIDEO_FONT_WIDTH; i++) {
			if (bits & 0x80) {
				*dst = color;
				*(dst+1) = color;
			}

			bits <<= 1;
			dst+=2;
		}

		line += p->line_length;
	}

	painter_move_dxy(p, VIDEO_FONT_WIDTH * 2, 0);
}

static void painter_char(struct painter* p, char ch, u32 color)
{
	int i, row;
	void *line = p->cur;

	for (row = 0; row < VIDEO_FONT_HEIGHT; row++) {
		uchar bits = video_fontdata[ch * VIDEO_FONT_HEIGHT + row];
		uint32_t *dst = line;

		for (i = 0; i < VIDEO_FONT_WIDTH; i++) {
			if (bits & 0x80)
				*dst = color;

			bits <<= 1;
			dst++;
		}

		line += p->line_length;
	}

	painter_move_dxy(p, VIDEO_FONT_WIDTH, 0);
}

// menu command

static int do_tmenu(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	struct udevice *vdev, *tdev;
	struct video_priv *vpriv;
	struct touchpanel_touch touches[10];
	int ret, i, row, j;

	if (argc < 2) {
		return CMD_RET_USAGE;
	}

        // set some params: (parse from argv in the future)
	char* const* items = argv + 1;
	int items_count = argc - 1;
	int w = 350, h = 120, x = 30, y = (600 - (h + 10) * items_count - 10) / 2;

	ret = uclass_first_device_err(UCLASS_VIDEO, &vdev);
	if (ret)
		return CMD_RET_FAILURE;

	ret = uclass_first_device_err(UCLASS_TOUCHPANEL, &tdev);
	if (ret)
		return CMD_RET_FAILURE;

	vpriv = dev_get_uclass_priv(vdev);

	if (vpriv->bpix != VIDEO_BPP32) {
		printf("tmenu requires 32BPP video device\n");
		return CMD_RET_FAILURE;
	}

        struct painter p = {
		.fb = vpriv->fb,
		.fb_end = vpriv->fb + vpriv->fb_size,
		.cur = vpriv->fb,
		.line_length = vpriv->line_length,
		.bpp = VNBYTES(vpriv->bpix),
		.cols = vpriv->xsize,
		.rows = vpriv->ysize,
	};

	int selected = -1;
	int redraw = 1;

	ret = touchpanel_start(tdev);
	if (ret < 0) {
		printf("Failed to start %s, err=%d\n", tdev->name, ret);
		return CMD_RET_FAILURE;
	}

next:
	while (1) {
		if (redraw) {
			// redraw output
			row = y;
			for (i = 0; i < items_count; i++) {
				painter_set_xy(&p, x, row);
				painter_rect_fill(&p, w, h, i == selected ? 0xff555500 : 0xff005500);
				painter_set_xy(&p,
					       x + (w - strlen(items[i]) * VIDEO_FONT_WIDTH * 2) / 2,
					       row + (h - VIDEO_FONT_HEIGHT * 2) / 2);

				for (j = 0; items[i][j]; j++)
					painter_bigchar(&p, items[i][j], 0xffffffff);

				row += h + 10;
			}

			video_sync(vdev, true);
			redraw = 0;
		}

		// don't be too busy reading i2c
		udelay(100 * 1000);

		// handle input
		ret = touchpanel_get_touches(tdev, touches, ARRAY_SIZE(touches));
		if (ret < 0) {
			printf("Failed to get touches from %s, err=%d\n", tdev->name, ret);
			return CMD_RET_FAILURE;
		}

		for (i = 0; i < ret; i++) {
			int tx = touches[i].x;
			int ty = touches[i].y;

			if (tx < x || tx > x + w)
				continue;

			row = y;
			for (j = 0; j < items_count; j++) {
				if (ty > row && ty < row + h) {
					// got match
					if (selected != j) {
						selected = j;
						redraw = 1;
					}
					goto next;
				}

				row += h + 10;
			}
		}

		if (selected != -1) {
			// we are done
			char buf[16];
			snprintf(buf, sizeof buf, "ret=%d", selected);
			set_local_var(buf, 1);
			selected = -1;
			redraw = 1;
			break;
		}
	}

	ret = touchpanel_stop(tdev);
	if (ret < 0) {
		printf("Failed to stop %s, err=%d\n", tdev->name, ret);
		return CMD_RET_FAILURE;
	}

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(tmenu, 8, 1, do_tmenu, "tmenu", "tmenu item1 [item2...] - show touch menu");
