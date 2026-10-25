#ifndef COMMON_H
#define COMMON_H

#include"types.h"

/* Magic string to identify whether stegged or not */
#define MAGIC_STRING "#*"  //read from user

OperationType check_operation_type(char *argv[]);

/* Get file size */
uint get_file_size(FILE *fptr);

void display_help();

void display_status(char *status,char *done);

void display_comerror(char *error);

#endif
