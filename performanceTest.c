#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include "src/magic.h"

/**
 * Performance test using random operations
 * 1) Check program performance over increasing number of operations/maps: small, medium, large
 * 2) Check Stress test performance and robustness under load
 * 3) Check Spike test in order to test sudden increasing load  
 * 4) Check Volume test for large size bytestream 
*/

void printSectionHeader(const char* title) {
    printf("\n====== %s ======\n", title);
}

/* Performance test: small workload with random operations */
void runSmallPerformanceTest() {
    printSectionHeader("SMALL PERFORMANCE TEST");
    
    int nbOperations = 100;
    int nbMaps = 1000;

    MAGIC m = MAGICinit();
    clock_t start, end;
    double cpu_time_used;
    
    // Add 100 operations with random positions
    start = clock();
    for (int i = 0; i < nbOperations; i++) {
        int pos = rand() % 1000;  // Random position between 0 and 999
        int len = (rand() % 5) + 1;  // Random length between 1 and 5
        
        if (i % 2 == 0) {
            MAGICadd(m, pos, len);
        } else {
            MAGICremove(m, pos, len);
        }
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time to add %d random operations: %f seconds\n", nbOperations, cpu_time_used);
    
    // Perform 1000 random map operations (IN_OUT)
    start = clock();
    for (int i = 0; i < nbMaps; i++) {
        int pos = rand() % 1000;  // Random position between 0 and 999
        MAGICmap(m, STREAM_IN_OUT, pos);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time for %d random IN_OUT map operations: %f seconds\n",nbMaps, cpu_time_used);
    
    // Perform 1000 random map operations (OUT_IN)
    start = clock();
    for (int i = 0; i < nbMaps; i++) {
        int pos = rand() % 1000;  // Random position between 0 and 999
        MAGICmap(m, STREAM_OUT_IN, pos);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time for %d random OUT_IN map operations: %f seconds\n", nbMaps, cpu_time_used);
    
    MAGICdestroy(m);
}


/* Performance test: medium workload with random operations */
void runMediumPerformanceTest() {

     printSectionHeader("MEDIUM PERFORMANCE TEST");
    
    int nbOperations = 1000;
    int nbMaps = 10000;

    MAGIC m = MAGICinit();
    clock_t start, end;
    double cpu_time_used;
    
    // Add 100 operations with random positions
    start = clock();
    for (int i = 0; i < nbOperations; i++) {
        int pos = rand() % 1000;  // Random position between 0 and 999
        int len = (rand() % 5) + 1;  // Random length between 1 and 5
        
        if (i % 2 == 0) {
            MAGICadd(m, pos, len);
        } else {
            MAGICremove(m, pos, len);
        }
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time to add %d random operations: %f seconds\n", nbOperations, cpu_time_used);
    
    // Perform 1000 random map operations (IN_OUT)
    start = clock();
    for (int i = 0; i < nbMaps; i++) {
        int pos = rand() % 1000;  // Random position between 0 and 999
        MAGICmap(m, STREAM_IN_OUT, pos);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time for %d random IN_OUT map operations: %f seconds\n",nbMaps, cpu_time_used);
    
    // Perform 1000 random map operations (OUT_IN)
    start = clock();
    for (int i = 0; i < nbMaps; i++) {
        int pos = rand() % 1000;  // Random position between 0 and 999
        MAGICmap(m, STREAM_OUT_IN, pos);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time for %d random OUT_IN map operations: %f seconds\n", nbMaps, cpu_time_used);
    
    MAGICdestroy(m);
}


/* Performance test: large workload */
void runLargePerformanceTest() {
    printSectionHeader("LARGE PERFORMANCE TEST");
    int nbOperations = 10000;   // 10,000 operations
    int nbMaps = 100000;        // 100,000 maps
    int positionRange = 100000; // Position range 0-99,999 (in the bytestream)
    
    MAGIC m = MAGICinit();
    clock_t start, end;
    double cpu_time_used;
    
    // Add operations with random positions
    printf("Adding %d random operations across position range 0-%d...\n", 
           nbOperations, positionRange-1);
    start = clock();
    for (int i = 0; i < nbOperations; i++) {
        int pos = rand() % positionRange; // Random position across full range
        int len = (rand() % 10) + 1;      // Random length between 1 and 10
        
        if (i % 2 == 0) {
            MAGICadd(m, pos, len);
        } else {
            MAGICremove(m, pos, len);
        }
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time to add %d random operations: %f seconds\n", nbOperations, cpu_time_used);
    
    // Perform random map operations (IN_OUT)
    printf("\nPerforming %d random IN_OUT mapping operations...\n", nbMaps);
    start = clock();
    for (int i = 0; i < nbMaps; i++) {
        int pos = rand() % positionRange; // Random position across full range
        MAGICmap(m, STREAM_IN_OUT, pos);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time for %d random IN_OUT map operations: %f seconds\n", nbMaps, cpu_time_used);
    printf("Average time per IN_OUT map: %f ms\n", (cpu_time_used * 1000) / nbMaps);
    
    // Perform random map operations (OUT_IN)
    printf("\nPerforming %d random OUT_IN mapping operations...\n", nbMaps);
    start = clock();
    for (int i = 0; i < nbMaps; i++) {
        int pos = rand() % positionRange; // Random position across full range
        MAGICmap(m, STREAM_OUT_IN, pos);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time for %d random OUT_IN map operations: %f seconds\n", nbMaps, cpu_time_used);
    printf("Average time per OUT_IN map: %f ms\n", (cpu_time_used * 1000) / nbMaps);
    
    MAGICdestroy(m);
}

/* Stress test: large workload for robustness */
void runStressTest() {
    printSectionHeader("STRESS TEST");
    
    
    MAGIC m = MAGICinit();
    clock_t start, end;
    double cpu_time_used;
    
    // Add a large number of operations with bursts of operations in specific areas
    start = clock();
    
    // First burst: 10,000 operations clustered in the first 10,000 positions
    for (int i = 0; i < 10000; i++) {
        int pos = rand() % 10000;
        int len = (rand() % 5) + 1;
        
        if (rand() % 2 == 0) {
            MAGICadd(m, pos, len);
        } else {
            MAGICremove(m, pos, len);
        }
    }
    
    // Second burst: 10,000 operations clustered in the middle range (40,000-60,000)
    for (int i = 0; i < 10000; i++) {
        int pos = 40000 + (rand() % 20000);
        int len = (rand() % 5) + 1;
        
        if (rand() % 2 == 0) {
            MAGICadd(m, pos, len);
        } else {
            MAGICremove(m, pos, len);
        }
    }
    
    // Third burst: 10,000 operations clustered at the end (90,000-100,000)
    for (int i = 0; i < 10000; i++) {
        int pos = 90000 + (rand() % 10000);
        int len = (rand() % 5) + 1;
        
        if (rand() % 2 == 0) {
            MAGICadd(m, pos, len);
        } else {
            MAGICremove(m, pos, len);
        }
    }
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time to add 30,000 clustered operations: %f seconds\n", cpu_time_used);
    
    // Now test mapping performance for each cluster
    start = clock();
    for (int i = 0; i < 10000; i++) {
        int pos = rand() % 10000;  // First cluster
        MAGICmap(m, STREAM_IN_OUT, pos);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time for 10,000 map operations in first cluster: %f seconds\n", cpu_time_used);
    
    start = clock();
    for (int i = 0; i < 10000; i++) {
        int pos = 40000 + (rand() % 20000);  // Middle cluster
        MAGICmap(m, STREAM_IN_OUT, pos);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time for 10,000 map operations in middle cluster: %f seconds\n", cpu_time_used);
    
    start = clock();
    for (int i = 0; i < 10000; i++) {
        int pos = 90000 + (rand() % 10000);  // End cluster
        MAGICmap(m, STREAM_IN_OUT, pos);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time for 10,000 map operations in end cluster: %f seconds\n", cpu_time_used);
    
    MAGICdestroy(m);
}

/*
 * Spike Test: Tests the system's ability to handle sudden large increases in load
 */
void runSpikeTest() {
    printSectionHeader("SPIKE TEST");
    
    MAGIC m = MAGICinit();
    if (m == NULL) {
        printf("Failed to initialize MAGIC\n");
        return;
    }
    
    clock_t start, end;
    double cpu_time_used;
    
    // First, create a baseline of operations
    printf("Establishing baseline with 1,000 operations...\n");
    start = clock();
    for (int i = 0; i < 1000; i++) {
        int pos = rand() % 10000;
        int len = (rand() % 10) + 1;
        
        if (i % 2 == 0) {
            MAGICadd(m, pos, len);
        } else {
            MAGICremove(m, pos, len);
        }
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Baseline created in %f seconds\n", cpu_time_used);
    
    // Measure baseline mapping performance
    start = clock();
    for (int i = 0; i < 10000; i++) {
        int pos = rand() % 10000;
        MAGICmap(m, STREAM_IN_OUT, pos);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Baseline mapping: 10,000 operations in %f seconds (%f ms/map)\n", 
           cpu_time_used, (cpu_time_used * 1000) / 10000);
    
    // Now simulate a spike with a large burst of operations
    printf("\nSimulating spike with 20,000 rapid operations...\n");
    start = clock();
    for (int i = 0; i < 20000; i++) {
        int pos = rand() % 10000;
        int len = (rand() % 10) + 1;
        
        if (i % 2 == 0) {
            MAGICadd(m, pos, len);
        } else {
            MAGICremove(m, pos, len);
        }
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Spike operations completed in %f seconds\n", cpu_time_used);
    
    // Measure post-spike mapping performance
    start = clock();
    for (int i = 0; i < 10000; i++) {
        int pos = rand() % 10000;
        MAGICmap(m, STREAM_IN_OUT, pos);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Post-spike mapping: 10,000 operations in %f seconds (%f ms/map)\n", 
           cpu_time_used, (cpu_time_used * 1000) / 10000);
    
    MAGICdestroy(m);
}

/*
 * Volume Test: Tests the system with large volume of data and position values
 */
void runVolumeTest() {
    printSectionHeader("VOLUME TEST");
    printf("Testing with large position values (simulating large streams)\n");
    
    MAGIC m = MAGICinit();
    if (m == NULL) {
        printf("Failed to initialize MAGIC\n");
        return;
    }
    
    clock_t start, end;
    double cpu_time_used;
    
    // Large positions to simulate multi-gigabyte streams
    // We'll spread operations across the entire integer range
    unsigned int maxPos = INT_MAX / 2;  // Half of max int to avoid overflow
    
    printf("Position range: 0 to %u\n", maxPos);
    
    // Add operations at various points in the large stream
    printf("Adding operations with large position values...\n");
    start = clock();
    
    // Create operations in different regions of the large position space
    for (int i = 0; i < 5000; i++) {
        unsigned int region = rand() % 5;  // select random of 5 different regions
        unsigned int pos;
        
        // Each region covers 20% of the position space
        switch (region) {
            case 0:
                pos = rand() % (maxPos / 5);  // First 20%
                break;
            case 1:
                pos = (maxPos / 5) + (rand() % (maxPos / 5));  // 20-40%
                break;
            case 2:
                pos = (2 * maxPos / 5) + (rand() % (maxPos / 5));  // 40-60%
                break;
            case 3:
                pos = (3 * maxPos / 5) + (rand() % (maxPos / 5));  // 60-80%
                break;
            case 4:
                pos = (4 * maxPos / 5) + (rand() % (maxPos / 5));  // 80-100%
                break;
            default:
                pos = rand() % maxPos;
                break;
        }
        
        int len = (rand() % 1000) + 1;  // Length between 1 and 1000
        
        if (i % 2 == 0) {
            MAGICadd(m, pos, len);
        } else {
            MAGICremove(m, pos, len);
        }
    }
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time to add 5,000 operations with large positions: %f seconds\n", cpu_time_used);
    
    // Test mapping across the position space
    printf("Testing mapping with large position values...\n");
    start = clock();
    
    for (int i = 0; i < 10000; i++) {
        unsigned int region = rand() % 5;  // 5 different regions
        unsigned int pos;
        
        // Select a position from one of the 5 regions
        switch (region) {
            case 0:
                pos = rand() % (maxPos / 5);
                break;
            case 1:
                pos = (maxPos / 5) + (rand() % (maxPos / 5));
                break;
            case 2:
                pos = (2 * maxPos / 5) + (rand() % (maxPos / 5));
                break;
            case 3:
                pos = (3 * maxPos / 5) + (rand() % (maxPos / 5));
                break;
            case 4:
                pos = (4 * maxPos / 5) + (rand() % (maxPos / 5));
                break;
            default:
                pos = rand() % maxPos;
                break;
        }
        
        // testing IN_OUT 
        MAGICmap(m, STREAM_IN_OUT, pos);
        
    }
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time for 10,000 mappings with large positions: %f seconds\n", cpu_time_used);
    
    MAGICdestroy(m);
}

int main() {
    srand(time(NULL));  // Initialize random seed once at program start
    
    printf("Starting performance tests with random operations\n");
    
    runSmallPerformanceTest();
    runMediumPerformanceTest();
    runLargePerformanceTest();

    runStressTest();
    runSpikeTest();    
    runVolumeTest();
}