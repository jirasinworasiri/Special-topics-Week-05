#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_timer.h>
#include <esp_heap_caps.h>

#define ARRAY_SIZE 4096
#define ITERATIONS 100
#define TEST_RUNS 5

// Test arrays in different memory locations
static uint32_t sram_array[ARRAY_SIZE];
static uint32_t *psram_array = NULL;

// Performance measurement functions
uint64_t measure_sequential_access(uint32_t *array, const char* memory_type) {
    uint64_t start_time = esp_timer_get_time();
    uint32_t sum = 0;
    
    for(int run = 0; run < TEST_RUNS; run++) {
        for(int iter = 0; iter < ITERATIONS; iter++) {
            for(int i = 0; i < ARRAY_SIZE; i++) {
                sum += array[i];
            }
        }
    }
    
    uint64_t end_time = esp_timer_get_time();
    uint64_t duration = end_time - start_time;
    
    printf("%s Sequential Access: %llu μs (sum=%lu)\n", memory_type, duration, (unsigned long)sum);
    return duration;
}

uint64_t measure_random_access(uint32_t *array, const char* memory_type) {
    uint64_t start_time = esp_timer_get_time();
    uint32_t sum = 0;
    
    for(int run = 0; run < TEST_RUNS; run++) {
        for(int iter = 0; iter < ITERATIONS; iter++) {
            for(int i = 0; i < ARRAY_SIZE; i++) {
                // Pseudo-random index to break cache locality
                int index = (i * 2654435761U) % ARRAY_SIZE;
                sum += array[index];
            }
        }
    }
    
    uint64_t end_time = esp_timer_get_time();
    uint64_t duration = end_time - start_time;
    
    printf("%s Random Access: %llu μs (sum=%lu)\n", memory_type, duration, (unsigned long)sum);
    return duration;
}

uint64_t measure_stride_access(uint32_t *array, int stride, const char* test_name) {
    uint64_t start_time = esp_timer_get_time();
    uint32_t sum = 0;
    
    for(int run = 0; run < TEST_RUNS; run++) {
        for(int iter = 0; iter < ITERATIONS; iter++) {
            for(int i = 0; i < ARRAY_SIZE; i += stride) {
                sum += array[i];
            }
        }
    }
    
    uint64_t end_time = esp_timer_get_time();
    uint64_t duration = end_time - start_time;
    
    printf("%s (stride %d): %llu μs (sum=%lu)\n", test_name, stride, duration, (unsigned long)sum);
    return duration;
}

void initialize_arrays() {
    printf("Initializing test arrays...\n");
    
    // Initialize SRAM array
    for(int i = 0; i < ARRAY_SIZE; i++) {
        sram_array[i] = i * 7 + 13;  // Some pattern
    }
    
    // Try to allocate PSRAM array (if available)
    psram_array = heap_caps_malloc(ARRAY_SIZE * sizeof(uint32_t), MALLOC_CAP_SPIRAM);
    if(psram_array) {
        printf("PSRAM array allocated successfully\n");
        for(int i = 0; i < ARRAY_SIZE; i++) {
            psram_array[i] = i * 7 + 13;
        }
    } else {
        printf("PSRAM not available, using internal memory\n");
        psram_array = heap_caps_malloc(ARRAY_SIZE * sizeof(uint32_t), MALLOC_CAP_INTERNAL);
        if(psram_array) {
            for(int i = 0; i < ARRAY_SIZE; i++) {
                psram_array[i] = i * 7 + 13;
            }
        }
    }
}

void app_main() {
    printf("ESP32 Cache Performance Analysis\n");
    printf("================================\n");
    
    printf("Array size: %d elements (%d KB)\n", ARRAY_SIZE, (ARRAY_SIZE * 4) / 1024);
    printf("Iterations per test: %d\n", ITERATIONS);
    printf("Test runs: %d\n\n", TEST_RUNS);
    
    initialize_arrays();
    
    // Test 1: Sequential vs Random Access (SRAM)
    printf("\n=== Test 1: Sequential vs Random Access (Internal SRAM) ===\n");
    uint64_t sram_sequential = measure_sequential_access(sram_array, "SRAM");
    uint64_t sram_random = measure_random_access(sram_array, "SRAM");
    
    double sram_ratio = (double)sram_random / sram_sequential;
    printf("SRAM Performance Ratio (Random/Sequential): %.2fx\n", sram_ratio);
    
    // Test 2: External Memory (if available)
    if(psram_array) {
        printf("\n=== Test 2: External Memory Access ===\n");
        uint64_t psram_sequential = measure_sequential_access(psram_array, "External");
        uint64_t psram_random = measure_random_access(psram_array, "External");
        
        double psram_ratio = (double)psram_random / psram_sequential;
        printf("External Memory Performance Ratio: %.2fx\n", psram_ratio);
        
        printf("\nMemory Type Comparison (Sequential Access):\n");
        double memory_ratio = (double)psram_sequential / sram_sequential;
        printf("External/Internal Speed Ratio: %.2fx\n", memory_ratio);
    }
    
    // Test 3: Different Stride Patterns
    printf("\n=== Test 3: Stride Access Patterns ===\n");
    uint64_t stride1 = measure_stride_access(sram_array, 1, "Stride 1");
    uint64_t stride2 = measure_stride_access(sram_array, 2, "Stride 2");
    uint64_t stride4 = measure_stride_access(sram_array, 4, "Stride 4");
    uint64_t stride8 = measure_stride_access(sram_array, 8, "Stride 8");
    uint64_t stride16 = measure_stride_access(sram_array, 16, "Stride 16");
    
    printf("\nStride Analysis:\n");
    printf("Stride 2/1 ratio: %.2fx\n", (double)stride2/stride1);
    printf("Stride 4/1 ratio: %.2fx\n", (double)stride4/stride1);
    printf("Stride 8/1 ratio: %.2fx\n", (double)stride8/stride1);
    printf("Stride 16/1 ratio: %.2fx\n", (double)stride16/stride1);
    
    if(psram_array) {
        free(psram_array);
    }
    
    printf("\nCache performance analysis complete!\n");
}