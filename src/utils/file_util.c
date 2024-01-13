
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include "file_util.h"
#include<libgen.h>

char *str_duplicate(const char * str){
    if(str == NULL){
        return NULL;
    }
    size_t len = strlen(str) + 1;
    char *new_str = malloc(len);
    if (new_str != NULL) {
        strcpy(new_str, str);
    }

    return new_str;
}



long get_file_size(FILE *file){
    long file_size = -1;
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return file_size;
}
char *add_extension(const char *file_name , const char *extension){
    char *new_file_name = malloc(strlen(file_name)+strlen(extension)+1);
    strcpy(new_file_name,file_name);
    strcat(new_file_name,extension);
    return new_file_name;
}


char *get_dirname(const char *path){
    char *_path = malloc(sizeof(char)*strlen(path)+1);
    strcpy(_path,path);
    char *dir = str_duplicate(dirname(_path));
    free(_path);
    return dir;
}

char *get_basename(const char *path){
    char *_path = malloc(sizeof(char)*strlen(path));
    strcpy(_path,path);
    char *base = str_duplicate(basename(_path));
    free(_path);
    return base;
}

int is_file(const char *const path){
    struct stat st;
    stat(path,&st);
    return S_ISREG(st.st_mode);
}

int is_dir(const char *const path){
    struct stat st;
    stat(path,&st);
    return S_ISDIR(st.st_mode);
}

int is_valid_path(const char *const path){
    if(access(path,F_OK)!=-1){
        return 1;
    }
    return 0;
}

