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