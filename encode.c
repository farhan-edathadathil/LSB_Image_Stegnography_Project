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
    copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    printf("header susscessfully copied \n");
    encode_magic_string(encInfo);
    printf("magic string encoded \n");
    encode_secret_file_extn(encInfo);

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

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char temp[54];
    rewind(fptr_src_image);
    fread(temp,1,54,fptr_src_image);
    rewind(fptr_dest_image);
    fwrite(temp,1,54,fptr_dest_image);
    return e_success;

}

Status encode_magic_string(EncodeInfo *encInfo)
{
    char image_buffer[8];
    int size =strlen(encInfo->magic_string)+1;
    for(int i=0;i<size;i++)
    {
        fread(image_buffer,1,8,encInfo->fptr_src_image);
        encode_byte_to_lsb(encInfo->magic_string[i],image_buffer);
        fwrite(image_buffer,1,8,encInfo->fptr_stego_image);    
    }
return e_success;
}

Status encode_byte_to_lsb(const unsigned char data, char *image_buffer)
{

    for(int j=0;j<8;j++)
    {    
        image_buffer[j]=(image_buffer[j] & ~(1)) | (data>>j & 1);    
    }
    return e_success;

}

Status encode_int_to_lsb(const unsigned int data, char *image_buffer)
{
    for(int j=0;j<32;j++)
    {    
        image_buffer[j]=(image_buffer[j] & ~(1)) | (data>>j & 1);    
    }
    return e_success;
}

Status encode_secret_file_extn(EncodeInfo *encInfo)
{
    char image_buffer[32];
    uint size =strlen(encInfo->extn_secret_file);
    fread(image_buffer,1,32,encInfo->fptr_src_image);
    encode_byte_to_lsb(size,image_buffer);
    fwrite(image_buffer,1,32,encInfo->fptr_stego_image);    
    
    //char image_buffer[8];
    for(int i=0;i<size;i++)
    {
        fread(image_buffer,1,8,encInfo->fptr_src_image);
        encode_byte_to_lsb(encInfo->extn_secret_file[i],image_buffer);
        fwrite(image_buffer,1,8,encInfo->fptr_stego_image);    
    }
    return e_success;
}

Status encode_secret_file_size(EncodeInfo *encInfo)
{
    char image_buffer[32];
    fread(image_buffer,1,32,encInfo->fptr_src_image);
    encode_byte_to_lsb(encInfo->size_secret_file,image_buffer);
    fwrite(image_buffer,1,32,encInfo->fptr_stego_image);   
    return e_success; 
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char image_buffer[8];
    unsigned char secret_buffer;
    for(int i=0;i<encInfo->size_secret_file;i++)
    {
        fread(image_buffer,1,8,encInfo->fptr_src_image);
        fread(&secret_buffer,1,1,encInfo->fptr_secret);
        encode_byte_to_lsb(secret_buffer,image_buffer);
        fwrite(image_buffer,1,8,encInfo->fptr_stego_image);    
    }
return e_success;
}