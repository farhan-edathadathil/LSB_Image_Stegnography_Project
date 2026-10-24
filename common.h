#ifndef COMMON_H
#define COMMON_H

#include"types.h"

/* Magic string to identify whether stegged or not */
#define MAGIC_STRING "#*"  //read from user

OperationType check_operation_type(char *argv[]);

void display_help();

void display_comerror(char *error);

#endif
