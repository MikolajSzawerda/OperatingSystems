#ifndef FILESYSTEM_BLOCK
#define FILESYSTEM_BLOCK

#include "type.h"

int save_block(FILE *fd, void *ptr, uint block_number, size_t data_size) {
    fseek(fd, block_number * BLOCK_SIZE, SEEK_SET);
    size_t r = fwrite(ptr, data_size, 1, fd);
    return r == 1;
}

int get_block(FILE *fd, void *ptr, uint block_number, int data_size) {
    fseek(fd, BLOCK_SIZE * block_number, SEEK_SET);
    size_t r = fread(ptr, data_size, 1, fd);
    return r == 1;
}

int allocate_n_blocks(int *buffer, uint n) {
    if(n>fs_super_block.available_blocks) return 0;
    int val;
    for (int i = 0, j = 0; i < fs_super_block.bitmap_vectors; i++) {
        while ((val = ffs(fs_allocation_map.map[i])) != 0 && j < n) {
            fs_allocation_map.map[i] ^= 1 << (val - 1);
            buffer[j++] = ((int) sizeof(int) * i * 8 + val - 1);
        }
        if (j >= n){
            fs_super_block.available_blocks-=n;
            return 1;
        }
    }
    return 0;
}


int deallocate_block(uint number) {
    int vector_num = floor((double) number / (sizeof(int)*8));
    int offset = number % ((int) sizeof(int)*8);
    fs_allocation_map.map[vector_num] ^= 1 << offset;
    fs_super_block.available_blocks++;
    return 1;
}

#endif