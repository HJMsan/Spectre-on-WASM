#include <stdio.h>
#include <stdlib.h>
#include <emscripten.h>
#include <emscripten/wasm_worker.h>

#define CACHE_FLUSH_ITERATIONS 4096
#define CACHE_FLUSH_STRIDE 4096
#define NLOOPS 1500
#define REPEATS 5
char array1[CACHE_FLUSH_STRIDE * CACHE_FLUSH_ITERATIONS];
//char array2[256 * CACHE_FLUSH_STRIDE * NLOOPS];
char *array2;
int offset = 0;
/*
EM_JS(void, clflush, (), {

});
*/

int count0 = 0;
int count1 = 0;
double average[2] = {0};

void speculative(int x, char *assign){
    char junk;
    if (x < 0x10U){
        junk = assign[array1[x] * CACHE_FLUSH_STRIDE];
    }
}

void storecache(int caching){
    int j, junk;
    for (j = 0;j < NLOOPS;j++){
        junk = array2[caching * CACHE_FLUSH_STRIDE + 2*CACHE_FLUSH_STRIDE*j];
    }
}

double cachetiming(int caching){
    int i, j, k, junk, mix_i, min;
    char *addr;
    double start, end;
    double result[2] = {0};

    array2 = (char*)calloc(1, 2 * CACHE_FLUSH_STRIDE * NLOOPS);
    emscripten_run_script("clflush()");
    emscripten_run_script("triggerGC()");
    //emscripten_sleep(100);
    for (k = 0;k < REPEATS;k++){
        storecache(caching);
        for(i = 0;i < 2;i++){
            //mix_i = ((i * 167) + 13) & 255;
            mix_i = i;
            addr = &array2[mix_i*CACHE_FLUSH_STRIDE];
            //time to measurement
            start = emscripten_performance_now();
            for (j = 0;j < NLOOPS;j++){
                junk = *(addr + 2*CACHE_FLUSH_STRIDE*j);
            }
            end = emscripten_performance_now();
            result[mix_i] += end - start;
        }
    }
    free(array2);
    puts("Show results.");
    min = 0;
    for(i = 0;i < 2;i++){
        printf("index %d: %f.\n", i, result[i]);
        if (result[min] > result[i])min = i;
    }
    puts("");
    printf("minimal index: %d.\n", min);
    if (min == caching){
        if(min == 0)count0++;
        if(min == 1)count1++;
    }
    return result[caching];
}
void test(void){
    average[0] += cachetiming(0);
    average[1] += cachetiming(1);
}


int main(void){
    for(int i = 0;i < 250;i++){
        test();
    }
    average[0] /= 250;
    average[1] /= 250;
    printf("average[0]: %lf\naverage[1]: %lf\n", average[0], average[1]);
    printf("count0: %d\n", count0);
    printf("count1: %d\n", count1);
    return 0;
}

