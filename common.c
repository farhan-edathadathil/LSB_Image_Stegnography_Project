#include<stdio.h>
#include"common.h"
#include"types.h"
#include<string.h>

OperationType check_operation_type(char *argv[])
{

    if(strcmp(argv[1],"-e")==0)
        return e_encode;
    else if(strcmp(argv[1],"-d")==0)
        return e_decode;
    else
        return e_unsupported;
}

void display_help()
{
    printf("\n");
    printf("************************************************************\n");
    printf("              LSB IMAGE STEGANOGRAPHY\n");
    printf("                     USER MANUAL\n");
    printf("************************************************************\n");

    printf("\nDESCRIPTION:\n");
    printf("  This project implements image steganography using the\n");
    printf("  Least Significant Bit (LSB) technique to conceal secret\n");
    printf("  data within a BMP image file and retrieve it when needed.\n");

    printf("\nUSAGE:\n");
    printf("  ./a.out <option> [arguments]\n");

    printf("\nOPTIONS:\n");
    printf("  -e    Encode a secret file into a BMP image file.\n");
    printf("  -d    Decode hidden data from a stego image file.\n");

    printf("\nENCODING:\n");
    printf("  Command :\n");
    printf("    ./a.out -e <source_image.bmp> <secret_file> [output_image.bmp]\n");
    printf("\n");
    printf("  Arguments:\n");
    printf("    source_image.bmp  Source BMP image file.\n");
    printf("    secret_file       File to hide inside the image.\n");
    printf("    output_image.bmp  Optional output BMP filename.\n");
    printf("                      Default: stego.bmp\n");
    

    printf("\n  Example:\n");
    printf("    ./a.out -e input.bmp secret.txt output.bmp\n");
    printf("    ./a.out -e input.bmp secret.c \n");

    printf("\nDECODING:\n");
    printf("  Command :\n");
    printf("    ./a.out -d <encoded_image.bmp> [output_file]\n");
    printf("\n");
    printf("  Arguments:\n");
    printf("    encoded_image.bmp  Image containing hidden data.\n");
    printf("    output_file        Optional name for the extracted file.\n");

    printf("\n  Example:\n");
    printf("    ./a.out -d stego.bmp\n");
    printf("    ./a.out -d stego.bmp recovered.txt\n");

    printf("\nSUPPORTED FILES:\n");
    printf("  Source image : .bmp\n");
    printf("  Secret file  : Any file type supported by the program.\n");
    printf("  Output image : .bmp\n");

    printf("\nNOTES:\n");
    printf("  * The source image must be an RGB BMP image with\n");
    printf("    a color depth of 24 bits per pixel (24 bpp).\n");


    printf("\n************************************************************\n\n");
   

}

void display_comerror(char *error)
{
    printf("\n%-9s : %s !!\n","[ERROR]",error);
    display_help();
    printf("\n******************* Programm Terminated ********************\n\n");
}