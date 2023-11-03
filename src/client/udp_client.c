#include "udp_client.h"

FILE* open_file(char* file_name){
    FILE *fp;
    if((fp = fopen(file_name,"r"))==NULL){
        printf("error in open file");
        exit(1);
    }
    return fp;
}

int main(int argc,char** argv){
    
}