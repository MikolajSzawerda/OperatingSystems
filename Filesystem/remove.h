#ifndef FILESYSTEM_REMOVE
#define FILESYSTEM_REMOVE

#include "file.h"

int find_file_location(FILE *fd, struct file_record* file, struct catalog_records* records, uint* cr_index, uint* f_index, char *name) {
    int index = 0;
    while (index < MAX_CATALOG_RECORDS && fs_super_inode.catalog_records[index] != -1) {
        if (!get_block(fd, records, fs_super_inode.catalog_records[index], sizeof(struct catalog_records)))
            return 0;
        int file_index = 0;
        while (file_index < MAX_FILE_RECORDS) {
            if (!strcmp(records->files[file_index].file_name, name)) {
                *cr_index = index;
                *file = records->files[file_index];
                *f_index = file_index;
                return 1;
            }
            file_index++;
        }
        index++;
    }
    return 0;
}

int remove_file(FILE* fd, char* name){
    struct catalog_records records;
    struct inode in;
    struct file_record file;
    uint cr_index;
    uint f_index;
    find_file_location(fd, &file, &records, &cr_index, &f_index, name);
    if (!get_block(fd, &in, file.inode, sizeof(struct inode))) return 0;
    int index=0;
    deallocate_block(file.inode);
    while(in.data[index]!=0){
        deallocate_block(in.data[index]);
        index++;
    }
    records.files[f_index].inode = -1;
    if (!save_block(fd, &records, fs_super_inode.catalog_records[cr_index], sizeof(struct catalog_records))) return 0;
    if(!save_private_blocks(fd)) return 0;
    add_free_catalog_record(cr_index);
    printf("FILE \"%s\" REMOVED SUCCESSFULLY\n", name);
    return 1;
}

#endif //FILESYSTEM_REMOVE
