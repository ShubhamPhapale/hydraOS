#ifndef __HYDRAOS__MEMORY_H
#define __HYDRAOS__MEMORY_H

#include <common/types.h>

namespace hydraos
{
    struct MemoryChunk
    {
        MemoryChunk *next;
        MemoryChunk *prev;
        bool allocated;
        hydraos::common::size_t size;
    };

    class MemoryManager
    {
        protected:
            MemoryChunk* first;

        public:
            static MemoryManager *activeMemoryManager;

            MemoryManager(hydraos::common::size_t start, hydraos::common::size_t size);
            ~MemoryManager();

            void* malloc(hydraos::common::size_t size);
            void free(void* ptr);
    };
}

void* operator new(hydraos::common::size_t size);
void* operator new[](hydraos::common::size_t size);

// placement new
void* operator new(hydraos::common::size_t size, void* ptr);
void* operator new[](hydraos::common::size_t size, void* ptr);

void operator delete(void* ptr);
void operator delete[](void* ptr);

// C-style memory functions
void* malloc(hydraos::common::size_t size);
void free(void* ptr);

// Memory utility functions
void* memcpy(void* dst, const void* src, hydraos::common::size_t n);
void* memset(void* dst, hydraos::common::uint8_t val, hydraos::common::size_t n);
int memcmp(const void* s1, const void* s2, hydraos::common::size_t n);

#endif
