#include <stdio.h>
#include <string.h>
#include <esp_system.h>
#include <esp_heap_caps.h>

// Global variables in different memory sections
// ใช้ DRAM_ATTR แทน .dram
static char sram_buffer[1024] DRAM_ATTR;
static const char flash_string[] = "Hello from Flash Memory!";
static char *heap_ptr;

// Function to display memory information
void print_memory_info() {
    printf("\n=== ESP32 Memory Layout Analysis ===\n");
    
    // Stack variables (in SRAM)
    int stack_var = 42;
    printf("Stack variable address: 0x%08lx\n", (unsigned long)&stack_var);
    
    // Global SRAM buffer
    printf("SRAM buffer address:    0x%08lx\n", (unsigned long)sram_buffer);
    
    // Flash constant string
    printf("Flash string address:   0x%08lx\n", (unsigned long)flash_string);
    
    // Heap allocation
    heap_ptr = malloc(512);
    printf("Heap allocation:        0x%08lx\n", (unsigned long)heap_ptr);
    
    // Heap information
    printf("\n=== Heap Information ===\n");
    printf("Free heap size:         %lu bytes\n", (unsigned long)esp_get_free_heap_size());
    printf("Min free heap size:     %lu bytes\n", (unsigned long)esp_get_minimum_free_heap_size());
    printf("Largest free block:     %lu bytes\n", (unsigned long)heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT));
    
    // Memory usage by type
    printf("\n=== Memory Usage by Type ===\n");
    printf("Internal SRAM:          %lu bytes\n", (unsigned long)heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    printf("SPI RAM (if available): %lu bytes\n", (unsigned long)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    printf("DMA capable memory:     %lu bytes\n", (unsigned long)heap_caps_get_free_size(MALLOC_CAP_DMA));
    
    free(heap_ptr);
}

void app_main() {
    printf("ESP32 Memory Architecture Analysis\n");
    printf("==================================\n");
    
    // Test memory operations
    strcpy(sram_buffer, "SRAM Test Data");
    printf("Flash string: %s\n", flash_string);
    printf("SRAM buffer: %s\n", sram_buffer);
    
    print_memory_info();
    
    printf("\nMemory analysis complete!\n");
}
