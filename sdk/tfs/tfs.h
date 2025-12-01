
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#pragma pack(1)

typedef struct tfs_bootsect_t
{
    uint8_t bootloader[400];        // 000-399
    char signature[4];              // 400-403
    uint32_t total_disk_blocks;     // 404-407
    uint32_t total_map_blocks;      // 408-411
    uint32_t root_address;          // 412-415
    uint32_t first_data_block;      // 416-419
    char name[20];                  // 420-439
    char mbr[72];                   // 440-511
} tfs_bootsect_t;

typedef struct tfs_mapptr_t
{
    uint8_t type;
    uint8_t next_block[3];
} tfs_mapptr_t;

enum
{
    TFS_PART_EMPTY  = 0x00,
    TFS_PART_FILE = 0x10,
    TFS_PART_DIR = 0x20,
    TFS_PART_RES = 0x40,
    TFS_PART_RESERVED = 0xff
};

typedef struct tfs_datetime_t
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t centi;
} tfs_datetime_t;

typedef struct tfs_dirrec_t
{
    uint16_t permissions;
    tfs_mapptr_t data_fork;
    uint32_t data_size;
    tfs_datetime_t creation_date;
    tfs_datetime_t modification_date;
    uint16_t user_id;
    uint16_t group_id;
    uint16_t kext_int_num;
    uint16_t kext_io_port;
    uint16_t kext_flags;
    uint32_t kext_id;
    char reserved[24];
    char name[64];
} tfs_dirrec_t;

#pragma pack()