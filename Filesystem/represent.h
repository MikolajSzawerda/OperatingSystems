#ifndef FILESYSTEM_REPRESENT
#define FILESYSTEM_REPRESENT

#include <sys/types.h>
#include <stdlib.h>
#include "file.h"

#define SUPER_BLOCK 0
#define SUPER_INODE 1
#define ALLOCATION_MAP 2
#define CATALOG_DESCRIPTORS 3
#define INODE 4
#define DATA_BLOCK 5
#define EMPTY_BLOCK 6

#define EMPTY 1
#define FILLED 0

struct filesystem_record {
    uint block;
    short type;
    short size;
};

int comp (const void * elem1, const void * elem2)
{
    struct filesystem_record f = *((struct filesystem_record*)elem1);
    struct filesystem_record s = *((struct filesystem_record*)elem2);
    if (f.block > s.block) return  1;
    if (f.block < s.block) return -1;
    return 0;
}

void get_type_label(char* buffer, int type){
    switch (type) {
        case SUPER_BLOCK:
            strcpy(buffer, "super block");
            break;
        case SUPER_INODE:
            strcpy(buffer, "super inode");
            break;
        case ALLOCATION_MAP:
            strcpy(buffer, "allocation map");
            break;
        case CATALOG_DESCRIPTORS:
            strcpy(buffer, "catalog descriptor");
            break;
        case INODE:
            strcpy(buffer, "inode");
            break;
        case DATA_BLOCK:
            strcpy(buffer, "data block");
            break;
        case EMPTY_BLOCK:
            strcpy(buffer, "empty block");
            break;
        default:
            strcpy(buffer, "unknown");
            break;
    }
}

void get_state_label(char* buffer, int state){
    switch (state) {
        case EMPTY:
            strcpy(buffer, "empty");
            break;
        case FILLED:
            strcpy(buffer, "filled");
            break;
        default:
            strcpy(buffer, "unknown");
            break;
    }
}

int get_flat_filesystem(FILE* fd, struct filesystem_record* buffer, uint* n){
    int index = 0;
    buffer[index++] = (struct filesystem_record) {0, SUPER_BLOCK, sizeof(struct super_block)*8};
    buffer[index++] = (struct filesystem_record) {1, SUPER_INODE, sizeof(struct super_inode)*8};
    for(int i=0;i<fs_super_block.bitmap_blocks_count; i++, index++) {
        buffer[index] = (struct filesystem_record) {index, ALLOCATION_MAP, BLOCK_SIZE};
    }

    struct catalog_records fsd;
    struct inode in;
    for(int i=0; i < MAX_CATALOG_RECORDS; i++){
        if(fs_super_inode.catalog_records[i] == -1) continue;
        buffer[index++] = (struct filesystem_record) {fs_super_inode.catalog_records[i], CATALOG_DESCRIPTORS, sizeof(struct catalog_records) * 8};
        if(!get_block(fd, &fsd, fs_super_inode.catalog_records[i], sizeof(struct catalog_records))) continue;
        for(int j=0; j < MAX_FILE_RECORDS; j++){
            if(fsd.files[j].inode==-1) continue;
            buffer[index++] = (struct filesystem_record) {fsd.files[j].inode, INODE, sizeof(struct inode)*8};
            if(!get_block(fd, &in, fsd.files[j].inode, sizeof(struct inode))) continue;
            for(int k=0;in.data[k]!=0;k++){
                buffer[index++] = (struct filesystem_record) {in.data[k], DATA_BLOCK, BLOCK_SIZE};
            }
            buffer[index-1].size = in.file_size % BLOCK_SIZE;
        }
    }

    *n = index;
    qsort(buffer, *n, sizeof(struct filesystem_record), comp);
}

void print_filesystem_map(FILE* fd, int print_only_filled){
    struct filesystem_record* map;
    uint n = fs_super_block.total_blocks-fs_super_block.available_blocks;
    map = (struct filesystem_record*) calloc(n,sizeof(struct filesystem_record));
    get_flat_filesystem(fd, map, &n);
    char label[30];
    char state_label[30];
    short size;
    printf("|------------------------------------------------------------------------|\n");
    for(int i=0, block_id=0, idx=0; i < fs_super_block.bitmap_vectors; i++){
        int bitmap_vector = fs_allocation_map.map[i];
        for(int j=0;j<sizeof(int)*8;j++, block_id++){
            int temp = bitmap_vector & 1;

            if(map[idx].block==block_id){
                size = map[idx].size;
                get_type_label(label, map[idx++].type);
            }
            else {
                if(idx<n) size=BLOCK_SIZE;
                else{
                    if(print_only_filled){
                        printf("|------------------------------------------------------------------------|\n");
                        free(map);
                        return;
                    }
                    size = 0;
                }
                get_type_label(label, EMPTY_BLOCK);
            }
            get_state_label(state_label, temp);
            printf("|ID: %-5d|Type: %-20s|SPACE: %10d|STATE: %-10s|\n", block_id, label, size, state_label);

            bitmap_vector >>= 1;
        }
    }
    printf("|------------------------------------------------------------------------|\n");
    free(map);
}

void print_catalog(FILE* fd) {
    printf("CATALOG: \"%s\"\n", fs_super_inode.catalog_name);
    struct catalog_records fsd;
    for(int i=0; i < MAX_CATALOG_RECORDS; i++){
        if(fs_super_inode.catalog_records[i] == -1) continue;
        if(!get_block(fd, &fsd, fs_super_inode.catalog_records[i], sizeof(struct catalog_records))) continue;
        for(int j=0; j < MAX_FILE_RECORDS; j++){
            if(fsd.files[j].inode==-1) continue;
            printf("\t- %s\n", fsd.files[j].file_name);
        }
    }
}

#endif //FILESYSTEM_REPRESENT
