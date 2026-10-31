#include<stdio.h>
#include"common.h"
#include"types.h"
#include<unistd.h>
#include<string.h>

OperationType check_operation_type(char *argv[])
{
    /*
    ============================================================================
    Function    : check_operation_type
    Description : Identifies whether the user has selected encoding,
                decoding or an unsupported operation.

    Logic       :
                1. Compares argv[1] with "-e".
                2. If it matches, returns e_encode.
                3. Otherwise, compares argv[1] with "-d".
                4. If it matches, returns e_decode.
                5. If neither option matches, returns e_unsupported.

    Benefit     : Identifies the requested operation and allows the main
                function to execute the appropriate functionality.

    Parameters  :
                argv - Array containing command-line arguments.
                        argv[1] contains the operation option.

    Return      : OperationType
                e_encode      - Encoding operation selected.
                e_decode      - Decoding operation selected.
                e_unsupported - Invalid operation selected.
    ============================================================================
    */

    if(strcmp(argv[1],"-e")==0)
        return e_encode;
    else if(strcmp(argv[1],"-d")==0)
        return e_decode;
    else
        return e_unsupported;
}

unsigned long get_file_size(FILE *fptr)
{
   /*
    ============================================================================
    Function    : get_file_size
    Description : Calculates and returns the size of a given file in bytes.

    Logic       :
                1. Moves the file pointer to the end of the file using
                    fseek().
                2. Uses ftell() to obtain the current file position,
                    which represents the file size.
                3. Stores the file size in the fsize variable.
                4. Calls rewind() to move the file pointer back to
                    the beginning of the file.
                5. Returns the calculated file size.

    Benefit     : Provides the file size required for checking image
                capacity, secret file size and input file validity.

    Parameters  :
                fptr - File pointer of the file whose size is calculated.

    Return      : uint
                Returns the size of the file in bytes.
    ============================================================================
    */
   
    unsigned long fsize;
    fseek(fptr,0,SEEK_END);
    fsize=ftell(fptr);
    rewind(fptr);
    return fsize;
}

void display_help()
{
    /*
    ============================================================================
    Function    : display_help
    Description : Displays the user manual containing application
                description, command-line syntax and usage instructions.

    Benefit     : Helps users understand the available operations,
                command-line arguments and application requirements.

    Parameters  :
                No parameters.

    Return      : void
                Does not return any value.
    ============================================================================
    */
    printf("\n");
    printf("************************************************************\n");
    printf("              LSB IMAGE STEGANOGRAPHY\n");
    printf("                     USER MANUAL\n");
    printf("************************************************************\n");

    printf("\nDESCRIPTION:\n");
    printf("  This project implements image steganography using the\n");
    printf("  Least Significant Bit (LSB) technique to conceal secret\n");
    printf("  data within a BMP image file and retrieve it when needed.\n");

    printf("\nUSAGE:\n");
    printf("  ./a.out <option> [arguments]\n");

    printf("\nOPTIONS:\n");
    printf("  -e    Encode a secret file into a BMP image file.\n");
    printf("  -d    Decode hidden data from a stego image file.\n");

    printf("\nENCODING:\n");
    printf("  Command :\n");
    printf("    ./a.out -e <source_image.bmp> <secret_file> [output_image.bmp]\n");
    printf("\n");
    printf("  Arguments:\n");
    printf("    source_image.bmp  Source BMP image file.\n");
    printf("    secret_file       File to hide inside the image.\n");
    printf("    output_image.bmp  Optional output BMP filename.\n");
    printf("                      Default: stego.bmp\n");
    

    printf("\n  Example:\n");
    printf("    ./a.out -e input.bmp secret.txt output.bmp\n");
    printf("    ./a.out -e input.bmp secret.txt \n");

    printf("\nDECODING:\n");
    printf("  Command :\n");
    printf("    ./a.out -d <encoded_image.bmp> [output_file]\n");
    printf("\n");
    printf("  Arguments:\n");
    printf("    encoded_image.bmp  Image containing hidden data.\n");
    printf("    output_file        Optional name for the extracted file.\n");

    printf("\n  Example:\n");
    printf("    ./a.out -d stego.bmp\n");
    printf("    ./a.out -d stego.bmp recovered.txt\n");

    printf("\nSUPPORTED FILES:\n");
    printf("  Source image : .bmp\n");
    printf("  Secret file  : Any file type supported by the program.\n");
    printf("  Output image : .bmp\n");

    printf("\nNOTES:\n");
    printf("  * The source image must be an RGB BMP image with\n");
    printf("    a color depth of 24 bits per pixel (24 bpp).\n");
    printf("  * The 2 character supported for magic string.\n");


    printf("\n************************************************************\n\n");
}

void display_status(char *status,char *done)
{
    /*
    ============================================================================
    Function    : display_status
    Description : Displays the execution progress of an operation and
                prints its completion message.

    Logic       :
                1. Executes a loop from 0 to 100.
                2. Displays the current operation status with a
                    percentage value.
                3. Uses usleep() to introduce a small delay between
                    percentage updates.
                4. Calls fflush() to immediately display the output.
                5. Clears the progress line after reaching 100%.
                6. Displays the final completion message.

    Benefit     : Provides visual feedback to the user about the
                progress and completion of different operations.

    Parameters  :
                status - Description of the operation currently running.
                done   - Message displayed after operation completion.

    Return      : void
                Does not return any value.
    ============================================================================
    */
    for(int i=0;i<=100;i++)
    {
        printf("%-9s : %s........%d%%\r","[STATUS]",status,i);
        usleep(10000);
        fflush(stdout);
    }
    printf("\r\033[K");  // Return to beginning and clear the line
    printf("%-9s : %s\n","[STATUS]",done);
}

void display_comerror(char *error)
{
    /*
    ============================================================================
    Function    : display_comerror
    Description : Displays a common application error message followed
                by the user manual and program termination message.

    Logic       :
                1. Prints the supplied error message.
                2. Calls display_help() to display application usage
                    instructions.
                3. Displays the program termination message.

    Benefit     : Provides a common error-handling mechanism for
                command-line argument and operation-related errors.

    Parameters  :
                error - String containing the error description.

    Return      : void
                Does not return any value.
    ============================================================================
    */
    printf("\n%-9s : %s !!\n","[ERROR]",error);
    display_help();
    printf("\n************** Programm Terminated **************\n\n");
}