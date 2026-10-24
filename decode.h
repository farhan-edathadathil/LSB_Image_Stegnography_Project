#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

typedef struct _DecodeInfo
{
    /* input stego info */
    char *input_stego_fname;
    FILE *fptr_input_stego;
    short bits_per_pixel;
    unsigned long size_image_file;

    /* Output File Info */
    char *output_fname;
    FILE *fptr_output;
    char *extn_output_file;
    unsigned long size_secret_file;

    //magic string
    char magic_string[10];
} DecodeInfo;

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);


void close_decfile(DecodeInfo *decInfo);

void display_decode_error(char *error);

#endif