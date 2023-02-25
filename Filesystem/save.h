#ifndef FILESYSTEM_SAVE
#define FILESYSTEM_SAVE
#include "block.h"
#define ALLOCATION_MAP_BLOCK 2
#define SUPER_INODE_BLOCK 1
#define SUPER_BLOCK_BLOCK 0

int save_allocation_map(FILE *fd, struct super_block *sb, struct allocation_map *map) {
    const char zero = 0;
    fseek(fd, ALLOCATION_MAP_BLOCK*BLOCK_SIZE, SEEK_SET);
    size_t r = fwrite(map->map, sizeof(int), sb->bitmap_vectors, fd);
    if (r != sb->bitmap_vectors) return 0;
    fseek(fd, BLOCK_SIZE * sb->private_blocks_count - 1, SEEK_SET);
    r = fwrite(&zero, sizeof(char), 1, fd);
    return r == 1;
}

int save_super_block(FILE* fd, struct super_block* sb) {
 return save_block(fd, sb, SUPER_BLOCK_BLOCK, sizeof(struct super_block));
}

int save_super_inode(FILE* fd, struct super_inode* si) {
    return save_block(fd, si, SUPER_INODE_BLOCK, sizeof(struct super_inode));
}

int save_private_blocks(FILE *fd) {
    if(!save_super_block(fd, &fs_super_block)) return 0;
    if(!save_super_inode(fd, &fs_super_inode)) return 0;
    if (!save_allocation_map(fd, &fs_super_block, &fs_allocation_map)) return 0;
    return 1;
}

#endif //FILESYSTEM_SAVE
