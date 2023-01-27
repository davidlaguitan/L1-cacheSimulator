#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>


#define LINE 256
#define MAX_BINARY_BITS 48
#define MAX_HEX_BITS 14
#define TABLE_SIZE 8

typedef struct Address {
    char *cacheAddress;
    int cacheTag;
    int cacheIndex;
    struct Address* next;
} Address;

typedef struct arrayitem {
    Address *head;
    Address *tail;
} arrayitem;

Address * hash_table[TABLE_SIZE];
arrayitem * array;

int find(Address *list, int tag);
struct Address* getAddress(struct Address* list, int findIndex);
int insert(int setIndex, char *stringAddress, int tag, int readOperation, int writeOperation, int assoc, int fifoPolicy, int lruPolicy);
int arraySize(Address *list);
void fifo(int setIndex);
void removeElement(int tag, int setIndex);
void insertBeginning(char* stringAddress, int setIndex, int tag);


int insert(int setIndex, char *stringAddress, int tag, int readOperation, int writeOperation, int assoc, int fifoPolicy, int lruPolicy) {
    
    Address *list = array[setIndex].head;
    if (fifoPolicy == 0) {
        if (readOperation == 0) {
            if (list == NULL) {
                printf("LIST IS NULL -- CACHE MISS\n");
                return 1; //list is empty... cache Miss
            } else {
                int findIndex = find(list, tag);
                if (findIndex == -1) { //tag not found... cache Miss
                    printf("TAG NOT FOUND -- CACHE MISS\n");
                    return 1;
                } else { //tag found... cache Hit
                    printf("TAG FOUND -- CACHE HIT\n");
                    return 2;
                }
            }
        } else {
            if (writeOperation == 0) {
                int arrayCount = arraySize(list);
                if (arrayCount >= assoc) {
                    fifo(setIndex);
                }
                printf("Size of Array = %d\n", arrayCount);
                Address *temp = malloc(sizeof(struct Address));
                temp -> cacheAddress = malloc(sizeof(char) * 14);
                strcpy(temp -> cacheAddress, stringAddress);
                temp -> cacheIndex = setIndex;
                temp -> cacheTag = tag;
                temp -> next = NULL;
                if (list == NULL) {
                    array[setIndex].head = temp;
                    array[setIndex].tail = temp;
                    printf("LIST IS EMPTY -- WRITE MISS (MEMREAD + 1, MEMWRITE + 1)\n");
                    return 3; //list is empty.... write miss... one memory read, one memory write
                } else {
                    int findIndex = find(list, tag);
                    if (findIndex == -1) { //tag not found...
                        array[setIndex].tail -> next = temp;
                        array[setIndex].tail = temp; 
                        printf("TAG NOT FOUND -- WRITE MISS (MEMREAD + 1, MEMWRITE + 1)\n");
                        return 3; //tag not found... write miss... one memory read, one memory write
                    } else { //tag found, updating Address
                        Address* element = getAddress(list, findIndex);
                        element -> cacheAddress = malloc(sizeof(char) * 14);
                        strcpy(element -> cacheAddress, stringAddress);
                        printf("TAG FOUND -- WRITE HIT (MEMWRITE + 1)\n");
                        return 4; //write hit... one memory write
                    }             
                }
            }
            return -1;
        }
    }

    if (lruPolicy == 0) {
        if (readOperation == 0) {
            if (list == NULL) {
                printf("LIST IS NULL -- CACHE MISS\n");
                return 1; //list is empty... cache Miss
            } else {
                int findIndex = find(list, tag);
                if (findIndex == -1) { //tag not found... cache Miss
                    printf("TAG NOT FOUND -- CACHE MISS\n");
                    return 1;
                } else { //tag found... cache Hit
                    printf("TAG FOUND -- CACHE HIT\n");
                    return 2;
                }
            }
        } else {
            if (writeOperation == 0) {
                int arrayCount = arraySize(list);
                if (arrayCount >= assoc) {
                    fifo(setIndex);
                }
                printf("Size of Array = %d\n", arrayCount);
                Address *temp = malloc(sizeof(struct Address));
                temp -> cacheAddress = malloc(sizeof(char) * 14);
                strcpy(temp -> cacheAddress, stringAddress);
                temp -> cacheIndex = setIndex;
                temp -> cacheTag = tag;
                temp -> next = NULL;
                if (list == NULL) {
                    array[setIndex].head = temp;
                    array[setIndex].tail = temp;
                    printf("LIST IS EMPTY -- WRITE MISS (MEMREAD + 1, MEMWRITE + 1)\n");
                    return 3; //list is empty.... write miss... one memory read, one memory write
                } else {
                    int findIndex = find(list, tag);
                    if (findIndex == -1) { //tag not found... add to front
                        insertBeginning(stringAddress, setIndex, tag);
                        printf("TAG NOT FOUND -- WRITE MISS (MEMREAD + 1, MEMWRITE + 1)\n");
                        return 3; //tag not found... write miss... one memory read, one memory write
                    } else { //tag found, remove from list, add to front
                        removeElement(tag, setIndex);
                        insertBeginning(stringAddress, setIndex, tag);
                        printf("TAG FOUND -- WRITE HIT (MEMWRITE + 1)\n");
                        return 4; //write hit... one memory write
                    }             
                }
            }
            return -1;
        }    
    }
    return -2;
}
int arraySize(Address *list) {
    int count = 0;
    Address *temp = list;
    while (temp != NULL) {
        temp = temp -> next;
        count++;
    }
    return count;
}

void fifo(int setIndex) {
    Address *list = array[setIndex].head;
    Address *temp = list;
    array[setIndex].head = temp -> next;
}

void insertBeginning(char *stringAddress, int setIndex, int tag) {
    Address *newAddress;
    newAddress = malloc(sizeof(struct Address));
    if (newAddress == NULL) {
        printf("unable to allocate memory.\n");
    } else {
        newAddress -> cacheAddress = malloc(sizeof(char) * 14);
        strcpy(newAddress -> cacheAddress, stringAddress);
        newAddress -> cacheTag = tag;
        newAddress -> next = array[setIndex].head;
        array[setIndex].head = newAddress;
    }
}

void removeElement(int tag, int setIndex) {
    Address *list = array[setIndex].head;
    if (list == NULL) {
        printf("This tag does not exist\n");
    } else {
        int findIndex = find(list, tag);
        if (findIndex == -1) {
            printf("This tag does not exist\n");
        } else {
            Address *temp = list;
            if (temp -> cacheTag == tag) {
                array[setIndex].head = temp -> next;
                return;
            }
            while (temp -> next -> cacheTag != tag) {
                temp = temp -> next;
            }
            if (array[setIndex].tail == temp -> next) {
                temp -> next = NULL;
                array[setIndex].tail = temp;
            } else {
                temp -> next = temp -> next -> next;
            }
        }
    }
}



int find(Address *list, int tag) {
    int retval = 0;
    Address *temp = list;
    while (temp != NULL) {
        if (temp -> cacheTag == tag) {
            return retval;
        }
        temp = temp -> next;
        retval++;
    }
    return -1;
}

struct Address* getAddress(struct Address* list, int findIndex) {
    int i = 0;
    Address *temp = list;
    while (i != findIndex) {
        temp = temp -> next;
        i++;
    }
    return temp;
}

void init_hash_table() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        array[i].head = NULL;
        array[i].tail = NULL;
    }
}

void print_table() {
    printf("***Start***\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        Address *temp = array[i].head;
        if (temp == NULL) {
            printf("\t%i\t---\n", i);
        } else {
            printf("\t%i\t", i);
            while (temp != NULL) {
                printf("[tag= %d   hex= %s] -> ", temp -> cacheTag, temp -> cacheAddress);
                temp = temp -> next;
            }
            printf("\n");
        }
    }
    printf("*******************END**********************\n\n");
}

int main (int argc, char* argv[]) {
    char line[LINE];
    char operation[LINE];
    int cacheSize;
    int assoc;
    char replace[LINE];
    int blockSize;
    int sets;
    long unsigned int hexAddress;
    char stringAddress[MAX_HEX_BITS];
    char *remaining;
    int setIndex;
    int tag;
    int setBits;
    int blockBits;
    int shiftOperand;
    int fifoPolicy;
    int lruPolicy;
    int readOperation;
    int writeOperation;
    int cacheMiss = 0;
    int cacheHit = 0;
    int memoryRead = 0;
    int memoryWrite = 0;

    

    sscanf(argv[1], "%d", &cacheSize);
    sscanf(argv[2], "%*6c%d", &assoc);
    sscanf(argv[3], "%s", replace);
    sscanf(argv[4], "%d", &blockSize);

    fifoPolicy = strcmp(replace, "fifo");
    lruPolicy = strcmp(replace, "lru");

    if (fifoPolicy != 0 && lruPolicy!= 0) {
        printf("ERROR: Not a valid policy...\n");
        exit(1);
    } else {
        if (fifoPolicy == 0) {
        printf("FIFO REPLACEMENT POLICY\n");
        } else {
            if (lruPolicy == 0) {
                printf("LRU REPLACEMENT POLICY\n");
            }
        }
    }

    if (ceil(log2(cacheSize)) != floor(log2(cacheSize))) {
        printf("ERROR: Cache size is not a power of 2...\n");
        exit(2);
    } else {
        printf("Cache size is a power of 2\n");
    }

    if (ceil(log2(blockSize)) != floor(log2(blockSize))) {
        printf("ERROR: Block size is not a power of 2...\n");
        exit(3);
    } else {
        printf("Block size is a power of 2\n");
    }

    sets = cacheSize / (assoc * blockSize);
    setBits = log2(sets);
    blockBits = log2(blockSize);
    shiftOperand = setBits + blockBits;

    FILE* input_file = NULL;
    input_file = fopen(argv[5], "r");
    if (input_file == NULL) {
        fprintf(stderr, "error\n");
        exit(1);
    }

    array = (struct arrayitem*)malloc(TABLE_SIZE * sizeof(struct arrayitem*));
    init_hash_table();
    print_table();

    while(fgets(line, LINE, input_file)) {
        
        int lineLength;
        int bitSize;
        sscanf(line, "%s %s%n", operation, stringAddress, &lineLength);
        hexAddress = strtol(stringAddress, &remaining, 16);
        printf("********************START***********************\n");
        printf("OPERATION: %s\t HEX ADDRESS: %s\t DECIMAL ADDRESS: %lu\n", operation, stringAddress, hexAddress);

        tag = hexAddress >> shiftOperand;
        printf("TAG IN DECIMAL FORM: %d\n", tag);
        bitSize = (lineLength - 4) * 4;
        printf("BIT SIZE: %d-bit address\n", bitSize);
        
        
        if (bitSize > 48) {
            printf("ERROR: MEMORY SIZE TOO LARGE!");
            exit(4);
        }

        int tagBits = bitSize - shiftOperand;
        int hexShifts = tagBits/4;
        char shiftBlockCombo = stringAddress[2 + hexShifts];

        switch (shiftBlockCombo) {
            case '0' : setIndex = 0; break;
            case '1' : setIndex = 0; break;
            case '2' : setIndex = 0; break;
            case '3' : setIndex = 0; break;
            case '4' : setIndex = 1; break;
            case '5' : setIndex = 1; break;
            case '6' : setIndex = 1; break;
            case '7' : setIndex = 1; break;
            case '8' : setIndex = 2; break;
            case '9' : setIndex = 2; break; 
            case 'a' : setIndex = 2; break;
            case 'A' : setIndex = 2; break;
            case 'b' : setIndex = 2; break;
            case 'B' : setIndex = 2; break;
            case 'c' : setIndex = 3; break;
            case 'C' : setIndex = 3; break;
            case 'd' : setIndex = 3; break;
            case 'D' : setIndex = 3; break;
            case 'e' : setIndex = 3; break;
            case 'E' : setIndex = 3; break;
            case 'f' : setIndex = 3; break;
            case 'F' : setIndex = 3; break;
            default: printf("ERROR!\n");
        }
            
        printf("-----numbers(start)------\n");
        printf("CACHE SIZE: %d\n", cacheSize);
        printf("NUMBER OF SETS: %d\n", sets);
        printf("ASSOCIATVITY: %d\n", assoc);
        printf("REPLACEMENT POLICY: %s\n", replace);
        printf("BLOCK SIZE: %d\n", blockSize);
        printf("HEX ADDRESS: %s\n", stringAddress);
        printf("HEX ADDRESS IN DECIMAL FORM: %ld\n", hexAddress);
        printf("AMOUNT OF SET BITS: %d\n", setBits);
        printf("AMOUNT OF BLOCK BITS: %d\n", blockBits);
        printf("AMOUNT OF TAG BITS: %d\n", tagBits);
        printf("SET BLOCK BIT: %c\n", shiftBlockCombo);
        printf("SET INDEX: %d\n", setIndex);
        printf("-----numbers(end)------\n");

        readOperation = strcmp(operation, "R");
        writeOperation = strcmp(operation, "W");
        //printf("readOps = %d\n", readOperation);
        //printf("writeOps = %d\n", writeOperation);

        int memoryFlag = insert(setIndex, stringAddress, tag, readOperation, writeOperation, assoc, fifoPolicy, lruPolicy);
        if (memoryFlag == 1) {
            cacheMiss++;
        }
        if (memoryFlag == 2) {
            cacheHit++;
        }
        if (memoryFlag == 3) {
            memoryRead++;
            memoryWrite++;
        }
        if (memoryFlag == 4) {
            memoryWrite++;
        }

    }
    printf("******************\n");
    printf("memread:%d\n", memoryRead);
    printf("memwrite:%d\n", memoryWrite);
    printf("cachehit:%d\n", cacheHit);
    printf("cachemiss:%d\n", cacheMiss);

    print_table();

    
    fclose(input_file);
    return 0;
}