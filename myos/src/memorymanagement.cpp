#include <memorymanagement.h>

using namespace myos;
using namespace myos::common;

void printf(char* str);
void printfHex(uint8_t);
void printfHex32(uint32_t);

//> class MemoryManager{
    //> protected:
        // MemoryManager* first;
    //<> public:
        MemoryManager* MemoryManager::activeMemoryManager = 0;

        //constructor
        MemoryManager::MemoryManager(size_t start, size_t size){
            activeMemoryManager = this;

            //ensure enough free memory for initial memory chunk
            if(size < sizeof(MemoryChunk)){
                first = 0;
            }else{
                first = (MemoryChunk*)start;

                first -> allocated = false;
                first -> prev = 0;
                first -> next = 0;
                first -> size = size - sizeof(MemoryChunk);
            }
        }

        //destructor
        MemoryManager::~MemoryManager(){
            if(activeMemoryManager == this){
                activeMemoryManager = 0;
            }
        }

        void* MemoryManager::malloc(size_t size){

            //>find first memory chunk with enough space
                MemoryChunk* result = 0;

                //loop through linked list
                for(MemoryChunk* chunk = first; chunk != 0; chunk = chunk->next){

                    if(chunk->size > size && !chunk->allocated){
                        result = chunk;
                        break;
                    }
                }

            //<>if no chunk found, return 0
                if(result == 0){
                    return 0;
                }

            //<>if there's enough space for an additional MemoryChunk after allocation

                //+1 is to ensure data can be stored after the MemoryChunk header
                if(result->size >= size + sizeof(MemoryChunk) + 1){

                    //>create unallocated MemoryChunk after the newly allocated one
                        MemoryChunk* temp = (MemoryChunk*)((size_t)result + sizeof(MemoryChunk) + size);
                        temp->allocated = false;
                        temp->size = result->size - size - sizeof(MemoryChunk);
                        temp->prev = result;

                    //<>modify pointers
                        temp->next = result->next;
                        if(temp->next != 0){

                            //inform the next MemoryChunk, it's prev MemoryChunk has changed
                            temp->next->prev = temp;

                        }
                        result->next = temp;
                    //<
                }

            //<>return pointer to newly allocated memory
                result->size = size;
                result->allocated = true;

                //return pointer to beginning of allocated memory
                return (void*)(((size_t)result) + sizeof(MemoryChunk));
            //<
        }

        void MemoryManager::free(void* ptr){

            //point to beginning of MemoryChunk instead of beginning of allocated space
            MemoryChunk* chunk = (MemoryChunk*)((size_t)ptr - sizeof(MemoryChunk));

            //deallocate chunk
            chunk->allocated = false;

            //>merge any neighboring unallocated chunks

                //if the previous chunk is unallocated
                if(chunk->prev != 0 && !chunk->prev->allocated){
                    chunk->prev->next = chunk->next;
                    chunk->prev->size += chunk->size + sizeof(MemoryChunk);

                    //>update "previous" pointer of next chunk
                        if(chunk->next != 0){
                            chunk->next->prev = chunk->prev;
                        }
                    //<

                    chunk = chunk->prev;
                }

                //if the next chunk is unallocated
                if(chunk->next != 0 && !chunk->next->allocated){
                    chunk->size += chunk->next->size + sizeof(MemoryChunk);
                    chunk->next = chunk->next->next;

                    //update "previous" pointer of next chunk
                    if(chunk->next != 0){
                        chunk->next->prev = chunk;
                    }
                }
            //<
        }

        void MemoryManager::printNumChunks(){
            uint32_t count = 0;
            for(MemoryChunk* chunk = first; chunk != 0; chunk = chunk->next){
                count++;
            }
            printfHex32(count);
        }

        void MemoryManager::listChunks(){
            uint32_t count = 0;
            for(MemoryChunk* chunk = first; chunk != 0; chunk = chunk->next){
                printf("Chunk# "); printfHex32(count); printf("\n");
                printf("ChunkLoc: "); printfHex32((uint32_t)chunk); printf("\n");
                printf("prev: "); printfHex32((uint32_t)chunk->prev); printf("\n");
                printf("next: "); printfHex32((uint32_t)chunk->next); printf("\n");
                printf("allocated: "); printfHex32((uint32_t)chunk->allocated); printf("\n");
                printf("size: "); printfHex32((uint32_t)chunk->size); printf("\n");
                printf("\n");

                //keep at end of loop
                count++;
            }
        }
    //<
//<


void* operator new(unsigned size){
    if(myos::MemoryManager::activeMemoryManager == 0){
        return 0;
    }
    return myos::MemoryManager::activeMemoryManager->malloc(size);
}

void* operator new[](unsigned size){
    if(myos::MemoryManager::activeMemoryManager == 0){
        return 0;
    }
    return myos::MemoryManager::activeMemoryManager->malloc(size);
}

//>placement new
    void* operator new(unsigned size, void* ptr){
        return ptr;
    }

    void* operator new[](unsigned size, void* ptr){
        return ptr;
    }
//<

void operator delete(void* ptr){
    if(myos::MemoryManager::activeMemoryManager != 0){
        myos::MemoryManager::activeMemoryManager->free(ptr);
    }
}
void operator delete[](void* ptr){
    if(myos::MemoryManager::activeMemoryManager != 0){
        myos::MemoryManager::activeMemoryManager->free(ptr);
    }
}
