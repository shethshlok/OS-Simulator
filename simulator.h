// protect from multiple compiling
#ifndef SIMULATOR_H
#define SIMULATOR_H


// header files
#include "configops.h"
#include "metadataops.h"
#include "simtimer.h"
#include "pthread.h"

// data structs
typedef struct ProcessControlBlockStruct
    {
        int processId;
        struct OpCodeTypeStruct *appStartPtr;
        ProcessState state;
        struct ProcessControlBlockStruct *nextPcbPtr;
        int timeRemaining;
        double blockedTime;
        double finishedTime;
    } PCBType;

typedef struct OutputStringStruct
    {
        char line[MAX_STR_LEN];
        struct OutputStringStruct *nextLine;
    } OutputString;

typedef struct MemoryBlockStruct
    {
        int physicalMin;
        int physicalMax;
        int logicalMin;
        int logicalMax;
        bool used;
        int pcbID;
        struct MemoryBlockStruct *nextBlock;
    } MemoryBlock;

typedef struct InterruptStruct
{
    struct ProcessControlBlockStruct *intPcb;
    struct InterruptStruct *nextPcb;
} InterruptType;


// function prototypes

/*
Name: runSim
Process: actively runs the simulator and does the processing, To be codded
Function Input/Paraneters: pointer pointing to meta data (ConfigDataType *),
                           pointer pointing to config data
Function Output/Parameters: none
Function Output/Returned:
Device Input/device: none
Device Output/device: none
Dependencies: printf
*/
void runSim( ConfigDataType *configDataPtr, OpCodeType *metaDataPtr );
int calculateTime(OpCodeType *metaPtr, ConfigDataType *configPtr );
ProcessState changeState(ProcessState state );
void generateLogFile( ConfigDataType *configPtr, OutputString *simulatorString );
PCBType *getPCB(ConfigDataType *configDataPtr, OpCodeType *metaDataPtr );
void saveProcessStart(PCBType *currentPcb, char time[], OutputString *tempString, OutputString *logString, ConfigDataType *configPtr );
void saveProcessEnd( char time[], PCBType *currentPcb, OutputString *tempString, OutputString *logString, ConfigDataType *configPtr );
OutputString *addString( OutputString* logString, OutputString *tempString );
void logSimulator(OutputString *loggingString, ConfigDataType *configPtr );
void printForMointor(ConfigDataType *configPtr, OutputString *loggingString );
PCBType *getNextProcess(PCBType *processPointer, ConfigDataType *configPtr, PCBType *processHead );

// ------------------------
bool checkForPhysical( MemoryBlock *memoryPtr, int maxReq );
bool checkForLogical(MemoryBlock *memoryPtr, int maxReq, int minReq );
bool memoryAllocate(ConfigDataType *configPtr, PCBType *pcb, MemoryBlock **memory, OutputString* logString );
bool saveProcessing(char time[], PCBType *currentPcb, ConfigDataType *configDataPtr, OutputString *tempString, OutputString *logString, MemoryBlock *memory, InterruptType **intQ, PCBType* headpcb);
MemoryBlock *initializeMemory(ConfigDataType *configDataPtr);
void displayMemoryBlocks(MemoryBlock *memory, OutputString* logString, ConfigDataType* configPtr  );
void clearMemory( ConfigDataType *configPtr, MemoryBlock **memoryPtr, PCBType *pcbPtr, OutputString* logString );

// ---------------------------
// bool interruptManager( InterruptType **intQ, ConfigDataType *configPtr, PCBType* mainPcbPtr );
bool checkPreemptive(ConfigDataType *configPtr);
bool processCPUCycles(ConfigDataType *configPtr, PCBType* currentPCB, InterruptType **intQ, OutputString *logString, PCBType* headPcb );
bool interruptManager( PCBType *currentPcb, InterruptType **intQ, OutputString *logString, ConfigDataType* configPtr, PCBType* headPcb );



#endif
