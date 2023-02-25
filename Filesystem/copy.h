#ifndef FILESYSTEM_COPY
#define FILESYSTEM_COPY

#include "file.h"
#include "assert.h"
#include "save.h"



int copy_file_to_vdisk(FILE *fd, char *source_name, char* destination_name) {
    if(strlen(destination_name) > MAX_FILE_NAME_LENGTH) return 0;
    if(!is_name_unique(fd, destination_name)) return 0;
    FILE *source = fopen(source_name, "r");
    if (!source){
        fclose(source);
        return 0;
    }
    uint file_size = get_file_size(source);
    if(file_size>fs_super_block.max_file_size){
        fclose(source);
        return 0;
    }
    uint needed_blocks = ceil((double) file_size / BLOCK_SIZE);
    if(needed_blocks+1 > fs_super_block.available_blocks){
        fclose(source);
        return 0;
    }
    struct catalog_records records;
    uint records_index;
    get_or_create_catalog_record(fd, &records, &records_index);
    uint records_block = fs_super_inode.catalog_records[records_index];
    int file_record_index = get_new_file_index(&records);

    struct file_record *new_file = &records.files[file_record_index];
    strcpy(new_file->file_name, destination_name);
    struct inode node = {MAIN_FILE_INODE, file_size};
    int in_buffer;
    if(!allocate_n_blocks(&in_buffer, 1)) return 0;
    new_file->inode = in_buffer;
    if (!save_block(fd, &records, records_block, sizeof(struct catalog_records))) return 0;
    if (!save_block(fd, &node, in_buffer, sizeof(struct inode))) return 0;

    assert(needed_blocks > 0);
    int *buffer = (int *) calloc(needed_blocks, sizeof(int));
    if(!allocate_n_blocks(buffer, needed_blocks)){
        free(buffer);
        fclose(source);
        return 0;
    }
    fseek(source, 0, SEEK_SET);
    int *content_buffer = (int *) malloc(BLOCK_SIZE);
    for (int i = 0; i < needed_blocks; i++) {
        node.data[i] = buffer[i];
        memset(content_buffer, 0, BLOCK_SIZE);
        read_file_block(source, content_buffer);
        if (!save_block(fd, content_buffer, node.data[i], BLOCK_SIZE)) {
            fclose(source);
            free(buffer);
            free(content_buffer);
            return 0;
        }
    }
    if (!save_block(fd, &node, in_buffer, sizeof(struct inode))) return 0;
    fs_super_block.file_count+=1;
    save_private_blocks(fd);
    free(buffer);
    free(content_buffer);
    fclose(source);
    printf("FILE \"%s\" ADDED SUCCESSFULLY AS \"%s\" TO VIRTUAL DISK\n", source_name, destination_name);
    return 1;
}


int copy_file_to_disk(FILE *fd, char *source, char *destination) {
    struct inode node;
    struct file_record file;
    find_file(fd, &file, source);
    if (!get_block(fd, &node, file.inode, sizeof(struct inode))) return 0;
    FILE *dest = fopen(destination, "w+");
    if (!dest){
        fclose(dest);
        return 0;
    }
    char *buffer = (char *) malloc(BLOCK_SIZE);
    int inode_index = 0;
    uint remaining_file_size = node.file_size;
    while (node.data[inode_index] != 0) {
        if (!get_block(fd, buffer, node.data[inode_index], BLOCK_SIZE)) {
            fclose(dest);
            free(buffer);
            return 0;
        }
        uint real_block_size = (double) remaining_file_size / BLOCK_SIZE > 1.0 ? BLOCK_SIZE : remaining_file_size;
        size_t r = fwrite(buffer, real_block_size, 1, dest);
        if (r != 1) {
            free(buffer);
            fclose(dest);
            return 0;
        }
        inode_index++;
        remaining_file_size -= real_block_size;
    }
    printf("COPIED \"%s\" FROM DISK AS \"%s\"\n", source, destination);
    free(buffer);
    fclose(dest);
    return 1;
}

#endif