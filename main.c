#include <stdio.h>
#include "encode.h"
#include "types.h"

int main(int argc,char **argv)
{
    if(argc<2)
    {
        printf("\nNot enough arguments given !!\n\n");
        return e_failure;
    }
    EncodeInfo encInfo;

    if(check_operation_type(argv)==e_encode)
    {
        if(argc<4)
        {
            printf("\nNot enough arguments given for encodeing!!\n\n");
            return e_failure;
        }
        printf("We choosed Encoding !!\n");
        if(read_and_validate_encode_args(argv,&encInfo)!=e_success)
        {
            return e_failure;
        }
        do_encoding(&encInfo);
    }
    else if(check_operation_type(argv)==e_decode)
    {
        if(argc<3)
        {
            printf("\nNot enough arguments given for decodeing!!\n\n");
            return e_failure;
        }
        printf("We choosed Decoding !!\n");
    }
    else
    {
        printf("\nYou have given Unsupported argument !!\nPass -e for encoding , -d for decoding\n\n");
        return e_failure;
    }

}

