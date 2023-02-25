#ifndef FILESYSTEM_TYPE
#define FILESYSTEM_TYPE

#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include <strings.h>
#include <string.h>
#include <errno.h>
#define BLOCK_SIZE  4096
#define MAX_CATALOG_RECORDS ((BLOCK_SIZE - 196)/32)
#define MAX_INODE_DATA ((BLOCK_SIZE-64)/32)
#define MAIN_FILE_INODE 0
#define MAX_FILE_NAME_LENGTH 12


struct super_block {
    size_t disk_size;
    size_t total_blocks;
    uint bitmap_blocks_count;
    uint bitmap_vectors;
    uint private_blocks_count;
    uint max_file_size;
    uint file_count;
    uint max_file_count;
    uint available_blocks;
};

struct allocation_map {
    int* map;
};

struct super_inode {
    char catalog_name[8];
    int free_catalog_records_count;
    uint* free_catalog_records;
    uint catalog_records[MAX_CATALOG_RECORDS];
};

struct file_record {
    char file_name[MAX_FILE_NAME_LENGTH];
    uint inode;
};

#define MAX_FILE_RECORDS (BLOCK_SIZE / (sizeof(struct file_record)*8))

struct catalog_records {
    struct file_record files[MAX_FILE_RECORDS];
};

struct inode {
    int type;
    uint file_size;
    uint data[MAX_INODE_DATA];
};


extern struct super_block fs_super_block;
extern struct super_inode fs_super_inode;
extern struct allocation_map fs_allocation_map;
#endif
