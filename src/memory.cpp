#include <memory.h>

using namespace hydraos;
using namespace hydraos::common;

MemoryManager* MemoryManager::activeMemoryManager = 0;

MemoryManager::MemoryManager(size_t start, size_t size)
{
    activeMemoryManager = this;
    
    if(size < sizeof(MemoryChunk))
    {
        first = 0;
    }
    else
    {
        first = (MemoryChunk*)start;
        
        first->allocated = false;
        first->prev = 0;
        first->next = 0;
        first->size = size - sizeof(MemoryChunk);
    }
}

MemoryManager::~MemoryManager()
{
    if(activeMemoryManager == this)
        activeMemoryManager = 0;
}

void* MemoryManager::malloc(size_t size)
{
    MemoryChunk *result = 0;
    
    for(MemoryChunk* chunk = first; chunk != 0 && result == 0; chunk = chunk->next)
    {
        if(chunk->size > size && !chunk->allocated)
            result = chunk;
    }
    
    if(result == 0)
        return 0;
    
    if(result->size >= size + sizeof(MemoryChunk) + 1)
    {
        MemoryChunk *temp = (MemoryChunk*)((size_t)result + sizeof(MemoryChunk) + size);
        
        temp->allocated = false;
        temp->size = result->size - size - sizeof(MemoryChunk);
        temp->prev = result;
        temp->next = result->next;
        if(temp->next != 0)
            temp->next->prev = temp;
        
        result->size = size;
        result->next = temp;
    }
    
    result->allocated = true;
    return (void*)(((size_t)result) + sizeof(MemoryChunk));
}

void MemoryManager::free(void* ptr)
{
    MemoryChunk *chunk = (MemoryChunk*)((size_t)ptr - sizeof(MemoryChunk));
    
    chunk->allocated = false;
    
    // Merge with next chunk if it's free
    if(chunk->next != 0 && !chunk->next->allocated)
    {
        chunk->size += chunk->next->size + sizeof(MemoryChunk);
        chunk->next = chunk->next->next;
        if(chunk->next != 0)
            chunk->next->prev = chunk;
    }
    
    // Merge with previous chunk if it's free
    if(chunk->prev != 0 && !chunk->prev->allocated)
    {
        chunk->prev->size += chunk->size + sizeof(MemoryChunk);
        chunk->prev->next = chunk->next;
        if(chunk->next != 0)
            chunk->next->prev = chunk->prev;
    }
}

// C++ operator new/delete implementations
void* operator new(size_t size)
{
    if(MemoryManager::activeMemoryManager == 0)
        return 0;
    return MemoryManager::activeMemoryManager->malloc(size);
}

void* operator new[](size_t size)
{
    if(MemoryManager::activeMemoryManager == 0)
        return 0;
    return MemoryManager::activeMemoryManager->malloc(size);
}

void* operator new(size_t size, void* ptr)
{
    return ptr;
}

void* operator new[](size_t size, void* ptr)
{
    return ptr;
}

void operator delete(void* ptr)
{
    if(MemoryManager::activeMemoryManager != 0)
        MemoryManager::activeMemoryManager->free(ptr);
}

void operator delete[](void* ptr)
{
    if(MemoryManager::activeMemoryManager != 0)
        MemoryManager::activeMemoryManager->free(ptr);
}

// C-style malloc/free
void* malloc(size_t size)
{
    if(MemoryManager::activeMemoryManager == 0)
        return 0;
    return MemoryManager::activeMemoryManager->malloc(size);
}

void free(void* ptr)
{
    if(MemoryManager::activeMemoryManager != 0)
        MemoryManager::activeMemoryManager->free(ptr);
}

// Memory utility functions
void* memcpy(void* dst, const void* src, size_t n)
{
    const uint8_t* sp = (const uint8_t*)src;
    uint8_t* dp = (uint8_t*)dst;
    for(; n != 0; n--)
        *dp++ = *sp++;
    return dst;
}

void* memset(void* dst, uint8_t val, size_t n)
{
    uint8_t* temp = (uint8_t*)dst;
    for(; n != 0; n--)
        *temp++ = val;
    return dst;
}

int memcmp(const void* s1, const void* s2, size_t n)
{
    const uint8_t* p1 = (const uint8_t*)s1;
    const uint8_t* p2 = (const uint8_t*)s2;
    for(; n != 0; n--)
    {
        if(*p1 != *p2)
            return *p1 - *p2;
        p1++;
        p2++;
    }
    return 0;
}
