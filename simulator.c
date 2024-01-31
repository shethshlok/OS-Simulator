// header files
#include "simulator.h"


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
void runSim( ConfigDataType *configDataPtr, OpCodeType *metaDataPtr )
    {
      PCBType *pcbPointer = NULL, *tempPtr = NULL, *headPtr;
      char timeStamp[TIME_LENGTH];
      OutputString *logString = NULL, *bufferString = (OutputString*) malloc(sizeof(OutputString));
      MemoryBlock *memory;
      InterruptType *intQ = (InterruptType*) malloc(sizeof(InterruptType));
      intQ->nextPcb = NULL;
      bool blockedPCB = false;
      PCBType* headPcb = NULL;


      if( configDataPtr->logToCode == LOGTO_FILE_CODE )
         {
            printf("\nSimulator running for output to file only\n");
         }

      pcbPointer = getPCB(configDataPtr, metaDataPtr );

      sprintf(bufferString->line, "Simulator Run\n");
      printForMointor(configDataPtr, bufferString );
      
      sprintf(bufferString->line, "-------------\n\n");
      printForMointor(configDataPtr, bufferString );

      accessTimer(ZERO_TIMER, timeStamp);
      sprintf(bufferString->line, " %s, OS: Simulator Start\n", timeStamp);
      logString = addString(logString, bufferString);
      printForMointor(configDataPtr, bufferString );
      
      tempPtr = pcbPointer;
      headPcb = pcbPointer;
      while( tempPtr != NULL )
      {
         accessTimer(LAP_TIMER, timeStamp);
         tempPtr->state = changeState( tempPtr->state );
         sprintf(bufferString->line, " %s, OS: Process %d set to READY state from NEW state\n", timeStamp, tempPtr->processId );
         logString = addString(logString, bufferString);
         printForMointor(configDataPtr, bufferString );
         tempPtr = tempPtr->nextPcbPtr;
      }

      memory = initializeMemory(configDataPtr);
      if( configDataPtr->memDisplay )
      {  
         sprintf(bufferString->line, "--------------------------------------------------\n");
         logString = addString(logString, bufferString);
         printForMointor(configDataPtr, bufferString );
         sprintf(bufferString->line, "After memory initialization\n" );
         logString = addString(logString, bufferString);
         printForMointor(configDataPtr, bufferString );
         displayMemoryBlocks(memory, logString, configDataPtr );
         sprintf(bufferString->line, "--------------------------------------------------\n");
         logString = addString(logString, bufferString);
         printForMointor(configDataPtr, bufferString );
         
      }

      headPtr = pcbPointer;
      tempPtr = getNextProcess(headPtr, configDataPtr, headPtr);

      // main loop
      while( tempPtr != NULL )
      {
       memory = initializeMemory(configDataPtr);
       saveProcessStart(tempPtr, timeStamp, bufferString, logString, configDataPtr );
       blockedPCB = saveProcessing(timeStamp, tempPtr, configDataPtr, bufferString, logString, memory, &intQ,  headPcb );
       if( !blockedPCB )
       {
         saveProcessEnd(timeStamp, tempPtr, bufferString, logString, configDataPtr);
       }
       tempPtr = getNextProcess( tempPtr, configDataPtr, headPtr);
      }

      accessTimer(LAP_TIMER, timeStamp );
      sprintf(bufferString->line, " %s, OS: System stop\n", timeStamp);
      logString = addString(logString, bufferString);
      printForMointor(configDataPtr, bufferString );

      accessTimer(LAP_TIMER, timeStamp );
      sprintf(bufferString->line, " %s, OS: Simulation end\n", timeStamp);
      logString = addString(logString, bufferString);
      printForMointor(configDataPtr, bufferString );

      logSimulator(logString, configDataPtr );
      

    }

bool interruptManager( PCBType *currentPcb, InterruptType **intQ, OutputString *logString, ConfigDataType* configPtr, PCBType* headPcb )
{
    double currentTime;
    char time[TIME_LENGTH];
    OutputString *bufferString = (OutputString*) malloc(sizeof(OutputString));
    PCBType* mainPCB = headPcb;

   bool blocked = false;
   PCBType* minNode = mainPCB;
   
   while( mainPCB != NULL )
   {
      if( mainPCB->state == BLOCKED_STATE )
      {
         currentTime = accessTimer(LAP_TIMER, time);
         if( mainPCB->finishedTime < currentTime )
         {
            blocked = true;
            if( mainPCB->finishedTime < minNode->finishedTime )
            {
               minNode = mainPCB;
            }
         }
      }
      mainPCB = mainPCB->nextPcbPtr;
   }

   if( blocked )
   {
      minNode->state = READY_STATE;
      minNode->finishedTime = 0;
      accessTimer(ZERO_TIMER, time); 
      sprintf(bufferString->line, "\n %s, OS: Interrupted by process %d, %s %sput operation\n", time, minNode->processId, minNode->appStartPtr->strArg1, minNode->appStartPtr->inOutArg );
      logString = addString(logString, bufferString);
      printForMointor(configPtr, bufferString );

      accessTimer(ZERO_TIMER, time); 
      sprintf(bufferString->line, " %s, OS: Process %d set from BLOCKED to READY\n", time, minNode->processId );
      logString = addString(logString, bufferString);
      printForMointor(configPtr, bufferString );

      minNode->appStartPtr = minNode->appStartPtr->nextNode;
   }

   return blocked;


}

bool checkPreemptive(ConfigDataType *configPtr)
{

   ConfigDataCodes algo = configPtr->cpuSchedCode;
   
   if( algo == CPU_SCHED_FCFS_N_CODE || algo == CPU_SCHED_SJF_N_CODE )
   {
      return false;
   }
   else if( algo == CPU_SCHED_FCFS_P_CODE || algo == CPU_SCHED_RR_P_CODE || algo == CPU_SCHED_SRTF_P_CODE )
   {
      return true;
   }

   return false;

}

bool processCPUCycles(ConfigDataType *configPtr, PCBType* currentPCB, InterruptType **intQ, OutputString *logString, PCBType* headPcb )
{
   int milliseconds;
   pthread_t cpuThread;
   OutputString* tempString = (OutputString*) malloc(sizeof(OutputString));
   int quantum = configPtr->quantumCycles;
   char time[TIME_LENGTH];
   bool interrupted = false;

   accessTimer(LAP_TIMER, time);
   
   // here we go cycle by cycle for cpu
   // take the cpu opcode

    // print the cpu cycle start first
    sprintf(tempString->line, " %s, Process: %d, %s process operation start\n", time, currentPCB->processId, currentPCB->appStartPtr->command );
    logString = addString(logString, tempString);
    printForMointor(configPtr, tempString );

   // get the io cycle time per cycle
   milliseconds = configPtr->ioCycleRate;

   while( currentPCB->appStartPtr->intArg2 != 0 && quantum != 0 && !interrupted )
   {
    // spend the time
    pthread_create(&cpuThread, NULL, &runTimer, (void*) &milliseconds);
    pthread_join(cpuThread, NULL);
   
    // after one cycle decrement it from the opcode
    currentPCB->appStartPtr->intArg2--;

    if( checkPreemptive(configPtr) ) 
    {
      quantum--;
      interrupted = interruptManager(currentPCB, intQ, logString, configPtr, headPcb );
    }
   }

   accessTimer(LAP_TIMER, time);

   // print the end of cycles
   if( quantum != 0 && !interrupted )
   {
      sprintf(tempString->line, " %s, Process: %d, %s process operation end\n", time, currentPCB->processId, currentPCB->appStartPtr->command );
      currentPCB->appStartPtr = currentPCB->appStartPtr->nextNode;
   }
   else if( quantum == 0 )
   {
      sprintf(tempString->line, " %s, OS: Process: %d quantum time out, cpu process operation end", time, currentPCB->processId );
   }
   logString = addString(logString, tempString);
   printForMointor(configPtr, tempString );

   return interrupted;
  
}


bool saveProcessing(char time[], PCBType *currentPcb, ConfigDataType *configDataPtr, OutputString *tempString, OutputString *logString, MemoryBlock *memory, InterruptType **intQ, PCBType* headpcb)
{
   int milliseconds;
   pthread_t thread;
   bool blocked = false;
   bool preemptive = checkPreemptive(configDataPtr);
   
   while( compareString(currentPcb->appStartPtr->command, "app" ) != STR_EQ && !blocked )
   {
      accessTimer(LAP_TIMER, time);
      currentPcb->state = changeState(currentPcb->state);
      if(compareString(currentPcb->appStartPtr->command, "dev") == STR_EQ )
         {
          sprintf(tempString->line, " %s, Process: %d, %s %sput operation start\n", time, currentPcb->processId, currentPcb->appStartPtr->strArg1, currentPcb->appStartPtr->inOutArg);
          if( preemptive )
          {
          logString = addString(logString, tempString);
          printForMointor(configDataPtr, tempString );
          // input operation start
          // set the state to blocked state
          currentPcb->state = BLOCKED_STATE;
          // store the time to the variable
          currentPcb->blockedTime = accessTimer(LAP_TIMER, time);
          sprintf(tempString->line, " %s, OS: Process %d blocked for %sput operation\n", time, currentPcb->processId, currentPcb->appStartPtr->inOutArg );
          logString = addString(logString, tempString);
          printForMointor(configDataPtr, tempString );
          sprintf(tempString->line, " %s, OS: Process %d set from RUNNING to BLOCKED\n", time, currentPcb->processId );
          logString = addString(logString, tempString);
          printForMointor(configDataPtr, tempString );
          currentPcb->finishedTime = currentPcb->blockedTime + (double)((configDataPtr->ioCycleRate*currentPcb->appStartPtr->intArg2)/1000.0);
          // send the next PCB using the next getNextProcess
          blocked = true;
          }
         }
      else if( compareString(currentPcb->appStartPtr->command, "cpu") == STR_EQ )
         {
          // go cycle by cycle
          // function tht does that and return blocked for interrupt 
            blocked = processCPUCycles(configDataPtr, currentPcb, intQ, logString, headpcb );
         }
      else
         {
          sprintf(tempString->line, " %s, Process: %d, %s %s request (%d, %d)\n", time, currentPcb->processId, currentPcb->appStartPtr->command, currentPcb->appStartPtr->strArg1, currentPcb->appStartPtr->intArg2, currentPcb->appStartPtr->intArg3);
         }
      if( !blocked && compareString(currentPcb->appStartPtr->command, "cpu" ) != STR_EQ )
      {
      logString = addString(logString, tempString);
      printForMointor(configDataPtr, tempString );
      milliseconds = calculateTime(currentPcb->appStartPtr, configDataPtr );
      pthread_create(&thread, NULL, &runTimer, (void*) &milliseconds);
      pthread_join(thread, NULL);
      accessTimer(LAP_TIMER, time);
      
      if( compareString(currentPcb->appStartPtr->command, "dev") == STR_EQ && !preemptive )
         {
          sprintf(tempString->line, " %s, Process: %d, %s %sput operation end\n", time, currentPcb->processId, currentPcb->appStartPtr->strArg1, currentPcb->appStartPtr->inOutArg);
         }
      else if( compareString( currentPcb->appStartPtr->command, "mem" ) == STR_EQ )
         {
          if(memoryAllocate(configDataPtr, currentPcb, &memory, logString ))
          {
            sprintf(tempString->line, " %s, Process: %d, successful %s %s request\n", time, currentPcb->processId, currentPcb->appStartPtr->command, currentPcb->appStartPtr->strArg1 );
          }
          else
          {
            sprintf(tempString->line, " %s, Process: %d, failed mem allocate request\n\n", time, currentPcb->processId );
            logString = addString(logString, tempString);
            printForMointor(configDataPtr, tempString );
            sprintf(tempString->line, " %s, OS: Segmentation fault, Process %d ended", time, currentPcb->processId );
            while( compareString(currentPcb->appStartPtr->nextNode->command, "app" ) != STR_EQ )
            {
               currentPcb->appStartPtr = currentPcb->appStartPtr->nextNode;
            }
          }
         }
      logString = addString(logString, tempString);
      printForMointor(configDataPtr, tempString );
         
      currentPcb->appStartPtr = currentPcb->appStartPtr->nextNode;
      }
      
   
   }

   sprintf(tempString->line, "\n");
   logString = addString(logString, tempString);
   printForMointor(configDataPtr, tempString );

   return blocked;


}




// ------------------------------------------    

PCBType *getNextProcess(PCBType *processPointer, ConfigDataType *configPtr, PCBType *processHead)
{  
   PCBType *tempPtr = processHead;
   int smallestTime = 0;
   PCBType *nextNode = NULL;
   
   if( configPtr->cpuSchedCode == CPU_SCHED_FCFS_N_CODE || configPtr->cpuSchedCode == CPU_SCHED_RR_P_CODE )
   {
      if( processPointer->processId == 0 && processPointer->state == READY_STATE )
      {
         return processPointer;
      }
      else if( processPointer != NULL )
      {
         processPointer = processPointer->nextPcbPtr;
         return processPointer;
      }
      
   }

   if( configPtr->cpuSchedCode == CPU_SCHED_FCFS_P_CODE )
   {
      while( tempPtr != NULL )
      {
         if( tempPtr->state == READY_STATE )
         {
            return tempPtr;
         }
         tempPtr = tempPtr->nextPcbPtr;
      }
   }

   if( configPtr->cpuSchedCode == CPU_SCHED_SJF_N_CODE || configPtr->cpuSchedCode == CPU_SCHED_SRTF_P_CODE )
   {
      while( tempPtr != NULL )
      {
         smallestTime += tempPtr->timeRemaining;
         tempPtr = tempPtr->nextPcbPtr;
      }
      tempPtr = processHead;
      
      while( tempPtr != NULL )
      {
         if( tempPtr->timeRemaining < smallestTime && tempPtr->state == READY_STATE )
         {
            smallestTime = tempPtr->timeRemaining;
            nextNode = tempPtr;
         }
         tempPtr = tempPtr->nextPcbPtr;
      }

      free(tempPtr);
      return nextNode;
   }

   return NULL;
}

bool memoryAllocate(ConfigDataType *configPtr, PCBType *pcb, MemoryBlock **memory, OutputString* logString )
{
   MemoryBlock *memoryWkgPtr = *memory, *newMemory;

   int reqStart = pcb->appStartPtr->intArg2;
   int reqSize = pcb->appStartPtr->intArg3 - 1;
   
   if( checkForLogical( memoryWkgPtr, reqStart, reqSize ) )
   {
      // find the last used
      while( memoryWkgPtr->nextBlock != NULL && memoryWkgPtr->nextBlock->nextBlock != NULL )
      {
         memoryWkgPtr = memoryWkgPtr->nextBlock;
      }

      // commmon
      newMemory = initializeMemory(configPtr);
      newMemory->used = true;
      newMemory->pcbID = pcb->processId;
      newMemory->logicalMin = reqStart;
      newMemory->logicalMax = reqStart + reqSize;

      if( !memoryWkgPtr->used )
      {
         // set the new memory block
         newMemory->physicalMin = 0;
         newMemory->physicalMax = reqSize;
         memoryWkgPtr->physicalMin = newMemory->physicalMax + 1;

         // add to the linked list
         newMemory->nextBlock = memoryWkgPtr;
         *memory = newMemory;

      }
      else
      {
         // set the newMemory Block
         newMemory->physicalMin = memoryWkgPtr->physicalMax + 1;
         newMemory->physicalMax = newMemory->physicalMin + reqSize;

         // update the open
         memoryWkgPtr->nextBlock->physicalMin = newMemory->physicalMax + 1;

         // join the link
         newMemory->nextBlock = memoryWkgPtr->nextBlock;
         memoryWkgPtr->nextBlock = newMemory;
         
      }
      
      if( configPtr->memDisplay )
      {
         printf("--------------------------------------------------\n");
      
         printf("After allocate success\n");

         displayMemoryBlocks(*memory, logString, configPtr );

         printf("--------------------------------------------------\n");
      }

      return true;
   }
   
   if( configPtr->memDisplay )
   {
      printf("--------------------------------------------------\n");
   
      printf("After allocate overlap failure\n");

      displayMemoryBlocks(*memory, logString, configPtr );

      printf("--------------------------------------------------\n");
   }

   return false;

}

void displayMemoryBlocks(MemoryBlock *memory, OutputString* logString, ConfigDataType* configPtr  )
{
    MemoryBlock *headPtr = memory;
    OutputString* bufferString = (OutputString*) malloc(sizeof(OutputString));
    
    while (headPtr != NULL)
    {
        if(headPtr->used)
        {
            sprintf(bufferString->line, "%d [ %s, P#: %d, %d-%d ] %d\n", headPtr->physicalMin, "Used", headPtr->pcbID, headPtr->logicalMin, headPtr->logicalMax, headPtr->physicalMax);
        }
        else
        {
            sprintf(bufferString->line, "%d [ %s, P#: x, %d-%d ] %d\n", headPtr->physicalMin, "Open", headPtr->logicalMin, headPtr->logicalMax, headPtr->physicalMax);
        }
        logString = addString(logString, bufferString);
        printForMointor(configPtr, bufferString );

        headPtr = headPtr->nextBlock;
    }

}

bool checkForLogical(MemoryBlock *memoryPtr, int maxReq, int minReq )
   {
    int upperLimit = minReq + maxReq - 1;
    bool available = true;
    int minUsed, maxUsed;
    MemoryBlock *tempPtr = memoryPtr;
      
    while( tempPtr != NULL )
       {
        if( tempPtr->used )
           {
            minUsed = tempPtr->logicalMin;
            maxUsed = tempPtr->logicalMax;
            if( minReq >= minUsed && upperLimit <= maxUsed )
               {
                available = false;
               }
           }
        tempPtr = tempPtr->nextBlock;
       }

      tempPtr = memoryPtr;

    return available && checkForPhysical( tempPtr, maxReq );
    
   }

bool checkForPhysical( MemoryBlock *memoryPtr, int maxReq )
{
   int blockSize = maxReq - 1;
   bool available = true;

   // check for open has enough space
   while( memoryPtr != NULL )
   {
      if( !memoryPtr->used )
      {
       if( blockSize >= (memoryPtr->physicalMax - memoryPtr->physicalMin - 1) )
          {
           available = false;
          }
      }
      memoryPtr = memoryPtr->nextBlock;
   }

   return available;
}

void clearMemory( ConfigDataType *configPtr, MemoryBlock **memoryPtr, PCBType *pcbPtr, OutputString* logString )
{
   *memoryPtr = initializeMemory(configPtr);
   
   if( configPtr->memDisplay )
   {
      printf("--------------------------------------------------\n");
      printf("After clear process %d success\n", pcbPtr->processId );
      displayMemoryBlocks(*memoryPtr, logString, configPtr );
      printf("--------------------------------------------------\n");
   }
   
}



MemoryBlock *initializeMemory(ConfigDataType *configDataPtr)
{
   MemoryBlock *memory = (MemoryBlock*) malloc(sizeof(MemoryBlock));
   
   memory->used = false;
   memory->pcbID = 0;
   memory->logicalMin = 0;
   memory->logicalMax = 0;
   memory->physicalMin = 0;
   memory->physicalMax = configDataPtr->memAvailable - 1;
   memory->nextBlock = NULL;

   return memory;
}


void logSimulator(OutputString *loggingString, ConfigDataType *configPtr )
{
   OutputString *tempString = loggingString;
   
   if( configPtr->logToCode == LOGTO_FILE_CODE || configPtr->logToCode == LOGTO_BOTH_CODE )
   {
      generateLogFile( configPtr, tempString );
   }
}

void saveProcessEnd( char time[], PCBType *currentPcb, OutputString *tempString, OutputString *logString, ConfigDataType *configPtr )
{
   sprintf(tempString->line, " %s, OS: Process %d ended\n", time, currentPcb->processId );
   logString = addString(logString, tempString);
   printForMointor(configPtr, tempString );
   sprintf(tempString->line, " %s, OS: Process %d set to EXIT\n", time, currentPcb->processId );
   logString = addString(logString, tempString);
   printForMointor(configPtr, tempString );
   currentPcb->state = changeState(currentPcb->state);
}

void saveProcessStart(PCBType *currentPcb, char time[], OutputString *tempString, OutputString *logString, ConfigDataType *configPtr )
{
   accessTimer(LAP_TIMER, time);
   sprintf(tempString->line, " %s, OS: Process %d selected with %d ms remaining\n", time, currentPcb->processId, currentPcb->timeRemaining );
   logString = addString(logString, tempString);
   printForMointor(configPtr, tempString );
   accessTimer(LAP_TIMER, time);
   sprintf(tempString->line, " %s, OS: Process %d set from READY to RUNNING\n\n", time, currentPcb->processId);
   currentPcb->state = changeState(currentPcb->state);
   logString = addString(logString, tempString);
   printForMointor(configPtr, tempString );
   currentPcb->appStartPtr = currentPcb->appStartPtr->nextNode;
}

ProcessState changeState(ProcessState state )
{
   if( state == NEW_STATE )
   {
      return READY_STATE;
   }
   if( state == READY_STATE )
   {
      return RUNNING_STATE;
   }
   
   return EXIT_STATE;
}

int calculateTime(OpCodeType *metaPtr, ConfigDataType *configPtr )
{
   int milliSeconds = 0;
   if( compareString( metaPtr->command, "dev" ) == STR_EQ )
   {
      milliSeconds = (configPtr->ioCycleRate*metaPtr->intArg2);
   }
   if( compareString( metaPtr->command, "cpu" ) == STR_EQ )
   {
      milliSeconds = (configPtr->procCycleRate*metaPtr->intArg2);
   }

   return milliSeconds;
}

PCBType *getPCB(ConfigDataType *configDataPtr, OpCodeType *metaDataPtr )
{
   PCBType *pcbPointer= NULL, *tempPtr, *headPtr = NULL;
   int processID = 0;

   OpCodeType *wkgPtr = metaDataPtr;
   
   while( wkgPtr != NULL )
        {
          if( compareString( wkgPtr->command,"app") == STR_EQ && compareString( wkgPtr->strArg1, "start" ) == STR_EQ )
          {
            tempPtr = (PCBType *) malloc( sizeof( PCBType ) );
            tempPtr->appStartPtr = wkgPtr;
            tempPtr->processId = processID;
            processID++;
            tempPtr->timeRemaining = 0;
            tempPtr->state = NEW_STATE;

            wkgPtr = wkgPtr->nextNode;

            while( compareString( wkgPtr->command, "app" ) != STR_EQ )
            {
               tempPtr->timeRemaining += calculateTime(wkgPtr, configDataPtr);
               wkgPtr = wkgPtr->nextNode;
            }

            if( processID == 1 )
            {
               headPtr = tempPtr;
               pcbPointer = headPtr;
            }
            else
            {
               headPtr->nextPcbPtr = tempPtr;
               headPtr = headPtr->nextPcbPtr;
            }

           tempPtr = tempPtr->nextPcbPtr;
           tempPtr = NULL;
          }
         wkgPtr = wkgPtr->nextNode;
        }

 return pcbPointer;
}

OutputString *addString( OutputString* logString, OutputString *tempString )
      {
         if( logString == NULL )
         {
            logString = (OutputString*) malloc(sizeof(OutputString));

            copyString(logString->line, tempString->line );

            logString->nextLine = NULL;

            return logString;
         }


         logString->nextLine = addString( logString->nextLine, tempString );

         return logString;
      }

void generateLogFile( ConfigDataType *configPtr, OutputString *simulatorString )
   {
      FILE *fileName;
      char writeCharacter[] = "w";

      fileName = fopen(configPtr->logToFileName, writeCharacter );

      if( fileName == NULL )
      {
         printf("Failed to open file\n");
      }
         
      fprintf(fileName, "\n==================================================\n");

      fprintf(fileName, "Simulator Log File Header\n\n");

      fprintf(fileName, "%-32s: %s\n", "File Name", configPtr->metaDataFileName);

      fprintf(fileName, "%-32s: %s\n", "CPU Scheduling", "FCFS-N" );

      fprintf(fileName, "%-32s: %d\n", "Quantum Cycles", configPtr->quantumCycles );

      fprintf(fileName, "%-32s: %d\n", "Memory Available (KB)", configPtr->memAvailable );

      fprintf(fileName, "%-32s: %d\n", "Processor Cycle Rate (ms/cycle)", configPtr->procCycleRate );

      fprintf(fileName, "%-32s: %d\n\n", "I/O Cycle Rate (ms/cycle)", configPtr->ioCycleRate );

      fprintf(fileName, "================\n");

      fprintf(fileName, "Begin Simulation\n\n");

      while( simulatorString != NULL )
      {
         fprintf(fileName, "%s", simulatorString->line);
         simulatorString = simulatorString->nextLine;
      }

      fprintf(fileName, "\nEnd Simulation - Complete\n");

      fprintf(fileName, "=========================\n");

      fclose(fileName);

   }

void printForMointor(ConfigDataType *configPtr, OutputString *loggingString )
{

   if( configPtr->logToCode == LOGTO_MONITOR_CODE || configPtr->logToCode == LOGTO_BOTH_CODE )
   {
      printf("%s", loggingString->line );
   }
}









