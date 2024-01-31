// header files
#include "StringUtils.h"

// local constants
   // none

// function implementations 

/*
Name: compareString
Process: compares two strings with the following results:
          if left string less than right string, returns less than zero
          if left string greater than right string, returns greater than zero
          if left string equals right string, returns zero
          - equals test includes length
Function Input/Parameters: c-style left and right strings (char *)
Function Output/Parameters: none
Function Output/Returned: result as specified (int)
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getstringLength
*/

int compareString( const char *oneStr, const char *otherStr )
   {
     // initialize function/variable
     int diff, index = 0;

     // loop to end of shortest string,
     // with overrun protection
     while( oneStr[ index ] != NULL_CHAR && otherStr[ index ] != NULL_CHAR 
                                                && index < MAX_STR_LEN )
        {
          // get difference in characters
          diff = oneStr[ index ] - otherStr[ index ];

          // check for difference between characters
          if( diff != 0 )
             {
              // return difference
              return diff;
             }

          // increment index
          index++;
         }
      // end loop

     // return differnece in lengths, if any
         // function: getStringLength
     return ( getStringLength(oneStr) - getStringLength(otherStr) );
   }


/*
Name: concatenatestring
Process: appends one string onto another
Function Input/Parameters: c-style source string (char *)
Function Output/Parameters: c-style destination string (char *)
Function Output/Returned:
                           none
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getstringLength
*/

void cancatenatesString( char *destStr, const char *sourceStr )
   {
    // initialize function/variables

       // set destination index
          // function: getStringLength
       int destIndex = getStringLength( destStr );

       // get source string length
          // function: getStringLength
       int sourceStrLen = getStringLength( sourceStr );

       // create temporary string pointer
       char *tempStr;

       // create other variables
       int sourceIndex = 0;

    // copy source string in case of aliasing
       // function: malloc, copyString
    tempStr = (char *)malloc(sizeof( sourceStrLen + 1 ) );
    copyString( tempStr, sourceStr );

    // loop to end of source string
    while( tempStr[ sourceIndex ] != NULL_CHAR && destIndex < MAX_STR_LEN )
       {
        // assign characters to end o destination string
        destStr[ destIndex ] = tempStr[ sourceIndex ];

        // update indices
        destIndex++; sourceIndex++;

        // set temporary end of destination string
        destStr[ destIndex ] = NULL_CHAR;
       }
    //end loop

    // release memory used for temp string
       // function: free
    free( tempStr );
   }

/*
Name: copyString
Process: copies one string into another,
          overwriting data in the destination string
Function Input/Parameters: c-style source string (char *)
Function Output/Parameters: c-style destination string (char *)
Function Output/Returned: none
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getstringLength
*/
void copyString( char *destStr, const char *sourceStr )
   {
    // initialize functions/variables
    int index = 0;

    // check for source/destination not the same (aliasing)
    if( destStr != sourceStr )
       {
         // loop to end of source string
         while( sourceStr[ index ] != NULL_CHAR && index < MAX_STR_LEN )
            {
             //assign characters to end of destination string
             destStr[ index ] = sourceStr[ index ];

             // update index
             index++;

             // set temporary end of destination string
             destStr[ index ] = NULL_CHAR;
            }
       }
    // end loop
   }


/*
Name: findSubString
 Process: linear search for given substring within another string
 Function Input/Parameters: c-style source test string (char *),
                            c-style source search string (char *)
Function Output/Parameters: none
 Function Output/Returned: index of found substring, or
                          SUBSTRING_NOT_FOUND constant
                           if string not found
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getstringLength
*/
int findSubString( const char *testStr, const char *searchSubStr )
   {
     // initialize function/variables

        // initialize test string length
           // function: getStringLength
        int testStrLen = getStringLength( testStr );

        // initialize master index - location of sub string start point
        int masterIndex = 0;

        // initialize other variables
        int searchIndex, internalIndex;
      // loop across test string
      while( masterIndex < testStrLen )
         {
          // set internal loop index to current test string index
          internalIndex = masterIndex;

          // set internal search index to zero
          searchIndex = 0;

          // loop to end of test string
          // while test string/sub string chracters are the same
          while( internalIndex <= testStrLen && testStr[ internalIndex ] 
                                             == searchSubStr[ searchIndex ] )
             {
              // increment test string, substring indices
              internalIndex++; searchIndex++;

              // check for end of substring (search completed)
              if( searchSubStr[ searchIndex ] == NULL_CHAR )
                 {
                  // return beginning location of sub string
                  return masterIndex;
                 } 
             }
          // end internal comparison loop

          // increment current beginning location index
          masterIndex++;
         }
      // end loop across test string

      // assume test have failed at this point, return SUBSTRING_NOT_FOUND
      return SUBSTRING_NOT_FOUND;
   }

/*
Name: getStringConstrained
Process: captures a string from the input stream
         with various constraints
Function input / Parameters’: input stream (FILE *)
                              clears leading non printable (bool),
                              clears leading space (bool),
                              stops at non printable (bool),
                              stops at specified delimiter (char)
                              Note: consumes delimiter
Function Output/Parameters: string returned (char *)
Function Output/Returned: success of operation (bool)
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: fgetc
*/
bool getStringConstrained(
                          FILE *inStream,
                          bool clearLeadingNonPrintable,
                          bool clearLeadingSpace,
                          bool stopAtNonPrintable,
                          char delimiter,
                          char *capturedString
                         )
   {
    // initliaze variables
    int intChar = EOF, index = 0;

    // initalize output string]
    capturedString[ index ] = NULL_CHAR;

    // capture first value in stream
      // function: fgetc
    intChar = fgetc( inStream );

    // loop to clear no printable or space, if indicated
    while( ( intChar!= EOF ) 
               && ( ( clearLeadingNonPrintable && intChar < (int) SPACE ) 
                           || ( clearLeadingSpace && intChar == (int) SPACE ) )
         )
       {
         // get next character
            // function: fgetc
            intChar = fgetc( inStream );
       }
    // end clear non printable/space loop

    // check for end of file found
    if( intChar == EOF )
       {
         // return failed operation
         return false;
       }   

    // loop to capture input
    while(
          // continues if not at end od file and max string length not reached
          ( intChar != EOF && index < MAX_STR_LEN - 1 )

          // AND
          // continues if not printable flag set and characters are printable
          //    OR continues if not printable flag not set
          && ( ( stopAtNonPrintable && intChar >= (int) SPACE )
                 || ( !stopAtNonPrintable ) )

          // AND
          // continues if specified delimiter is not found
          && ( intChar != (char)delimiter )
         )
       {
         // place character in array element
         capturedString[ index ] = (char) intChar;

         // increment array index
         index++;

         // set next element to null character / end of c-string
         capturedString[ index ] = NULL_CHAR;

         // get next character as integer
            // function: fgetc
            intChar = fgetc( inStream );
       }
    // end loop

    // return successful operation
    return true;

   }

/*
Name: getStringLength
Process: finds the length of a string
by counting characters up to the NULL_CHAR character
runction Input/Paramsters: c-style string (char *)
Function Output/Parameters: none
Function Output/Returned: length of string
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: none
*/

int getStringLength( const char *testStr )
   {
     // initialize function/variables
     int index = 0;

     // loop to end of string, protect from overflow
     while( index < STD_STR_LEN && testStr[ index ] != NULL_CHAR )
         {
          // update index
          index++;
         }
      // end loop

      // return index/length
      return index;
   }

/*
Name: getStringToDelimiter
process: captures a string from the input stream
         to a specified delimiter;
         Note: consumes delimiter
Function Input/Parameters: input stream (FILE *)
                           stops at specified delimiter (char),
Function Output/Paraneters: string returned (char *)
Function utput/Returned: success of operation (bool)
Device input/Keyboard: none
Device Output/Monitor: none
Dependencies: getStringConstrained
*/

bool getStringToDelimiter(
                          FILE *inStream,
                          char delimiter,
                          char *capturedString
                         )
   {
     // call engine function with delimiter
         // function: getStringConstrained
      return getStringConstrained(
                      inStream,        // file stream pointer
                      true,            // clears leading non printable character
                      true,            // bool clearLeadingSpace
                      true,            //stops at non printable
                      delimiter,       // stops at delimiter
                      capturedString   // returns string
                                 );
   }

/*
Name: getstringtoLineEnd
Process: captures a string from the input stream

to the end of the line
Function Input/Paraneters: input stream (PILE *)
Function Output/Paraneters: string returned (char *)
Function output/Returned: success of operation (bool)
Device Input/Keyboard: none
Device output/Monitor: none
Dependencies: getstringconstrained
*/
bool getStringToLineEnd(
                        FILE *inStream,
                        char *capturedString
                       )
   {
     // call engine function with delimiter
        // function: getStringConstrained
      return getStringConstrained(
                   inStream,           // file stream pointer
                   true,               // clears leading non printble char 
                   true,               // bool clearLeadingSpace
                   true,               // stops at non printable
                   NON_PRINTABLE_CHAR, // non printable delimiter
                   capturedString      // returns string
                                 );
   }

/*
Name: getSubString
Process: captures sub string within larger string
         between two inclusive indices
         returns empty string if either index is out of range,
         assumes enough memory in destination string
Function Input/Parameters: c-style source string (char *),
                           start and end indices (int)
Function Output/Paraneters: c-style destination string (char *)
Function Output/Returned: none
Device Input/Keyboard: none
Device Output/Moniter: none
Dependencies: getstringlength, malloc, copystring, free
*/
void getSubString( char *destStr, const char *sourceStr, 
                                          int startIndex, int endIndex )
   {
     // initialize function/variables

        // set length of source string
            // function: getStringlength
        int sourceStrLen = getStringLength( sourceStr );

         // initialize the destination index to zero
         int destIndex = 0;

         // initialize source index to start index (parameter)
         int sourceIndex = startIndex;

         //create pointer for temp string
         char *tempStr;

     // check for indices within limits
     if( startIndex >= 0 && startIndex <= endIndex 
                                    && endIndex < sourceStrLen )
        {
         // create temporary string
            // function: malloc, copyString
         tempStr = (char *)malloc ( sourceStrLen + 1 );
         copyString( tempStr, sourceStr );

         // loop across requested substring (indices)
         while( sourceIndex <= endIndex )
            {
              // assign source character to destination element
              destStr[ destIndex ] = tempStr[ sourceIndex ];

              // increment indices
              destIndex++; sourceIndex++;

              // set temporary end of destination string
              destStr[ destIndex ] = NULL_CHAR;
            }

         // return memory for temporary string
            // function: free
         free( tempStr );
        }

   }


/*
Name: setstrToLowercase
Process: iterates through string, sets any upper case Lette:
         to lower case; no effect on other letters
Function Input/Paraneters: c-styie source string (char *)
Function Cutput/Paraneters: c-style destination string (char *)
Function cutput/Returned: none
Device Input/Keyboard: none
Device Output/Meniter: none
Dependencies: toLowercase
*/
void setStrToLowerCase( char *destStr, const char *sourceStr )
   {
     // initialize function/variables

        // get source string length
           // function: getStringLength
        int sourceStrLen = getStringLength( sourceStr );

        // create temporary string pointer
        char *tempStr;

        // create other variables
        int index = 0;

     // copy source string in case of aliasing
         // function: malloc, copyString
     tempStr = (char *)malloc( sizeof( sourceStrLen + 1 ) );
     copyString( tempStr, sourceStr );

     // loop across source string
     while( tempStr[ index ] != NULL_CHAR && index < MAX_STR_LEN )
        {
         // set individual character to lower case as needed
         // assign to destination string
         destStr[ index ] = toLowerCase( tempStr[ index ] );

         // update index
         index++;

         // set temporary end of destination string
         destStr[ index ] = NULL_CHAR;
        }

    // end loop

    // release memory used for temp string
        // function: free
    free( tempStr );
   }

/*
Name: toLowercase
Process: if character is upper case, sets it to lower case
otherwise returns character unchanged
Function Input/Paraneters: test character (char)
Function Output/Paraneters: none
Function Output/Returned: character to set to lower case, if appropriate
Device Input/Keyboard: none
Device output/Monitor: none
Dependencies: none
*/
char toLowerCase( char testChar )
   {
     // check for upper case letter
     if( testChar >= 'A' && testChar <= 'Z' )
        {
         // return lower case letter
         return testChar - 'A' + 'a';
        }

     // otherwise, assume to uppercase letter,
     // return character unchanged
     return testChar;
   }
















