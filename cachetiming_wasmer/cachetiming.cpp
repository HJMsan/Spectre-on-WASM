#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctime>

#define CACHE_FLUSH_STRIDE 4096
//#define CACHE_FLUSH_STRIDE 128
#define NLOOPS 5
char array[256 * CACHE_FLUSH_STRIDE];
int count = 0;
clock_t average = 0;

void cachetiming(int index){
    int i, j, mix_i, min;
    char *addr;
    char junk;
    clock_t start, end;
    clock_t result[256] = {0};
    for(j = 0;j < NLOOPS;j++){
        usleep(3000);
        junk = array[index * CACHE_FLUSH_STRIDE];
        for(i = 0;i < 256;i++){
            mix_i = ((i * 167) + 13) & 255;
            addr = &array[mix_i * CACHE_FLUSH_STRIDE];

            start = clock();
            junk = *addr;
            end = clock();
            result[mix_i] += end - start;
        }
    }
    //puts("Show results.");
    min = 0;
    for(i = 0;i < 256;i++){
        //printf("index %d: %lld.\n", i, result[i]);
        if(result[min] >= result[i])min = i;
    }
    //puts("");
    printf("target: %d\t Answer: %d\n", index, min);
    if(index == min){
        count++;
        average += result[min];
    }
}


int main(void){
    for(int i = 0;i < 256;i++)array[i * CACHE_FLUSH_STRIDE] = i;
    for(int i = 0;i < 256;i++)cachetiming(i);
    //printf("count: %d\naverage: %lld\n", count, average/500);
    return 0;
}
