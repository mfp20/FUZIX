#include <stddef.h>

#include "tm4c129x.h"

#undef CONFIG_AUDIO
#define CONFIG_BANKS (65536U / 512U)
#undef CONFIG_BANK16
#undef CONFIG_BANK16_LOW
#undef CONFIG_BANK16FC
#undef CONFIG_BANK32
#undef CONFIG_LOW_PAGE
#undef CONFIG_HAS_LOW_PAGE
#undef CONFIG_COMMON_COPY
#undef CONFIG_BANK_65C816
#undef CONFIG_BANK_8086
#undef CONFIG_BANK_LINEAR
#undef CONFIG_BANK_FIXED
#undef CONFIG_BANK8
#undef CONFIG_VIDMAP8
#undef CONFIG_SUPERVISOR_SPACE
#undef CONFIG_IBMPC_EMM
#undef CONFIG_CPM_EMU
#undef CONFIG_SWAP
#undef CONFIG_SWAP_ONLY
#undef CONFIG_DYNAMIC_SWAP
#undef CONFIG_DYNAMIC_BUFPOOL
#undef CONFIG_SPLIT_UDATA
#undef CONFIG_UDATA_TEXTTOP
#undef CONFIG_BUDDY_MALLOC
#undef CONFIG_INPUT
#undef CONFIG_INPUT_GRABMAX
#undef CONFIG_VT
#undef CONFIG_VT_SIMPLE
#undef CONFIG_VT_MULTI
#undef CONFIG_UNIKEY
#undef CONFIG_INDIRECT_QUEUES
#undef CONFIG_BLOCK_SLEEP
#undef CONFIG_BLKBUF_EXTERNAL
#define CONFIG_BLKBUF_HELPERS
#define CONFIG_TRIM
#undef CONFIG_BIG_FS
#undef CONFIG_MBR_OFFSET
#undef CONFIG_LEVEL_0
#define CONFIG_LEVEL_2
#undef CONFIG_SINGLETASK
#define CONFIG_MULTI
#undef CONFIG_PARENT_FIRST
#undef CONFIG_ACCT
#undef CONFIG_PROFIL
#define CONFIG_32BIT
#define CONFIG_FLAT
#undef CONFIG_USERMEM_C
#define CONFIG_USERMEM_DIRECT
#undef CONFIG_VMMU
#undef CONFIG_CUSTOM_VALADDR
#undef CONFIG_IDUMP
#undef CONFIG_MONITOR
#undef CONFIG_DEV_PLATFORM
#undef CONFIG_DEV_GPIO
#undef CONFIG_DEV_MEM
#undef CONFIG_DEV_I2C
#undef CONFIG_DEV_PTY
#define CONFIG_PTY_DEV
#define CONFIG_SD
#undef CONFIG_IDE
#undef CONFIG_IDE_BSWAP
#undef CONFIG_PPIDE
#undef CONFIG_MULTI_IDE
#undef CONFIG_SCSI
#undef CONFIG_FLOPPY_NOHD
#undef CONFIG_FDC765_MULTISECTOR
#undef CONFIG_NET
#undef CONFIG_NET_NATIVE
#undef CONFIG_NET_WIZNET
#undef CONFIG_NET_WIZNET5300
#undef CONFIG_WIZNET_INDIRECT
#undef CONFIG_RTC
#undef CONFIG_RTC_INTERVAL
#undef CONFIG_RTC_FULL
#undef CONFIG_RTC_EXTENDED
#undef CONFIG_RTC_RP5C01_NVRAM
#undef CONFIG_NO_CLOCK
#undef CONFIG_LP_GENERIC
#undef CONFIG_GPT
#undef CONFIG_FONT_4X6
#undef CONFIG_FONT_4x8
#undef CONFIG_FONT6X8
#undef CONFIG_FONT8X8
#undef CONFIG_FONT8X8SMALL
#undef CONFIG_FONT_8X8_EXP2
#define CONFIG_LARGE_IO_DIRECT(x) !0
#undef CONFIG_PDP11_04
#undef CONFIG_CALL_R2L
#undef CONFIG_PLATFORM_SUSPEND
#undef CONFIG_PLATFORM_SWAPCTL
#define CONFIG_STACKSIZE 2048U
#define CONFIG_IDLETHREAD_STACKSIZE CONFIG_STACKSIZE
#define CONFIG_INLINE_IRQ
#undef CONFIG_SOFT_IRQ
#define CONFIG_HAVE_LONG_LONG 1

#define NUM_DEV_TTY 1U

#define BOOT_TTY (512 + 1)    /* Set this to default device for stdio, stderr */
                         /* In this case, the default is the first TTY device */
#define TTYDEV BOOT_TTY

#define SD_DRIVE_COUNT 1U
#define MAX_BLKDEV 2U
#define MAX_SWAPS 0U
#define NMOUNTS 8U
#define NBUFS 16U
#define TICKSPERSEC 120U

#define BOOTDEVICENAMES "hd#"
#define CMDLINE NULL
