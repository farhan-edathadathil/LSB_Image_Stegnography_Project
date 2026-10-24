#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include "decode.h"
#include "encode.h"
#include "types.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // check name of source file,secret file,stego file extension
    // char *extn=strrchr(argv[2],'.');
    // if(extn==NULL || strcmp(extn,".bmp")!=0)
    // {
    //     display_error("Invalid Input file extention");
    //     return e_failure;
    // }
    // decInfo->src_image_fname=argv[2];
    // display_status("Validating source file name","Valid source file name");

    // //secret file checking
    // extn=strrchr(argv[3],'.');
    // if(extn==NULL || strlen(extn)<2)
    // {
    //     display_error("Secret file must contain an extention");
    //     return e_failure;
    // }
    // if(strcmp(decInfo->src_image_fname,argv[3])==0)
    // {
    //     display_error("Source file name and secret file name can't be same");
    //     return e_failure;
    // }   
    // decInfo->secret_fname=argv[3];
    // decInfo->extn_secret_file=extn+1;
    // display_status("Validating secret file name","Valid secret file name");

    // //check out put file name
    // if(argv[4]!=NULL)
    // {
    //     extn=strrchr(argv[4],'.');
    //     if(extn==NULL || strcmp(extn,".bmp")!=0)
    //     {
    //         printf("INVALID : Invalid output file extention !!\nFIX : Default output file name choosed !!\n");
    //         goto skip;
    //     }
    //     if((strcmp(decInfo->src_image_fname,argv[4])==0) || (strcmp(decInfo->secret_fname,argv[4])==0))
    //     {
    //         printf("INVALID : Two file name can't be same !!\nFIX : Default output file name choosed !!\n");
    //         goto skip;
    //     }
    //     display_status("Validating output file name","Valid output file name");
    //     decInfo->stego_image_fname=argv[4];
    // }
    // else
    // {
    //     skip:
    //     decInfo->stego_image_fname="stego.bmp";
    // }

    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    return e_success;
}


void close_decfile(DecodeInfo *decInfo)
{

}

void display_decode_error(char *error)
{
    printf("\n%-9s : %s !!\n","[ERROR]",error);
    printf("\n**************** Decoding FAILED ****************\n\n");
}