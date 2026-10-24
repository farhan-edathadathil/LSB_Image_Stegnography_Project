#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include <unistd.h>
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
    // check name of source file,secret file,stego file extension
    char *extn=strrchr(argv[2],'.');
    if(extn==NULL || strcmp(extn,".bmp")!=0)
    {
        printf("\nInvalid source file extention !!\n\n");
        return e_failure;
    }
    encInfo->src_image_fname=argv[2];
    display_status("Validating source file name","Valid source file name");

    //secret file checking
    extn=strrchr(argv[3],'.');
    if(extn==NULL || strlen(extn)<2)
    {
        printf("\nSecret file must contain an extention !!\n\n");
        return e_failure;
    }
    if(strcmp(encInfo->src_image_fname,argv[3])==0)
    {
        printf("Source file name and secret file name can't be same !!\n");
        return e_failure;
    }   
    encInfo->secret_fname=argv[3];
    encInfo->extn_secret_file=extn+1;
    display_status("Validating secret file name","Valid secret file name");

    //check out put file name
    if(argv[4]!=NULL)
    {
        extn=strrchr(argv[4],'.');
        if(extn==NULL || strcmp(extn,".bmp")!=0)
        {
            printf("Invalid output file extention !!\nDefault output file name choosed !!\n");
            goto skip;
        }
        if((strcmp(encInfo->src_image_fname,argv[4])==0) || (strcmp(encInfo->secret_fname,argv[4])==0))
        {
            printf("Two file name can't be same !!\nDefault output file name choosed !!\n");
            goto skip;
        }
        display_status("Validating output file name","Valid output file name");
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
    display_status("Files are openning","All file successfully opened");

    again :
    printf("Enter the magic string : ");
    if(fgets(encInfo->magic_string,sizeof(encInfo->magic_string),stdin) != NULL)
    {
        if (strchr(encInfo->magic_string, '\n') == NULL)
        {
            char ch;
            printf("\nINVALID :- Input may be too long!! -Try again\n\n");
            while ((ch = getchar()) != '\n' && ch != EOF);
            goto again;
        }
        *(strchr(encInfo->magic_string, '\n'))='\0';
    }
    else
    {
        printf("something went wrong , Try again\n\n");
        goto again;
    }
    display_status("Validating magic string","valid magic string");

    check_capacity(encInfo);
    display_status("Checking source file capacity","Valid source file capacity");

    copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    display_status("Copiying header bytes to output file","Header bytes susscessfully copied");

    encode_magic_string(encInfo);
    display_status("Encoding magic string","Magic string successfully encoded");

    encode_secret_file_extn_data(encInfo);
    display_status("Encoding secret file extension data","Secret file extension length and extension successfully encoded");

    encode_secret_file_size(encInfo);
    display_status("Encoding secret file size","Secret file size successfully encoded");

    encode_secret_file_data(encInfo);
    display_status("Encoding secret file data","Secret file data successfully encoded");

    copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image,encInfo);
    display_status("Copying remaining data","Remaining data successfully copied");

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
    if(encInfo->size_secret_file==0)
    {
        char ch;
        again :
        printf("\nEmpthy secret file !!\nDo you want to continue encoding (y/n): ");
        scanf("%c",&ch);
        if(getchar()!='\n')
        {
            printf("invalid input!! -Try again\n");
            while ((ch = getchar()) != '\n' && ch != EOF);
            goto again;
        }
        if(ch=='y')
        {
            printf("Process resuming...\n\n");
            goto resume;
        }
        else if(ch=='n')
        {
            printf("\nEncoding Stopped !! \n\n");
            exit(e_failure);
        }
        else
        {
            printf("invalid input!! -Try again\n");
            goto again;
        }
    }
    resume :
    encInfo->size_image_file=get_file_size(encInfo->fptr_src_image);
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
    //printf("width -%u \nheight -%u\n",width,height);
    return (width * height * 3);  //calculating the byte of data the image can hide 1byte data 8byte image data to hide
}

uint get_file_size(FILE *fptr)
{
    unsigned long fsize;
    fseek(fptr,0,SEEK_END);
    fsize=ftell(fptr);
    rewind(fptr);
    return fsize;
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
    int size =strlen(encInfo->magic_string)+1;
    encode_data_to_image(size,encInfo->magic_string,encInfo);
    return e_success;
}

Status encode_data_to_image(int size,char *data,EncodeInfo *encInfo)
{
    char image_buffer[8];
    for(int i=0;i<size;i++)
    {
        if(fread(image_buffer,1,8,encInfo->fptr_src_image)!=8)
        {
            printf("Reading from source file failed !!\n\n ");
            exit(e_failure);
        }
        encode_byte_to_lsb(data[i],image_buffer);
        if(fwrite(image_buffer,1,8,encInfo->fptr_stego_image)!=8)
        {
            printf("Writing to output file failed !!\n\n ");
            exit(e_failure);
        }    
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

Status encode_secret_file_extn_data(EncodeInfo *encInfo)
{
    //encoding extension length
    int extn_len=strlen(encInfo->extn_secret_file);
    char *ptr=(char *)(&extn_len);
    encode_data_to_image(sizeof(extn_len),ptr,encInfo);

    //encoding extension
    encode_data_to_image(extn_len,encInfo->extn_secret_file,encInfo);
    
    return e_success;  
}

Status encode_secret_file_size(EncodeInfo *encInfo)
{
    //encoding secret file size
    char *ptr=(char *)(&(encInfo->size_secret_file));
    encode_data_to_image(sizeof(encInfo->size_secret_file),ptr,encInfo);
    
    return e_success; 
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char secret_buffer;
    for(int i=0;i<encInfo->size_secret_file;i++)
    {
        if(fread(&secret_buffer,1,1,encInfo->fptr_secret)!=1)
        {
            printf("Reading from secret file !!\n\n");
            exit(e_failure);
        }
        encode_data_to_image(sizeof(secret_buffer),&secret_buffer,encInfo);  
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest,EncodeInfo *encInfo)
{
    unsigned long remining_size= encInfo->size_image_file-((unsigned)ftell(fptr_src));
    char *buffer=malloc(remining_size);
    if(fread(buffer,1,remining_size,fptr_src)!=remining_size)
    {
        printf("Reading from source file Failed !!\n\n");
        exit(e_failure);
    }
    if(fwrite(buffer,1,remining_size,fptr_dest)!=remining_size)
    {
        printf("Writing to output file Failed !!\n\n");
        exit(e_failure);
    }
    return e_success;  
}

void display_status(char *status,char *done)
{
    for(int i=0;i<=100;i++)
    {
        printf("%s........%d%%\r",status,i);
        usleep(10000);
        fflush(stdout);
    }
    printf("\r\033[K");  // Return to beginning and clear the line
    printf("%s\n", done);
}