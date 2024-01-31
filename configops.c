// header file
#include "configops.h"


// function prototypes

/*
Name: cleanConfigData
Process: frees dynamically allocated config data structure
         if it has not already been freed
Function Input/Parameters: pointer to config data structure (ConfigDataType *)
Function Output/Parameters: none
Function Output/Returned: NULL (ConfigDataType *)
Device Input/device: none
Device Output/device: none
Dependencies: free
*/
ConfigDataType *clearConfigData( ConfigDataType *configData )
   {
    // check that config data pointer is not null
    if( configData != NULL )
       {
        // free data strucutre memory
            // function: free
        free( configData );

        // set config pointer to NULL
        configData = NULL;
       }

    // return NULL config data pointer
    return NULL;
   }

/*
Name: configCodeToString
Process: utility function converts configuration code numbers
         to the string they represent
Function Input/Paraneters: configuration code (int)
Function Output/Paraneters: resulting output string (char *)
Function Output/Returned: none
Device Input/device: none
Device Output/monitor: none
Dependencies: copyString
*/
void configCodeToString( int code, char *outString )
   {
    // Define array with eight times, and short (10) lengths
    char displayStrings[ 8 ][ 10 ] = { "SJF-N", "SRTF-P", "FCFS-P",
                                       "RR-P", "FCFS-N", "Monitor",
                                       "File", "Both"};

    // copy string to return parameter
         // function: copyString
    copyString( outString, displayStrings[ code ] );
   }

/*
Name: displayConfigdata
Process: screen dunp/display of all config data
Function Input/Paraneters: pointer to config data structure (ConfigataType *)
Function Output/Parameters: none
Function Qutput/Returned: none
Device Input/device: none
Device Output/nonitor: displayed as specified
Dependencies: configCodeTostring, printf
*/
void displayConfigData( ConfigDataType *configData )
   {
    // initialize function/variables
    char displayString[ STD_STR_LEN ];

    // print lines of display for all member values
         // function: printf, codeToString (translates coded items)
    printf( "Config File Display\n" );
    printf( "-------------------\n" );
    printf( "Version                : %3.2f\n", configData->version );
    printf( "Program file name      : %s\n", configData->metaDataFileName );
    configCodeToString( configData->cpuSchedCode, displayString );
    printf( "CPU schedule selection : %s\n", displayString );
    printf( "Quantum time           : %d\n", configData->quantumCycles );
    printf( "Memory Display         : " );
    if( configData->memDisplay )
       {
        printf( "On\n" );
       }
    else
       {
        printf( "Off\n" );
       }
    printf( "Memory Available       : %d\n", configData->memAvailable );
    printf( "Process cycle rate     : %d\n", configData->procCycleRate );
    printf( "I/O cycle rate         : %d\n", configData->ioCycleRate );
    configCodeToString( configData->logToCode, displayString );
    printf( "Log to selection       : %s\n", displayString );
    printf( "Log file name          : %s\n\n", configData->logToFileName );
   }

/*
Name: getConfigdata
Process: driver function for capturing configuration data from a config file
Function Input/Parameters: file name (const char *)
Function Output/Parameters: pointer to config data pointer (ConfigdataType **),
end/result state message pointer (char *)
Function Output/Returned: Boolean result of data access operation (bool)
Device Input/file: config data uploaded
Device Output/device: none
Dependencies: copyString, fopen, getStringToDelimiter, compareString,
              fclose, malloc, free, stripTraillingSpaces, getDataLineCode,
              fscanf, setStrToLowerCase, valueInRange, getCpuSchedCode,
              getLogToCode 
*/
bool getConfigData( const char *fileName,
                                ConfigDataType **configData, char *endStateMsg )
   {
    // initialize function/variables

         // set constant number of data lines
         const int NUM_DATA_LINES = 10;

         // set read only constant
         const char READ_ONLY_FLAG[] = "r";

         // create pointer for data input
         ConfigDataType *tempData;

         // declare other variables
         FILE *fileAccessPtr;
         char dataBuffer[ MAX_STR_LEN ], lowerCaseDataBuffer[ MAX_STR_LEN ];
         int intData, dataLineCode, lineCtr = 0;
         double doubleData;

    // set endStateMsg to success
         // function: copyString
    char testMsg[] = "Configuration file upload successful";
    copyString( endStateMsg, testMsg );

    // initialize config data pointer in case of return error
    *configData = NULL;

    // open file
         // function: fopen
    fileAccessPtr = fopen( fileName, READ_ONLY_FLAG );

    // check for file open failure
    if( fileAccessPtr == NULL )
       {
        // set end state message to config file access error
            // function: copyString
        char testMsg[] = "Configuration file access error";
        copyString( endStateMsg, testMsg );

        // return file access error
        return false;
       }

    // get first line, check for failure
         // function: getStringToDeLimiter, compareString
    if( !getStringToDelimiter( fileAccessPtr, COLON, dataBuffer )
        || compareString( dataBuffer,
                              "Start Simulator Configuration File" ) != STR_EQ )
       {
        // close file access
            // function: fclose
        fclose( fileAccessPtr );

        // set end state message to corrupt leader line error
            // function: copyString
        copyString( endStateMsg, "Corrupt configuration leader line error" );

        // return corrupt file data
        return false;
       }

    // create temporary pointer to configuration data structure
         // function: malloc
    tempData = (ConfigDataType *) malloc( sizeof( ConfigDataType ) );

    // loop to end of config data items
    while( lineCtr < NUM_DATA_LINES )
       {
        // get line leader, check for failure
            // function: getStringToDelimiter
        if( !getStringToDelimiter( fileAccessPtr, COLON, dataBuffer ) )
            {
             // free temp struct memory
                  // function: free
             free( tempData );

             // close file access
                  // function: fclose
             fclose( fileAccessPtr );

             //set end state message to line capture
                  // function: copyString
             copyString( endStateMsg,
                               "Configuration data leader line capture error" );
             // return incomplete file error
             return false;
            }

        // strip trailling spaces of config leader lines
        // for use in other functions
            // function: stripTrailingSpaces
        stripTrailingSpaces( dataBuffer );

        // find correct data line code number from string
            // function: getDataLineCode
        dataLineCode = getDataLineCode( dataBuffer );

        // check for data line found
        if( dataLineCode != CFG_CORRUPT_PROMPT_ERR )
           {
            // get data value

               // check for version number (double value)
               if( dataLineCode == CFG_VERSION_CODE )
                  {
                   // get version number
                        // function: scanf
                   fscanf( fileAccessPtr, "%lf", &doubleData );
                  }

               // otherwise, check for metaData, file names,
               //   CPU scheduling names, or memory display setting (strings)
               else if( dataLineCode == CFG_MD_FILE_NAME_CODE 
                        || dataLineCode == CFG_LOG_FILE_NAME_CODE
                           || dataLineCode == CFG_CPU_SCHED_CODE
                              || dataLineCode == CFG_LOG_TO_CODE
                                 || dataLineCode == CFG_MEM_DISPLAY_CODE )
                  {
                   // get string input
                        // function: fscanf
                   fscanf( fileAccessPtr, "%s", dataBuffer );

                   // set string to lower case for testing in valueInRange
                        // function: setStrToLowerCase
                   setStrToLowerCase( lowerCaseDataBuffer, dataBuffer );
                  }

               // otherwise, assume integer data
               else
                  {
                   // get integer input
                        // function: fscanf
                   fscanf( fileAccessPtr, "%d", &intData );
                  }   
            // check for data value in range
                  // function: valueInRange
            if( valueInRange( dataLineCode, intData,
                                             doubleData, lowerCaseDataBuffer ) )
               {
                // assign to data pointer depending on config item
                // (all config line possibilities)
                     // function: copyString, getCpuSchedCode,
                     //           compareString, getLogToCode, as needed
                switch( dataLineCode )
                  {
                   case CFG_VERSION_CODE:

                        tempData->version = doubleData;
                        break;

                   case CFG_MD_FILE_NAME_CODE:

                        copyString( tempData->metaDataFileName, dataBuffer );
                        break;

                   case CFG_CPU_SCHED_CODE:

                        tempData->cpuSchedCode
                                       = getCpuSchedCode( lowerCaseDataBuffer );
                        break;

                   case CFG_QUANT_CYCLES_CODE:

                        tempData->quantumCycles = intData;
                        break;

                   case CFG_MEM_DISPLAY_CODE:

                        tempData->memDisplay
                              = compareString( lowerCaseDataBuffer, "on" ) == 0;
                        break;

                   case CFG_MEM_AVAILABLE_CODE:

                        tempData->memAvailable = intData;
                        break;

                   case CFG_PROC_CYCLES_CODE:

                        tempData->procCycleRate = intData;
                        break;

                   case CFG_IO_CYCLES_CODE:

                        tempData->ioCycleRate = intData;
                        break;

                   case CFG_LOG_TO_CODE:

                        tempData->logToCode
                                          = getLogToCode( lowerCaseDataBuffer );
                        break;

                   case CFG_LOG_FILE_NAME_CODE:

                        copyString( tempData->logToFileName, dataBuffer );
                        break;
                  }
               }

            // otherwise, assume data value not in range
            else
               {
                // free temp struct memory
                     // function: free
                free( tempData );

                // close file access
                     // function: fclose
                fclose( fileAccessPtr );

                // set end state message to configuration data out of range
                     // function: copyString
                copyString( endStateMsg, "Configuration item out of range" );

                // return data out of range
                return false;
               }
           }
         // otherwise, assume data line not found
         else
            {
             // free temp struct memory
             free( tempData );

             // close file access
             fclose( fileAccessPtr );

             // set end state message to configuration corrupt prompt error
                  // function: copyString
             copyString( endStateMsg, "Corrupted configuration prompt" );

             // return corrupt config file code
             return false;
            }
            
         // increment line counter
         lineCtr++;            
       }
    // end master loop

    // acquire end of sim config string
         // function: getStringToDelimiter, compareString
    if( !getStringToDelimiter( fileAccessPtr, PERIOD, dataBuffer )
        || compareString( dataBuffer,
                                    "End Simulator Configuration File" )
                                                                     != STR_EQ )
       {
        // free temp struct memory
            // function: free
        free( tempData );

        // close file access
            // function: fclose
       fclose( fileAccessPtr );

       // set end state message to corrupt configuration end line
            // function: copyString
        copyString( endStateMsg, "Configuration end line capture error" );

        // return corrupt file data
        return false;
       }

    // test for "file only" output so memory diagnostics do not display
    tempData->memDisplay = tempData->memDisplay
                                      && tempData->logToCode != LOGTO_FILE_CODE;

    // assign temporary pointer to parameter return pointer
    *configData = tempData;

    // close file access
         // function: fclose
    fclose( fileAccessPtr );

    // return no error code
    return true;
   }

/*
Name: getCpuSchedCode
Process: converts cpu schedule string to code (all scheduling possibilities)
Function Input/Parameters: Lower case code string (const char *)
Function Output/Parameters: none
Function Output/Returned: cpu schedule code (ConfigDataCodes)
Device Input/device: none
Device Output/device: none
Dependencies: compareString
*/
ConfigDataCodes getCpuSchedCode( const char *lowerCaseCodeStr )
   {
    // initialize variables/function

         // set defualt return to FCFS_N
         ConfigDataCodes returnVal = CPU_SCHED_FCFS_N_CODE;

    // check for SJF-N
         // function: compareString
    if( compareString( lowerCaseCodeStr, "sjf-n" ) == STR_EQ )
       {
        // set SJG-N code
        returnVal = CPU_SCHED_SJF_N_CODE;
       }

    // check for SRTF-P
         // function: compareString
    if( compareString( lowerCaseCodeStr, "srtf-p" ) == STR_EQ )
       {
        // set SRTF-P code
        returnVal = CPU_SCHED_SRTF_P_CODE;
       }

    // check for FCFS-P
         // function: compareString
    if( compareString( lowerCaseCodeStr, "fcfs-p" ) == STR_EQ )
       {
        // set FCFS-P code
        returnVal = CPU_SCHED_FCFS_P_CODE;
       }

    // check for RR-P
         // function: compareString
    if( compareString( lowerCaseCodeStr, "rr-p" ) == STR_EQ )
       {
        // set RR-P code
        returnVal = CPU_SCHED_RR_P_CODE; 
       }

    // return the selected value
    return returnVal;
   }
   
/*
Name: getDataLineCode
Process: converts Leader Line string to configuration code value
         (alt config file leader Lines)
Function Input/Parameters: config Leader Line string (const char *)
Function Output/Parameters: none
Function Output/Returned: configuration code value (ConfigCodeMessages)
Device Input/device: none
Device Output/device: none
Dependencies: compareString
*/
ConfigCodeMessages getDataLineCode( const char *dataBuffer )
   {
    // return appropriate code depending on prompt string provided
         // function: compareString
    if( compareString( dataBuffer, "Version/Phase" ) == STR_EQ )
       {
        return CFG_VERSION_CODE;
       }

    if( compareString( dataBuffer, "File Path" ) == STR_EQ )
       {
        return CFG_MD_FILE_NAME_CODE;
       }

    if( compareString( dataBuffer, "CPU Scheduling Code" ) == STR_EQ )
       {
        return CFG_CPU_SCHED_CODE;
       }

    if( compareString( dataBuffer, "Quantum Time (cycles)" ) == STR_EQ )
       {
        return CFG_QUANT_CYCLES_CODE;
       }

    if( compareString( dataBuffer, "Memory Display (On/Off)" ) == STR_EQ )
       {
        return CFG_MEM_DISPLAY_CODE;
       }

    if( compareString( dataBuffer, "Memory Available (KB)" ) == STR_EQ )
       {
        return CFG_MEM_AVAILABLE_CODE;
       }

    if( compareString( dataBuffer, "Processor Cycle Time (msec)" ) == STR_EQ )
       {
        return CFG_PROC_CYCLES_CODE;
       }

    if( compareString( dataBuffer, "I/O Cycle Time (msec)" ) == STR_EQ )
       {
        return CFG_IO_CYCLES_CODE;
       }

    if( compareString( dataBuffer, "Log To" ) == STR_EQ )
       {
        return CFG_LOG_TO_CODE;
       }

    if( compareString( dataBuffer, "Log File Path" ) == STR_EQ )
       {
        return CFG_LOG_FILE_NAME_CODE;
       }

    // return corrupt leader line error code
    return CFG_CORRUPT_PROMPT_ERR;
   }

/*
Name: getLogToCode
Process: converts "log to" text to configuration data code
Function Input/Parameters: lower case log to string (const char *)
*/
ConfigDataCodes getLogToCode( const char *lowerCaseLogToStr )
   {
    // initialize function/variables

         // set default to log to monitor
         ConfigDataCodes returnVal = LOGTO_MONITOR_CODE;

    // check for BOTH
         // function: compareString
    if( compareString( lowerCaseLogToStr, "both" ) == STR_EQ )
       {
        // set return value to both code
        returnVal = LOGTO_BOTH_CODE;
       }

    // check for FILE
         // function: compareString
    if( compareString( lowerCaseLogToStr, "file" ) == 0 )
       {
        // set value to file code
        returnVal = LOGTO_FILE_CODE;
       }
       
    return returnVal;
   }

/*
Name: stripTraiLingSpaces
Process: renoves trailing spaces from input config Leader Lines
Function Input/Paraneters: config Leader Line string (char *)
Function Qutput/Paraneters: updated config Leader Line string (char *)
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: getStringlength
*/
void stripTrailingSpaces( char *str )
   {
   // initialize index to length of string - 1 to get highest array index
   int index = getStringLength( str ) - 1;

   // loop while space is found at end of string
   while( str[index] == SPACE )
      {
         // set element to NULL_CHAR
         str[ index ] = NULL_CHAR;

         // decrement index
         index--;
      }
   // end loop from end of string
   }

/*
Name: valueInRange
Process: checks for config data balues in range, including string values
         (att config data values)
Function Input/Paraneters: Line code number for specific config value (int),
                           integer value, as needed (int),
                           double value, as needed (double),
                           string value, as needed (const char *)
Function Output/Paraneters: none
Function Output/Returned: Boolean result of range test (bool)
Device Tnput/device: none
Device Output/device: none
Dependencies: compareString
*/
bool valueInRange( int lineCode, int intVal,
                              double doubleVal, const char *lowerCaseStringVal )
   {
   // initialize function/variables

         // set result to true, all tests are to find false
         bool result = true;

   // use line code to identify prompt line
   switch( lineCode )
      {
       // for version code
       case CFG_VERSION_CODE:

         // check if limits of version code are executed
         if( doubleVal < 0.00 || doubleVal > 10.00 )
            {
             // set boolean result to false
             result = false;
            }
         // break
         break;

       // for cpu scheduling code
       case CFG_CPU_SCHED_CODE:

         // check for not finding one of the scheduling strings
            // function: compareString
         if( compareString( lowerCaseStringVal, "fcfs-n" ) != STR_EQ 
             && compareString( lowerCaseStringVal, "sjf-n" ) != STR_EQ 
             && compareString( lowerCaseStringVal, "srtf-p" ) != STR_EQ 
             && compareString( lowerCaseStringVal, "fcfs-p" ) != STR_EQ 
             && compareString( lowerCaseStringVal, "rr-p" ) != STR_EQ )
            {
             // set Boolean result to false
             result = false;
            }

         // break
         break;

       // for quantum cycles
       case CFG_QUANT_CYCLES_CODE:

         // check for quantum cycles limits exceeded
         if( intVal < 0 || intVal > 100 )
            {
             // set Boolean result to false
             result = false;
            }

         // break
         break;

       // for memory display
       case CFG_MEM_DISPLAY_CODE:

         // check for not finding either "on" or "off"
            // function: compareString
         if( compareString( lowerCaseStringVal, "on" ) != STR_EQ 
             && compareString( lowerCaseStringVal, "off" ) != STR_EQ )
            {
             // set Boolean result to false
             result = false;
            }

         // break
         break;

       // for memory available
       case CFG_MEM_AVAILABLE_CODE:

         // check for available memory limits exceeded
         if( intVal < 1024 || intVal > 102400 )
            {
             // set Boolean result to false
             result = false;
            }

         // break
         break;

       // check for process cycles
       case CFG_PROC_CYCLES_CODE:

         // check for process cycles limits exceeded
         if( intVal < 1 || intVal > 100 )
            {
             // set Boolean result to false
             result = false;
            }

         // break
         break;

       // check for I/O cycles
       case CFG_IO_CYCLES_CODE:

         // check for I/O cycles limit exceeded
         if( intVal < 1 || intVal > 100 )
            {
             // set Boolean result to false
             result = false;
            }
         
         // break
         break;

       // check for log to operation
       case CFG_LOG_TO_CODE:

         // check for not finding one of the log strings
            // function: compareString
         if( compareString( lowerCaseStringVal, "both" ) != STR_EQ 
             && compareString( lowerCaseStringVal, "monitor" ) != STR_EQ 
             && compareString( lowerCaseStringVal, "file" ) != STR_EQ )
            {
             // set Boolean result to false
             result = false;
            }

         // break
         break;
      }

   // return result of limit analysis
   return result;
   }

