#pragma once

#define _ENABLE_PHYSICAL_DEVICE_
#define FAKE_HANDLE                     (0xFFFF1234)

//#define ENABL_SCSI_PT_WITH_BUF
//#define ENABLE_4k_SEC_LEN


//---------------------------
#ifdef ENABLE_4k_SEC_LEN
#define BYTE_PER_SECTOR                 (_4K)
#define MAX_SEC_LEN                     (0x10)   // (80h * 512)/4k = 0x10
#else
#define BYTE_PER_SECTOR                 (512)
#define MAX_SEC_LEN                     (0x80)
#endif

//---------------------------
#define SECTOR_TO_BYTE(SEC)             ((SEC) * BYTE_PER_SECTOR)

//UFI cmd
#define UFI_OP_TEST_UNIT_READY          (0x00)
#define UFI_OP_REQUEST_SENSE            (0x03)
#define UFI_OP_INQUIRY                  (0x12)
#define UFI_OP_READ_FORMAT_CAPACITY     (0x23)
#define UFI_OP_READ_CAPACITY            (0x25)
#define UFI_OP_READ_10                  (0x28)
#define UFI_OP_WRITE_10                 (0x2A)

//SCSI cmd
#define SCSI_OP_SAT12                   (0xA1)

