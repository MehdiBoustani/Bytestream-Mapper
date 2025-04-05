#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include "src/magic.h"

/**
 * Performance test: Endurance test
 * Checks how MAGIC ADT performs over time as more operations are added 
*/

void printSectionHeader(const char* title) {
    printf("\n====== %s ======\n", title);
}

/* 
 * Endurance Test: Tests the MAGIC ADT's ability to handle expected load
 * over a longer period of time, checking for performance degradation
 */
void runEnduranceTest(int durationSeconds) {
    printSectionHeader("ENDURANCE TEST");
    printf("Running endurance test for %d seconds...\n", durationSeconds);
    
    MAGIC m = MAGICinit();
    if (m == NULL) {
        printf("Failed to initialize MAGIC\n");
        return;
    }
    
    clock_t overall_start, overall_end;
    clock_t iteration_start, iteration_end;
    double cpu_time_used;
    int iteration = 0;
    int totalOperations = 0;
    int totalMappings = 0;
    
    overall_start = clock();
    time_t end_time = time(NULL) + durationSeconds;
    
    // Run operations for the specified duration
    while (time(NULL) < end_time) {
        iteration++;
        printf("Iteration %d - ", iteration);
        
        // Perform a batch of operations
        int batchSize = 1000;
        iteration_start = clock();
        
        for (int i = 0; i < batchSize; i++) {
            int pos = rand() % 100000;
            int len = (rand() % 10) + 1;
            
            if (rand() % 2 == 0) {
                MAGICadd(m, pos, len);
            } else {
                MAGICremove(m, pos, len);
            }
        }
        
        totalOperations += batchSize;
        iteration_end = clock();
        cpu_time_used = ((double) (iteration_end - iteration_start)) / CLOCKS_PER_SEC;
        printf("Added %d operations in %f seconds ", batchSize, cpu_time_used);
        
        // Perform a batch of mappings
        int mapBatchSize = 10000;
        iteration_start = clock();
        
        for (int i = 0; i < mapBatchSize; i++) {
            int pos = rand() % 100000;
            MAGICmap(m, STREAM_IN_OUT, pos);
        }
        
        totalMappings += mapBatchSize;
        iteration_end = clock();
        cpu_time_used = ((double) (iteration_end - iteration_start)) / CLOCKS_PER_SEC;
        printf("| Mapped %d positions in %f seconds | Avg map time: %f ms\n", 
               mapBatchSize, cpu_time_used, (cpu_time_used * 1000) / mapBatchSize);
    }
    
    overall_end = clock();
    cpu_time_used = ((double) (overall_end - overall_start)) / CLOCKS_PER_SEC;
    
    printf("\nEndurance Test Complete:\n");
    printf("Total operations: %d\n", totalOperations);
    printf("Total mappings: %d\n", totalMappings);
    printf("Total time: %f seconds\n", cpu_time_used);
    
    MAGICdestroy(m);
}

int main() {
    srand(time(NULL));  // Initialize random seed once at program start
    
    
    // Run the endurance test 
    // Adjust the duration as needed - currently set to 30 seconds
    runEnduranceTest(30);
}