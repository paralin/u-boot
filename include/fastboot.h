/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2008 - 2009
 * Windriver, <www.windriver.com>
 * Tom Rix <Tom.Rix@windriver.com>
 *
 * Copyright 2011 Sebastian Andrzej Siewior <bigeasy@linutronix.de>
 *
 * Copyright 2014 Linaro, Ltd.
 * Rob Herring <robh@kernel.org>
 */
#ifndef _FASTBOOT_H_
#define _FASTBOOT_H_

#define FASTBOOT_VERSION	"0.4"

/* The 64 defined bytes plus \0 */
#define FASTBOOT_COMMAND_LEN	(64 + 1)
#define FASTBOOT_RESPONSE_LEN	(64 + 1)

/**
 * All known commands to fastboot
 */
enum {
	FASTBOOT_COMMAND_GETVAR = 0,
	FASTBOOT_COMMAND_DOWNLOAD,
#if CONFIG_IS_ENABLED(FASTBOOT_FLASH)
	FASTBOOT_COMMAND_FLASH,
	FASTBOOT_COMMAND_ERASE,
#endif
	FASTBOOT_COMMAND_BOOT,
	FASTBOOT_COMMAND_CONTINUE,
	FASTBOOT_COMMAND_REBOOT,
	FASTBOOT_COMMAND_REBOOT_BOOTLOADER,
	FASTBOOT_COMMAND_SET_ACTIVE,
#if CONFIG_IS_ENABLED(FASTBOOT_CMD_OEM_FORMAT)
	FASTBOOT_COMMAND_OEM_FORMAT,
#endif

	FASTBOOT_COMMAND_COUNT
};

/**
 * fastboot_response() - Writes a response of the form "$tag$reason".
 *
 * @tag: The first part of the response
 * @response: Pointer to fastboot response buffer
 * @format: printf style format string
 */
void fastboot_response(const char *tag, char *response,
		       const char *format, ...)
	__attribute__ ((format (__printf__, 3, 4)));

/**
 * fastboot_fail() - Write a FAIL response of the form "FAIL$reason".
 *
 * @reason: Pointer to returned reason string
 * @response: Pointer to fastboot response buffer
 */
void fastboot_fail(const char *reason, char *response);

/**
 * fastboot_okay() - Write an OKAY response of the form "OKAY$reason".
 *
 * @reason: Pointer to returned reason string, or NULL to send a bare "OKAY"
 * @response: Pointer to fastboot response buffer
 */
void fastboot_okay(const char *reason, char *response);

/**
 * fastboot_set_reboot_flag() - Set flag to indicate reboot-bootloader
 *
 * Set flag which indicates that we should reboot into the bootloader
 * following the reboot that fastboot executes after this function.
 *
 * This function should be overridden in your board file with one
 * which sets whatever flag your board specific Android bootloader flow
 * requires in order to re-enter the bootloader.
 */
int fastboot_set_reboot_flag(void);

/**
 * fastboot_set_progress_callback() - set progress callback
 *
 * @progress: Pointer to progress callback
 *
 * Set a callback which is invoked periodically during long running operations
 * (flash and erase). This can be used (for example) by the UDP transport to
 * send INFO responses to keep the client alive whilst those commands are
 * executing.
 */
void fastboot_set_progress_callback(void (*progress)(const char *msg));

/*
 * fastboot_init() - initialise new fastboot protocol session
 *
 * @buf_addr: Pointer to download buffer, or NULL for default
 * @buf_size: Size of download buffer, or zero for default
 */
void fastboot_init(void *buf_addr, u32 buf_size);

/**
 * fastboot_boot() - Execute fastboot boot command
 *
 * If ${fastboot_bootcmd} is set, run that command to execute the boot
 * process, if that returns, then exit the fastboot server and return
 * control to the caller.
 *
 * Otherwise execute "bootm <fastboot_buf_addr>", if that fails, reset
 * the board.
 */
void fastboot_boot(void);

/**
 * fastboot_handle_command() - Handle fastboot command
 *
 * @cmd_string: Pointer to command string
 * @response: Pointer to fastboot response buffer
 *
 * Return: Executed command, or -1 if not recognized
 */
int fastboot_handle_command(char *cmd_string, char *response);

/**
 * fastboot_data_remaining() - return bytes remaining in current transfer
 *
 * Return: Number of bytes left in the current download
 */
u32 fastboot_data_remaining(void);

/**
 * fastboot_data_download() - Copy image data to fastboot_buf_addr.
 *
 * @fastboot_data: Pointer to received fastboot data
 * @fastboot_data_len: Length of received fastboot data
 * @response: Pointer to fastboot response buffer
 *
 * Copies image data from fastboot_data to fastboot_buf_addr. Writes to
 * response. fastboot_bytes_received is updated to indicate the number
 * of bytes that have been transferred.
 */
void fastboot_data_download(const void *fastboot_data,
			    unsigned int fastboot_data_len, char *response);

/**
 * fastboot_data_complete() - Mark current transfer complete
 *
 * @response: Pointer to fastboot response buffer
 *
 * Set image_size and ${filesize} to the total size of the downloaded image.
 */
void fastboot_data_complete(char *response);

#if defined(CONFIG_TARGET_ODROID_XU4) || defined(CONFIG_TARGET_ODROID_XU3)

typedef struct _ext4_file_header {
	unsigned int magic;
	unsigned short major;
	unsigned short minor;
	unsigned short file_header_size;
	unsigned short chunk_header_size;
	unsigned int block_size;
	unsigned int total_blocks;
	unsigned int total_chunks;
	unsigned int crc32;
}	ext4_file_header;

typedef struct _ext4_chunk_header {
	unsigned short type;
	unsigned short reserved;
	unsigned int chunk_size;
	unsigned int total_size;
}	ext4_chunk_header;

#define EXT4_FILE_HEADER_MAGIC	0xED26FF3A
#define EXT4_FILE_HEADER_MAJOR	0x0001
#define EXT4_FILE_HEADER_MINOR	0x0000
#define EXT4_FILE_BLOCK_SIZE	0x1000

#define EXT4_FILE_HEADER_SIZE	(sizeof(ext4_file_header))
#define EXT4_CHUNK_HEADER_SIZE	(sizeof(ext4_chunk_header))

#define EXT4_CHUNK_TYPE_RAW	0xCAC1
#define EXT4_CHUNK_TYPE_FILL	0xCAC2
#define EXT4_CHUNK_TYPE_NONE	0xCAC3

/* This is the interface file between the common cmd_fastboot.c and
   the board specific support.

   To use this interface, define CONFIG_FASTBOOT in your board config file.
   An example is include/configs/omap3430labrador.h
   ...
   #define CONFIG_FASTBOOT	        1    / * Using fastboot interface * /
   ...

   An example of the board specific spupport for omap3 is found at
   cpu/omap3/fastboot.c

*/

/* From fastboot client.. */
#define FASTBOOT_INTERFACE_CLASS     0xff
#define FASTBOOT_INTERFACE_SUB_CLASS 0x42
#define FASTBOOT_INTERFACE_PROTOCOL  0x03

#define FASTBOOT_VERSION "0.4"

/* The fastboot client uses a value of 2048 for the
   page size of it boot.img file format.
   Reset this in your board config file as needed. */
#ifndef CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE
#define CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE 2048
#endif

struct cmd_fastboot_interface
{
	/* This function is called when a buffer has been
	   recieved from the client app.
	   The buffer is a supplied by the board layer and must be unmodified.
	   The buffer_size is how much data is passed in.
	   Returns 0 on success
	   Returns 1 on failure

	   Set by cmd_fastboot	*/
	int (*rx_handler)(const unsigned char *buffer,
			  unsigned int buffer_size);

	/* This function is called when an exception has
	   occurred in the device code and the state
	   off fastboot needs to be reset

	   Set by cmd_fastboot */
	void (*reset_handler)(void);

	/* A getvar string for the product name
	   It can have a maximum of 60 characters

	   Set by board	*/
	char *product_name;

	/* A getvar string for the serial number
	   It can have a maximum of 60 characters

	   Set by board */
	char *serial_no;

	/* Nand block size
	   Supports the write option WRITE_NEXT_GOOD_BLOCK

	   Set by board */
	unsigned int nand_block_size;

	/* Transfer buffer, for handling flash updates
	   Should be multiple of the nand_block_size
	   Care should be take so it does not overrun bootloader memory
	   Controlled by the configure variable CFG_FASTBOOT_TRANSFER_BUFFER

	   Set by board */
	unsigned char *transfer_buffer;

	/* How big is the transfer buffer
	   Controlled by the configure variable
	   CFG_FASTBOOT_TRANSFER_BUFFER_SIZE

	   Set by board	*/
	unsigned int transfer_buffer_size;
};

/* Status values */
#define FASTBOOT_OK			0
#define FASTBOOT_ERROR			-1
#define FASTBOOT_DISCONNECT		1
#define FASTBOOT_INACTIVE		2

/* Magic string to enable fastboot during preboot */
#define FASTBOOT_REBOOT_MAGIC		"REBOOT-FASTBOOT"
#define FASTBOOT_REBOOT_MAGIC_SIZE	15

/* Android bootimage file format */
#define FASTBOOT_BOOT_MAGIC "ANDROID!"
#define FASTBOOT_BOOT_MAGIC_SIZE	8
#define FASTBOOT_BOOT_NAME_SIZE		16
#define FASTBOOT_BOOT_ARGS_SIZE		512

/* Input of fastboot_tx_status */
#define FASTBOOT_TX_ASYNC		0
#define FASTBOOT_TX_SYNC		1

#if defined(CONFIG_FASTBOOT)
/* A board specific test if u-boot should go into the fastboot command
   ahead of the bootcmd
   Returns 0 to continue with normal u-boot flow
   Returns 1 to execute fastboot */
extern int fastboot_preboot(void);

/* Initizes the board specific fastboot
   Returns 0 on success
   Returns 1 on failure */
extern int fastboot_init(struct cmd_fastboot_interface *interface);

/* Cleans up the board specific fastboot */
extern void fastboot_shutdown(void);

/*
 * Handles board specific usb protocol exchanges
 * Returns 0 on success
 * Returns 1 on disconnects, break out of loop
 * Returns 2 if no USB activity detected
 * Returns -1 on failure, unhandled usb requests and other error conditions
*/
extern int fastboot_poll(void);

/* Is this high speed (2.0) or full speed (1.1) ?
   Returns 0 on full speed
   Returns 1 on high speed */
extern int fastboot_is_highspeed(void);

/* Return the size of the fifo */
extern int fastboot_fifo_size(void);

/* Send a status reply to the client app
   buffer does not have to be null terminated.
   buffer_size must be not be larger than what is returned by
   fastboot_fifo_size
   Returns 0 on success
   Returns 1 on failure */
extern int fastboot_tx_status(const char *buffer, unsigned int buffer_size, const u32 need_sync_flag);

/* A board specific variable handler.
   The size of the buffers is governed by the fastboot spec.
   rx_buffer is at most 57 bytes
   tx_buffer is at most 60 bytes
   Returns 0 on success
   Returns 1 on failure */
extern int fastboot_getvar(const char *rx_buffer, char *tx_buffer);

#else

/* Stubs for when CONFIG_FASTBOOT is not defined */
#define fastboot_preboot() 0
#define fastboot_init(a) 1
#define fastboot_shutdown()
#define fastboot_poll() 1
#define fastboot_is_highspeed() 0
#define fastboot_fifo_size() 0
#define fastboot_tx_status(a, b, c) 1
#define fastboot_getvar(a,b) 1

#endif /* CONFIG_FASTBOOT */

#endif /* #if defined(CONFIG_TARGET_ODROID_XU4) || defined(CONFIG_TARGET_ODROID_XU3) */

#endif /* _FASTBOOT_H_ */
