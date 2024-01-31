// Preprocessor directive
#ifndef STRING_UTLIS_H
#define STRING_UTLIS_H

//Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "StandardConstants.h"

// function prototypes
void cancatenatesString( char *destStr, const char *sourceStr );
int compareString( const char *onestr, const char *otherstr );
void copyString( char *destStr, const char *sourceStr );
int findSubString( const char *testStr, const char *searchSubStr );
bool getStringConstrained(
                          FILE *inStream,
                          bool clearLeadingNonPrintable,
                          bool clearLeadingSpace,
                          bool stopAtNonPrintable,
                          char delimiter,
                          char *capturedString
                         );
int getStringLength( const char *testStr );
bool getStringToDelimiter(
    FILE *inStream,
    char delimiter,
    char *capturedString);
bool getStringToLineEnd(
                        FILE *inStream,
                        char *capturedString
                       );
void getSubString( char *destStr, const char *sourceStr, 
                                          int startIndex, int endIndex );
void setStrToLowerCase( char *destStr, const char *sourceStr );
char toLowerCase( char testChar );


#endif  // end STRINGUTLIS_H
