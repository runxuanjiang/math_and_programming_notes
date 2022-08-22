# Coures Notes from Systems Classes

## EECS 370 - Computer Architecture

### Important Concepts
* Two's Complement Binary Representation
    * Way of representing numbers in binary where the first bit in an n-bit number represents -2^(n-1) and all the other bits are treated normally.
    * Can represent numbers from -2^(n-1) to 2^(n-1) - 1.
    * Two's complement number can be negated by flipping all the bits and adding 1.
    * For extending a number to more bits, add all 0's in front if the number is positive, and all 1's if the number is negative.
* Memory layout of variables and classes/structs
    * In memory, each variable is aligned based on its size:
        * Char is byte aligned, short is half-word aligned (i.e. least significant bit of address is 0), int is word aligned (last 2 bits of address is 0)
        * So if each address is 1 byte, then Char is 1 byte, short is 2 bytes, int is 4 bytes, etc.
            * An n-byte variable has to be at a an address A such that A%n == 0.
        * Structs are aligned based on its largest field. Size of the overall struct is a multiple of the largest field.
        * So it makes sense to put largest things first in structs.
* Floating point arithmetic
    * Represent real numbers in binary by using scientific notation. Includes the following components/bits:
        * Sign bit (0 for positive, 1 is negative).
        * Significand (23 bits) - the most significant bits after the decimal point of the number (encoded in binary).
            * Since we are working in binary, the number (if non-zero) in scientific notation will always look like 1.xxxxx... so we can ignore the leading '1'.
            * The significand takes the first 23 bits after the decimal point.
        * Exponent (8 bits) - equals the 8 bits in binary minus 127 (so exponent ranges from -127 to 128).
            * Note that the exponent represents a binary exponent. So if the exponent is 4, then we multiply the significand (with a leading 1) by 2^4 = 16. This is equivalent to shifting the decimal 4 times to the right in the binary number.
    * Zero is represented by all zero values for everything.
    * Multiplication - add the exponents (after accounting for bias of 127) and multiply the significands.
    * Addition - must align the numbers so the exponents are the same.
    * Doubles use the same format but have more bits. Max for float is 3.4\*10^38. Max for double is 1.8\*10^308.


    
### ISAs and Assembly
* Basic Architecture Concepts (Von Neumann Architecture)
    * Programs (written in C++ for example) are compiled into assembly (each command corresponds to a machine code operation, 1-1 correspondence) which are translated into machine code
    * Data and instructions (code) are both stored in memory
        * Instructions stored sequentially
        * Program Counter (PC) points to where in instructions
        * Data can be stored in memory (RAM) and moved to registers (on CPU) which is used for doing actual operations.
    * RISC vs CISC - define machine language operations
        * RISC (Reduced Instruction Set) - lowest level of instructions (i.e., the base ISA) is simple, all instructions same length
        * CISC (Complex Instruction Set Computer) - instructions are more complex and can have different lengths, make machine language similar to high-level language
    * Encoding assembly
        * Each instruction is a certain number of bits, which is partitioned into the opcode (defines the operation, such as add, subtract, etc.), and the remaining bits define the input/destination registers.
        * There are special purpose registers, such as stack pointer, 0 value register, etc.
    * Memory architecture - see ![image](memory_architecture.png)

* Instruction Set Architecture (ISA)
    * Refers to the most fundamental level of instructions able to be computed by the processor.
    * Instructions, including the type of instruction, input registers, output registers, and literal values, are encoded into a string of bits.
        * For example, ARMV8 has 32 bit instructions. But each register in ARM stores 64 bits.
        * Instructions bits correspond to numbers in machine code.
        * Types of instructions include:
            * Arithmetic (add, subtract, multiply)
            * Data transfer - load and store data between memory and registers
            * Logical operators
            * Branches and conditional branches (jump to different places in code)
    * Data loading
        * In ARM, each data address contains 1 byte (8 bits) of data.
        * Since each register is 64 bits (8 bytes), loading data that is smaller than this size will cause the data to be sign-extended.
        * Big vs. Little Endian - refers to the order in which bits are loaded to the register
            * Little Endian - The earlier bits map to least significant bits in the register (reverse order in terms of memory address and reading bits left to right).
            * Big Endian - The earlier bits map to the most significant bits in the register (same order in terms of memory address).


* Addressing modes - defines how memory is accessed by the CPU
    * Direct addressing - get the memory location directly. Like array indexing: M[1500].
        * Not useful because each instruction usually only have a limited number of bits (3-5) for storing locations.
    * Indirect addressing - get the memory location which is stored at another memory location: M[M[1500]]
    * Register Indirect - get the memory location which is stored in a register
    * Base + displacement - base memory address is stored in memory, displacement is given in instruction
        * Useful for accessing class objects since we can calculate object sizes and know where each member is
    * PC-relative addressing - same as base_displacement except PC register is the base

* Function calls - how the processor manages function calls (high level function calls)
    * Passing in parameters
        * Parameters are stored in registers (if they fit) and the rest are stored in memory, in the **call stack**.
        * Call Stack
            * Section of memory used to store data for simulating function calls, including:
                * function parameters (that are not saved in registers)
                * Local variables
                * Return address and temporary storage when registers run out of space.
            * Sections of the call stack are called **stack frames**. They are allocated when functions are called, and deallocated when the function returns.
        * ARM memory map
            * Top is dynamic data, then static data, and finally text.
                * Dynamic data - top is stack, which contains stack frames.
                    * More stack frames are added downwards in the stack as functions are called
                    * Heap is at the bottom, where dynamically allocated data is stored (i.e. using malloc() and free())
                * Static - used for storing global variables
                * Text - used for storing instructions/code.
        * Calling Convention - How to make sure register data is consistent during function calls.
            * When a function calls another function, need to make sure that data stored in registers can be gotten back if the called function rewrites those registers.
                * The data will be saved in the stack frame.
            * Caller-save - Before calling a function, the higher-level function will save the registers used by its data that it will need to access after that function.
                * Downside is that it may save registers that won't be touched by the called function.
            * Callee-save - Before the function runs, it will save all the registers that it will use.
                * Downside is that it will save registers that weren't used by the higher level function.
            * In reality, both caller-save and callee-save are used. Specifically, some registers are set as caller-save registers, and some are set as callee-save. When data is stored in registers (or will be used by a function) they will be saved according to the type of register it is.
        * Stack frames
            * Each stack frame has a frame pointer, which points to the start of each stack frame.
            * Stack frames are connected by a linked list of frame pointers

* Compilers, Linkers, and Assemblers
    * After source code (i.e. C or C++ code) is written, it is compiled, then assembled, then linked, then loaded.
    * Compiler - converts .C file to .s (assembly code) file. This can be done in parallel for each source file.
    * Assembler - converts assembly code to .o (object) file. Can be done in parallel for each source file.
        * Object files contain machine code instructions and other data:
            * Header - contains sizes of each of the other sections of the file.
            * Text - machine code instructions
            * Data - List of initialized global and static data, their address, and their values.
            * Symbol table - maps symbol names to values (including unresolved labels). Used by linker file to bind values to names.
            * Relocation table - identifies instructions that use addresses of variables in symbol table, so that the linker can replace the address with the new updated address after linking.
    * Linker - combines multiple object files into an executable (i.e., a.out)
        * Takes text segments and data segments and combines them.
        * Resolvse references to variables (replaces addresses in relocation table)
    * Loader - used to run the executable. Loads the file code into memory and asks the OS to schedule it as a new process.

### Memory
* Cache
    * Memory stored in the processor (SRAM) is fast, but expensive. Cheap storage (like SSD, HDD) is slow. However, the processor only needs access to a small amount of memroy at one time, so to make things fast we can cache data on SRAM.
        * Use SRAM (fastest) for cache (ram attached to CPU).
        * Use DRAM (not as fast) for main memory (ram sticks).
        * Use flash/disk (non-volatile, slow) as virtual memory.
    * Temporal Locality - refers to the idea that a memory location accessed once will likely be accessed again, which is why we should Cache it.
        * Leads to least recently used (LRU) eviction scheme - when cache is full, evict the item that is least recently used.
    * Content addressable memory
        * On the CPU, the cache is implemented as content addressable memory (CAM), which acts like a hash map and maps memory addresses to data (stored in SRAM).
    * Spatial Locality - refers to the idea that we will likely use memory stored close to where we have previously accessed memory.
        * Block Size - instead of cacheing a single data address for each cache line, we can store a "block" of data representing a contiguous sequence of address data.
            * Also reduces number of cache keys to keep track of in CAM.
    * Dealing with writes
        * Write-through - when writing to cache and there is a hit, write to the cache and the memory.
        * Write-back - when writing to cache and there is a hit, write only to cache and set a "dirty" bit to true so that when the cache item is evicted it will know to update memory.
        * Allocate-on-write - when writing and there is a cache miss, update the value and bring it to cache.
        * No Allocate-on-write - on cache miss, write directly to memory.
    * Direct-mapped caches
        * Each line in the cache only maps to a specific section of memory.
            * Makes searching if item is in cache faster, since only have to search one line.
            * But evictions are inefficient since something that is not LRU may be evicted.
    * Set associative caches
        * Hybrid between direct-mapped cache and fully associative (not direct-mapped) cache.
        * Data is partitioned into sections, and each section gets some number of cache lines.
            * If each partition gets only one line, it's a direct-mapped cache.
            * If there is only one partition, it's a fully associative cache.
        * n-way set associative cache has n ways (lines) per partition.

* Virtual Memory
    * Solves two main problems with memory:
        * Main memory (DRAM) is limited and there may not be enough space for all programs at once or even just a single program.
        * Each program needs to keep its memory separate from other programs.
    * Main idea:
        * Each program has its own "virtual" address space. These address spaces each map to an actual address space ("physical" address) in main memory or disk.
            * The extra memory in disk for storing stuff that doesn't fit in memory is called the **swap partition**.
        * Each program therefore doesn't have to worry about accessing other program's memory.
        * Total amount of memory available does not limit programs (as data can be stored on disk).
    * Page table
        * Each program maintains a page table which maps virtual memory addresses to addresses in DRAM or disk.
        * Page tables are stored in memory and managed by the OS, which knows the physical address of the page table for each program.
        * Each page contains a chunk of memory (for example 4KB in x86). Each virtual address is thus composed of:
            * Virtual page number (which maps to some physical page number)
            * Page offset to determine which part of the page to address, which is the same for both the physical and virtual address (but the base is different)
        * If the page is not in DRAM, it is on disk. This is called a **page fault**.
            * DRAM can be used as a cache - on a page fault we evict the lru page (or use other eviction algorithm like policy-clock) and move the new page in.
    * Multi-level page tables
        * Almost no programs will use all the virtual pages, so we won't often need all the entries in the page table itself. The page table itself can be really big, which is a problem.
        * We can further partition pages in the page table into chunks, and map these chunks from a higher, less granular page table. We then only need to create a lower-level page table for chunks that are used (therefore we save memory for the unused chunks). This is called a **Hierarchical page table**.
        * The address will include a 1st level offset (selects the 2nd level page table), 2nd level offset (selects the page in physical memory), and page offset (selects the address in physical memory).
    * Translation look-aside buffer (TLB)
        * Finding an address in a multi-level page table now requires 2 or more page table lookups in memory. This can be slow.
        * TLB is a fast cache that maps virtual pages (including all offsets except the last page offset) to physical memory.
    * Caches for disk
        * To further optimize memory, we can have a data cache between the CPU and memory.
        * Physically-addressed cache - the cache maps physical addresses to data values.
            * This is slow because the CPU needs to look up the physical address first using the TLB and page table, but it simplifies obtaining the data.
        * Virtually-addressed cache - The cache maps virtual memory addresses to data (comes before TLB and page table)
            * Faster since TLB/page table only needs to be accessed on a cache miss.
            * Requires more complexity for determining which process the virtual page belongs to.
            * Cache aliasing - when multiple virtual pages (of the same process) map to a single physical page, which causes issues with updates.
                * Only causes a problem for virtually-addressed cache.
        * Virtually indexed physically tagged (VIPT) cache
            * Data cache now contains both the data and the physical address for each virtual address key.
            * CPU accesses both TLB and the data cache in parallel, which is faster than sequentially as in physically-addressed cache.
            * The physical page from TLB and physical address from data cache are compared to prevent aliasing.
        

## EECS 482 - Operating Systems

### Overview
* The OS abstracts away hardware elements for processes:
    * CPU -> Threads
    * Disk -> File System
    * RAM -> Virtual Memory

### CPU and Threads
* **Process** - OS abstraction for a unit of execution (also called a job or task). Can be seen as a program in execution. (The **program** is the instructions for the process).
    * The OS manages processes so that each process has its own memory (address space) and CPU.
    * Components:
        * Process ID (PID)
        * Set of **threads** executing instructions of the program. There can be multiple threads at once.
        * **Address Space** - the memory used by the program as it runs.
    * The address space contains both data shared by threads and per-thread data:
        * code/inputs, memory, files, and network connections are shared by threads.
        * Execution stack, program counter are thread-specific state.

* Synchronization Basics
    * An operation is **Atomic** if it happens in entirety or not at all.
        * Memory load/store is atomic in most computers.
    * **Critical Section** - section of code that needs to run atomically to produce correct behavior/output.
    * **Busy-waiting** - when one thread waits for another thread to exit a critical section, and does not free up CPU in the process.
        * Locks will usually prevent busy waiting and free the CPU while waiting.
    * **Lock/mutex** - object that prevents other threads from entering a critical section:
        * lock() - wait until lock is free, then acquire it.
        * unlock() - release the lock
        * downsides - requires waiting, best to minimize critical sections. Often more efficient to have many locks for more granular things than one big lock.
    * **Condition Variables** - used when one thread needs to wait for another thread to perform some action.
        * wait(mutex) - does the following steps atomically: release the mutex, add current thread to a waiting list/queue, put the thread to sleep/idle.
        * signal() - wakes up one thread waiting on the condition variable.
        * broadcast() - wakes up all threads waiting on the condition variable.
        * Prevents busy-waiting for a condition.
        * Spurious wake-up - when a thread gets signalled but then sees that condition needed is not satisfied so has to go to sleep again.
    * **Monitor** - refers to synchornization achieved with a lock (for mutual exclusion) and condition variables (for ordering) combined together
        * Mesa monitor - threads must contend for the lock when woken up (more popular).
        * Hoare monitors - woken up monitor gets special priority and is given lock by the signalling thread.
    * Preventing deadlock
        * Deadlock occurs when there are limited resources and some threads hold resources while waiting to obtain others, and there is a cylical chain of requests.
        * Can be avoided by having a total ordering of all resources, and make sure all threads try to obtain resources based on the ordering.
        * See dining philosophers.

* Synchronization Advanced
    * Reader-writer locks
        * Multiple threads should be able to read at the same time.
        * Multiple readers can access critical region, but when there is a writer only one (reader or writer) should be able to access.
        * Can be implemented with two CV's (each with a lock), one for readers and one for writers.
    * Semaphores
        * Generalized version of mutex. Contains a non-negative integer, with two operations:
            * down/wait - atomically decrements the integer. If it is zero, then sleep/wait until signalled (similar to lock or CV).
            * up/release - atomically increases the integer by one. Signals the other threads that are waiting.
        * Mutex semaphore - integer can only be set as 1 or 0, up() automatically sets to 1. Same functionality as a mutex.
        * Counting semaphore - integer is non-negative, used for representing a resource with many units like producer-consumer or reader-writer locks.
        * Note that a semaphore isn't tied to a specific thread - any thread can call up/down on the semaphore (useful for shared resource like reader lock).
        * Locks and CV's are usually interchangeable with semaphores (with slightly different implementations). For examples, see slides 6.
    
* Concurrency Implementation
    * Threads
        * Each thread transitions from several states based on their status and whether they are waiting. The states are new, ready, running, blocked, and terminated. The "ready" state is needed since the numnber of threads might exceed the number of CPU's.
        * ![image](thread_states.png)
            * New - thread was just created.
            * Ready - thread is ready to run and waiting for CPU to be free (TCB is on a "ready queue").
            * Running - thread is running on the CPU.
            * Blocked - thread is waiting on a lock or CV, the TCB is on the queue for the lock/CV.
            * Terminated - thread is done running.
        * When thread is not running, its "context" is stored in memory in a **Thread Control Block (TCB)**. This includes registers, stack pointer, program counter, etc.
        * Context switch - when a thread returns control to OS and OS chooses a new thread to run.
            * Occurs on internal events - wait() or lock(), IO requests.
                * Yield - thread relinquishes CPU and gets put on end of run queue.
                * Join - thread waits for another thread to finish first. The current thread x will wait for thread y to finish by calling y.join().
            * External events - timer and interrupts switch control to OS interrupt handler.
                * Interrupts can be disabled when running OS code (intterupts should never be disabled when running user code since there could be infinite loops and such).
                    * Usually done for atomic operations, such as locks adn CV's.
            * OS will choose a new thread to run from the ready queue, or wait for next timer interrupt if there are none ready.
            * OS will save the TCB of the current running thread (done through assembly, or can use linux swapcontext()).
            * OS will load the TCB of the new thread and resume execution.
        * Creating new threads
            * Involves creating a new TCB which includes stack and other variables, then putting the TCB onto the ready queue.

* Job/task scheduling:
    * Many policies for scheduling which tasks to run first.
    * First come first serve.
    * Round robin - each task gets allocated some amount of time to run then switches.
    * Shortest time to completion.
    * Priority queue - each task gets a priority. To prevent starvation, the priority goes up if a task has been waiting a long time.
    * Proportional share - a proportion of the CPU goes to real-time tasks, such as audio playback.

### Memory

* Memory Overview
    * **Address Space** - the memory space used by a single process (includes heap, stack, code, etc.)
    * **Hardware interface** - the physical memory shared by all processes.
    * Abstractions provided by OS:
        * Virtual memory - address space for each process can be larger than physical memory.
        * Address independence - Each address space has the same addresses.
        * Protection - one process can't access data in another process's address space.

    * Historic memory architectures:
        * Uni-programming - only one process can be run at a time and uses all the memory.
        * Static address translation - each process is assigned a contiguous chunk of memory and the process is responsible for keeping track of where the chunk is in physical memory.

* Dynamic address translation and virtual addresses
    * Each process gets its own address space and memory references are translated to physical addresses by the OS using hardware (MMU)
    * Translation is done by **Memory Management Unit (MMU)** which accesses the TLB.
    * Implementations of virtual memory:
        * Base and bounds - each virtual address space is mapped to a contiguous part of physical memory.
            * Disadvantages - hard to grow address space (need to copy everything), virtual address can't be larger than physical address, causes fragmentation of physical memory due to small processes, if heap grows then all the offsets for everything above the heap (like the stack) need to change.
        * Segmentation - divides address space into segments (i.e. code segment, heap segment, etc.) and each segment is a contiguous portion of physical memory.
            * Allows for easy sharing of code segments (like code itself can be shared) and each segment can grow independently.
            * Disadvantages - each segment is limited by the size of physical memory, external fragmentation.

        * Paging - divide virtual memory into fixed-size pages, each mapping to the same size page in physical memory. Pages are kept track of using a page table.
            * Pages that don't exist in physical memory can be mapped out to disk.
            * Each page can have different protections (like read-only)
            * See 370 notes for more details

* Kernel address space
    * As described above, there is a page table per process, managed by the OS. The current page table is stored in the page table base register (PTBR) by the OS.
    * Page tables can be stored in both physical memory.
    * Can also be kept in the OS kernel's virtual address space.
        * Benefit is that page tables can be paged out to disk, but the kernel's own page table must be kept in physical memory.
    * Kernel may sometimes need to directly access locations in physical memory (like code for swapping out pages and the kernel's own page table needs to be stored in physical memory always). To achieve this, the kernel is able to bypass the MMU.
        * Usually a portion of the kernel's virtual address space is mapped directly to physical memory.
    * Kernel may sometimes need to access a user's virtual memory (i.e., for reading a file).
        * To do this, the kernel's virtual address space is mapped into every process's address space. So each process's page table will have a section for the process, and a section for the OS.
            * Problem is that the kernel's page table needs to be replicated to each process's page table. Overhead for this is usually low.
            * This allows all physical memory mapping to be managed by MMU which is fast since it's hardware.
        * To prevent processes from accessing kernel data, keep a "mode bit" so that only the OS code can read the kernel address space portion of the process page table.
            * This is safe because the OS code is already written and pre-defined, so we know it's safe.
            * Allows easy switching between process and kernel code (like when cin is called).
    * Creating an address space when a process is created.
        * Process control block (similar to TLB) is created. 
        * Page tables are initialized and execution code is copied over into memory.
        * Registers are initialized.
        * Mode is set to "user" and program starts.

* Multiprocessing
    * Thrashing - when a lot of programs actively used all of their address space, causing a lot of page faults and significantly reducing speed.
        * Can be prevented by increasing RAM or running fewer tasks for longer times.
        * Thrashing can be measured by the **working set** - all pages used in the last T seconds and seeing if it fits into memory.
    * Linux cloning with Fork
        * When a process is forked (replicated), the page table is copied over, but the actual data is not copied over in memory.
        * The OS maintains a reference count for each page in memory. When there is a write with a page with refcount > 1, then a new copy of that page is created.
            * This is called **copy-on-write** and it reduces work when forking processes.

### Network

* Network Abstraction
    * Each computer only has one network interface card (NIC). The OS abstracts the network into **Sockets**, which is a virtual NIC (communication endpoint) used by each process.
        * Sockets are specified by port number.
        * Sockets manage when and how much data to send/receive. It sends the data to the NIC, which contacts other NIC's in the network. When receiving data, the socket receives data from the NIC (distribution done by OS).
    * TCP - abstracts away network issues and makes it seem like packets are sent and received in the correct order.
        * Receiver is able to re-order packets even though they receive it in an arbitrary order.
        * If messages are dropped the sender will re-send until it gets an ack.
        * Can send messages of arbitrary size (gets broken down into fragments and streamed).
        * Large overhead for processing

### File Systems and Storage

* File Systems
    * The OS provides an abstraction to files (disk) represented as a large set of files with rich naming convention, with the same hierarchical interface irrespective of hardware.
        * Many different types of storage, specific implementation is abstracted away by device drivers.
    * File representation
        * File metadata - file size, owner and permissions, time of last access, etc. Stored in the file header, called the **inode**.
        * Pointer to data - store an array of logical block numbers (also in inode)
            * Hard drives are partitioned into units of data called "logical blocks", which can be indexed by logical block number (similar to pages in memory).
            * Makes it easy to grow file and for random access within the file.
            * To speed up sequential access of the file, make file blocks map to disk blocks close to each other.
    * Indexed files
        * Each file header stores an array of block pointers, mapping file block indexes (array indices) to disk blocks (similar to a page table).
        * To save file header memory for small files, can use multi-level file indexing (similar to multi-level page table).
            * The key would then be a range of file blocks, mapping to the logical block containing the next level file index table.
            * To further optimize and reduce the number of seeks (looking up the file index) for small files, and make all block entries fit in one header for large files, can make the secondary indices have different depths (so smaller files won't require as many seeks).
    * File names
        * Files are usually named in a hierarchical manner. I.e. "/home/user/desktop".
        * File systems are represented via a tree data structure. Each inode for a directory points to the data location, and the data contains a map from file/directory names to inode locations.
            * Modern systems use b-tree or hash map.
        * Directories and other devices can be treated the same way. A directory name will point to the header file for the directory, and a device name will point to the header for that device (i.e. another hard drive or shared network drive).
            * The data for the directory will just contain the contents of the directory (names of the files it contains).
    * File cache
        * Caches actual data contained in a logical block. Maps device name and logical block number to data. Stored in RAM.
        * RAM used for file cache is separate from virtual memory.
    * File transactions
        * Need to maintain ACID (atomicity and durability) of file system in case of failures.
        * Done by **shadowing** - during a transaction, a copy of the data is created and the changes are made there. When all the changes are done, the inode pointer is modified to point to the new copy instead of the old copy.
            * Preserves data integrity if transaction fails.
        * Can also use append-only log and commit records to keep track of which changes have been made.
            * If system crashes, re-do all the changes before the latest commit.
        
## EECS 491 - Distributed Systems