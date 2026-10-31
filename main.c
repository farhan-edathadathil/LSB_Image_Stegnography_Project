/*
 NAME : FARHAN E
 ID   : 26018_199
 DATE : 30/10/2026


                  LSB IMAGE STEGANOGRAPHY

 DESCRIPTION:

 This project is a command-line based LSB Image Steganography
 program developed in C. It is designed to hide a secret file
 inside a 24-bit BMP image and retrieve the original file from the
 encoded image using the Least Significant Bit (LSB) technique.

 The program modifies the least significant bits of image bytes to
 store secret information while keeping changes in pixel values
 minimal.

 ENCODING:

 During encoding, the program takes a source BMP image and a secret
 file as command-line arguments.

 The encoding process follows these steps:

      1. Validates the source image and secret file.
      2. Opens the source image, secret file and destination image.
      3. Checks whether the image uses 24 bits per pixel.
      4. Calculates the available image capacity.
      5. Checks whether the image can accommodate the secret file.
      6. Copies the original BMP header into the output image.
      7. Encodes the user-provided magic string.
      8. Encodes the secret file extension length and extension.
      9. Encodes the original secret file size.
     10. Encodes the actual secret file contents.
     11. Copies the remaining image data into the output image.
     12. Closes all opened files.

 Each secret data byte is divided into 8 individual bits. These bits
 are stored in the LSBs of 8 image bytes using bitwise operations.


 DECODING:

 During decoding, the program takes the encoded BMP image and
 extracts the hidden secret file.

 The decoding process follows these steps:

      1. Validates and opens the input stego image.
      2. Checks the image size and pixel depth.
      3. Reads the magic string provided by the user.
      4. Extracts the hidden magic string from the image.
      5. Compares both magic strings for authentication.
      6. Extracts the secret file extension length and extension.
      7. Constructs the output filename and creates the file.
      8. Extracts the original secret file size.
      9. Extracts the secret file contents from the image.
     10. Writes the extracted data into the output file.
     11. Closes all opened files.

 During decoding, the program extracts the LSB from every 8 image
 bytes and reconstructs one original secret data byte.


 MAIN LOGIC:

 The project uses the Least Significant Bit of image bytes to hide
 secret information.

 During encoding, each secret data bit replaces the LSB of a
 corresponding image byte. During decoding, these LSBs are extracted
 and combined to reconstruct the original data.

 The secret information is stored in a predefined sequence consisting
 of the magic string, file extension length, file extension, file size
 and actual file contents.

 This sequence allows the decoding process to identify the hidden
 data, determine its original file type and extract the exact amount
 of information required.


 SAMPLE ENCODING OUTPUT:

    ************************************************
            Encoding Operation Selected
    ************************************************

    [STATUS]  : Valid source file name
    [STATUS]  : Valid secret file name
    [INVALID] : Two file name can't be same !!
    [FIX]     : Default output file name choosed !!
    [STATUS]  : All file opened successfully
    [INPUT]   : Enter the magic string : *#
    [STATUS]  : Valid magic string
    [STATUS]  : Valid source file capacity and bits per pixel
    [STATUS]  : Header bytes copied successfully
    [STATUS]  : Magic string encoded successfully
    [STATUS]  : Secret file extension length and extension encoded successfully
    [STATUS]  : Secret file size encoded successfully
    [STATUS]  : Secret file data encoded successfully
    [STATUS]  : Remaining data copied successfully
    [STATUS]  : All files closed successfully

    ************************************************
            Encoding Completed Successfully
    ************************************************

 SAMPLE DECDING OUTPUT:

    ************************************************
            Decoding Operation Selected
    ************************************************

    [STATUS]  : Valid input file name
    [STATUS]  : Default output file name choosed
    [STATUS]  : Input file opened and validated successfully
    [INPUT]   : Enter the magic string that you have : *#
    [STATUS]  : Magic string authenticated successfully
    [STATUS]  : Extension size and extension decoded successfully
    [STATUS]  : Secret data size decoded successfully
    [STATUS]  : Secret file created successfully
    [STATUS]  : Secret data decoded and saved successfully
    [STATUS]  : All files closed successfully

    ************************************************
            Decoding Completed Successfully
    ************************************************


 OVERALL:

 The main purpose of this project is to understand how binary data
 can be hidden inside an image using bitwise operations and how the
 original information can be retrieved.

 It provides practical experience in C programming, binary file
 handling, pointers, structures, bitwise manipulation, file operations
 and LSB-based steganography.

 ============================================================================
 */
#include<stdio.h>
#include"encode.h"
#include"decode.h"
#include"types.h"
#include"common.h"

int main(int argc,char **argv)
{
    /*
    ============================================================================
    Function    : main
    Description : Controls the execution of the LSB Image Steganography
                application and selects encoding or decoding operations.

    Logic       :
                1. Checks whether sufficient command-line arguments
                    are provided.
                2. Declares EncodeInfo and DecodeInfo structures to
                    store encoding and decoding information.
                3. Calls check_operation_type() to identify the
                    requested operation.
                4. If encoding is selected:
                    a. Validates the number of arguments.
                    b. Displays the encoding operation message.
                    c. Calls read_and_validate_encode_args().
                    d. Calls do_encoding() to perform encoding.
                5. If decoding is selected:
                    a. Validates the number of arguments.
                    b. Displays the decoding operation message.
                    c. Calls read_and_validate_decode_args().
                    d. Calls do_decoding() to perform decoding.
                6. Displays an error and help information if an
                    unsupported operation is selected.

    Benefit     : Acts as the entry point of the application and controls
                the complete encoding and decoding execution flow.

    Parameters  :
                argc - Number of command-line arguments.
                argv - Array of command-line argument strings.

    Return      : int
                Returns e_failure when argument validation or operation
                selection fails. Returns 0 when main reaches its end
                after a successful operation.
    ============================================================================
    */
    if(argc<2)
    {
        display_comerror("Not enough arguments given");
        return e_failure;
    }
    
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    if(check_operation_type(argv)==e_encode)
    {
        if(argc>5)
        {
            display_error("More number of arguments given for encoding");
            display_help();
            return e_failure;
        }
        if(argc<4)
        {
            display_error("Not enough arguments given for encodeing");
            display_help();
            return e_failure;
        }
        printf("\n************************************************\n");
        printf("          Encoding Operation Selected\n");
        printf("************************************************\n\n");

        if(read_and_validate_encode_args(argv,&encInfo)!=e_success)
            return e_failure;

        do_encoding(&encInfo);
    }
    else if(check_operation_type(argv)==e_decode)
    {
        if(argc>4)
        {
            display_decode_error("More number of arguments given for decoding");
            display_help();
            return e_failure;
        }
        if(argc<3)
        {
            display_decode_error("Not enough arguments given for decodeing");
            display_help();
            return e_failure;
        }
        printf("\n************************************************\n");
        printf("          Decoding Operation Selected\n");
        printf("************************************************\n\n");
        if(read_and_validate_decode_args(argv,&decInfo)!=e_success)
            return e_failure;
        do_decoding(&decInfo);
    }
    else
    {
        display_comerror("You have given Unsupported argument");
        return e_failure;
    }

}

