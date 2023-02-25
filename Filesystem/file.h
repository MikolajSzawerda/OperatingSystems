#ifndef FILESYSTEM_FILE
#define FILESYSTEM_FILE
#include "block.h"
#include "save.h"

int add_catalog_record(FILE *fd, uint *records_index) {
    int index = 0;
    int buffer;
    if(!allocate_n_blocks(&buffer, 1)) return 0;
    struct catalog_records records;
    memset(&records.files, 0, sizeof(struct catalog_records));
    while (index < MAX_CATALOG_RECORDS && fs_super_inode.catalog_records[index] != -1) index++;
    if (index >= MAX_CATALOG_RECORDS) return 0;
    fs_super_inode.catalog_records[index] = buffer;
    for (int i = 0; i < MAX_FILE_RECORDS; i++) records.files[i].inode = -1;
    *records_index = index;
    int r = save_block(fd, &records, buffer, sizeof(struct catalog_records));
    if(r) printf("CATALOG RECORDS BLOCK ADDED SUCCESSFULLY\n");
    return r;
}


int read_file_block(FILE *fd, int *buffer) {
    size_t r = fread(buffer, BLOCK_SIZE, 1, fd);
    return r <= 0;
}

int get_new_file_index(struct catalog_records *records) {
    int index = 0;
    while (index < MAX_FILE_RECORDS && records->files[index].inode != -1) index++;
    return index < MAX_FILE_RECORDS ? index : -1;
}

int can_add_new_file(struct catalog_records *records) {
    return get_new_file_index(records) != -1;
}

int add_free_catalog_record(uint records_index){
    int index=0;
    while(index<fs_super_inode.free_catalog_records_count){
        if(fs_super_inode.free_catalog_records[index] == records_index) return 1;
        index++;
    }
    fs_super_inode.free_catalog_records[++fs_super_inode.free_catalog_records_count] = records_index;
}

int get_or_create_catalog_record(FILE *fd, struct catalog_records *records, uint *cr_index) {
    *cr_index = fs_super_inode.free_catalog_records_count;
    uint cr_block;
    int index=fs_super_inode.free_catalog_records_count;
    while(index>=0){
        *cr_index = fs_super_inode.free_catalog_records[index];
        cr_block = fs_super_inode.catalog_records[*cr_index];
        get_block(fd, records, cr_block, sizeof(struct catalog_records));
        if(can_add_new_file(records)) break;
        index--;
    }
    if(index<0){
        if(!add_catalog_record(fd, cr_index)) return 0;
        fs_super_inode.free_catalog_records[++fs_super_inode.free_catalog_records_count]=*cr_index;
        save_private_blocks(fd);
        cr_block = fs_super_inode.catalog_records[*cr_index];
        get_block(fd, records, cr_block, sizeof(struct catalog_records));
        if(can_add_new_file(records)) return 1;
        else return 0;
    }
    return 1;
}

size_t get_file_size(FILE *file) {
    uint old_pos = ftell(file);
    fseek(file, 0, SEEK_END);
    uint size = ftell(file);
    fseek(file, old_pos, SEEK_SET);
    return size;
}


int find_file(FILE *fd, struct file_record *file, char *name) {
    int fsd_index = 0;
    struct catalog_records fsd;
    while (fsd_index < MAX_CATALOG_RECORDS && fs_super_inode.catalog_records[fsd_index] != -1) {
        if (!get_block(fd, &fsd, fs_super_inode.catalog_records[fsd_index], sizeof(struct catalog_records)))
            return 0;
        int file_index = 0;
        while (file_index < MAX_FILE_RECORDS) {
            if (!strcmp(fsd.files[file_index].file_name, name)) {
                file->inode = fsd.files[file_index].inode;
                strcpy(file->file_name, fsd.files[file_index].file_name);
                return 1;
            }
            file_index++;
        }
        fsd_index++;
    }
    return 0;
}

int is_name_unique(FILE* fd, char* name){
    int fsd_index = 0;
    struct catalog_records fsd;
    while (fsd_index < MAX_CATALOG_RECORDS && fs_super_inode.catalog_records[fsd_index] != -1) {
        if (!get_block(fd, &fsd, fs_super_inode.catalog_records[fsd_index], sizeof(struct catalog_records)))
            return 0;
        int file_index = 0;
        while (file_index < MAX_FILE_RECORDS) {
            if (!strcmp(fsd.files[file_index].file_name, name)) {
                return 0;
            }
            file_index++;
        }
        fsd_index++;
    }
    return 1;
}

#endif
