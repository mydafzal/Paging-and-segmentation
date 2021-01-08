#include "headerf.h"

//segment number
int segmentnumber = 0;

//allocate memory block
SEGMENTBLOCK *allocateMemBlock(size_t size){
    SEGMENTBLOCK *block = sbrk(0);
    if(sbrk(BLOCK_SIZE + size) == (void*)-1){
        return NULL;
    }else{
        block->next = NULL;
        block->prev = NULL;
        block->isfree = false;
        block->size = size;
        return block;
    }
}

//allocate memory block for Paging
MEMPAGEBLOCKS *allocateMemPageBlock(size_t size)
{
    MEMPAGEBLOCKS *block = sbrk(0);
    if(sbrk(MEM_PAGE_BLOCK_SIZE + size) == (void*) - 1){ return NULL;}
    else{
        sbrk(MEM_PAGE_BLOCK_SIZE + size);
        block->next = NULL;
        block->isfree = true;
        block->memoryaddress = sbrk(0);
        return block;
    }
}

//allocate next memory block for Paging
void allocateNextMemPageBlock(size_t size, MEMPAGEBLOCKS **head)
{
    MEMPAGEBLOCKS *current = *head;
    void *allocate_mem = NULL;
    if(current == NULL){
        *head = allocateMemPageBlock(size);
    }else{
        while(current->next != NULL){
            current = current->next;
        }
        MEMPAGEBLOCKS *newblock = sbrk(0);

        allocate_mem = sbrk(MEM_PAGE_BLOCK_SIZE + size);      
        if(allocate_mem == (void*) - 1){ }
        else{
            sbrk(MEM_PAGE_BLOCK_SIZE + size);
            newblock->next = NULL;
            newblock->isfree = true;
            newblock->memoryaddress = sbrk(0);
            current->next = newblock;
      }
    }
}

//allocate memory block for Segmentation
MEMSEGMENTBLOCKS *allocateMemSegmentBlock(size_t size)
{
    MEMSEGMENTBLOCKS *block = sbrk(0);
    if(sbrk(MEM_SEGMENT_BLOCK_SIZE + size) == (void*) - 1){ return NULL;    }
    else{
        sbrk(MEM_SEGMENT_BLOCK_SIZE + size);
        block->next = NULL;
        block->isfree = false;
        block->size = size;
        block->memoryaddress = sbrk(0);
        return block;
    }
}

//allocate next memory block for Segmentation
void allocateNextMemSegmentBlock(size_t size, MEMSEGMENTBLOCKS **head)
{
    MEMSEGMENTBLOCKS *current = *head;
    void *allocate_mem = NULL;
    if(current == NULL){
        *head = allocateMemSegmentBlock(size);
    }else{
        while(current->next != NULL){
            current = current->next;
        }
        MEMSEGMENTBLOCKS *newblock = sbrk(0);

        allocate_mem = sbrk(MEM_SEGMENT_BLOCK_SIZE + size);      
        if(allocate_mem == (void*) - 1){ }
        else{
            sbrk(MEM_SEGMENT_BLOCK_SIZE + size);
            newblock->next = NULL;
            newblock->isfree = false;
            newblock->size = size;
            newblock->memoryaddress = sbrk(0);
            current->next = newblock;
      }
    }
}


// divide memory processes/program and memory into equal 4KB of sizes
void divideProcMemoryIntoPageBlocks(PROCINTNODE *procinthead, VITUALMEMPAGEBLOCKS **virtmempageblockshead,
                                    MEMPAGEBLOCKS **mempageblocksHead)
{
    PROCINTNODE *current = procinthead;
    unsigned int pagenumber = 0;
    void *address = (void*)0x12345678;
    while(current != NULL){
        if(current->size < PAGE_SIZE){
            virtualMemoryPageBlocksInsertAtEnd(current->process, PAGE_SIZE, pagenumber, address, &(*virtmempageblockshead));
            current = current->next;
        }else{
            virtualMemoryPageBlocksInsertAtEnd(current->process, PAGE_SIZE, pagenumber, address, &(*virtmempageblockshead));
            current->size = (current->size - PAGE_SIZE);
            pagenumber++;
            address += PAGE_MEM_SIZE;
        }
    }
    for(int i = 0; i < MAX_PAGES; i++){
        allocateNextMemPageBlock(PAGE_MEM_SIZE, &(*mempageblocksHead));
    }
}
void mapVirtPhyAddressPageTable(VITUALMEMPAGEBLOCKS **virtmempageblockshead, MEMPAGEBLOCKS **mempageblocksHead)
{
    MEMPAGEBLOCKS *currentmem = *mempageblocksHead;
    MEMPAGEBLOCKS *currentmem2 = *mempageblocksHead;
    VITUALMEMPAGEBLOCKS *currentvirt = *virtmempageblockshead;
    MEMPAGEBLOCKS *currtemp = NULL;
    int count = 0;


    printf("\n => Memory mapped Virtual/Physical Page Table\n");
    printf("\n-----------------------------------------------------------------------------------");
    printf("\n|    Process    |    PageNumber    |    VirtualAddress    |    PhysicalAddress    |\n");
    printf("-----------------------------------------------------------------------------------\n");

    while(currentmem != NULL){
            if(currentmem->isfree){
                    printf("|       P%d      |        %d         |     %p       |        %p      |\n",
                        currentvirt->process, currentvirt->pagenumber, currentvirt->memoryaddress, currentmem->memoryaddress);
                    
                    currentmem->isfree = false;
                    count++;

                    if(count == 256){
                        printf("***- 1MB block of pages loaded -***\n");
                        count = 0;
                    }
            }

            currentvirt = currentvirt->next;
            currentmem = currentmem->next;

            if(currentvirt == NULL){
                goto exitLoop;
            }
    }

    printf("-----------------------------------------------------------------------------------\n");
    currentmem = *mempageblocksHead;
    while(currentmem != NULL){
        if(!(currentmem->isfree)){
            currentmem->isfree = true;
            printf("Memory Frame freed, Address = %p\n", currentmem->memoryaddress);
        }
        currentmem = currentmem->next;
    }
    mapVirtPhyAddressPageTable(&currentvirt, &(*mempageblocksHead));

exitLoop:
    currentmem = *mempageblocksHead;
    while(currentmem != NULL){
        if(!(currentmem->isfree)){
            currentmem->isfree = true;
            printf("Memory Frame freed, Address = %p\n", currentmem->memoryaddress);
        }
        currentmem = currentmem->next;
    }
}
void allocateMemoryUsingSegmentation(PROCINTNODE **procinthead, MEMSEGMENTBLOCKS **memSegBlockshead)
{
    PROCINTNODE *current = *procinthead;
    int total_size = 0;

    printf("\nsbrk(0) = %p\n", sbrk(0));

    while(current != NULL){
        if(current->size <= MAX_SEGMENT_SIZE){
            if(total_size + current->size < MAX_MEM_SIZE){
                void *start_address = sbrk(0) + 1;
                allocateNextMemSegmentBlock(current->size + 1, &(*memSegBlockshead));
                void *end_address = sbrk(0);

                total_size += current->size;

                printf("\nProcess = P%d, Start Address=%p, End Address=%p, Segment Number=%d\n",
                        current->process, start_address, end_address, segmentnumber);

                printf("\nTotal Size=%d\n", total_size);

                current->size = 0;
                current = current->next;
                segmentnumber++;
            }else{
                total_size = 0;
                break;
            }
        }else{
                if(total_size + MAX_SEGMENT_SIZE < MAX_MEM_SIZE){
                    void *start_address = sbrk(0) + 1;
                    allocateNextMemSegmentBlock(MAX_SEGMENT_SIZE, &(*memSegBlockshead));
                    void *end_address = sbrk(0);
                    total_size += MAX_SEGMENT_SIZE;

                    printf("\nProcess = P%d, Start Address=%p, End Address=%p, Segment Number=%d\n",
                        current->process, start_address, end_address, segmentnumber);

                    printf("\nTotal Size=%d\n", total_size); 

                    current->size = current->size - MAX_SEGMENT_SIZE;
                    segmentnumber++;
                }else{
                    total_size = 0;
                    break;
                }
            }
    }

    //delete memory nodes of size 0 or used
    current = *procinthead;
    deleteProcIntNodeZeroData(&current);
    current = *procinthead;

    if(countPROCINTNodes(current) == 1){
        if(current->size == 0){ *procinthead = NULL;}
    }else{
            *procinthead = (*procinthead)->next;
    }

    //call function to continue process to all processes
    allocateMemoryUsingSegmentRemain(&(*procinthead), &(*memSegBlockshead));
}

// get the start_address of free memory for next segment of process
void getFreeMemoryAddress(MEMSEGMENTBLOCKS *memSegBlockshead, unsigned int size, void **start_address)
{
    MEMSEGMENTBLOCKS *current = memSegBlockshead;

    int size_temp = current->size;

    while(current != NULL){
        if(current->isfree){
            if(size_temp >= size){
                *start_address=current->memoryaddress;
                size_temp = 0;
                break;
            }
            size_temp += current->size;
            current = current->next;
        }else{ current = current->next; }
    }
}

// allocate memory to remaining segments
void allocateMemoryUsingSegmentRemain(PROCINTNODE **procinthead, MEMSEGMENTBLOCKS **memSegBlockshead)
{
    PROCINTNODE *current = *procinthead;
    int total_size = 0;
    MEMSEGMENTBLOCKS *segcurrent = *memSegBlockshead;

    //lets free the used memory
    while(segcurrent != NULL){
        segcurrent->isfree = true;
        segcurrent = segcurrent->next;
    }
    
    segcurrent = *memSegBlockshead;
    void *start_address;
    
    while(current != NULL){
        if(current->size <= MAX_SEGMENT_SIZE){
            if(total_size + current->size < MAX_MEM_SIZE){
                
                getFreeMemoryAddress(*memSegBlockshead, current->size, &start_address);

                void *end_address = start_address + current->size;

                total_size += current->size;

                printf("\nProcess = P%d, Start Address=%p, End Address=%p, Segment Number=%d\n",
                        current->process, start_address, end_address, segmentnumber);

                    /* do your work here on allocated memory
                        declare your program loader code here
                    */

                printf("\nTotal Size=%d\n", total_size);

                current->size = 0;
                current = current->next;
                segmentnumber++;
            }else{
                total_size = 0;
                break;
            }
        }else{
                if(total_size + MAX_SEGMENT_SIZE < MAX_MEM_SIZE){

                    if(segcurrent->next == NULL)
                        segcurrent = *memSegBlockshead;
                    else
                        segcurrent = segcurrent->next;

                    getFreeMemoryAddress(segcurrent, MAX_SEGMENT_SIZE, &start_address);

                    void *end_address = start_address + MAX_SEGMENT_SIZE;

                    printf("\nProcess = P%d, Start Address=%p, End Address=%p, Segment Number=%d\n",
                        current->process, start_address, end_address, segmentnumber);

                    

                    total_size += MAX_SEGMENT_SIZE;

                    printf("\ntotal_size=%d\n", total_size);

                    current->size = current->size - MAX_SEGMENT_SIZE;
                    segmentnumber++;
                }else{
                    total_size = 0;
                    break;
                }
            }
    }    
}

void AllocatePAGING(SEGMENTBLOCK *s_blockHead,VITUALMEMPAGEBLOCKS *virtmempageBlocks,
                    MEMPAGEBLOCKS *mempageBlocks, INTNODE *dataList)
{
    //get data from input file
    PROCINTNODE *procintHead = NULL;
    procintHead = getDataFromINTNODEList(dataList, &procintHead);
    
    divideProcMemoryIntoPageBlocks(procintHead, &virtmempageBlocks, &mempageBlocks);

    printf("\n => Allocate memory using Paging\n");

    mapVirtPhyAddressPageTable(&virtmempageBlocks, &mempageBlocks);
}

void AllocateSEGMENTATION(SEGMENTBLOCK *s_blockHead, MEMSEGMENTBLOCKS *memSegBlocksHead, INTNODE *dataList)
{
    //get data from input file
    PROCINTNODE *procintHead = NULL;
    procintHead = getDataFromINTNODEList(dataList, &procintHead);
    
    printf("\n => Allocate memory using Segmentation\n");
    do{
        if(procintHead->next == NULL){
            if(procintHead->size == 0)
                break;
        }else{
            deleteProcIntNodeZeroData(&procintHead);
            allocateMemoryUsingSegmentation(&procintHead, &memSegBlocksHead);
        }
    }while(procintHead != NULL);

}