#define _MYHEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#define MAX_MEM_SIZE (1024*1024)    // 1048576
#define PAGE_SIZE 4096
#define MAX_PAGES 256
#define PAGE_MEM_SIZE 0x256
#define MAX_SEGMENT_SIZE 256000 //256 KB

typedef struct segmentBlock{
    struct segmentBlock *next;
    struct segmentBlock *prev;
    bool isfree;
    size_t size;
}SEGMENTBLOCK;

#define BLOCK_SIZE sizeof(SEGMENTBLOCK)

typedef struct memVirtualPageBlocks{
    struct memVirtualPageBlocks *next;
    size_t size;
    int process;
    int pagenumber;
    void *memoryaddress;
}VITUALMEMPAGEBLOCKS;


typedef struct memPageBlocks{
    struct memPageBlocks *next;
    bool isfree;
    void *memoryaddress;
}MEMPAGEBLOCKS;

#define MEM_PAGE_BLOCK_SIZE sizeof(MEMPAGEBLOCKS)


typedef struct memSegmentBlocks{
    struct memSegmentBlocks *next;
    bool isfree;
    size_t size;
    void *memoryaddress;
}MEMSEGMENTBLOCKS;

#define MEM_SEGMENT_BLOCK_SIZE sizeof(MEMSEGMENTBLOCKS)

typedef struct node {
    char data;
    struct node *next;
}NODE;

typedef struct intNode {
    int data;
    struct intNode *next;
}INTNODE;

 
typedef struct procIntNode {
    int process;
    int size;
    struct procIntNode *next;
}PROCINTNODE;

extern NODE* createNewNode(char );
extern INTNODE* intCreateNewNode(int );
extern NODE* insertAtEnd(char , NODE **);
extern INTNODE* intInsertAtEnd(int , INTNODE **);
extern NODE* removeExtraSpaces(NODE** );
extern int countINTNodes(INTNODE *);
extern void deleteCharList(NODE **);
extern void deleteINTList(INTNODE **);
extern INTNODE *deleteINTFront(INTNODE **);
extern INTNODE *deleteINTEnd(INTNODE **);
extern INTNODE* deleteIntFromSecondNode(INTNODE **);
extern void separateNumbDatas(NODE *head, INTNODE **);
extern INTNODE *getProcessData(const char *);
extern PROCINTNODE* procIntCreateNewNode(int ,int );
extern VITUALMEMPAGEBLOCKS* virtualMemPageBlocksCreateNewNode(int ,size_t ,int ,void *);
extern PROCINTNODE* procInsertAtEnd(int ,int , PROCINTNODE **);
extern VITUALMEMPAGEBLOCKS* virtualMemoryPageBlocksInsertAtEnd(int ,size_t ,int ,void *, VITUALMEMPAGEBLOCKS **);
extern PROCINTNODE *getDataFromINTNODEList(INTNODE *, PROCINTNODE **);
extern int getProcessMemSizeSum(PROCINTNODE *);
extern int countPROCINTNodes(PROCINTNODE *);
extern void deleteProcIntNodeZeroData(PROCINTNODE **);
extern SEGMENTBLOCK *allocateMemBlock(size_t );
extern MEMPAGEBLOCKS *allocateMemPageBlock(size_t );
extern void allocateNextMemPageBlock(size_t , MEMPAGEBLOCKS **);
extern MEMSEGMENTBLOCKS *allocateMemSegmentBlock(size_t );
extern void allocateNextMemSegmentBlock(size_t , MEMSEGMENTBLOCKS **);
extern void divideProcMemoryIntoPageBlocks(PROCINTNODE *, VITUALMEMPAGEBLOCKS **,  MEMPAGEBLOCKS **);
extern void mapVirtPhyAddressPageTable(VITUALMEMPAGEBLOCKS **,MEMPAGEBLOCKS ** );
extern void allocateMemoryUsingSegmentation(PROCINTNODE **,MEMSEGMENTBLOCKS **);
extern void getFreeMemoryAddress(MEMSEGMENTBLOCKS *,unsigned int ,void **);
extern void allocateMemoryUsingSegmentRemain(PROCINTNODE **,MEMSEGMENTBLOCKS **);
extern void AllocatePAGING(SEGMENTBLOCK *, VITUALMEMPAGEBLOCKS *, MEMPAGEBLOCKS *, INTNODE *);
extern void AllocateSEGMENTATION(SEGMENTBLOCK *, MEMSEGMENTBLOCKS *,  INTNODE *);