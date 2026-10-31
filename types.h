/*
============================================================================

                       TYPES.H

 DESCRIPTION:

 This header file defines the user-defined data types used throughout
 the project.

 It defines the Status enumeration to represent the success or failure
 of function execution and the OperationType enumeration to identify
 whether the user has selected encoding, decoding or an unsupported
 operation.

 It also defines uint as an alias for unsigned int.

 The definitions are shared between different source files to maintain
 consistency in function return types and operation handling.

============================================================================
*/

#ifndef TYPES_H
#define TYPES_H

/* User defined types */
typedef unsigned int uint;

/* Status will be used in fn. return type */
typedef enum
{
    e_success,
    e_failure
    
} Status;

typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

#endif
