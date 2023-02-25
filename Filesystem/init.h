#ifndef FILESYSTEM_INIT
#define FILESYSTEM_INIT
#include "file.h"



int create_disk(size_t disk_size, const char *disk_name) {
    uint available_blocks = floor((double) (disk_size) / BLOCK_SIZE);
    uint num_of_bits_vector = ceil((double) (available_blocks) / sizeof(int));
    uint size_of_allocation_map = ceil((double) (available_blocks + sizeof(size_t)) / BLOCK_SIZE);
    uint max_file_size = MAX_INODE_DATA*BLOCK_SIZE;
    uint private_blocks = size_of_allocation_map + 2;
    if(private_blocks>available_blocks) return 0;
    printf("Available blocks: %d\n", available_blocks);
    printf("Max file count: %lu\n", MAX_FILE_RECORDS * MAX_CATALOG_RECORDS);
    printf("Max file size: %d\n", max_file_size);
    printf("Max data blocks per file: %d\n", MAX_INODE_DATA);
    printf("Max catalog descriptors: %d\n", MAX_CATALOG_RECORDS);
    printf("Max files in catalog descriptors: %lu\n", MAX_FILE_RECORDS);

    FILE *fd = fopen(disk_name, "w+");
    if (!fd) exit(-1);

    struct super_block sb = {disk_size, available_blocks,
                             size_of_allocation_map, num_of_bits_vector, private_blocks,
                             max_file_size, 0, MAX_FILE_RECORDS, available_blocks};
    fs_super_block = sb;

    struct super_inode super_node;
    strcpy(super_node.catalog_name, "main");
    super_node.free_catalog_records_count = -1;
    for (int i = 0; i < MAX_CATALOG_RECORDS; i++) super_node.catalog_records[i] = -1;
    fs_super_inode = super_node;

    int *map = (int *) calloc(num_of_bits_vector, sizeof(int));
    memset(map, 0xFF, num_of_bits_vector);
    struct allocation_map am = {map};
    fs_allocation_map = am;
    int *dummy_buffer = (int *) calloc(private_blocks, sizeof(int));
    allocate_n_blocks(dummy_buffer, private_blocks);
    free(dummy_buffer);
    if (!save_private_blocks(fd)) {
        free(map);
        fclose(fd);
        exit(-1);
    }
    free(map);
    fclose(fd);
    printf("DISK CREATED SUCCESSFULLY!\n");
    return 1;
}


int load_super_block(struct super_block *sb, FILE *fd) {
    fseek(fd, 0, SEEK_SET);
    size_t r = fread(sb, sizeof(struct super_block), 1, fd);
    return r == 1;
}

int load_super_inode(struct super_inode *sb, FILE *fd) {
    fseek(fd, BLOCK_SIZE, SEEK_SET);
    size_t r = fread(sb, sizeof(struct super_inode), 1, fd);
    if(r){
        sb->free_catalog_records_count = -1;
        sb->free_catalog_records = (uint*)calloc(MAX_CATALOG_RECORDS, sizeof(uint));
        int index=0;
        struct catalog_records records;
        while (index < MAX_CATALOG_RECORDS && fs_super_inode.catalog_records[index] != -1){
            if(get_block(fd, &records, fs_super_inode.catalog_records[index], sizeof(struct catalog_records))){
                if(can_add_new_file(&records)) sb->free_catalog_records[++sb->free_catalog_records_count]=index;
                index++;
            }
        }
    }
    return r == 1;
}

int load_allocation_map(struct super_block *sb, struct allocation_map *am, FILE *fd) {
    fseek(fd, 2 * BLOCK_SIZE, SEEK_SET);
    int *map = (int *) calloc(sb->bitmap_vectors, sizeof(int));
    size_t r = fread(map, sizeof(int), sb->bitmap_vectors, fd);
    am->map = map;
    return r == sb->bitmap_vectors;
}



void load_disk(FILE *fd) {
    if (!load_super_block(&fs_super_block, fd)) {
        fclose(fd);
        exit(-1);
    }
    if (!load_super_inode(&fs_super_inode, fd)) {
        fclose(fd);
        exit(-1);
    }
    if (!load_allocation_map(&fs_super_block, &fs_allocation_map, fd)) {
        free(fs_allocation_map.map);
        free(fs_super_inode.free_catalog_records);
        fclose(fd);
        exit(-1);
    }

}

int remove_vdisk(FILE* fd, char* disk_name){
    fclose(fd);
    return !remove(disk_name);
}

#endif