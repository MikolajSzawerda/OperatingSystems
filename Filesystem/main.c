#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "filesystem.h"

#define CREATE_DISK 0
#define COPY_TO_VDISK 1
#define COPY_FROM_VDISK 2
#define SHOW_CATALOG 3
#define DELETE_FILE 4
#define DELETE_VDISK 5
#define SHOW_VDISK_MAP 6
#define LOAD_VDISK 7
#define EXIT 8

struct super_block fs_super_block;
struct allocation_map fs_allocation_map;
struct super_inode fs_super_inode;
FILE *fd;

int convert_action_to_int(char *action) {
    if (!strcmp(action, "create_disk")) return CREATE_DISK;
    if (!strcmp(action, "copy_to_vdisk")) return COPY_TO_VDISK;
    if (!strcmp(action, "copy_from_vdisk")) return COPY_FROM_VDISK;
    if (!strcmp(action, "show_catalog")) return SHOW_CATALOG;
    if (!strcmp(action, "delete_file")) return DELETE_FILE;
    if (!strcmp(action, "delete_vdisk")) return DELETE_VDISK;
    if (!strcmp(action, "show_vdisk_map")) return SHOW_VDISK_MAP;
    if (!strcmp(action, "load_vdisk")) return LOAD_VDISK;
    if (!strcmp(action, "exit")) return EXIT;
    return -1;
}

void file_to_vdisk(char *file_name, char *destination) {
    if (!copy_file_to_vdisk(fd, file_name, destination)) {
        printf("ERROR COPING FILE \"%s\" AS \"%s\" TO VIRTUAL DISK\n", file_name, destination);
    }
}

void file_out_of_disk(char *file_name) {
    if (!remove_file(fd, file_name)) {
        printf("ERROR REMOVING FILE \"%s\"\n", file_name);
    }
}

void file_to_disk(char *file_name, char *destination) {
    if (!copy_file_to_disk(fd, file_name, destination)) {
        printf("ERROR COPING FILE \"%s\" TO MACHINE DISK\n", file_name);
    }
}

void disk_load(FILE *handle) {
    load_disk(handle);
    printf("DISK LOADED SUCCESSFULLY\n");
}


int main() {
    char action[20];
    while (scanf("%s", action) != EOF) {
        int act = convert_action_to_int(action);
        if (act == CREATE_DISK) {
            size_t disk_size;
            char disk_name[20];
            scanf("%zd %s", &disk_size, disk_name);
            if (!create_disk(disk_size, disk_name)) {
                printf("ERROR WHEN CREATING DISK");
                exit(-1);
            }
        } else if (act == LOAD_VDISK) {
            char disk_name[20];
            scanf("%s", disk_name);
            fd = fopen(disk_name, "r+");
            disk_load(fd);
        } else if (act == COPY_TO_VDISK) {
            assert(fd != NULL && "DISK SHOULD BE LOADED");
            char source_name[30];
            char destination_name[MAX_FILE_NAME_LENGTH];
            scanf("%s %s", source_name, destination_name);
            file_to_vdisk(source_name, destination_name);
        } else if (act == SHOW_VDISK_MAP) {
            assert(fd != NULL && "DISK SHOULD BE LOADED");
            print_filesystem_map(fd, 1);
        } else if (act == COPY_FROM_VDISK) {
            assert(fd != NULL && "DISK SHOULD BE LOADED");
            char source_name[MAX_FILE_NAME_LENGTH];
            char destination_name[30];
            scanf("%s %s", source_name, destination_name);
            file_to_disk(source_name, destination_name);
        } else if (act == SHOW_CATALOG) {
            assert(fd != NULL && "DISK SHOULD BE LOADED");
            print_catalog(fd);
        } else if (act == DELETE_FILE) {
            assert(fd != NULL && "DISK SHOULD BE LOADED");
            char file_name[MAX_FILE_NAME_LENGTH];
            scanf("%s", file_name);
            file_out_of_disk(file_name);
        } else if (act == DELETE_VDISK) {
            assert(fd != NULL && "DISK SHOULD BE LOADED");
            char disk_name[20];
            scanf("%s", disk_name);
            remove_vdisk(fd, disk_name);
            printf("DISK REMOVED SUCCESSFULLY\n");
            fd = NULL;
        } else if (act == EXIT) {
            break;
        } else {
            printf("UNKNOWN \"%s\" ACTION\n", action);
        }
    }
    if (fd != NULL) fclose(fd);
    free(fs_allocation_map.map);
    free(fs_super_inode.free_catalog_records);
    return 0;
}
