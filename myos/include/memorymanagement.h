#ifndef __MYOS__MEMORYMANAGEMENT_H
#define __MYOS__MEMORYMANAGEMENT_H

#include <common/types.h>

namespace myos{

    struct MemoryChunk{
        MemoryChunk *next;
        MemoryChunk *prev;
        bool allocated;
        size_t size;
    };

    class MemoryManager{
        protected:
            MemoryManager* first;
        public:
            static MemoryManager *activeMemoryManager;

            MemoryManager(size_t first, size_t size);
            ~MemoryManager();

            void* malloc(size_t size);
            void free(void* ptr);
    };
}