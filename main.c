#include<stdio.h>
#include"encode.h"
#include"decode.h"
#include"types.h"
#include"common.h"

int main(int argc,char **argv)
{
    if(argc<2)
    {
        display_comerror("Not enough arguments given");
        return e_failure;
    }
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    if(check_operation_type(argv)==e_encode)
    {
        if(argc<4)
        {
            display_error("Not enough arguments given for encodeing");
            display_help();
            return e_failure;
        }
        printf("\n************************************************\n");
        printf("        Encoding Operation Selected\n");
        printf("************************************************\n\n");

        if(read_and_validate_encode_args(argv,&encInfo)!=e_success)
            return e_failure;

        do_encoding(&encInfo);
    }
    else if(check_operation_type(argv)==e_decode)
    {
        if(argc<3)
        {
            display_error("Not enough arguments given for decodeing");
            display_help();
            return e_failure;
        }
        printf("\n************************************************\n");
        printf("        Decoding Operation Selected\n");
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

