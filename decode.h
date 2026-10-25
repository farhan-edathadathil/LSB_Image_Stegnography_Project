#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

typedef struct _DecodeInfo
{
    /* input stego info */
    char *input_stego_fname;
    FILE *fptr_input_stego;
    short bits_per_pixel;
    unsigned long size_stego_file;

    /* Output File Info */
    char output_fname[15];
    FILE *fptr_output;
    unsigned long size_secret_file;

    //magic string
    char magic_string[4];
    int magic_len;
} DecodeInfo;

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status open_file_and_checks(DecodeInfo *decInfo);

Status authenticate_magic_str(DecodeInfo *decInfo);

Status decode_extn_data(DecodeInfo *decInfo);

Status create_secret_file(DecodeInfo *decInfo);

Status decode_secret_data_size(DecodeInfo *decInfo);

Status decode_secret_data(DecodeInfo *decInfo);

Status fsize_check(unsigned long cur_data_size,DecodeInfo *decInfo);

Status decode_data_from_image(int size,void *data,DecodeInfo *decInfo);

Status decode_lsb_to_byte(char *data,char *image_buffer);

void close_decfile(DecodeInfo *decInfo);

void display_decode_error(char *error);

#endif