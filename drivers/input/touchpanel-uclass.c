// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2018 Ondrej Jirman <megous@megous.com>
 */

#include <common.h>
#include <errno.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <touchpanel.h>

int touchpanel_start(struct udevice *dev)
{
	const struct touchpanel_ops *ops = touchpanel_get_ops(dev);

	if (!ops || !ops->start)
		return -ENOSYS;

	return ops->start(dev);
}

int touchpanel_stop(struct udevice *dev)
{
	const struct touchpanel_ops *ops = touchpanel_get_ops(dev);

	if (!ops || !ops->stop)
		return -ENOSYS;

	return ops->stop(dev);
}

int touchpanel_get_touches(struct udevice *dev,
			   struct touchpanel_touch* touches, int max_touches)
{
	const struct touchpanel_ops *ops = touchpanel_get_ops(dev);

	if (!ops || !ops->get_touches)
		return -ENOSYS;

	return ops->get_touches(dev, touches, max_touches);
}

static int touchpanel_pre_probe(struct udevice *dev)
{
	struct touchpanel_priv *uc_priv;

	uc_priv = dev_get_uclass_priv(dev);
	if (!uc_priv)
		return -ENXIO;

	uc_priv->size_x = dev_read_u32_default(dev, "touchscreen-size-x",
						-ENODATA);
	uc_priv->size_y = dev_read_u32_default(dev, "touchscreen-size-y",
						-ENODATA);

	if (uc_priv->size_x == -ENODATA || uc_priv->size_y == -ENODATA)
		uc_priv->size_x = uc_priv->size_y = -ENODATA;

	return 0;
}

UCLASS_DRIVER(touchpanel) = {
	.id		= UCLASS_TOUCHPANEL,
	.name		= "touchpanel",
	.pre_probe	= touchpanel_pre_probe,
	.per_device_auto_alloc_size = sizeof(struct touchpanel_priv),
};
