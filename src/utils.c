#include "utils.h"


bool is_little_edian(){
    int x = 1;
    char *c = (char*)&x;
    if (*c == 1){
        return true;
    }else{
        return false;
    }
}

FILE* open_file(char* file_name){
    FILE *fp;
    if((fp = fopen(file_name,"r"))==NULL){
        printf("error in open file");
        exit(1);
    }
    return fp;
}