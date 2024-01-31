// protect from multiple compiling
#ifndef METADATAOPS_H
#define METADATAOPS_H

// header files
#include "datatypes.h"
#include "StandardConstants.h"
#include "StringUtils.h"

// constants 
typedef enum { BAD_ARG_VAL = -1,
               NO_ACCESS_ERR,
               MD_FILE_ACCESS_ERR,
               MD_CORRUPT_DESCRIPTOR_ERR,
               OPCMD_ACCESS_ERR,
               CORRUPT_OPCMD_ERR,
               CORRUPT_OPCMD_ARG_ERR,
               UNBALANCED_START_END_ERR,
               COMPLETE_OPCMD_FOUND_MSG,
               LAST_OPCMD_FOUND_MSG } OpCodeMessages;

// op code struct to store information related to opcode
typedef struct OpCodeTypeStruct
   {
    int pid;
    char command[ STD_STR_LEN ];
    char inOutArg[ STD_STR_LEN ];
    char strArg1[ STD_STR_LEN ];
    int intArg2;
    int intArg3;
    double opEndTime;
    struct OpCodeTypeStruct* nextNode;
   } OpCodeType;

// function prototypes

/*
Name: addNode
Process: adds metadata node to Linked List recursively,
handles enpty List condition
Function Input/Paraneters: pointer to head or next Linked node (OpCodeType *),
pointer to new node (OpCodeType *)
Function Output/Parameters: none
Function Output/Returned: pointer to previous node, or head node (OpCodeType *)
Device Input/device: none
Device Output/device: none
Dependencies: malloc, copyString
*/
OpCodeType *addNode( OpCodeType *localPtr, OpCodeType *newNode );

/*
Name: cleartietaDataList
Process: recursively traverses List, frees dynamically allocated nodes
Function Input/Parameters: node op code (const OpCodeType *)
Function Qutput/Parameters: none
Function Output/Returned: NULL (OpCodeType *)
Device Input/device: none
Device Output/device: none
Dependencies: free
*/
OpCodeType *clearMetaDataList( OpCodeType *localPtr );

/*
Name: displayMetaData
Process: data dump/display of all op code items
Function Input/Parameters: pointer to head
                           of op code/metadata list (const OpCodeType *)
Function Output/Parameters: none
Function Output/Returned: calculated time (int)
Device Input/device: none
Device Output/device: none
Dependencies: printf, compareString
*/
void displayMetaData( const OpCodeType *localPtr );

/*
Name: getCommand
Process: parses three Letter command part of op code string
Function Input/Paraneters: input op code string (const char *),
                           starting index (int)
Function Output/Paraneters: parsed command (char *)
Function Qutput/Returned: updated starting index for use
                          by calling function (int)
Device Input/device: none 
Device Output/device: none
Dependencies: none
*/
int getCommand( char *cmd, const char *inputStr, int index );

/*
Name: getNetaData
Process: main driver function to upload, parse, and store List
         of op code commands in a Linked List
Function Input/Paraneters: file name (const char *)
Function Qutput/Paraneters: pointer
                            to op code Linked List head pointer (OpCodeType **),
                            result messoge of function state
                            after completion (char *)
Function Qutput/Returned: Boolean result of operation (bool)
Device Input/file: op code List uploaded
Device Output/device: none
Dependencies: copyString, fopen, getStringToDelimiter, compareString, fclose,
              malloc, getOpConmand, ugdateStartCount, updateEndCount,
              cleanMetaDataList, addNode
*/
bool getMetaData( const char *fileName,
                                OpCodeType **opCodeDataHead, char *endStateMsg );

/*
Name: getNumberArg
Process: starts at given index, captures and assenbles integer argunent,
         and returns as parameter
Function Input/Paraneters: input string (const char *), starting index (int)
Function Output/Paraneters: pointer to captured integer value
Function Qutput/Returned: updated index for next function start
Device Input/device: none
Device Output/device: none
Dependencies: isDigit
*/
int getNumberArg( int *number, const char *inputStr, int index );

/*
Name: getOpConmand
Process: acquires one op connand Line from a previously opened file,
         parses it, and sets various struct members according
         to the three Letter conmand
Function Input/Paraneters: pointer to open file handle (FILE *)
Function Output/Parameters: pointer to one op code strcut (OpCodeType *)
Function Output/Returned: coded result of operation (OpCodeMessoges)
Device Input/fite: op code Line uploaded
Dependencies: getStringToDelimiter, getCommand, copyString, verifyValidCommand,
              CompareString, getStringArg, verifyFirstStringArg, getNumberArg
*/
OpCodeMessages getOpCommand( FILE *filePtr, OpCodeType *inData );

/*
Name: getStringArg
Process: starts at given index, captures and assembles string argument,
         and returns as parameter
Function Input/Parameters: input string (const char *), starting index (int)
Function Output/Parameters: pointer to captured string argument (char *)
Function Output/Returned: updated index for next function start
Device Input/device: none
Device Output/device: none
Dependencies: none
*/
int getStringArg( char *strArg, const char *inputStr, int index );

/*
Name: isDigit
Process: tests character paraneter for digit, returns true if is digit,
         false otherwise
Function Input/Parameters: test character (char)
Function Qutput/Paraneters: none
Function Output/Returned: Boolean result of test (bool)
Device Input/device: none
Device Output/device: none
Dependencies: none
*/
bool isDigit( char testChar );

/*
Name: updateEndCount
Process: manages count of “end arguments to be compared at end
         of process input
Function Input/Paraneters: initial count (int),
                           test string for “end” (const char *)
Function Output/Paraneters: none
Function Output/Returned: updated count, if “end” string found,
                          otherwise no change
Device Input/device: none
Device Output/device: none
Dependencies: compareString
*/
int updateEndCount( int count, const char *opString );

/*
Name: updateStartCount
Process: manages count of “start” arguments to be compared at end
         of process input
Function Input/Parameters: initial count (int),
                           test string for “start” (const char *)
Function Output/Paraneters: none
Function Output/Returned: updated count, if “start” string found,
                          otherwise no change
Device Input/device: none
Device Output/device: none
Dependencies: compareString
*/
int updateStartCount( int count, const char *opString );

/*
Name: verifyFirstStringArg
Process: checks for all possibilities of first argument string of op command
Function Input/Parameters: test string (const char *)
Function Output/Paraneters: none
Function Output/Returned: Boolean result of test (bool)
Device Input/device: none
Device Output/device: none
Dependencies: compareString
*/
bool verifyFirstStringArg( const char *strArg );

/*
Name: verifyValidConmand
Process: checks for all possibilities of three-letter op code command
Function Input/Paraneters: test string for conmand (const char *)
Function Qutput/Paraneters: none
Function Output/Returned: Boolean result of test (bool)
Device Input/device: none
Device Output/device: none
Dependencies: compareString
*/
bool verifyValidCommand( char *testCmd );


#endif
