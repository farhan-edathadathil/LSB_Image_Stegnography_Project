/*
============================================================================

                       COMMON.H

 DESCRIPTION:

 This header file contains the function declarations for common
 operations used throughout the project.

 It includes functions for identifying the selected operation,
 calculating file size, displaying application help, showing
 execution progress and displaying common error messages.

 It includes types.h to access the user-defined data types.

 These functions provide common functionality that can be accessed
 by the main, encoding and decoding modules.

============================================================================
*/

#ifndef COMMON_H
#define COMMON_H

#include"types.h"


OperationType check_operation_type(char *argv[]);

/* Get file size */
unsigned long get_file_size(FILE *fptr);

void display_help();

void display_status(char *status,char *done);

void display_comerror(char *error);

#endif
