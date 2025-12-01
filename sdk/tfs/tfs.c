#include "tfs.h"
#include <sys/stat.h>

FILE *_img = NULL;
tfs_bootsect_t _bootsect;
tfs_mapptr_t *_map = NULL;

bool path_get(const char *path, int part_index, char *dest, size_t dest_size) 
{
    if (path == NULL || dest == NULL || dest_size == 0 || part_index < 0) 
    {
        return false;
    }
    
    const char *start = path;
    int current_part = 0;
    
    while (*start != '\0') 
    {
        while (*start == '/') start++;
        if (*start == '\0') break;
        
        const char *end = start;
        while (*end != '/' && *end != '\0') end++;
        
        if (current_part == part_index) {
            size_t length = end - start;
            if (length >= dest_size) {
                length = dest_size - 1;
            }
            
            strncpy(dest, start, length);
            dest[length] = '\0';
            return true;
        }
        
        current_part++;
        start = (*end == '\0') ? end : end + 1;
    }
    
    return false;
}

uint32_t mapptr_to_addr(tfs_mapptr_t *mapptr)
{
    return (mapptr->next_block[0] & 0xff) | ((mapptr->next_block[1] & 0xff) << 8)  | ((mapptr->next_block[2] & 0xff) << 16);
}

void addr_to_mapptr(tfs_mapptr_t *mapptr, uint32_t address)
{
    mapptr->next_block[0] = (address) & 0xff;
    mapptr->next_block[1] = (address >> 8) & 0xff;
    mapptr->next_block[2] = (address >> 16) & 0xff;
}

void block_write(void *block, uint32_t address)
{
    fseek(_img, address * 512, SEEK_SET);
    fwrite(block, 1, 512, _img);
    fflush(_img);
    //printf("W%06x ",address * 512);
}

void block_read(void *block, uint32_t address)
{
    fseek(_img, address * 512, SEEK_SET);
    fread(block, 1,512, _img);
    //printf("R%06x ",address * 512);
}

void tfs_mount()
{
    block_read(&_bootsect, 0);
    uint32_t address = 8;
    tfs_mapptr_t backupmap[128];
    if(strncmp(_bootsect.signature, "TFS1", 4))
    {
        fprintf(stderr, "error: file system signature corrupted. '%4s'\n", _bootsect.signature);
        exit(1);
    }
    _map = malloc(_bootsect.total_map_blocks * 512);
    if(!_map)
    {
        fprintf(stderr, "error: out of memory\n");
        exit(1);
    }
    for (size_t b = 0; b < _bootsect.total_map_blocks; b++)
    {
        block_read(&_map[b * 128], address++);
        block_read(backupmap, address++);
        if(memcmp(&_map[b * 128], backupmap, 512))
        {
            fprintf(stderr, "error: file system map corrupted at block %d.\n", address - 2);
            exit(1);
        }
    }
}

void tfs_umount()
{
    uint32_t address = 8;
    for (size_t b = 0; b < _bootsect.total_map_blocks; b++)
    {
        block_write(&_map[b * 128], address++);
        block_write(&_map[b * 128], address++);
    }
    free(_map);
}

uint32_t tfs_alloc(uint8_t type)
{
    for (size_t i = 0; i < _bootsect.total_disk_blocks; i++)
    {
        if(_map[i].type == TFS_PART_EMPTY)
        {
            _map[i].type = type;
            addr_to_mapptr(&_map[i], 0);
            return i;
        }
    }
    tfs_umount();
    fprintf(stderr, "error: disk is full.");
    exit(1);
}

bool tfs_read_next(void *block, uint32_t *address)
{
    if(*address == 0) return false;
    if(*address >= _bootsect.total_disk_blocks) return false;
    if(mapptr_to_addr(&_map[*address]) == 0) return false;
    *address = mapptr_to_addr(&_map[*address]);
    block_read(block, *address);
    return true;
}

int cmd_mkfs(int argc, char **argv)
{
    char block[512];
    uint32_t size = argc >= 2 ? (atoi(argv[1]) << 1) : 0;
    char *name = argc >= 3 ? argv[2] : "";
    if(size <= 0)
    {
        fprintf(stderr, "error: disk size not defined.\n");
        return 1;
    }
    memset(block, 0, 512);
    block_write(block, size - 1);
    block_read(&_bootsect, 0);
    strncpy(_bootsect.signature, "TFS1", 4);
    memset(_bootsect.name, 0, 20);
    strncpy(_bootsect.name, name, 20);
    _bootsect.total_disk_blocks = size;
    _bootsect.total_map_blocks = size / 128 | ((size % 128 > 0) ? 1 : 0);
    _bootsect.mbr[70] = 0x55;
    _bootsect.mbr[71] = 0xaa;
    _map = malloc(_bootsect.total_map_blocks * 128);
    if(!_map)
    {
        fprintf(stderr, "error: out of memory\n");
        return 1;
    }
    memset(_map, 0, _bootsect.total_map_blocks * 128);
    _bootsect.first_data_block = 8 + (_bootsect.total_map_blocks << 1);
    for (size_t i = 0; i < _bootsect.first_data_block; i++)
    {
        _map[i].type = TFS_PART_RESERVED;
    }
    for (size_t i = _bootsect.total_disk_blocks; i < (_bootsect.total_map_blocks * 128); i++)
    {
        _map[i].type = TFS_PART_RESERVED;
    }
    block_write(&_bootsect, 0);
    _bootsect.root_address = tfs_alloc(TFS_PART_DIR);
    block_write(&_bootsect, 0);
    memset(block, 0, 512);
    block_write(block, _bootsect.root_address);
    return 0;
}

int cmd_bootsect(int argc, char **argv)
{
    if(argc != 2)
    {
        fprintf(stderr, "error: boot sector file not defined.\n");
        return 1;
    }
    FILE *file = fopen(argv[1], "rb");
    if(!file)
    {
        fprintf(stderr, "error: can't open boot sector file.\n");
        return 1;
    }
    memset(_bootsect.bootloader, 1, 400);
    fread(_bootsect.bootloader, 1, 400, file);
    block_write(&_bootsect, 0);
    fclose(file);
    return 0;
}

int cmd_bootloader(int argc, char **argv)
{
    char block[512];
    if(argc != 2)
    {
        fprintf(stderr, "error: boot sector file not defined.\n");
        return 1;
    }
    FILE *file = fopen(argv[1], "rb");
    if(!file)
    {
        fprintf(stderr, "error: can't open boot sector file.\n");
        return 1;
    }
    for (size_t i = 1; i < 8; i++)
    {
        memset(block, 1, 512);
        fread(block, 1, 512, file);
        block_write(block, i);
    }
    fclose(file);
    return 0;
}

int cmd_df(int argc, char **argv)
{
    char block[512];
    if(argc != 1)
    {
        fprintf(stderr, "error: invalid arguments.\n");
        return 1;
    }
    printf("FS 512-blocks     Used           Available      Capacity\n");
    uint32_t free = 0;
    for (size_t i = 0; i < _bootsect.total_disk_blocks; i++)
    {
        if(_map[i].type == TFS_PART_EMPTY) free++;
    }
    printf
    (
        "/  %-14d %-14d %-14d %7d%%\n", 
        _bootsect.total_disk_blocks, 
        _bootsect.total_disk_blocks - free, 
        free, 
        (_bootsect.total_disk_blocks - free) * 100 / _bootsect.total_disk_blocks 
    );
    
    return 0;
}

int cmd_ls(int argc, char **argv)
{
    char *mon[] = 
    {
        "Jan",
        "Feb",
        "Mar",
        "Apr",
        "May",
        "Jun",
        "Jul",
        "Aug",
        "Sep",
        "Oct",
        "Nov",
        "Dez"
    };
    char *path;
    if(argc == 1)
    {
        path = "/";
    }
    else 
    {
        if(argc != 2)
        {
            fprintf(stderr, "error: invalid arguments.\n");
            return 1;
        }
        path = argv[1];
    }
    char part[65];
    int part_i = 0;
    bool found_part;
    uint32_t dir_address = _bootsect.root_address;
    tfs_dirrec_t dir[4];
    while(path_get(path, part_i++, part, 64))
    {
        found_part = false;
        do
        {
            block_read(dir, dir_address);
            for (size_t i = 0; i < 4; i++)
            {
                if((dir[i].permissions & S_IFDIR) && !strncmp(dir[i].name, part, 64))
                {
                    found_part = true;
                    dir_address = mapptr_to_addr(&dir[i].data_fork);
                    break;
                }
            }
            if(found_part) break;
        } while(tfs_read_next(dir, &dir_address));
        if(!found_part)
        {
            fprintf(stderr, "error: no such file or directory: %s\n", path);
            return 1;
        }
    }
    block_read(dir, dir_address);
    do
    {
        for (size_t i = 0; i < 4; i++)
        {
            if(dir[i].permissions != 0)
            {
                printf
                (
                    "%c%c%c%c%c%c%c%c%c%c 1 user%04d group%04d %12d %s %2d %02d:%02d %s\n",
                    dir[i].permissions & S_IFDIR ? 'd' : '-',
                    dir[i].permissions & 0400 ? 'r' : '-',
                    dir[i].permissions & 0200 ? 'w' : '-',
                    dir[i].permissions & 0100 ? 'x' : '-',
                    dir[i].permissions & 0040 ? 'r' : '-',
                    dir[i].permissions & 0020 ? 'w' : '-',
                    dir[i].permissions & 0010 ? 'x' : '-',
                    dir[i].permissions & 0004 ? 'r' : '-',
                    dir[i].permissions & 0002 ? 'w' : '-',
                    dir[i].permissions & 0001 ? 'x' : '-',
                    dir[i].user_id,
                    dir[i].group_id,
                    dir[i].data_size,
                    mon[dir[i].creation_date.month % 12],
                    dir[i].creation_date.day,
                    dir[i].creation_date.hour,
                    dir[i].creation_date.min,
                    dir[i].name
                );
            }
        }
    } while(tfs_read_next(dir, &dir_address));

    return 0;
}

int cmd_mkdir(int argc, char **argv)
{
    if(argc != 2)
    {
        fprintf(stderr, "error: invalid arguments.\n");
        return 1;
    }
    time_t t;
    struct tm *tm_info = malloc(sizeof(struct tm));
    char part[65];
    int part_i = 0;
    bool found_part;
    uint32_t dir_address = _bootsect.root_address;
    uint32_t dir_first_address = _bootsect.root_address;
    tfs_dirrec_t dir[4];
    t = time(NULL);
    localtime_r(&t, tm_info);
    while(path_get(argv[1], part_i++, part, 64))
    {
        found_part = false;
        do
        {
            block_read(dir, dir_address);
            for (size_t i = 0; i < 4; i++)
            {
                if((dir[i].permissions & S_IFDIR) && !strncmp(dir[i].name, part, 64))
                {
                    found_part = true;
                    dir_address = mapptr_to_addr(&dir[i].data_fork);
                    dir_first_address = dir_address;
                    break;
                }
            }
            if(found_part) break;
        } while(tfs_read_next(dir, &dir_address));
        if(!found_part)
        {
            dir_address = dir_first_address;
            do
            {
                block_read(dir, dir_address);
                for (size_t i = 0; i < 4; i++)
                {
                    if(dir[i].permissions == 0)
                    {
                        found_part = true;
                        memset(&dir[i], 0, sizeof(tfs_dirrec_t));
                        dir_first_address = tfs_alloc(TFS_PART_DIR);
                        addr_to_mapptr(&dir[i].data_fork, dir_first_address);
                        dir[i].data_fork.type = TFS_PART_DIR;
                        strncpy(dir[i].name, part, 64);
                        dir[i].permissions = 0666 | S_IFDIR;
                        dir[i].creation_date.year = tm_info->tm_year + 1900;
                        dir[i].creation_date.month = tm_info->tm_mon;
                        dir[i].creation_date.day = tm_info->tm_mday;
                        dir[i].creation_date.hour = tm_info->tm_hour;
                        dir[i].creation_date.min = tm_info->tm_min;
                        dir[i].creation_date.sec = tm_info->tm_sec;
                        dir[i].creation_date.centi = 0;
                        memcpy(&dir[i].modification_date, &dir[i].creation_date, sizeof(tfs_datetime_t));
                        block_write(dir, dir_address);
                        memset(dir, 0, 512);
                        block_write(dir, dir_first_address);
                        break;
                    }
                }
                if(found_part) break;
                if(!tfs_read_next(dir, &dir_address))
                {
                    addr_to_mapptr(&_map[dir_address], tfs_alloc(TFS_PART_DIR));
                    if(!tfs_read_next(dir, &dir_address))
                    {
                        fprintf(stderr, "error: can't alloc block.\n");
                        return 1;
                    }
                }
            } while(true);
        }
    }

    return 0;
}

int cmd_add(int argc, char **argv)
{
    if(argc != 3)
    {
        fprintf(stderr, "error: invalid arguments. %d\n", argc);
        return 1;
    }
    FILE *file = fopen(argv[2], "rb");
    if(!file)
    {
        fprintf(stderr, "error: can't open file: %s\n", argv[2]);
        return 1;
    }
    time_t t;
    struct tm *tm_info = malloc(sizeof(struct tm));
    uint8_t block[512];
    char part[65];
    char temp_part[65];
    int part_i = 0;
    bool found_part;
    uint32_t dir_address = _bootsect.root_address;
    uint32_t dir_first_address = _bootsect.root_address;
    uint32_t file_address;
    tfs_dirrec_t *file_rec;
    tfs_dirrec_t dir[4];
    t = time(NULL);
    localtime_r(&t, tm_info);
    while(path_get(argv[1], part_i++, part, 64))
    {
        found_part = false;
        do
        {
            block_read(dir, dir_address);
            for (size_t i = 0; i < 4; i++)
            {
                if(dir[i].permissions != 0 && !strncmp(dir[i].name, part, 64))
                {
                    if
                    (
                        (
                            !path_get(argv[1], part_i, temp_part, 64) &&
                            dir[i].data_fork.type == TFS_PART_DIR
                        )
                        ||
                        (
                            path_get(argv[1], part_i, temp_part, 64)&&
                            dir[i].data_fork.type == TFS_PART_FILE
                        )
                    )
                    {
                        fprintf(stderr, "error: invalid file path.\n");
                        return 1;
                    }
                    found_part = true;
                    if(dir[i].data_fork.type != TFS_PART_FILE)
                    {
                        dir_address = mapptr_to_addr(&dir[i].data_fork);
                        dir_first_address = dir_address;
                        dir[i].permissions |= S_IFDIR;
                    }
                    else
                    {
                        dir[i].permissions |= S_IFREG;
                    }
                    file_address = mapptr_to_addr(&dir[i].data_fork);
                    file_rec = &dir[i];
                    break;
                }
            }
            if(found_part) break;
        } while(tfs_read_next(dir, &dir_address));
        if(!found_part)
        {
            dir_address = dir_first_address;
            do
            {
                block_read(dir, dir_address);
                for (size_t i = 0; i < 4; i++)
                {
                    if(dir[i].permissions == 0)
                    {
                        found_part = true;
                        memset(&dir[i], 0, sizeof(tfs_dirrec_t));
                        if(path_get(argv[1], part_i, temp_part, 64))
                        {
                            dir_first_address = tfs_alloc(TFS_PART_DIR);
                            dir[i].data_fork.type = TFS_PART_DIR;
                        }
                        else
                        {
                            dir_first_address = tfs_alloc(TFS_PART_FILE);
                            dir[i].data_fork.type = TFS_PART_FILE;
                        }
                        addr_to_mapptr(&dir[i].data_fork, dir_first_address);
                        strncpy(dir[i].name, part, 64);
                        dir[i].permissions = 0666;
                        dir[i].creation_date.year = tm_info->tm_year + 1900;
                        dir[i].creation_date.month = tm_info->tm_mon;
                        dir[i].creation_date.day = tm_info->tm_mday;
                        dir[i].creation_date.hour = tm_info->tm_hour;
                        dir[i].creation_date.min = tm_info->tm_min;
                        dir[i].creation_date.sec = tm_info->tm_sec;
                        dir[i].creation_date.centi = 0;
                        memcpy(&dir[i].modification_date, &dir[i].creation_date, sizeof(tfs_datetime_t));
                        block_write(dir, dir_address);
                        memset(block, 0, 512);
                        block_write(block, dir_first_address);
                        file_rec = &dir[i];
                        file_address = mapptr_to_addr(&dir[i].data_fork);
                        break;
                    }
                }
                if(found_part) break;
                if(!tfs_read_next(dir, &dir_address))
                {
                    addr_to_mapptr(&_map[dir_address], tfs_alloc(TFS_PART_DIR));
                    if(!tfs_read_next(dir, &dir_address))
                    {
                        fprintf(stderr, "error: can't alloc block.\n");
                        return 1;
                    }
                }
            } while(true);
        }
    }
    file_rec->data_size = 0;
    int size = 0;
    memset(block, 0, 512);
    while((size = fread(block, 1, 512, file)) > 0)
    {
        file_rec->data_size += size;
        block_write(block, file_address);
        memset(block, 0, 512);
        if(size < 512) break;
        if(!tfs_read_next(block, &file_address))
        {
            uint32_t next_block_address = tfs_alloc(TFS_PART_FILE);
            addr_to_mapptr(&_map[file_address], next_block_address);
            if(!tfs_read_next(block, &file_address))
            {
                fprintf(stderr, "error: can't alloc data block.\n");
                return 1;
            }
        }
    }
    block_write(dir, dir_address);
    fclose(file);
    if(mapptr_to_addr(&_map[file_address]) != 0)
    {
        uint32_t trim_address = mapptr_to_addr(&_map[file_address]);
        addr_to_mapptr(&_map[file_address], 0);
        while(trim_address)
        {
            file_address = trim_address;
            trim_address = mapptr_to_addr(&_map[file_address]);
            addr_to_mapptr(&_map[file_address], 0);
            _map[file_address].type = TFS_PART_EMPTY;
        }
    }
    return 0;
}

int cmd_export(int argc, char **argv)
{
    if(argc != 3)
    {
        fprintf(stderr, "error: invalid arguments.\n");
        return 1;
    }
    FILE *file = fopen(argv[2], "wb");
    if(!file)
    {
        fprintf(stderr, "error: can't create file: %s\n", argv[2]);
        return 1;
    }
    char part[65];
    int part_i = 0;
    bool found_part;
    uint32_t dir_address = _bootsect.root_address;
    tfs_dirrec_t dir[4];
    uint8_t block[512];
    uint32_t file_address;
    int32_t size;
    while(path_get(argv[1], part_i++, part, 64))
    {
        found_part = false;
        do
        {
            block_read(dir, dir_address);
            for (size_t i = 0; i < 4; i++)
            {
                if(dir[i].permissions != 0 && !strncmp(dir[i].name, part, 64))
                {
                    found_part = true;
                    dir_address = mapptr_to_addr(&dir[i].data_fork);
                    file_address = mapptr_to_addr(&dir[i].data_fork);
                    size = dir[i].data_size;
                    break;
                }
            }
            if(found_part) break;
        } while(tfs_read_next(dir, &dir_address));
        if(!found_part)
        {
            fprintf(stderr, "error: no such file or directory: %s\n", argv[1]);
            return 1;
        }
    }
    do
    {
        block_read(block, file_address);
        fwrite(block, 1, size < 512 ? size : 512, file);
        size -= 512;
    } while(size > 0 && tfs_read_next(block, &file_address));
    fclose(file);
    return 0;
}


int main(int argc, char **argv)
{
    int ret = 1;
    if(argc < 3)
    {
        printf("Usage: tfs [imagefile] mkfs [size in kib] [partition name]\n");
        printf("       format existing image or disk\n");
        printf("Usage: tfs [imagefile] genfs [size in kib] [partition name]\n");
        printf("       create new image\n");
        printf("Usage: tfs [imagefile] bootsect [file]\n");
        printf("       write boot sector (first 400 bytes)\n");
        printf("Usage: tfs [imagefile] bootloader [file]\n");
        printf("       write bootloader on reserved space (max 3.5KiB)\n");
        printf("Usage: tfs [imagefile] df\n");
        printf("       show disk free stats.\n");
        printf("Usage: tfs [imagefile] ls [path]\n");
        printf("       list directory contents.\n");
        printf("Usage: tfs [imagefile] mkdir [path]\n");
        printf("       make directory.\n");
        printf("Usage: tfs [imagefile] add [path] [source data file]\n");
        printf("       add file.\n");
        printf("Usage: tfs [imagefile] export [path] [destination data file]\n");
        printf("       export data from file.\n");
        return 1;
    }
    if(!strcmp(argv[2], "genfs"))
    {
        _img = fopen(argv[1], "wb+");
        if(!_img)
        {
            fprintf(stderr, "error: can't create disk image: %s\n", argv[1]);
            return 1;
        }
        ret = cmd_mkfs(argc - 2, &argv[2]);
    }
    else if(!strcmp(argv[2], "mkfs"))
    {
        _img = fopen(argv[1], "rb+");
        if(!_img)
        {
            fprintf(stderr, "error: can't open disk image: %s\n", argv[1]);
            return 1;
        }
        ret = cmd_mkfs(argc - 2, &argv[2]);
    }
    else 
    {
        _img = fopen(argv[1], "rb+");
        if(!_img)
        {
            fprintf(stderr, "error: can't open disk image: %s\n", argv[1]);
            return 1;
        }
        tfs_mount();
        if(!strcmp(argv[2], "bootsect"))
        {
            ret = cmd_bootsect(argc - 2, &argv[2]);
        }
        else if(!strcmp(argv[2], "bootloader"))
        {
            ret = cmd_bootloader(argc - 2, &argv[2]);
        }
        else if(!strcmp(argv[2], "df"))
        {
            ret = cmd_df(argc - 2, &argv[2]);
        }
        else if(!strcmp(argv[2], "ls"))
        {
            ret = cmd_ls(argc - 2, &argv[2]);
        }
        else if(!strcmp(argv[2], "mkdir"))
        {
            ret = cmd_mkdir(argc - 2, &argv[2]);
        }
        else if(!strcmp(argv[2], "add"))
        {
            ret = cmd_add(argc - 2, &argv[2]);
        }
        else if(!strcmp(argv[2], "export"))
        {
            ret = cmd_export(argc - 2, &argv[2]);
        }
        else
        {
            fprintf(stderr, "error: command unknown: %s\n", argv[2]);
            return 1;
        }
    }
    tfs_umount();
    fclose(_img);
    return ret;
}
