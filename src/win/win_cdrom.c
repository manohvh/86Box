/*
 * 86Box	A hypervisor and IBM PC system emulator that specializes in
 *		running old operating systems and software designed for IBM
 *		PC systems and compatibles from 1981 through fairly recent
 *		system designs based on the PCI bus.
 *
 *		This file is part of the 86Box distribution.
 *
 *		Handle the platform-side of CDROM drives.
 *
 * Version:	@(#)win_cdrom.c	1.0.10	2018/10/09
 *
 * Authors:	Sarah Walker, <http://pcem-emulator.co.uk/>
 *		Miran Grca, <mgrca8@gmail.com>
 *		Fred N. van Kempen, <decwiz@yahoo.com>
 *
 *		Copyright 2016-2018 Miran Grca.
 *		Copyright 2017,2018 Fred N. van Kempen.
 */
#define UNICODE
#define BITMAP WINDOWS_BITMAP
#include <windows.h>
#include <windowsx.h>
#undef BITMAP
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include "../config.h"
#include "../disk/hdd.h"
#include "../scsi/scsi_device.h"
#include "../cdrom/cdrom.h"
#include "../disk/zip.h"
#include "../cdrom/cdrom_image.h"
#include "../cdrom/cdrom_null.h"
#include "../scsi/scsi_disk.h"
#include "../plat.h"
#include "../ui.h"
#include "win.h"


void
cdrom_eject(uint8_t id)
{
    cdrom_drive_t *drv = &cdrom_drives[id];
    cdrom_image_t *img = &cdrom_image[id];

    if (drv->host_drive == 0) {
	/* Switch from empty to empty. Do nothing. */
	return;
    }

    if (img->prev_image_path) {
	free(img->prev_image_path);
	img->prev_image_path = NULL;
    }

    if (drv->host_drive == 200) {
	img->prev_image_path = (wchar_t *) malloc(1024);
	wcscpy(img->prev_image_path, img->image_path);
    }
    drv->prev_host_drive = drv->host_drive;
    drv->handler->exit(id);
    cdrom_close_handler(id);
    memset(img->image_path, 0, 2048);
    cdrom_null_open(id);
    if (drv->insert) {
	/* Signal disc change to the emulated machine. */
	drv->insert(drv->p);
    }

    ui_sb_check_menu_item(SB_CDROM|id, IDM_CDROM_IMAGE | id, MF_UNCHECKED);
    drv->host_drive=0;
    ui_sb_check_menu_item(SB_CDROM|id, IDM_CDROM_EMPTY | id, MF_CHECKED);
    ui_sb_update_icon_state(SB_CDROM|id, 1);
    ui_sb_enable_menu_item(SB_CDROM|id, IDM_CDROM_RELOAD | id, MF_BYCOMMAND | MF_ENABLED);
    ui_sb_update_tip(SB_CDROM|id);

    config_save();
}


void
cdrom_reload(uint8_t id)
{
    cdrom_drive_t *drv = &cdrom_drives[id];
    cdrom_image_t *img = &cdrom_image[id];

    if ((drv->host_drive == drv->prev_host_drive) || !drv->prev_host_drive || drv->host_drive) {
	/* Switch from empty to empty. Do nothing. */
	return;
    }

    cdrom_close_handler(id);
    memset(img->image_path, 0, 2048);

    if (drv->prev_host_drive == 200) {
	wcscpy(img->image_path, img->prev_image_path);
	free(img->prev_image_path);
	img->prev_image_path = NULL;
	image_open(id, img->image_path);
	if (drv->insert) {
		/* Signal disc change to the emulated machine. */
		drv->insert(drv->p);
	}
	if (wcslen(img->image_path) == 0) {
		ui_sb_check_menu_item(SB_CDROM|id, IDM_CDROM_EMPTY | id, MF_CHECKED);
		drv->host_drive = 0;
		ui_sb_check_menu_item(SB_CDROM|id, IDM_CDROM_IMAGE | id, MF_UNCHECKED);
		ui_sb_update_icon_state(SB_CDROM|id, 1);
	} else {
		ui_sb_check_menu_item(SB_CDROM|id, IDM_CDROM_EMPTY | id, MF_UNCHECKED);
		drv->host_drive = 200;
		ui_sb_check_menu_item(SB_CDROM|id, IDM_CDROM_IMAGE | id, MF_CHECKED);
		ui_sb_update_icon_state(SB_CDROM|id, 0);
	}
    }

    ui_sb_enable_menu_item(SB_CDROM|id, IDM_CDROM_RELOAD | id, MF_BYCOMMAND | MF_GRAYED);
    ui_sb_update_tip(SB_CDROM|id);

    config_save();
}


void
zip_eject(uint8_t id)
{
    zip_disk_close(zip[id]);
    if (zip_drives[id].bus_type) {
	/* Signal disk change to the emulated machine. */
	zip_insert(zip[id]);
    }

    ui_sb_update_icon_state(SB_ZIP | id, 1);
    ui_sb_enable_menu_item(SB_ZIP|id, IDM_ZIP_EJECT | id, MF_BYCOMMAND | MF_GRAYED);
    ui_sb_enable_menu_item(SB_ZIP|id, IDM_ZIP_RELOAD | id, MF_BYCOMMAND | MF_ENABLED);
    ui_sb_update_tip(SB_ZIP | id);
    config_save();
}


void
zip_reload(uint8_t id)
{
    zip_disk_reload(zip[id]);
    if (wcslen(zip_drives[id].image_path) == 0) {
	ui_sb_enable_menu_item(SB_ZIP|id, IDM_ZIP_EJECT | id, MF_BYCOMMAND | MF_GRAYED);
	ui_sb_update_icon_state(SB_ZIP|id, 1);
    } else {
	ui_sb_enable_menu_item(SB_ZIP|id, IDM_ZIP_EJECT | id, MF_BYCOMMAND | MF_ENABLED);
	ui_sb_update_icon_state(SB_ZIP|id, 0);
    }

    ui_sb_enable_menu_item(SB_ZIP|id, IDM_ZIP_RELOAD | id, MF_BYCOMMAND | MF_GRAYED);
    ui_sb_update_tip(SB_ZIP|id);

    config_save();
}
