/*
 * 86Box	A hypervisor and IBM PC system emulator that specializes in
 *		running old operating systems and software designed for IBM
 *		PC systems and compatibles from 1981 through fairly recent
 *		system designs based on the PCI bus.
 *
 *		This file is part of the 86Box distribution.
 *
 *		Implementation of the CD-ROM drive with SCSI(-like)
 *		commands, for both ATAPI and SCSI usage.
 *
 * Version:	@(#)scsi_cdrom.h	1.0.0	2018/10/09
 *
 * Author:	Miran Grca, <mgrca8@gmail.com>
 *
 *		Copyright 2018 Miran Grca.
 */
#ifndef EMU_SCSI_CDROM_H
#define EMU_SCSI_CDROM_H


#define CDROM_TIME (5LL * 100LL * (1LL << TIMER_SHIFT))


#ifdef SCSI_DEVICE_H
typedef struct {
    /* Common block. */
    mode_sense_pages_t ms_pages_saved;

    cdrom_drive_t *drv;

    uint8_t *buffer,
	    atapi_cdb[16],
	    current_cdb[16],
	    sense[256];

    uint8_t status, phase,
	    error, id,
	    features, pad0,
	    pad1, pad2;

    uint16_t request_length, max_transfer_len;

    int requested_blocks, packet_status,
	total_length, do_page_save,
	unit_attention;

    uint32_t sector_pos, sector_len,
	     packet_len, pos;

    int64_t callback;

    int media_status, data_pos,
	request_pos, total_read,
	old_len;

    uint8_t previous_command,
	    pad3, pad4, pad5;
} scsi_cdrom_t;
#endif


extern scsi_cdrom_t	*scsi_cdrom[CDROM_NUM];

#define scsi_cdrom_sense_error dev->sense[0]
#define scsi_cdrom_sense_key dev->sense[2]
#define scsi_cdrom_asc dev->sense[12]
#define scsi_cdrom_ascq dev->sense[13]
#define scsi_cdrom_drive cdrom_drives[id].host_drive


extern void	scsi_cdrom_reset(void *p);


#endif	/*EMU_SCSI_CDROM_H*/
