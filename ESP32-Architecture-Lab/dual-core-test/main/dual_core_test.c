#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <esp_system.h>
#include <esp_timer.h>

// Inter-core communication
static QueueHandle_t core_queue;
static SemaphoreHandle_t print_mutex;

// Performance counters
static volatile uint32_t core0_counter = 0;
static volatile uint32_t core1_counter = 0;
static volatile uint64_t core0_total_time = 0;
static volatile uint64_t core1_total_time = 0;

// Communication stats
static volatile uint32_t messages_sent = 0;
static volatile uint32_t messages_received = 0;
static volatile uint32_t queue_overflow_count = 0;
static volatile uint64_t total_latency = 0;

// Message structure for inter-core communication
typedef struct {
    uint32_t sender_core;
    uint32_t message_id;
    uint64_t timestamp;
    char data[32];
} core_message_t;

void safe_printf(const char* format, ...) {
    xSemaphoreTake(print_mutex, portMAX_DELAY);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    xSemaphoreGive(print_mutex);
}

// Task for Core 0 (PRO_CPU)
void core0_task(void *parameter) {
    core_message_t message;
    uint64_t task_start = esp_timer_get_time();
    
    safe_printf("Core 0 Task Started (PRO_CPU)\n");
    
    for(int i = 0; i < 120; i++) {
        uint64_t iteration_start = esp_timer_get_time();
        
        // Simulate protocol processing work
        uint32_t checksum = 0;
        for(int j = 0; j < 1000; j++) {
            checksum += j * 997;
        }
        
        // Send message to Core 1 every 10 iterations
        if(i % 10 == 0) {
            message.sender_core = 0;
            message.message_id = i;
            message.timestamp = esp_timer_get_time();
            snprintf(message.data, sizeof(message.data), "Message #%d from Core 0", i);

            if(xQueueSend(core_queue, &message, pdMS_TO_TICKS(5)) == pdTRUE) {
                messages_sent++;
                safe_printf("Core 0: Sent '%s'\n", message.data);
            } else {
                queue_overflow_count++;
                safe_printf("Core 0: Queue Full! (overflow count=%lu)\n", queue_overflow_count);
                vTaskDelay(pdMS_TO_TICKS(10)); // back-pressure
            }
        }
        
        core0_counter++;
        uint64_t iteration_time = esp_timer_get_time() - iteration_start;
        core0_total_time += iteration_time;
        
        vTaskDelay(pdMS_TO_TICKS(20));  // ลด delay จาก 50ms → 20ms เพื่อกดดัน consumer
    }
    
    uint64_t task_end = esp_timer_get_time();
    safe_printf("Core 0 Task Completed in %llu ms\n", (task_end - task_start) / 1000);
    vTaskDelete(NULL);
}

// Task for Core 1 (APP_CPU)
void core1_task(void *parameter) {
    core_message_t received_message;
    uint64_t task_start = esp_timer_get_time();
    
    safe_printf("Core 1 Task Started (APP_CPU)\n");
    
    for(int i = 0; i < 150; i++) {
        uint64_t iteration_start = esp_timer_get_time();
        
        // Simulate application processing work
        float result = 0.0;
        for(int j = 0; j < 500; j++) {
            result += sqrtf(j * 1.7f);
        }
        
        // Check for messages from Core 0
        if(xQueueReceive(core_queue, &received_message, pdMS_TO_TICKS(10)) == pdTRUE) {
            uint64_t latency = esp_timer_get_time() - received_message.timestamp;
            messages_received++;
            total_latency += latency;
            safe_printf("Core 1: Received '%s' (latency: %llu μs)\n", 
                       received_message.data, latency);
        }
        
        core1_counter++;
        uint64_t iteration_time = esp_timer_get_time() - iteration_start;
        core1_total_time += iteration_time;
        
        vTaskDelay(pdMS_TO_TICKS(30));
    }
    
    uint64_t task_end = esp_timer_get_time();
    safe_printf("Core 1 Task Completed in %llu ms\n", (task_end - task_start) / 1000);
    vTaskDelete(NULL);
}

// Monitoring task (can run on either core)
void monitor_task(void *parameter) {
    TickType_t last_wake_time = xTaskGetTickCount();
    
    for(int i = 0; i < 10; i++) {
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(1000));  // Every 1 second
        
        safe_printf("\n=== System Monitor ===\n");
        safe_printf("Core 0: %lu iterations, Avg time: %llu μs\n", 
                   core0_counter, core0_counter > 0 ? core0_total_time / core0_counter : 0);
        safe_printf("Core 1: %lu iterations, Avg time: %llu μs\n", 
                   core1_counter, core1_counter > 0 ? core1_total_time / core1_counter : 0);
        safe_printf("Messages Sent: %lu, Received: %lu\n", messages_sent, messages_received);
        safe_printf("Queue Overflow Count: %lu\n", queue_overflow_count);
        safe_printf("=======================\n");
    }
    
    vTaskDelete(NULL);
}

void app_main() {
    printf("ESP32 Dual-Core Architecture Analysis\n");
    printf("=====================================\n");
    
    core_queue = xQueueCreate(10, sizeof(core_message_t));
    print_mutex = xSemaphoreCreateMutex();
    
    if(core_queue == NULL || print_mutex == NULL) {
        printf("Failed to create synchronization objects!\n");
        return;
    }
    
    printf("Creating tasks...\n");
    
    xTaskCreatePinnedToCore(core0_task, "Core0Task", 4096, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(core1_task, "Core1Task", 4096, NULL, 2, NULL, 1);
    xTaskCreate(monitor_task, "MonitorTask", 2048, NULL, 1, NULL);
    
    vTaskDelay(pdMS_TO_TICKS(12000));  // Wait 12 seconds
    
    printf("\n=== Final Results ===\n");
    printf("Core 0 total iterations: %lu\n", core0_counter);
    printf("Core 1 total iterations: %lu\n", core1_counter);
    printf("Core 0 average time per iteration: %llu μs\n", 
           core0_counter > 0 ? core0_total_time / core0_counter : 0);
    printf("Core 1 average time per iteration: %llu μs\n", 
           core1_counter > 0 ? core1_total_time / core1_counter : 0);
    
    printf("Messages Sent: %lu\n", messages_sent);
    printf("Messages Received: %lu\n", messages_received);
    printf("Average Latency: %llu μs\n", 
           messages_received > 0 ? total_latency / messages_received : 0);
    printf("Queue Overflow Count: %lu\n", queue_overflow_count);
    
    printf("\nDual-core analysis complete!\n");
}
