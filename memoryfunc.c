#include "headerf.h"

// function create new node with input data as integer type in linked list PROCINTNODE
PROCINTNODE* procIntCreateNewNode(int process, int size)
{
    PROCINTNODE *newNode = NULL;
    newNode=(PROCINTNODE*)malloc(sizeof(PROCINTNODE));
    if(newNode == NULL) {
        printf("Memory allocation error\n");
        return NULL;
    }
    newNode->process = process;
    newNode->size = size;
    newNode->next = NULL;
    return newNode;
}

// function create new node of VITUALMEMPAGEBLOCKS of linked list 
VITUALMEMPAGEBLOCKS* virtualMemPageBlocksCreateNewNode(int process, size_t size, int pagenumber, void *memoryaddress)
{
    VITUALMEMPAGEBLOCKS *newNode = NULL;
    newNode=(VITUALMEMPAGEBLOCKS*)malloc(sizeof(VITUALMEMPAGEBLOCKS));
    if(newNode == NULL) {
        printf("Memory allocation error\n");
        return NULL;
    }
    newNode->process = process;
    newNode->size = size;
    newNode->pagenumber=pagenumber;
    newNode->memoryaddress=memoryaddress;
    newNode->next = NULL;
    return newNode;
}

// function to insert new node at the end of list PROCINTNODE 
PROCINTNODE* procInsertAtEnd(int process, int size, PROCINTNODE **head)
{
    PROCINTNODE *current = *head;
    if (current == NULL) {
        *head = procIntCreateNewNode(process, size);
    }else{
        while(current->next != NULL){
            current = current->next;
        }
        current->next = procIntCreateNewNode(process, size);
    }
    return *head;
}

// function to insert new node at the end of list VITUALMEMPAGEBLOCKS 
VITUALMEMPAGEBLOCKS* virtualMemoryPageBlocksInsertAtEnd(int process,size_t size,int pagenumber,
                                                void *memoryaddress, VITUALMEMPAGEBLOCKS **head)
{
    VITUALMEMPAGEBLOCKS *current = *head;
    if (current == NULL) {
        *head = virtualMemPageBlocksCreateNewNode(process, size, pagenumber, memoryaddress);
    }else{
        while(current->next != NULL){
            current = current->next;
        }
        current->next = virtualMemPageBlocksCreateNewNode(process, size, pagenumber, memoryaddress);
    }
    return *head;
}

// function to create new list of PROCINTNODE by reading data from INTNODE list 
PROCINTNODE *getDataFromINTNODEList(INTNODE *inthead, PROCINTNODE **prochead)
{
    INTNODE *current = inthead;
    int count = 0;
    int process;
    while(current != NULL){
        count++;
        switch(count){
            case 1 : process = current->data;
                    current = current->next;
                    break;

            case 2 : 
                    *prochead = procInsertAtEnd(process, current->data, &(*prochead));
                    count = 0;
                    current = current->next;
                    break;
            }
    }
    return *prochead;
}

// function to count nodes of integer type linked list PROCINTNODE
int countPROCINTNodes(PROCINTNODE *prochead)
{
    int count = 0;
    while(prochead != NULL){
        count++;
        prochead = prochead->next;
  }
    return count;
}

// function to delete node whose contain 0 value in PROCINTNODE list 
void deleteProcIntNodeZeroData(PROCINTNODE **procinthead)
{
    PROCINTNODE *current = *procinthead;
    PROCINTNODE *temp = *procinthead;

    if(current == NULL) { }else{
        if(countPROCINTNodes(current) == 1){
            if(current->size == 0){
                *procinthead=NULL;
                free(current);
            }
        }else{
            while(current != NULL){
                if(current->size == 0){
                    temp->next = current->next;
                    free(current);
                    current = temp->next;
                }else{
                    temp = current;
                    current = current->next;
                }
            }
        }
    }
    current = *procinthead;
    if(current-> size == 0){
        *procinthead = current->next;
    }
}