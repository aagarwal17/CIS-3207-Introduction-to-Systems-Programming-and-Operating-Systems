/**
 * @brief Header file for optionProcessor.c, holding function headers from this file to be used by other files in this project
 * 
 * @param theNum 
 * @return int 
 */
int validPID(char *theNum);
void parseStat(char * inputPID, char * theArr[]);
void getVMem(char *virtMem, char * theNum);
void getCommandLine(char * myCmdArr[], char * theNum);
int isUserId(char *thePID);

