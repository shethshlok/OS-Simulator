// protect from multiple compiling
#ifndef OS_SIM_DRIVER_H
#define OS_SIM_DRIVER_H

// header files
#include <stdio.h>
#include "datatypes.h"
#include "StringUtils.h"
#include "configops.h"
#include "metadataops.h"
#include "simulator.h"

// program constants
typedef enum { MIN_NUM_ARGS = 3, LAST_FOUR_LETTERS = 4 } PRGRM_CONSTANTS;

// command line struct for storing command line switch settings
typedef struct CmdLineDataStruct
   {
     bool programRunFlag;
     bool configDisplayFlag;
     bool mdDisplayFlag;
     bool runSimFlag;

     char fileName[ STD_STR_LEN ];
   } CmdLineData;

// Function prototypes

/*
Name: clearCmdlineStruct
Process: sets command Line structure data to defaults
         Booleans to false, fileName to empty string
Function Input/Parameters: pointer to command Line structure (CmdlineData *)
Function Output/Parameters: pointer to command Line structure (CmdlineData *)
                            with updated members
Function Qutput/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: none
*/
void clearCmdLineStruct( CmdLineData *clDataPtr );

/*
Name: processCmdLine
Process: checks for at least two argument,
         then sets booleans depending on command line switches
         which can be in any order,
         also captures config file name which must be the last argument
Function Input/Parameters: numberer of arguments (int)
                           vector of arguments (char *)
Function Output/Parameters: pointer to command line structure (CmdLineData *)
                            with updated members,
                            set to default values if failure
                            to capture arguments
Function Output/Returned: Boolean result of argument capture,
                          true if at least one switch and config file name,
                          false otherwise
Device Input/device: none
Device Output/device: none
Dependencies: tbd
*/
bool processCmdLine( int numArgs, char **strVector, CmdLineData *clDataPtr );

/*
Name: showCommandLineFormat
Process: displays command line format as assistance to user
Function Input/Paraneters: none
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/monitor: data displayed as specified
Dependencies: printf
*/
void showCommandLineFormat();

#endif 
