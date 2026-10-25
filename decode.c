#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include "decode.h"
#include "encode.h"
#include"common.h"
#include "types.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // check name of source file,secret file,stego file extension
    char *extn=strrchr(argv[2],'.');
    if(extn==NULL || strcmp(extn,".bmp")!=0)
    {
        display_error("Invalid Input file extention");
        return e_failure;
    }
    decInfo->input_stego_fname=argv[2];
    display_status("Validating input file name","Valid input file name");

    //check out put file name
    if(argv[3]!=NULL)
    {
        extn=strrchr(argv[3],'.');
        if(extn!=NULL)
        {
            *extn='\0';
        }
        if(strlen(argv[3])==0)
        {
            printf("[INVALID] : No valid file name found!!\n%-9s : Default output file name choosed !!\n","[FIX]");
            goto skip;
        }
        if((strlen(argv[3])+3)>sizeof(decInfo->output_fname))
        {
            printf("[INVALID] : Output file name with extension is too large !!\n%-9s : Default output file name choosed !!\n","[FIX]");
            goto skip;
        }
        display_status("Validating output file name","Valid output file name");
        strcpy(decInfo->output_fname,argv[3]);
    }
    else
    {
        display_status("Choosing default output file name","Default output file name choosed");
        skip:
        strcpy(decInfo->output_fname,"output");
    }
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    open_file_and_checks(decInfo);
    display_status("Encoded input file is openning and validating","Input file opened and validated successfully");


    if(authenticate_magic_str(decInfo)!=e_success)
    {
        display_decode_error("Magic string Authentication failed");
        exit(e_failure);
    }
    display_status("Authenticating magic string","Magic string authenticated successfully");

    decode_extn_data(decInfo);
    display_status("Decoding extension size and extension","Extension size and extension decoded successfully");

    create_secret_file(decInfo);
    display_status("Creating secret file","Secret file created successfully");

    decode_secret_data_size(decInfo);
    display_status("Decoding secret data size","Secret data size decoded successfully");

    decode_secret_data(decInfo);
    display_status("Decoding secret data","Secret data decoded and saved successfully");

    close_decfile(decInfo);
    display_status("Closing all files","All files closed successfully");

    printf("\n************************************************\n");
	printf("        Decoding Completed Successfully\n");
	printf("************************************************\n");

    return e_success;
}

Status open_file_and_checks(DecodeInfo *decInfo)
{
    decInfo->fptr_input_stego=fopen(decInfo->input_stego_fname,"r");
    if(decInfo->fptr_input_stego==NULL)
    {
        printf("\nERROR : %s file not found in PWD !!\n\n",decInfo->input_stego_fname);
        printf("\n**************** Decoding FAILED ****************\n");
        printf("\n************** Programm Terminated **************\n\n");
        exit(e_failure);
    }
    if((decInfo->size_stego_file=get_file_size(decInfo->fptr_input_stego))<54)
    {
        printf("\nERROR : %s file is empty or corrupted !!\n\n",decInfo->input_stego_fname);
        printf("\n**************** Decoding FAILED ****************\n");
        printf("\n************** Programm Terminated **************\n\n");
        exit(e_failure);
    }
    return e_success;
}

Status authenticate_magic_str(DecodeInfo *decInfo)
{
    again :
    printf("%-9s : Enter the magic string that you have : ","[INPUT]");
    if(fgets(decInfo->magic_string,(sizeof(decInfo->magic_string)),stdin) != NULL)
    {
        if (strchr(decInfo->magic_string, '\n') == NULL)
        {
            char ch;
            printf("[INVALID] : Input may be too long , only 2 character is valid !! -Try again\n");
            while ((ch = getchar()) != '\n' && ch != EOF);
            goto again;
        }
        *(strchr(decInfo->magic_string, '\n'))='\0';
        if(strlen(decInfo->magic_string)<2)
        {
            printf("[INVALID] : Empty input or not given 2 character !! -Try again\n");
            goto again;
        }
    }
    else
    {
        printf("[INVALID] : Something went wrong , Try again\n");
        goto again;
    }

    //decoding magic string from file
    decInfo->magic_len=strlen(decInfo->magic_string);
    char str[decInfo->magic_len+1];
    fseek(decInfo->fptr_input_stego,54,SEEK_SET);

    fsize_check(decInfo->magic_len,decInfo);

    decode_data_from_image(decInfo->magic_len,str,decInfo);
    str[decInfo->magic_len]='\0';
    if(strcmp(decInfo->magic_string,str)!=0)
        return e_failure;
    return e_success;
}

Status decode_extn_data(DecodeInfo *decInfo)
{
    int extn_len;
    fsize_check(sizeof(extn_len),decInfo);
    decode_data_from_image(sizeof(extn_len),&extn_len,decInfo);
    char extn[extn_len+1];
    fsize_check(extn_len,decInfo);
    decode_data_from_image(extn_len,extn,decInfo);
    extn[extn_len]='\0';
    if((strlen(decInfo->output_fname)+extn_len+2)>sizeof(decInfo->output_fname))
    {
        printf("[INVALID] : Output file name with extension is too large !!\n%-9s : Default output file name choosed !!\n","[FIX]");
        strcpy(decInfo->output_fname,"output");
    }
    if(strchr(extn,'.')==NULL)
        strcat(decInfo->output_fname,".");
    strcat(decInfo->output_fname,extn);
    return e_success;
}

Status create_secret_file(DecodeInfo *decInfo)
{
    decInfo->fptr_output=fopen(decInfo->output_fname,"w");
    return e_success;
}

Status decode_secret_data_size(DecodeInfo *decInfo)
{
    fsize_check(sizeof(decInfo->size_secret_file),decInfo);
    decode_data_from_image(sizeof(decInfo->size_secret_file),&(decInfo->size_secret_file),decInfo);
    return e_success;
}

Status decode_secret_data(DecodeInfo *decInfo)
{
    fsize_check(decInfo->size_secret_file,decInfo);
    char buffer[8];
    int count=decInfo->size_secret_file/sizeof(buffer);
    for(int i=0;i<count;i++)
    {
        decode_data_from_image(sizeof(buffer),buffer,decInfo);
        fwrite(buffer,1,sizeof(buffer),decInfo->fptr_output);
        if(ferror(decInfo->fptr_output))
        {
            display_decode_error("Writing to output file failed");
            exit(e_failure);
        }
    }
    decode_data_from_image((decInfo->size_secret_file % sizeof(buffer)),buffer,decInfo);
    fwrite(buffer,1,(decInfo->size_secret_file % sizeof(buffer)),decInfo->fptr_output);
    if(ferror(decInfo->fptr_output))
    {
        display_decode_error("Writing to output file failed");
        exit(e_failure);
    }
    return e_success;
}

Status fsize_check(unsigned long cur_data_size,DecodeInfo *decInfo)
{
    if((ftell(decInfo->fptr_input_stego)+cur_data_size) > decInfo->size_stego_file)
    {
        printf("\nERROR : %s file is corrupted !!\n\n",decInfo->input_stego_fname);
        printf("\n**************** Decoding FAILED ****************\n");
        printf("\n************** Programm Terminated **************\n\n");
        exit(e_failure);
    }
    return e_success;
}

Status decode_data_from_image(int size,void *data,DecodeInfo *decInfo)
{
    char image_buffer[8];
    for(int i=0;i<size;i++)
    {
        if(fread(image_buffer,1,8,decInfo->fptr_input_stego)!=8)
        {
            display_error("Reading from source file failed");
            exit(e_failure);
        }
        decode_lsb_to_byte(((char *)data+i),image_buffer);
    }
    return e_success;
}

Status decode_lsb_to_byte(char *data,char *image_buffer)
{
    *data=0;
    for(int i=0;i<8;i++)
    {
        *data=(*data | ((image_buffer[i] & 1)<<i));
    }
    return e_success;
}

void close_decfile(DecodeInfo *decInfo)
{
    fclose(decInfo->fptr_input_stego);
    fclose(decInfo->fptr_output);
}

void display_decode_error(char *error)
{
    printf("\n%-9s : %s !!\n","[ERROR]",error);
    printf("\n**************** Decoding FAILED ****************\n");
    printf("\n************** Programm Terminated **************\n\n");
}