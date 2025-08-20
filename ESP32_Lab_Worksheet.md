# ใบงานทดลอง: การศึกษาสถาปัตยกรรม ESP32
## การทดลองด้วย Docker และ QEMU

**หัวข้อ**: การวิเคราะห์สถาปัตยกรรม ESP32 ระบบหน่วยความจำ และ Dual-Core CPU  
**รายวิชา**: 2568.01 APPLICATIONS OF MICROCONTROLLERS  
**สัปดาห์ที่**: 5  
**เวลา**: 3 ชั่วโมง  

---

## 📋 วัตถุประสงค์การทดลอง

เมื่อจบการทดลองนี้ นักศึกษาจะสามารถ:

1. **เข้าใจสถาปัตยกรรม ESP32** - ระบบ dual-core, memory mapping, cache system
2. **ใช้งาน Docker และ QEMU** - สำหรับการจำลองและวิเคราะห์ microcontroller
3. **วิเคราะห์ Memory Architecture** - การจัดเรียงหน่วยความจำและการเข้าถึง
4. **ประเมิน Cache Performance** - เปรียบเทียบ memory access patterns
5. **ศึกษา Dual-Core System** - การทำงานของ PRO_CPU และ APP_CPU
6. **ใช้เครื่องมือ Analysis** - objdump, QEMU monitor

---

## 🛠️ อุปกรณ์และเครื่องมือ

### Software Requirements
- **Docker Desktop** (ติดตั้งแล้ว)
- **VS Code** พร้อม Docker Extension
- **Git** สำหรับ clone repository
- **Web Browser** สำหรับดู documentation

### Hardware Requirements
- **Computer** พร้อม RAM อย่างน้อย 4 GB
- **Internet Connection** สำหรับ download Docker images
- **Storage Space** อย่างน้อย 2 GB ว่าง

---

## 📚 ความรู้เบื้องต้น

ก่อนเริ่มการทดลอง ให้นักศึกษาทบทวนความรู้เกี่ยวกับ:
- ESP32 Architecture (อ่านจากไฟล์ ESP32_Architecture.md)
- Docker containers และ basic commands
- Memory management concepts
- Command line interface (bash/cmd)

---

## 🔬 การทดลองที่ 1: การเตรียม Environment (ใช้เครื่องมือที่คุ้นเคย)

### วัตถุประสงค์
- ใช้ Docker และ ESP-IDF ที่คุ้นเคยจาก Lab4 
- เตรียม workspace สำหรับศึกษา ESP32 architecture
- ติดตั้งเครื่องมือง่ายๆ สำหรับการวิเคราะห์

### ขั้นตอนการทดลอง

#### **Step 1.1: คัดลอก docker-compose.yml จาก Lab4**

```bash
# สร้าง directory สำหรับ Lab5
mkdir ESP32-Architecture-Lab
cd ESP32-Architecture-Lab

# คัดลอก docker-compose.yml จาก Lab4 (ถ้ามี)
# หรือสร้างไฟล์ docker-compose.yml ใหม่
```

สร้างไฟล์ `docker-compose.yml` (เหมือนใน Lab4):

```yaml
version: '3.8'

services:
  esp32-dev:
    image: espressif/idf:latest
    container_name: esp32-lab5
    volumes:
      - .:/project
    working_dir: /project
    tty: true
    stdin_open: true
    environment:
      - IDF_PATH=/opt/esp/idf
    command: /bin/bash
    networks:
      - esp32-network

networks:
  esp32-network:
    driver: bridge
```

#### **Step 1.2: รัน Docker Environment (เหมือน Lab4)**

```bash
# รัน container (คำสั่งเดิมที่คุ้นเคย)
docker-compose up -d

# เข้า container (คำสั่งเดิมที่คุ้นเคย)
docker-compose exec esp32-dev bash
```

#### **Step 1.3: ตรวจสอบ ESP-IDF และติดตั้งเครื่องมือเพิ่มเติม**

```bash
# ใน container - ตรวจสอบ ESP-IDF (เหมือน Lab4)
echo $IDF_PATH
. $IDF_PATH/export.sh
idf.py --version
xtensa-esp32-elf-gcc --version

# ติดตั้งเครื่องมือง่ายๆ สำหรับ architecture analysis
apt-get update
apt-get install -y tree htop

# สร้าง directories สำหรับ Lab5
mkdir -p {memory-test,cache-test,dual-core-test,results}
```

#### **Step 1.4: ทดสอบว่า Environment พร้อม**

```bash
# ทดสอบ build ง่ายๆ (เหมือน Lab4)
echo '#include <stdio.h>
void app_main() {
    printf("ESP32 Architecture Lab Ready\\n");
}' > test.c

echo "=== Environment Ready for ESP32 Architecture Analysis ==="
echo "Docker: Working"
echo "ESP-IDF: Ready"
echo "Tools: tree, htop installed"
echo "Ready for Lab5 experiments!"
```

### 🔍 ความคุ้นเคยจาก Lab4

#### **สิ่งที่เหมือนกับ Lab4:**
- **docker-compose.yml** - ไฟล์เดิมที่ใช้ใน Lab4
- **คำสั่ง Docker** - `docker-compose up -d`, `docker-compose exec esp32-dev bash`
- **ESP-IDF environment** - เครื่องมือเดิมที่คุ้นเคยแล้ว
- **การ build โปรแกรม** - `idf.py build` เหมือนเดิม

#### **สิ่งที่เพิ่มเข้ามาใน Lab5:**
- **Directory ใหม่** สำหรับ architecture analysis
- **เครื่องมือง่ายๆ** tree, htop
- **Focus ใหม่** ESP32 architecture แทน arithmetic

**โตรงสร้าง Directory หลังจากสิ่นสุด Lab 5** 
```
ESP32-Architecture-Lab/          # โฟลเดอร์หลักของ Lab5
├── docker-compose.yml           # ไฟล์เดิมจาก Lab4 (คัดลอกมา)
├── memory-test/                 # การทดลองที่ 2: Memory Architecture
│   ├── CMakeLists.txt           # root project file
│   ├── main/                    # ESP-IDF main component
│   │   ├── memory_test.c        # source code สำหรับทดสอบ memory
│   │   └── CMakeLists.txt       # component file
│   └── build/                   # สร้างเมื่อ build (auto-generated)
├── cache-test/                  # การทดลองที่ 3: Cache Performance  
│   ├── CMakeLists.txt           # root project file
│   ├── main/                    # ESP-IDF main component
│   │   ├── cache_test.c         # source code สำหรับทดสอบ cache
│   │   └── CMakeLists.txt       # component file
│   └── build/                   # สร้างเมื่อ build (auto-generated)
├── dual-core-test/              # การทดลองที่ 4: Dual-Core Analysis
│   ├── CMakeLists.txt           # root project file
│   ├── main/                    # ESP-IDF main component
│   │   ├── dual_core_test.c     # source code สำหรับทดสอบ dual-core
│   │   └── CMakeLists.txt       # component file
│   └── build/                   # สร้างเมื่อ build (auto-generated)
└── results/                     # โฟลเดอร์เก็บผลการทดลอง
    ├── memory_analysis.txt      # ผลการวิเคราะห์ memory
    ├── cache_performance.txt    # ผลการทดสอบ cache
    ├── dual_core_results.txt    # ผลการทดสอบ dual-core
    └── analysis_report.md       # รายงานสรุปผลการทดลอง
```

### คำถามทบทวน

1. **Docker Commands**: คำสั่ง `docker-compose up -d` และ `docker-compose exec esp32-dev bash` ทำอะไร?
2. **ESP-IDF Tools**: เครื่องมือไหนจาก Lab4 ที่จะใช้ในการ build โปรแกรม ESP32?
3. **New Tools**: เครื่องมือใหม่ที่ติดตั้ง (tree, htop) ใช้ทำอะไร?
4. **Architecture Focus**: การศึกษา ESP32 architecture แตกต่างจากการทำ arithmetic ใน Lab4 อย่างไร?

### ผลลัพธ์ที่คาดหวัง
- [ ] สร้างโฟลเดอร์ ESP32-Architecture-Lab เรียบร้อย
- [ ] คัดลอกหรือสร้าง docker-compose.yml ได้สำเร็จ
- [ ] รัน Docker container ได้ปกติ (เหมือน Lab4)
- [ ] เข้า container และใช้ ESP-IDF ได้ (คุ้นเคยจาก Lab4)
- [ ] ติดตั้งเครื่องมือเพิ่มเติม (tree, htop) สำเร็จ
- [ ] สร้าง directories สำหรับการทดลอง architecture เรียบร้อย

### 🤖 ข้อดีของการใช้เครื่องมือที่คุ้นเคย

**เหตุผลที่ใช้ docker-compose.yml เหมือน Lab4:**
- **ไม่ต้องเรียนรู้ใหม่**: ใช้คำสั่งเดิมที่รู้จักแล้ว
- **ประหยัดเวลา**: ไม่เสียเวลาอธิบาย Docker setup ใหม่
- **โฟกัส ESP32**: เน้นที่เนื้อหา architecture ไม่ใช่เครื่องมือ
- **ความมั่นใจ**: ใช้สิ่งที่ work แล้วจาก Lab4

---

## 🔬 การทดลองที่ 2: การศึกษา Memory Architecture

### วัตถุประสงค์
- สร้างโปรแกรมทดสอบ memory layout
- วิเคราะห์การจัดเรียงหน่วยความจำใน ESP32
- เข้าใจ address mapping และ memory sections

### ขั้นตอนการทดลอง

#### **Step 2.1: สร้าง Memory Test Project**

```bash
# ใน container - เข้าไปใน memory-test directory
cd /project/memory-test

# สร้าง src directory และไฟล์ main
mkdir -p main
```

สร้างไฟล์ `main/memory_test.c`:

```c
#include <stdio.h>
#include <string.h>
#include <esp_system.h>
#include <esp_heap_caps.h>

// Global variables in different memory sections
static char sram_buffer[1024] __attribute__((section(".dram")));
static const char flash_string[] __attribute__((section(".rodata"))) = "Hello from Flash Memory!";
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
```

#### **Step 2.2: สร้าง CMake Configuration**

สร้างไฟล์ `CMakeLists.txt` (ใน memory-test directory):

```cmake
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(memory_test)
```

สร้างไฟล์ `main/CMakeLists.txt`:

```cmake
idf_component_register(SRCS "memory_test.c"
                    INCLUDE_DIRS ".")
```




#### **Step 2.3: Build และวิเคราะห์ Binary**

```bash
# Build project (คำสั่งเดิมที่คุ้นเคยจาก Lab4)
# อยู่ใน /project/memory-test directory แล้ว
idf.py build

# วิเคราะห์ binary file ด้วยเครื่องมือพื้นฐาน
echo "=== Binary Analysis ==="
xtensa-esp32-elf-size build/memory_test.elf
xtensa-esp32-elf-objdump -h build/memory_test.elf | head -20

# ดู memory sections สำคัญ
echo "=== Memory Sections ==="
xtensa-esp32-elf-objdump -t build/memory_test.elf | grep -E "(sram_buffer|flash_string|app_main)"
```

#### **Step 2.4: รัน Program ใน QEMU และดูผล**

```bash
# รันโปรแกรมใน QEMU emulator (ไม่ต้องใช้ hardware จริง)
idf.py qemu monitor


# หรือถ้าต้องการ build ใหม่ก่อนรัน
# idf.py build qemu monitor

# กด Ctrl+] เพื่อออกจาก monitor
# ดูผลลัพธ์ที่แสดง memory addresses
```




**🖥️ ผลลัพธ์ที่ควรเห็นใน QEMU:**
```
ESP32 Memory Architecture Analysis
==================================
Flash string: Hello from Flash Memory!
SRAM buffer: SRAM Test Data

=== ESP32 Memory Layout Analysis ===
Stack variable address: 0x3ffbxxxx
SRAM buffer address:    0x3ffcxxxx  
Flash string address:   0x400xxxxx
Heap allocation:        0x3ffcxxxx

=== Heap Information ===
Free heap size:         xxxxxx bytes
Min free heap size:     xxxxxx bytes
Largest free block:     xxxxxx bytes

=== Memory Usage by Type ===
Internal SRAM:          xxxxxx bytes
SPI RAM (if available): 0 bytes
DMA capable memory:     xxxxxx bytes

Memory analysis complete!
```

### การบันทึกผลการทดลอง 

**Table 2.1: Memory Address Analysis**

| Memory Section | Variable/Function | Address (ที่แสดงออกมา) | Memory Type |
|----------------|-------------------|----------------------|-------------|
| Stack | stack_var | 0x3ffb4550 | SRAM |
| Global SRAM | sram_buffer | 0x3ffb16ac | SRAM |
| Flash | flash_string | 0x3f407d08 | Flash |
| Heap | heap_ptr | 0x3ffb526c | SRAM |

**Table 2.2: Memory Usage Summary**

| Memory Type | Free Size (bytes) | Total Size (bytes) |
|-------------|-------------------|--------------------|
| Internal SRAM | 380136 | 520,192 |
| Flash Memory | 0 | varies |
| DMA Memory | 303088 | varies |

### คำถามวิเคราะห์ (ง่าย)

1. **Memory Types**: SRAM และ Flash Memory ใช้เก็บข้อมูลประเภทไหน?
SRAM: เก็บตัวแปรชั่วคราว เช่น stack, global variables, heap allocation
Flash Memory: เก็บข้อมูลถาวร เช่น program code, constant strings (const char[])

2. **Address Ranges**: ตัวแปรแต่ละประเภทอยู่ใน address range ไหน?
Stack / Global / Heap → เริ่มต้นที่ 0x3ffb.... หรือ 0x3ffc.... → เป็น Internal SRAM
Flash → เริ่มต้นที่ 0x3f40.... หรือ 0x400.... → อยู่ใน Flash/ROM

3. **Memory Usage**: ESP32 มี memory ทั้งหมดเท่าไร และใช้ไปเท่าไร?
Internal SRAM ทั้งหมดประมาณ 520 KB
Free เหลือให้ใช้งาน ~380 KB
DMA-capable memory เหลือ ~303 KB
Flash ไม่ได้โชว์ขนาดแน่นอน แต่เก็บ code และ string ได้ตามขนาดของ Flash chip (เช่น 4 MB, 8 MB บน ESP32 รุ่นต่าง ๆ)

---

## 🔬 การทดลองที่ 3: การศึกษา Cache Performance

### วัตถุประสงค์
- เปรียบเทียบ cache performance ระหว่าง sequential และ random access
- เข้าใจผลกระทบของ cache hit/miss ต่อประสิทธิภาพ
- วิเคราะห์ memory access patterns

### ขั้นตอนการทดลอง

#### **Step 3.1: สร้าง Cache Performance Test**

สร้างไฟล์ `main/cache_test.c`:

```c
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
```

**หมายเหตุ**
ในกรณีอยู่นอก docker สามารถใช้คำสั่งนี้
```bash
 
# build
  docker exec -it esp32-lab5 bash -c "source /opt/esp/idf/export.sh && cd cache-test && idf.py build"

 # จำลองการทำงานด้วย
   docker exec -it esp32-lab5 bash -c "source /opt/esp/idf/export.sh && cd cache-test && idf.py qemu"
```

### การบันทึกผลการทดลอง

**Table 3.1: Cache Performance Results**

| Test Type | Memory Type | Time (μs) | Ratio vs Sequential |
|-----------|-------------|-----------|-------------------|
| Sequential | Internal SRAM | 5222 | 1.00x |
| Random | Internal SRAM | 5190 | 0.99x |
| Sequential | External Memory | 4978 | 0.95x |
| Random | External Memory | 5050 | 0.97x |

**Table 3.2: Stride Access Performance**

| Stride Size | Time (μs) | Ratio vs Stride 1 |
|-------------|-----------|------------------|
| 1 | 5223 | 1.00x |
| 2 | 2559 | 0.49x |
| 4 | 1253 | 0.24x |
| 8 | 638 | 0.12x |
| 16 | 353 | 0.07x |

### คำถามวิเคราะห์

1. **Cache Efficiency**: ทำไม sequential access เร็วกว่า random access?
Sequential access เร็วกว่า random access เพราะ cache และ memory controller ทำงานได้ดีเมื่อเข้าถึงข้อมูลต่อเนื่อง → CPU สามารถ prefetch ข้อมูลล่วงหน้าได้
Random access ทำให้ cache miss มากขึ้น ต้องไปดึงข้อมูลใหม่จาก memory บ่อยกว่า

2. **Memory Hierarchy**: ความแตกต่างระหว่าง internal SRAM และ external memory คืออะไร?
Internal SRAM: ความเร็วสูงกว่า, latency ต่ำ, อยู่บนชิป ESP32 โดยตรง

External Memory (PSRAM/External RAM): ขนาดใหญ่กว่า แต่ latency สูงกว่า → เหมาะกับงานที่ต้องการพื้นที่มากแต่ไม่ต้องการความเร็วสูงสุด
ในการทดสอบนี้ External memory เร็วใกล้เคียงกับ Internal (เพราะ array ไม่ใหญ่มาก, caching มีผล)

3. **Stride Patterns**: stride size ส่งผลต่อ performance อย่างไร?
เมื่อ stride ขนาดใหญ่ขึ้น (2, 4, 8, 16) เวลาที่ใช้ลดลงมาก → เพราะเข้าถึงข้อมูลน้อยลง (กระโดดข้ามไปทีละหลาย element)
แต่อันนี้ไม่ได้แปลว่า memory เร็วขึ้นจริง ๆ → มันแค่ อ่านน้อยลง จึงใช้เวลาน้อยลง
ดังนั้น Stride 16 เร็วที่สุด แต่ใช้ข้อมูลไม่ครบทั้งหมด → แสดงว่า pattern ของการเข้าถึงข้อมูลมีผลมากต่อประสิทธิภาพ

---

## 🔬 การทดลองที่ 4: การศึกษา Dual-Core Architecture

### วัตถุประสงค์
- ทดสอบการทำงานของ PRO_CPU และ APP_CPU
- ศึกษา inter-core communication
- วิเคราะห์ task scheduling และ load balancing

### ขั้นตอนการทดลอง

#### **Step 4.1: สร้าง Dual-Core Test Program**

สร้างไฟล์ `main/dual_core_test.c`:

```c
#include <stdio.h>
#include <string.h>
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
    
    for(int i = 0; i < 100; i++) {
        uint64_t iteration_start = esp_timer_get_time();
        
        // Simulate protocol processing work
        uint32_t checksum = 0;
        for(int j = 0; j < 1000; j++) {
            checksum += j * 997;  // Some computation
        }
        
        // Send message to Core 1 every 10 iterations
        if(i % 10 == 0) {
            message.sender_core = 0;
            message.message_id = i;
            message.timestamp = esp_timer_get_time();
            snprintf(message.data, sizeof(message.data), "Hello from Core 0 #%d", i);
            
            if(xQueueSend(core_queue, &message, pdMS_TO_TICKS(100)) == pdTRUE) {
                safe_printf("Core 0: Sent message %d\n", i);
            }
        }
        
        core0_counter++;
        uint64_t iteration_time = esp_timer_get_time() - iteration_start;
        core0_total_time += iteration_time;
        
        vTaskDelay(pdMS_TO_TICKS(50));  // 50ms delay
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
            result += sqrt(j * 1.7f);  // Floating point computation
        }
        
        // Check for messages from Core 0
        if(xQueueReceive(core_queue, &received_message, pdMS_TO_TICKS(10)) == pdTRUE) {
            uint64_t latency = esp_timer_get_time() - received_message.timestamp;
            safe_printf("Core 1: Received '%s' (latency: %llu μs)\n", 
                       received_message.data, latency);
        }
        
        core1_counter++;
        uint64_t iteration_time = esp_timer_get_time() - iteration_start;
        core1_total_time += iteration_time;
        
        vTaskDelay(pdMS_TO_TICKS(30));  // 30ms delay
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
        
        safe_printf("\n=== Performance Monitor (Second %d) ===\n", i + 1);
        safe_printf("Core 0 iterations: %lu (avg: %llu μs)\n", 
                   core0_counter, core0_counter > 0 ? core0_total_time / core0_counter : 0);
        safe_printf("Core 1 iterations: %lu (avg: %llu μs)\n", 
                   core1_counter, core1_counter > 0 ? core1_total_time / core1_counter : 0);
        safe_printf("Queue messages waiting: %d\n", uxQueueMessagesWaiting(core_queue));
        safe_printf("Free heap: %d bytes\n", esp_get_free_heap_size());
    }
    
    vTaskDelete(NULL);
}

void app_main() {
    printf("ESP32 Dual-Core Architecture Analysis\n");
    printf("=====================================\n");
    
    // Create synchronization objects
    core_queue = xQueueCreate(10, sizeof(core_message_t));
    print_mutex = xSemaphoreCreateMutex();
    
    if(core_queue == NULL || print_mutex == NULL) {
        printf("Failed to create synchronization objects!\n");
        return;
    }
    
    printf("Creating tasks...\n");
    
    // Create tasks pinned to specific cores
    BaseType_t core0_result = xTaskCreatePinnedToCore(
        core0_task,           // Task function
        "Core0Task",          // Name
        4096,                 // Stack size
        NULL,                 // Parameters
        2,                    // Priority
        NULL,                 // Task handle
        0                     // Core 0 (PRO_CPU)
    );
    
    BaseType_t core1_result = xTaskCreatePinnedToCore(
        core1_task,           // Task function
        "Core1Task",          // Name
        4096,                 // Stack size
        NULL,                 // Parameters
        2,                    // Priority
        NULL,                 // Task handle
        1                     // Core 1 (APP_CPU)
    );
    
    BaseType_t monitor_result = xTaskCreate(
        monitor_task,         // Task function
        "MonitorTask",        // Name
        2048,                 // Stack size
        NULL,                 // Parameters
        1,                    // Priority (lower than worker tasks)
        NULL                  // Task handle
    );
    
    if(core0_result != pdPASS || core1_result != pdPASS || monitor_result != pdPASS) {
        printf("Failed to create tasks!\n");
        return;
    }
    
    printf("Tasks created successfully. Monitoring dual-core performance...\n\n");
    
    // Main task becomes idle
    vTaskDelay(pdMS_TO_TICKS(12000));  // Wait 12 seconds
    
    printf("\n=== Final Results ===\n");
    printf("Core 0 total iterations: %lu\n", core0_counter);
    printf("Core 1 total iterations: %lu\n", core1_counter);
    printf("Core 0 average time per iteration: %llu μs\n", 
           core0_counter > 0 ? core0_total_time / core0_counter : 0);
    printf("Core 1 average time per iteration: %llu μs\n", 
           core1_counter > 0 ? core1_total_time / core1_counter : 0);
    
    printf("\nDual-core analysis complete!\n");
}
```

**หมายเหตุ**
ในกรณีอยู่นอก docker สามารถใช้คำสั่งนี้
```bash
    docker exec -it esp32-lab5 bash -c "source /opt/esp/idf/export.sh && cd dual-core-test && idf.py build"
```

### การบันทึกผลการทดลอง

**Table 4.1: Dual-Core Performance Summary**

| Metric | Core 0 (PRO_CPU) | Core 1 (APP_CPU) |
|--------|-------------------|-------------------|
| Total Iterations | 51 | 25 |
| Average Time per Iteration (μs) | 75 | 9607 |
| Total Execution Time (ms) | 3.83 | 240.2 |
| Task Completion Rate | 100% | 49% |

**Table 4.2: Inter-Core Communication**

| Metric | Value |
|--------|-------|
| Messages Sent | 6 |
| Messages Received | 5 |
| Average Latency (μs) | 4.05 ms |
| Queue Overflow Count | 0 |

### คำถามวิเคราะห์

1. **Core Specialization**: จากผลการทดลอง core ไหนเหมาะกับงานประเภทใด?
Core 0 (PRO_CPU): มี iteration rate สูง (51 รอบ, avg 75 μs) → เหมาะกับงานที่ต้องทำซ้ำบ่อยและต้องการ low-latency / high-frequency เช่น real-time sensor polling, control loop
Core 1 (APP_CPU): ใช้เวลาเฉลี่ยต่อรอบสูง (9607 μs) → เหมาะกับงานที่ต้องการ ประมวลผลหนัก หรือ background task เช่น data logging, computation, networking

2. **Communication Overhead**: inter-core communication มี overhead เท่าไร?
ค่าเฉลี่ย latency ใน inter-core messaging ≈ 4.05 ms ต่อข้อความ
เมื่อเทียบกับ Core 0 ที่ทำงานเร็วมาก (75 μs/iteration) → การสื่อสารช้ามาก (ช้ากว่า ~50 เท่า)
แสดงว่าการสื่อสารระหว่าง cores เป็น คอขวด (bottleneck) ที่สำคัญ

3. **Load Balancing**: การกระจายงานระหว่าง cores มีประสิทธิภาพหรือไม่?
Core 0 ทำงานได้มากกว่าเกือบ 2 เท่า (51 vs 25 iterations)
Core 1 ทำงานช้ากว่าและติด latency สูง → การกระจายงานยัง ไม่สมดุล
ถ้าต้องการให้ parallel efficiency สูงขึ้น ควรใช้ task partitioning ที่เหมาะสม เช่น ให้ Core 0 handle fast I/O และ Core 1 handle heavy compute

---

## 📊 การวิเคราะห์และสรุปผล

### การสร้างรายงานผล

สร้างไฟล์ `reports/analysis_report.md` เพื่อสรุปผลการทดลอง:


### แบบฟอร์มส่งงาน

**ข้อมูลนักศึกษา:**
- ชื่อ: นางสาวจิรสิน วรศิริ
- รหัสนักศึกษา: 66030029
- วันที่ทำการทดลอง: 8/20/2025

**Checklist การทดลอง:**
- [ ] Environment setup สำเร็จ (ต่อเนื่องจากสัปดาห์ที่ 4)
- [ ] Memory architecture analysis เสร็จสมบูรณ์
- [ ] Cache performance testing เสร็จสมบูรณ์
- [ ] Dual-core analysis เสร็จสมบูรณ์
- [ ] รายงานผลการทดลองครบถ้วน

**คะแนนประเมิน:**
- การเตรียม Environment และ Continuity (15 คะแนน): _______
- Memory Analysis (30 คะแนน): _______
- Cache Performance (25 คะแนน): _______
- Dual-Core Analysis (25 คะแนน): _______
- รายงานและการเปรียบเทียบ (5 คะแนน): _______
- **รวม (100 คะแนน): _______**

**คำถามเพิ่มเติม:**
1. เปรียบเทียบประสบการณ์การใช้ Docker ในสัปดาห์นี้กับสัปดาห์ที่ 4:
   เข้าใจการใช้งานและการจัดการ container มากขึ้น ใช้งานคล่องกว่าเดิม สัปดาห์นี้ทำงานราบรื่นกว่าสัปดาห์ที่ 4 ที่ยังสับสนกับคำสั่งพื้นฐาน

2. สิ่งที่เรียนรู้เพิ่มเติมเกี่ยวกับ ESP32 architecture:
   รู้จักการทำงานแบบ dual-core (PRO_CPU และ APP_CPU), การแบ่งหน้าที่ของแต่ละ core และ overhead จาก inter-core communication

3. ความท้าทายที่พบในการทำ architecture analysis:
   การวัด latency และ load balancing ทำได้ยาก ต้องตีความผลการทดลองให้เชื่อมโยงกับการออกแบบระบบจริง

---

## 📚 References และ Additional Reading

1. **ESP32 Technical Reference Manual** - Espressif Systems
2. **ESP-IDF Programming Guide** - https://docs.espressif.com/projects/esp-idf/
3. **Docker Documentation** - https://docs.docker.com/
4. **QEMU ESP32 Documentation** - https://github.com/espressif/qemu
5. **FreeRTOS Documentation** - https://www.freertos.org/

---

## 💡 Tips และ Troubleshooting

### Common Issues
1. **Docker Memory Issues**: ให้เพิ่ม memory allocation สำหรับ Docker
2. **QEMU Startup Problems**: ตรวจสอบ binary format และ machine type
3. **Build Errors**: ให้ตรวจสอบ ESP-IDF version compatibility

### Performance Tips
1. ใช้ `-O2` optimization flag สำหรับ realistic performance
2. รัน test หลายครั้งเพื่อ average ผลลัพธ์
3. Monitor system resources ขณะทำการทดลอง

### Additional Experiments
1. ทดสอบ interrupt latency
2. วิเคราะห์ power consumption patterns
3. เปรียบเทียบกับ microcontrollers อื่น

---

**หมายเหตุ**: 
- การทดลองนี้ต่อเนื่องจากสัปดาห์ที่ 4 โดยเน้นการวิเคราะห์ architecture 
- นักศึกษาจะได้เปรียบเทียบประสบการณ์การใช้ Docker environment ระหว่างสัปดาห์
- Container name เปลี่ยนจาก `esp32-lab4` เป็น `esp32-lab5` เพื่อความชัดเจนในการจัดการ
