// header files
#include "OS_SimDriver.h"

int main( int argc, char **argv )
   {
     // initialize program

         // initialize variables
         ConfigDataType *configDataPtr = NULL;
         OpCodeType *metaDataPtr = NULL;
         char errorMessage[ MAX_STR_LEN ];
         CmdLineData cmdLineData;
         bool configUploadSuccess = false;

         // show title
            // function: printf
         printf( "\nSimulator Program\n" );
         printf( "=================\n\n" );

     // process command line, check for program run
     // at least one correct switch and config file name (at end) verified
        // function: processCmdLine
     if( processCmdLine( argc, argv, &cmdLineData ) )
        {
         // upload config file, check for success
            // function: getConfigData
         if( getConfigData( cmdLineData.fileName,
                                                &configDataPtr, errorMessage ) )
            {
             // check for config display flag
             if( cmdLineData.configDisplayFlag )
                {
                 // display config data
                    // function: displayConfigData
                 displayConfigData( configDataPtr );
                }

             // set config file upload success flag
             configUploadSuccess = true;  
            }

         // otherwise, assume config file upload failure
         else
            {
             // show error message, end program
                // function: printf
             printf( "\nConfig Upload Error: %s, program aborted\n\n",
                                                                errorMessage );
            } 

         // check for config success and need for meta data
         if( configUploadSuccess
                    && ( cmdLineData.mdDisplayFlag || cmdLineData.runSimFlag ) )
            {
             // upload meta data file, check for success
                // function: getMetaData
             if( getMetaData( configDataPtr->metaDataFileName,
                                                  &metaDataPtr, errorMessage ) )
                {
                 // check meta data display flag
                 if( cmdLineData.mdDisplayFlag )
                    {
                     // display meta data display flag
                           // function: displayMetaData
                     displayMetaData( metaDataPtr );
                    }

                 // check run simulator flag
                 if( cmdLineData.runSimFlag )
                    {
                     // run simulator
                           // function: run sim
                     runSim( configDataPtr, metaDataPtr );
                    }
                }
             // otherwise, assume meta data upload failure
             else
                {
                 // show error message, end program
                     // function: printf
                 printf( "\nMetadata Upload Error: %s, program aborted\n",
                                                                  errorMessage);
                }
            }
         // end check need for metadata upload

             // clean up config data as needed
                  // function: clearConfigData
             configDataPtr = clearConfigData( configDataPtr );

             // clean up metadata as needed
                  // function: clearMetaData
             metaDataPtr = clearMetaDataList( metaDataPtr );
            }
         // end check for good command line
        
        // otherwise, assume command Line failure
        else
            {
             // show command argument requirements
                  // function: showCommandLineFormat
             showCommandLineFormat();
            }

    // show program end
         // function: printf
    printf( "\nSimulator Program End.\n\n" );

    // return success
    return 0;
   }

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
void clearCmdLineStruct( CmdLineData *clDataPtr )
  {
   // set all struct memebers to default
   clDataPtr->programRunFlag = false;
   clDataPtr->configDisplayFlag = false;
   clDataPtr->mdDisplayFlag = false;
   clDataPtr->runSimFlag = false;
   clDataPtr->fileName[ 0 ] = NULL_CHAR;
  }

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
Dependencies: clearCmdLineStruct, compareString, getStringLength,
              findSubString, copyString
*/
bool processCmdLine( int numArgs, char **strVector, CmdLineData *clDataPtr )
  {
   // initialize function/variables

      // initilize strucutre to defaults
          // function: clearStruct
      clearCmdLineStruct( clDataPtr );

      // initialize success flags to false
      bool atLeastOneSwitchFlag = false;
      bool correctConfigFileFlag = false;

      // initialize first arg index to one
      int argIndex = 1;

      // declare other variables
      int fileStrLen, fileStrSubLoc;

   // must have prog name, at least one switch, and config file name, minimum
   if( numArgs >= MIN_NUM_ARGS )
      {
            // loop across args (starting at 1 ) and program run flag
       while( argIndex < numArgs )
          {
           // check for -dc (display config flag)
           if( compareString( strVector[ argIndex ], "-dc" ) == STR_EQ )
              {
               // set config display flag
               clDataPtr->configDisplayFlag = true;

               // set at least one switch flag
               atLeastOneSwitchFlag = true;
              }

           // otherwise, check for -dm (display metadata file)
           else if( compareString( strVector[ argIndex ], "-dm" ) == STR_EQ )
              {
               // set meta data file dispaly flag
               clDataPtr->mdDisplayFlag = true;

               // set at least one switch flag
               atLeastOneSwitchFlag = true;
              }

           // otherwise, check for -rs (run Simulator)
           else if( compareString( strVector[ argIndex ], "-rs" ) == STR_EQ )
              {
               // set run simulator flag
               clDataPtr->runSimFlag = true;

               // set at least one switch flag
               atLeastOneSwitchFlag = true;
              }

           // otherwise, check for file name, ending in .cfg
           // must be last four
           else
              {
               // find lengths to verifiy file name
               // file name must be last argument and have ".cnf" extension
               fileStrLen = getStringLength( strVector[ numArgs - 1 ] );
               fileStrSubLoc =
                              findSubString( strVector[ numArgs - 1 ], ".cnf" );

                // verify string found and correct format for config file name
                if( fileStrSubLoc != SUBSTRING_NOT_FOUND
                            && fileStrSubLoc == fileStrLen - LAST_FOUR_LETTERS )
                   {
                    // set file name to variable
                    copyString( clDataPtr->fileName, strVector[ numArgs - 1 ] );

                    // set success flag to true
                    correctConfigFileFlag = true;
                   }
               // otherwise, assume bad config file name
               else
                  {
                   // reset struct, correct config file stays false
                      // function: clearStruct
                   clearCmdLineStruct( clDataPtr );
                  }
              }
              
           // update arg index
           argIndex++;   
          }
       // end arg loop
      }
   // end test for minimum number of commands line arguments

   // return verification of at least one switch and correct file name
   return atLeastOneSwitchFlag && correctConfigFileFlag;
  }

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
void showCommandLineFormat()
  {
   // display command line format
      // function: printf
   printf( "Command Line Format:\n" );
   printf( "     sim_01 [-dc] [-dm] [-rs] <config file name>\n" );
   printf( "     -dc [optional] displays configuration data\n" );
   printf( "     -dm [optional] displays meta data\n" );
   printf( "     -rs [optional] runs simulator\n" );
   printf( "     required config file name\n" );
  }

