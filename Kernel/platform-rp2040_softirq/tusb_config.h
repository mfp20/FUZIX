#ifndef TUSB_CONFIG_H
#define TUSB_CONFIG_H

#include "config.h"

//--------------------------------------------------------------------
// COMMON CONFIGURATION
//--------------------------------------------------------------------

#define CFG_TUSB_RHPORT0_MODE     (OPT_MODE_DEVICE)
//#ifndef CFG_TUSB_OS
//#define CFG_TUSB_OS                 (OPT_OS_PICO)
//#endif
//#ifndef CFG_TUSB_MEM_SECTION
//#define CFG_TUSB_MEM_SECTION
//#endif
//#ifndef CFG_TUSB_MEM_ALIGN
//#define CFG_TUSB_MEM_ALIGN          __attribute__ ((aligned(4)))
//#endif


//--------------------------------------------------------------------
// DEVICE CONFIGURATION
//--------------------------------------------------------------------

#ifndef CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_ENDPOINT0_SIZE    (64)
#endif


//------------- CLASS -------------//

#define CFG_TUD_CDC     (USB_DEV_TTY1+USB_DEV_TTY2+USB_DEV_TTY3+USB_DEV_CDC)
#define CFG_TUD_VENDOR  (USB_DEV_MPLEX_SYS+USB_DEV_MPLEX_USR+USB_DEV_VENDOR)

#if (((CFG_TUD_CDC*2)+(CFG_TUD_VENDOR*2))>15)
#error "Too many endpoints in use, disable some devices"
#endif

#define CFG_TUD_CDC_RX_BUFSIZE (64)
#define CFG_TUD_CDC_TX_BUFSIZE (64)

#define CFG_TUD_VENDOR_RX_BUFSIZE (1024) // TODO 512 (block device's default block size) might be better
#define CFG_TUD_VENDOR_TX_BUFSIZE (1024)

#define USBD_MAX_POWER_MA (250)

#define USB_PACKET_MAX_SIZE_DATA_SINGLE (64)
#define USB_PACKET_MAX_SIZE_DATA_SMALL  (128)
#define USB_PACKET_MAX_SIZE_DATA_MEDIUM (256)
#define USB_PACKET_MAX_SIZE_DATA_BIG    (512)

#endif
