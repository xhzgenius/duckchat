#include "csapp.h"
#include <time.h>
#define MAXNAME 50
#define MAXINF 502
#define FRONT 0
#define REAR 1

struct information{
    int connfd;
    int pos;
};

/*
 * Some useful functions --XHZGenius
 */

void strlwr(char *str){
    int i = 0;
    while(str[i]){
        if('A' <= str[i] && str[i] <= 'Z'){
            str[i] = str[i] - ('A' - 'a');
        }
        i++;
    }
}

void gettime(char *timestr){
    time_t now;
    struct tm *t;
    time(&now);
    t = gmtime(&now);
    char tmp[MAXINF];
    sprintf(tmp, "at %.2d:%.2d:%.2d, in %d-%d-%d", 8 + t->tm_hour, t->tm_min, t->tm_sec, 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday);
    strcpy(timestr, tmp);
}