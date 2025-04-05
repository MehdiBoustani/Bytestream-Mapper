#include <stdio.h>
#include <stdlib.h>
#include "src/magic.h"

/**
 * Correctness test used to check results of basic and edge operations 
 * 1) Check if program outputs same results as figure 1 in the assignment statement
 * 2) Check correctness for an empty MAGIC (bytes should remain in same position)
 * 3) Check correct results for only add operations
 * 4) Check correct results for only remove operations
 * 5) Check correct results for sequential add/remove operations
 * 6) Check some error cases (negative pos, or negative length of bytes)
*/

/* Test result tracking */
int tests_passed = 0;
int tests_failed = 0;

/* Helper functions */
void printTestResult(const char* testName, int actual, int expected) {
    if (actual == expected) {
        printf("✓ %s: PASSED (actual: %d, expected: %d)\n", testName, actual, expected);
        tests_passed++;
    } else {
        printf("✗ %s: FAILED (actual: %d, expected: %d)\n", testName, actual, expected);
        tests_failed++;
    }
}

void printSectionHeader(const char* title) {
    printf("\n====== %s ======\n", title);
}

/* Basic functionality tests */
void runBasicTests() {
    printSectionHeader("BASIC TESTS (Example of Figure 1)");
    
    // Initialize MAGIC
    MAGIC m = MAGICinit();
    if (!m) {
        printf("Failed to initialize MAGIC\n");
        return;
    }
    
    // Perform operations from Figure 1 in the project description
    MAGICremove(m, 3, 2);  // Remove 'd' and 'e'
    MAGICremove(m, 4, 3);  // Remove 'g', 'h', 'i'
    MAGICadd(m, 4, 2);     // Add 'R' and 'S'
    MAGICadd(m, 9, 3);     // Add 'T', 'U', 'V'
    
    // Test IN_OUT mappings
    printf("      Testing STREAM_IN_OUT mappings:\n");
    printTestResult("Map position 0", MAGICmap(m, STREAM_IN_OUT, 0), 0);
    printTestResult("Map position 1", MAGICmap(m, STREAM_IN_OUT, 1), 1);
    printTestResult("Map position 2", MAGICmap(m, STREAM_IN_OUT, 2), 2);
    printTestResult("Map position 3", MAGICmap(m, STREAM_IN_OUT, 3), -1);  // Removed
    printTestResult("Map position 4", MAGICmap(m, STREAM_IN_OUT, 4), -1);  // Removed
    printTestResult("Map position 5", MAGICmap(m, STREAM_IN_OUT, 5), 3);
    printTestResult("Map position 6", MAGICmap(m, STREAM_IN_OUT, 6), -1);  // Removed
    printTestResult("Map position 7", MAGICmap(m, STREAM_IN_OUT, 7), -1);  // Removed
    printTestResult("Map position 8", MAGICmap(m, STREAM_IN_OUT, 8), -1);  // Removed
    printTestResult("Map position 9", MAGICmap(m, STREAM_IN_OUT, 9), 6);
    printTestResult("Map position 10", MAGICmap(m, STREAM_IN_OUT, 10), 7);
    printTestResult("Map position 13", MAGICmap(m, STREAM_IN_OUT, 13), 13);
    
    // Test OUT_IN mappings
    printf("\n      Testing STREAM_OUT_IN mappings:\n");
    printTestResult("Map position 0", MAGICmap(m, STREAM_OUT_IN, 0), 0);
    printTestResult("Map position 1", MAGICmap(m, STREAM_OUT_IN, 1), 1);
    printTestResult("Map position 2", MAGICmap(m, STREAM_OUT_IN, 2), 2);
    printTestResult("Map position 3", MAGICmap(m, STREAM_OUT_IN, 3), 5);
    printTestResult("Map position 4", MAGICmap(m, STREAM_OUT_IN, 4), -1);  // Added
    printTestResult("Map position 5", MAGICmap(m, STREAM_OUT_IN, 5), -1);  // Added
    printTestResult("Map position 6", MAGICmap(m, STREAM_OUT_IN, 6), 9);
    printTestResult("Map position 7", MAGICmap(m, STREAM_OUT_IN, 7), 10);
    printTestResult("Map position 8", MAGICmap(m, STREAM_OUT_IN, 8), 11);  // Added
    printTestResult("Map position 9", MAGICmap(m, STREAM_OUT_IN, 9), -1);  // Added
    printTestResult("Map position 10", MAGICmap(m, STREAM_OUT_IN, 10), -1); // Added
    printTestResult("Map position 11", MAGICmap(m, STREAM_OUT_IN, 11), -1);
    
    // Clean up
    MAGICdestroy(m);
}

/* Edge case tests */
void runEdgeCaseTests() {
    printSectionHeader("EDGE CASE TESTS");
    
    // Test 1: Empty MAGIC instance
    MAGIC m1 = MAGICinit();
    // Bytes should remain in same position
    printTestResult("Empty MAGIC IN_OUT", MAGICmap(m1, STREAM_IN_OUT, 5), 5);
    printTestResult("Empty MAGIC OUT_IN", MAGICmap(m1, STREAM_OUT_IN, 5), 5);
    MAGICdestroy(m1);
    
    // Test 2: Only ADD operations
    MAGIC m2 = MAGICinit();
    MAGICadd(m2, 0, 2);  // Add at beginning
    MAGICadd(m2, 5, 3);  // Add in middle
    MAGICadd(m2, 10, 1); // Add at end
    
    printf("\nTesting STREAM_IN_OUT mappings:\n");
    printTestResult("Only ADDs position 0", MAGICmap(m2, STREAM_IN_OUT, 0), 2);
    printTestResult("Only ADDs position 4", MAGICmap(m2, STREAM_IN_OUT, 4), 9);
    printTestResult("Only ADDs position 5", MAGICmap(m2, STREAM_IN_OUT, 5), 11);
    printTestResult("Only ADDs position 10", MAGICmap(m2, STREAM_IN_OUT, 10), 16);
    
    printf("\nTesting STREAM_OUT_IN mappings:\n");
    printTestResult("Only ADDs OUT_IN position 0", MAGICmap(m2, STREAM_OUT_IN, 0), -1);  
    printTestResult("Only ADDs OUT_IN position 3", MAGICmap(m2, STREAM_OUT_IN, 3), 1);  
    printTestResult("Only ADDs OUT_IN position 14", MAGICmap(m2, STREAM_OUT_IN, 14), 8);
    MAGICdestroy(m2);
    
    printf("\n");
    // Test 3: Only REMOVE operations
    MAGIC m3 = MAGICinit();
    MAGICremove(m3, 0, 2);  // Remove from beginning
    MAGICremove(m3, 5, 3);  // Remove from middle
    MAGICremove(m3, 10, 1); // Remove from end
    

    printf("\nTesting STREAM_IN_OUT mappings:\n");
    printTestResult("Only REMOVEs position 0", MAGICmap(m3, STREAM_IN_OUT, 0), -1); // REMOVED
    printTestResult("Only REMOVEs position 3", MAGICmap(m3, STREAM_IN_OUT, 3), 1);
    printTestResult("Only REMOVEs position 5", MAGICmap(m3, STREAM_IN_OUT, 5), 3);
    printTestResult("Only REMOVEs position 9", MAGICmap(m3, STREAM_IN_OUT, 9), -1);  // REMOVED
    printTestResult("Only REMOVEs position 10", MAGICmap(m3, STREAM_IN_OUT, 10), 5);
    printTestResult("Only REMOVEs position 12", MAGICmap(m3, STREAM_IN_OUT, 12), 7);
    
    printf("\nTesting STREAM_OUT_IN mappings:\n");
    printTestResult("Only REMOVEs OUT_IN position 0", MAGICmap(m3, STREAM_OUT_IN, 0), 2);
    printTestResult("Only REMOVEs OUT_IN position 3", MAGICmap(m3, STREAM_OUT_IN, 3), 5);
    printTestResult("Only REMOVEs OUT_IN position 5", MAGICmap(m3, STREAM_OUT_IN, 5), 10);
    MAGICdestroy(m3);
    
    printSectionHeader("Overlapping operations");
    // Test 4: Overlapping operations
    MAGIC m4 = MAGICinit();
    MAGICadd(m4, 5, 3);    // Add 3 bytes at position 5
    MAGICremove(m4, 4, 6); // Remove 6 bytes starting at position 4, overlapping the added bytes
    
    printTestResult("Overlapping position 3", MAGICmap(m4, STREAM_IN_OUT, 3), 3);
    printTestResult("Overlapping position 4", MAGICmap(m4, STREAM_IN_OUT, 4), -1); // REMOVED
    printTestResult("Overlapping position 7", MAGICmap(m4, STREAM_IN_OUT, 7), 4);
    printTestResult("Overlapping position 10", MAGICmap(m4, STREAM_IN_OUT, 10), 7);
    MAGICdestroy(m4);
}

/* Sequential operations tests */
void runSequentialTests() {
    printSectionHeader("SEQUENTIAL OPERATIONS TESTS");
    
    MAGIC m = MAGICinit();
    
    // Create a series of sequential operations and verify each step
    
    // Initial state: "abcdefghijklm"
    MAGICremove(m, 2, 2);
    printTestResult("Step 1: Map 1", MAGICmap(m, STREAM_IN_OUT, 1), 1);
    printTestResult("Step 1: Map 2", MAGICmap(m, STREAM_IN_OUT, 2), -1);
    printTestResult("Step 1: Map 4", MAGICmap(m, STREAM_IN_OUT, 4), 2);
    
    // After add(5, 2): "abefg*hijklm" (* = added bytes)
    MAGICadd(m, 5, 2);
    printTestResult("Step 2: Map 1", MAGICmap(m, STREAM_IN_OUT, 1), 1);
    printTestResult("Step 2: Map 5", MAGICmap(m, STREAM_IN_OUT, 5), 3);
    printTestResult("Step 2: Map 6", MAGICmap(m, STREAM_IN_OUT, 6), 4);
    
    // After remove(3, 4): "abe**klm" (where ** = remaining added bytes)
    MAGICremove(m, 3, 4);
    printTestResult("Step 3: Map 2", MAGICmap(m, STREAM_IN_OUT, 2), -1);
    printTestResult("Step 3: Map 3", MAGICmap(m, STREAM_IN_OUT, 3), -1);
    printTestResult("Step 3: Map 7", MAGICmap(m, STREAM_IN_OUT, 7), 3);
    
    // After add(1, 3): "a***be**klm"
    MAGICadd(m, 1, 3);
    printTestResult("Step 4: Map 0", MAGICmap(m, STREAM_IN_OUT, 0), 0);
    printTestResult("Step 4: Map 1", MAGICmap(m, STREAM_IN_OUT, 1), 4);
    printTestResult("Step 4: Map 2", MAGICmap(m, STREAM_IN_OUT, 2), -1);
    
    // Test OUT_IN mapping for the final state
    printTestResult("Final OUT_IN 0", MAGICmap(m, STREAM_OUT_IN, 0), 0);
    printTestResult("Final OUT_IN 1", MAGICmap(m, STREAM_OUT_IN, 1), -1);
    printTestResult("Final OUT_IN 4", MAGICmap(m, STREAM_OUT_IN, 4), 1);
    printTestResult("Final OUT_IN 5", MAGICmap(m, STREAM_OUT_IN, 5), 4);
    
    MAGICdestroy(m);
}

void runErrorHandlingTests() {
    printSectionHeader("ERROR HANDLING TESTS");
    
    MAGIC m = MAGICinit();
    
    // Test invalid positions
    MAGICadd(m, -1, 5);  
    printTestResult("Add with negative position", MAGICmap(m, STREAM_IN_OUT, 0), 0);
    
    // Test invalid lengths
    MAGICadd(m, 10, -5);  
    printTestResult("Add with negative length", MAGICmap(m, STREAM_IN_OUT, 10), 10);
    
    MAGICdestroy(m);
}

int main() {
    printf("Starting tests for MAGIC ADT implementation...\n");
    
    runBasicTests();
    runEdgeCaseTests();
    runSequentialTests();
    runErrorHandlingTests();
    
    // Print summary
    printf("\n==== TEST SUMMARY ====\n");
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("Total tests: %d\n", tests_passed + tests_failed);
    printf("Success rate: %.2f%%\n", 100.0 * tests_passed / (tests_passed + tests_failed));
    
    return tests_failed > 0 ? 1 : 0;
}
