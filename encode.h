/*
============================================================================

                       ENCODE.H

 DESCRIPTION:

 This header file contains the EncodeInfo structure, macro definitions
 and function declarations required for the encoding operation.

 The EncodeInfo structure stores the source image information,
 secret file information, destination stego image information,
 file pointers, image capacity, file sizes and magic string.

 It also defines macros for secret data buffer size, image buffer size
 and maximum file extension length.

 The function declarations cover the complete encoding process,
 including argument validation, file opening, image capacity checking,
 BMP header copying, magic string encoding, secret file extension
 encoding, secret file size encoding, secret file data encoding
 and copying the remaining image data.

 These declarations allow the encoding functionality to be implemented
 separately in encode.c and accessed by other project modules.

============================================================================
*/

#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    unsigned long image_capacity;
    short bits_per_pixel;
    unsigned long size_image_file;

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char *extn_secret_file;
    unsigned long size_secret_file;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    //magic string
    char magic_string[4];
} EncodeInfo;


/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
unsigned long get_image_size_for_bmp(FILE *fptr_image,EncodeInfo *encInfo);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status encode_magic_string(EncodeInfo *encInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn_data(EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(int size,char *data,EncodeInfo *encInfo);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(const unsigned char data, char *image_buffer);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest,EncodeInfo *encInfo);

void close_encfile(EncodeInfo *encInfo);

/* display encoding error */
void display_error(char *error);

#endif
