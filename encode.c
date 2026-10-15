#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include "encode.h"
#include "types.h"

OperationType check_operation_type(char *argv[])
{

    if(strcmp(argv[1],"-e")==0)
        return e_encode;
    else if(strcmp(argv[1],"-d")==0)
        return e_decode;
    else
        return e_unsupported;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // chech name of source file,secret file,stego file extension
    if(strstr(argv[2],".bmp")==NULL)
    {
        printf("\nInvalid source file extention !!\n\n");
        return e_failure;
    }
    encInfo->src_image_fname=argv[2];
    printf("Valid source file extention \n");
    if(strstr(argv[3],".txt")==NULL)
    {
        printf("\nInvalid secret file extention !!\n\n");
        return e_failure;
    }
    encInfo->secret_fname=argv[3];
    strcpy(encInfo->extn_secret_file,"txt");
    printf("Valid secret file extention \n");
    if(argv[4]!=NULL)
    {
        if(strstr(argv[4],".bmp")==NULL)
        {
            printf("Invalid output file extention !!\nDefault output file name choosed !!\n");
            goto skip;
        }
        printf("Valid output file extention \n");
        encInfo->stego_image_fname=argv[4];
    }
    else
    {
        skip:
        encInfo->stego_image_fname="stego.bmp";
    }

    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    open_files(encInfo);
    printf("All file opening successfull\n");
    printf("Enter the magic string : ");
    scanf(" %s",encInfo->magic_string);
    check_capacity(encInfo);
    printf("Valid source file capacity \n");

    return e_success;

}

Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image=fopen(encInfo->src_image_fname,"r");
    if(encInfo->fptr_src_image==NULL)
    {
        printf("\n%s file not found in PWD !!\n\n",encInfo->src_image_fname);
        exit(e_failure);
    }
    encInfo->fptr_secret=fopen(encInfo->secret_fname,"r");
    if(encInfo->fptr_secret==NULL)
    {
        printf("\n%s file not found in PWD !!\n\n",encInfo->secret_fname);
        exit(e_failure);
    }
    encInfo->fptr_stego_image=fopen(encInfo->stego_image_fname,"w");
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity= get_image_size_for_bmp(encInfo->fptr_src_image);

    encInfo->size_secret_file= get_file_size(encInfo->fptr_secret);
    uint encode_data_bytes=0;
    encode_data_bytes=strlen(encInfo->magic_string)+sizeof(int)+strlen(encInfo->extn_secret_file)+sizeof(int)+encInfo->size_secret_file;
    if(encInfo->image_capacity < (encode_data_bytes*8))
    {
        printf("\n%s file don't have the capacity to hold this %s file data\nTry a diffrent source file !!\n\n",encInfo->src_image_fname,encInfo->secret_fname);
        exit(e_failure);
    }
    return e_success;
}

uint get_image_size_for_bmp(FILE *fptr_image)
{
    unsigned int width,height;

    fseek(fptr_image,18,SEEK_SET);
    fread(&width,4,1,fptr_image);
    fread(&height,4,1,fptr_image);
    return (width * height * 3);  //calculating the byte of data the image can hide 1byte data 8byte image data to hide
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return (uint)ftell(fptr);   
}