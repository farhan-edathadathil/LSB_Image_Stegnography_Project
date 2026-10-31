#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include"encode.h"
#include"common.h"
#include"types.h"


Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    /*
    ============================================================================
    Function    : read_and_validate_encode_args
    Description : Reads and validates the source image, secret file and
                output image filenames supplied through command-line
                arguments.

    Logic       :
                1. Extracts the source image extension using strrchr().
                2. Checks whether the source image has a .bmp extension.
                3. Stores the validated source image filename.
                4. Extracts and validates the secret file extension.
                5. Checks whether the source image and secret file
                    filenames are identical.
                6. Stores the secret filename and its extension.
                7. Checks whether an optional output filename is given.
                8. If provided:
                    a. Validates the output image extension.
                    b. Checks whether the filename contains a name
                        apart from its extension.
                    c. Checks whether the output filename matches
                        either input filename.
                    d. Stores the valid output filename.
                9. If the output filename is missing or invalid:
                    a. Selects "stego.bmp" as the default filename.
                    b. Checks whether the default filename conflicts
                        with either input filename.
                10. Returns e_success after successful validation.

    Benefit     : Ensures that the required filenames and extensions
                are validated before starting the encoding process.

    Parameters  :
                argv    - Array containing command-line arguments.
                encInfo - Structure used to store source image,
                            secret file and output image information.

    Return      : Status
                e_success - Required filenames are validated.
                e_failure - Invalid source image or secret filename.
    ============================================================================
    */
    char *extn=strrchr(argv[2],'.');
    if(extn==NULL || strcmp(extn,".bmp")!=0)
    {
        display_error("Invalid source file extention");
        return e_failure;
    }
    encInfo->src_image_fname=argv[2];
    display_status("Validating source file name","Valid source file name");

    //secret file checking
    extn=strrchr(argv[3],'.');
    if(extn==NULL || strlen(extn)<2)
    {
        display_error("Secret file must contain an extention");
        return e_failure;
    }
    if(strcmp(encInfo->src_image_fname,argv[3])==0)
    {
        display_error("Source file name and secret file name can't be same");
        return e_failure;
    }   
    encInfo->secret_fname=argv[3];
    encInfo->extn_secret_file=extn+1;
    display_status("Validating secret file name","Valid secret file name");
    char *input = encInfo->src_image_fname;
    char *scrt_ptr=encInfo->secret_fname;
    char *output = NULL;
    
    //check out put file name
    if(argv[4]!=NULL)
    {
        extn=strrchr(argv[4],'.');
        if(extn==NULL || strcmp(extn,".bmp")!=0)
        {
            printf("[INVALID] : Invalid output file extention !!\n%-9s : Default output file name choosed !!\n","[FIX]");
            goto skip;
        }
        if(strlen(argv[4])==strlen(extn))
        {
            printf("[INVALID] : No file name found only extension !!\n%-9s : Default output file name choosed !!\n","[FIX]");
            goto skip;
        }
        output = argv[4];
        if(strncmp(input, "./", 2) == 0)
            input += 2;
        if(strncmp(scrt_ptr, "./", 2) == 0)
            scrt_ptr += 2;
        if(strncmp(output, "./", 2) == 0)
            output += 2;
        if((strcmp(input,output)==0) || (strcmp(scrt_ptr,output)==0))
        {
            printf("[INVALID] : Two file name can't be same !!\n%-9s : Default output file name choosed !!\n","[FIX]");
            goto skip;
        }
        display_status("Validating output file name","Valid output file name");
        encInfo->stego_image_fname=argv[4];
    }
    else
    {
        display_status("Choosing default file name","Default output file name choosed");
        skip:
        encInfo->stego_image_fname="stego.bmp";
        
        output = encInfo->stego_image_fname;
        if(strncmp(input, "./", 2) == 0)
            input += 2;
        if(strncmp(scrt_ptr, "./", 2) == 0)
            scrt_ptr += 2;
        if(strncmp(output, "./", 2) == 0)
            output += 2;
        if((strcmp(input,output)==0) || (strcmp(scrt_ptr,output)==0))
        {
            display_error("One of your input file name same as default output file name, So give a output file name");
            exit(e_failure);
        }
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    /*
    ============================================================================
    Function    : do_encoding
    Description : Performs the complete LSB encoding process by hiding
                a secret file inside a 24-bit BMP image.

    Logic       :
                1. Calls open_files() to open the source image,
                    secret file and output image.
                2. Reads the magic string from the user using fgets().
                3. Checks whether the input contains a newline.
                4. Rejects oversized input and retries.
                5. Removes the newline character and validates that
                    the magic string contains at least two characters.
                6. Calls check_capacity() to verify image capacity
                    and supported bits per pixel.
                7. Calls copy_bmp_header() to copy the 54-byte BMP header.
                8. Calls encode_magic_string() to encode the magic string.
                9. Calls encode_secret_file_extn_data() to encode the
                    secret file extension length and extension.
                10. Calls encode_secret_file_size() to encode the
                    secret file size.
                11. Calls encode_secret_file_data() to encode the
                    actual secret file contents.
                12. Calls copy_remaining_img_data() to copy the
                    remaining source image data.
                13. Calls close_encfile() to close all opened files.
                14. Displays the successful encoding message.

    Benefit     : Coordinates all encoding functions and performs
                the complete secret file hiding operation.

    Parameters  :
                encInfo - Structure containing source image, secret file,
                            output image, file sizes and magic string.

    Return      : Status
                e_success - Encoding completed successfully.
    ============================================================================
    */
    open_files(encInfo);
    display_status("Files are openning","All file opened successfully");

    again :
    printf("%-9s : Enter the magic string : ","[INPUT]");
    if(fgets(encInfo->magic_string,(sizeof(encInfo->magic_string)),stdin) != NULL)
    {
        if (strchr(encInfo->magic_string, '\n') == NULL)
        {
            char ch;
            printf("[INVALID] : Input may be too long, only 2 character is valid !! -Try again\n");
            while ((ch = getchar()) != '\n' && ch != EOF);
            goto again;
        }
        *(strchr(encInfo->magic_string, '\n'))='\0';
        if(strlen(encInfo->magic_string)<2)
        {
            printf("[INVALID] : Empty input or not given 2 character !! -Try again\n");
            goto again;
        }
    }
    else
    {
         display_error("Something went wrong :- No input for magic string");
        exit(e_failure);
    }
    display_status("Validating magic string","Valid magic string");

    check_capacity(encInfo);
    display_status("Checking source file capacity and bits per pixel","Valid source file capacity and bits per pixel");

    copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    display_status("Copiying header bytes to output file","Header bytes copied successfully");

    encode_magic_string(encInfo);
    display_status("Encoding magic string","Magic string encoded successfully");

    encode_secret_file_extn_data(encInfo);
    display_status("Encoding secret file extension data","Secret file extension length and extension encoded successfully");

    encode_secret_file_size(encInfo);
    display_status("Encoding secret file size","Secret file size encoded successfully");

    encode_secret_file_data(encInfo);
    display_status("Encoding secret file data","Secret file data encoded successfully");

    copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image,encInfo);
    display_status("Copying remaining data","Remaining data copied successfully");

    close_encfile(encInfo);
    display_status("Closing all files","All files closed successfully");

    printf("\n************************************************\n");
	printf("        Encoding Completed Successfully\n");
	printf("************************************************\n");
    return e_success;

}

Status open_files(EncodeInfo *encInfo)
{
    /*
    ============================================================================
    Function    : open_files
    Description : Opens the source BMP image, secret file and output
                stego image for the encoding operation.

    Logic       :
                1. Opens the source image in read mode using fopen().
                2. Checks whether the source image was opened successfully.
                3. If opening fails, displays an error and terminates
                    the program.
                4. Opens the secret file in read mode.
                5. Checks whether the secret file was opened successfully.
                6. If opening fails, displays an error and terminates
                    the program.
                7. Opens the output stego image in write mode.
                8. Returns e_success.

    Benefit     : Establishes the required file pointers before starting
                the encoding process.

    Parameters  :
                encInfo - Structure containing filenames and file
                            pointers for the encoding operation.

    Return      : Status
                e_success - Source and secret files opened successfully.
                Note: The output file pointer is not checked for NULL.
    ============================================================================
    */
    encInfo->fptr_src_image=fopen(encInfo->src_image_fname,"rb");
    if(encInfo->fptr_src_image==NULL)
    {
        printf("\n%-9s : %s file not found in PWD !!\n\n","[ERROR]",encInfo->src_image_fname);
        printf("\n**************** Encoding FAILED ****************\n");
        printf("************** Programm Terminated **************\n\n");
        exit(e_failure);
    }
    encInfo->fptr_secret=fopen(encInfo->secret_fname,"rb");
    if(encInfo->fptr_secret==NULL)
    {
        printf("\n%-9s : %s file not found in PWD !!\n","[ERROR]",encInfo->secret_fname);
        printf("\n**************** Encoding FAILED ****************\n");
        printf("************** Programm Terminated **************\n\n");
        exit(e_failure);
    }
    encInfo->fptr_stego_image=fopen(encInfo->stego_image_fname,"wb");
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    /*
    ============================================================================
    Function    : check_capacity
    Description : Checks whether the source BMP image has sufficient
                capacity to store the secret file and encoding metadata.

    Logic       :
                1. Calls get_image_size_for_bmp() to calculate the
                    image capacity and validate 24-bit pixel depth.
                2. Calls get_file_size() to calculate the secret file size.
                3. If the secret file is empty:
                    a. Displays a warning.
                    b. Asks the user whether encoding should continue.
                    c. If 'y' is entered, resumes encoding.
                    d. If 'n' is entered, terminates encoding.
                    e. Rejects invalid input and retries.
                4. Calculates the complete encoding data size using:
                    a. Magic string length.
                    b. Size of the secret file extension length.
                    c. Secret file extension length.
                    d. Size of the secret file size variable.
                    e. Actual secret file size.
                5. Multiplies the total data size by 8 because each
                    secret data byte requires 8 image bytes.
                6. Compares the required image capacity with the
                    available image capacity.
                7. If capacity is insufficient, displays an error
                    and terminates the program.
                8. Returns e_success if sufficient capacity is available.

    Benefit     : Prevents encoding when the source image cannot
                accommodate the secret file and its metadata.

    Parameters  :
                encInfo - Structure containing image capacity,
                            secret file size and related information.

    Return      : Status
                e_success - Sufficient image capacity is available.
    ============================================================================
    */
    encInfo->image_capacity= get_image_size_for_bmp(encInfo->fptr_src_image,encInfo);

    encInfo->size_secret_file= get_file_size(encInfo->fptr_secret);
    if(encInfo->size_secret_file==0)
    {
        char ch;
        again :
        printf("[WARNING] : Empty secret file !!\n%-9s : Do you want to continue encoding (y/n): ","[INPUT]");
        scanf("%c",&ch);
        if(getchar()!='\n')
        {
            printf("[INVALID] : invalid input!! -Try again\n");
            while ((ch = getchar()) != '\n' && ch != EOF);
            goto again;
        }
        if(ch=='y')
        {
            display_status("Encoding resuming...","Encoding resumed");
            goto resume;
        }
        else if(ch=='n')
        {
            display_error("Encoding Stopped");
            exit(e_failure);
        }
        else
        {
            printf("[INVALID] : invalid input!! -Try again\n");
            goto again;
        }
    }
    resume :
    encInfo->size_image_file=get_file_size(encInfo->fptr_src_image);
    unsigned long encode_data_bytes=0;
    encode_data_bytes=strlen(encInfo->magic_string)+sizeof(int)+strlen(encInfo->extn_secret_file)+sizeof(long)+encInfo->size_secret_file;
    if(encInfo->image_capacity < (encode_data_bytes*8))
    {
        printf("\n%-9s : %s file don't have the capacity to hold this %s file data\n%-9s : Try a diffrent source file !!\n","[ERORR]",encInfo->src_image_fname,encInfo->secret_fname,"[FIX]");
        printf("\n**************** Encoding FAILED ****************\n");
        printf("************** Programm Terminated **************\n\n");
        exit(e_failure);
    }
    return e_success;
}

unsigned long get_image_size_for_bmp(FILE *fptr_image,EncodeInfo *encInfo)
{
    /*
    ============================================================================
    Function    : get_image_size_for_bmp
    Description : Calculates the data capacity of a BMP image and
                verifies that the image uses 24 bits per pixel.

    Logic       :
                1. Moves the image file pointer to byte position 18.
                2. Reads 4 bytes to obtain the image width.
                3. Reads the next 4 bytes to obtain the image height.
                4. Moves the file pointer to byte position 28.
                5. Reads 2 bytes to obtain the bits-per-pixel value.
                6. Checks whether the image uses 24 bits per pixel.
                7. If the image does not use 24 bpp, displays an error
                    and terminates the program.
                8. Calculates the image data capacity using:
                    width * height * (bits_per_pixel / 8).
                9. Returns the calculated capacity.

    Benefit     : Determines the available image data bytes that can
                be used for hiding secret information.

    Parameters  :
                fptr_image - File pointer of the source BMP image.
                encInfo    - Structure used to store bits-per-pixel
                            information.

    Return      : uint
                Returns the calculated image capacity in bytes.
    ============================================================================
    */
    unsigned int width,height;

    fseek(fptr_image,18,SEEK_SET);
    fread(&width,4,1,fptr_image);
    fread(&height,4,1,fptr_image);
    fseek(fptr_image,28,SEEK_SET);
    fread(&(encInfo->bits_per_pixel),2,1,fptr_image);
    if(encInfo->bits_per_pixel != 24)
    {
        display_error("Source file is not RGB(24 bits per pixel) pixel file");
        exit(e_failure);
    }
    return (((unsigned long)width) * ((unsigned long)height) * (encInfo->bits_per_pixel/8));  //calculating the byte of data the image can hide 1byte data 8byte image data to hide
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    /*
    ============================================================================
    Function    : copy_bmp_header
    Description : Copies the 54-byte BMP header from the source image
                to the destination stego image.

    Logic       :
                1. Declares a temporary buffer of 54 bytes.
                2. Calls rewind() to move the source image pointer
                    to the beginning.
                3. Reads 54 bytes from the source image into the buffer.
                4. Calls rewind() to move the destination image pointer
                    to the beginning.
                5. Writes the 54-byte header into the destination image.
                6. Returns e_success.

    Benefit     : Preserves the original BMP header so that the output
                stego file retains the basic BMP image structure.

    Parameters  :
                fptr_src_image  - File pointer of the source BMP image.
                fptr_dest_image - File pointer of the destination image.

    Return      : Status
                e_success - Header copy operation completed.
    ============================================================================
    */
    char temp[54];
    rewind(fptr_src_image);
    if(fread(temp,1,54,fptr_src_image) != 54)
    {
        display_error("Reading heder bytes failed");
        exit(e_failure);
    }
    rewind(fptr_dest_image);
    if(fwrite(temp,1,54,fptr_dest_image)!= 54)
    {
        display_error("Writing heder bytes failed");
        exit(e_failure);
    }
    return e_success;

}

Status encode_magic_string(EncodeInfo *encInfo)
{
    /*
    ============================================================================
    Function    : encode_magic_string
    Description : Encodes the user-provided magic string into the
                source BMP image using the LSB technique.

    Logic       :
                1. Calculates the magic string length using strlen().
                2. Calls encode_data_to_image() with the magic string
                    and its length.
                3. Returns e_success after encoding.

    Benefit     : Stores an identification string inside the image.
                During decoding, this string is used to authenticate
                whether the image contains the expected hidden data.

    Parameters  :
                encInfo - Structure containing the magic string and
                            source and destination file pointers.

    Return      : Status
                e_success - Magic string encoding completed.
    ============================================================================
    */
    int size =strlen(encInfo->magic_string);
    encode_data_to_image(size,encInfo->magic_string,encInfo);
    return e_success;
}

Status encode_data_to_image(int size,char *data,EncodeInfo *encInfo)
{
    /*
    ============================================================================
    Function    : encode_data_to_image
    Description : Encodes the supplied data into the least significant
                bits of the source image bytes.

    Logic       :
                1. Declares an 8-byte image buffer.
                2. Iterates through every byte of the input data.
                3. Reads 8 bytes from the source image for each
                    input data byte.
                4. Checks whether exactly 8 bytes were read.
                5. Calls encode_byte_to_lsb() to embed the current
                    data byte into the 8 image bytes.
                6. Writes the modified 8 bytes into the destination
                    stego image.
                7. Checks whether exactly 8 bytes were written.
                8. Repeats until all input data bytes are encoded.
                9. Returns e_success.

    Benefit     : Implements the main data transfer mechanism used
                to hide information inside the BMP image.

    Parameters  :
                size    - Number of data bytes to encode.
                data    - Pointer to the data that must be encoded.
                encInfo - Structure containing source and destination
                            image file pointers.

    Return      : Status
                e_success - Data encoding completed successfully.
    ============================================================================
    */
    char image_buffer[8];
    for(int i=0;i<size;i++)
    {
        if(fread(image_buffer,1,8,encInfo->fptr_src_image)!=8)
        {
            display_error("Reading from source file failed");
            exit(e_failure);
        }
        encode_byte_to_lsb(data[i],image_buffer);
        if(fwrite(image_buffer,1,8,encInfo->fptr_stego_image)!=8)
        {
            display_error("Writing to output file failed");
            exit(e_failure);
        }    
    }
    return e_success;
}

Status encode_byte_to_lsb(const unsigned char data, char *image_buffer)
{
    /*
    ============================================================================
    Function    : encode_byte_to_lsb
    Description : Embeds one data byte into the least significant bits
                of eight image bytes.

    Logic       :
                1. Iterates through the 8 bits of the input data byte.
                2. Extracts each bit using right shift and bitwise AND.
                3. Clears the LSB of the corresponding image byte
                    using bitwise AND with ~(1).
                4. Inserts the extracted data bit into the LSB
                    of the image byte using bitwise OR.
                5. Repeats until all 8 bits are embedded.
                6. Returns e_success.

    Benefit     : Performs the basic LSB modification required to
                hide one byte of secret information in the image.

    Parameters  :
                data         - One byte of secret information.
                image_buffer - Pointer to 8 image bytes in which
                                the data bits are embedded.

    Return      : Status
                e_success - All 8 bits are embedded successfully.
    ============================================================================
    */

    for(int j=0;j<8;j++)
    {    
        image_buffer[j]=(image_buffer[j] & ~(1)) | (data>>j & 1);    
    }
    return e_success;

}

Status encode_secret_file_extn_data(EncodeInfo *encInfo)
{
    /*
    ============================================================================
    Function    : encode_secret_file_extn_data
    Description : Encodes the secret file extension length and the
                actual extension into the source BMP image.

    Logic       :
                1. Calculates the secret file extension length using
                    strlen().
                2. Calls encode_data_to_image() to encode the extension
                    length using sizeof(extn_len) bytes.
                3. Calls encode_data_to_image() again to encode the
                    actual extension characters.
                4. Returns e_success.

    Benefit     : Preserves the original secret file extension so that
                the decoder can reconstruct the extracted filename
                with the appropriate extension.

    Parameters  :
                encInfo - Structure containing the secret file extension,
                            its length and image file pointers.

    Return      : Status
                e_success - Extension length and extension encoded.
    ============================================================================
    */
    //encoding extension length
    int extn_len=strlen(encInfo->extn_secret_file);
    encode_data_to_image(sizeof(extn_len),(char *)(&extn_len),encInfo);

    //encoding extension
    encode_data_to_image(extn_len,encInfo->extn_secret_file,encInfo);
    
    return e_success;  
}

Status encode_secret_file_size(EncodeInfo *encInfo)
{
    /*
    ============================================================================
    Function    : encode_secret_file_size
    Description : Encodes the size of the secret file into the source
                BMP image.

    Logic       :
                1. Obtains the address of size_secret_file.
                2. Determines its memory size using sizeof().
                3. Calls encode_data_to_image() to encode the complete
                    size variable into the image.
                4. Returns e_success.

    Benefit     : Stores the original secret file size so that the
                decoder knows exactly how many bytes must be extracted.

    Parameters  :
                encInfo - Structure containing the secret file size
                            and image file pointers.

    Return      : Status
                e_success - Secret file size encoded successfully.
    ============================================================================
    */
    //encoding secret file size
    encode_data_to_image(sizeof(encInfo->size_secret_file),(char *)(&(encInfo->size_secret_file)),encInfo);
    
    return e_success; 
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    /*
    ============================================================================
    Function    : encode_secret_file_data
    Description : Reads the actual contents of the secret file and
                encodes them into the source BMP image.

    Logic       :
                1. Declares a temporary character buffer.
                2. Iterates from zero to the secret file size.
                3. Reads one byte from the secret file.
                4. Checks whether one byte was successfully read.
                5. Calls encode_data_to_image() to encode the
                    current secret byte into 8 image bytes.
                6. Repeats until the entire secret file is encoded.
                7. Returns e_success.

    Benefit     : Performs the actual hiding of the secret file contents
                inside the source BMP image.

    Parameters  :
                encInfo - Structure containing the secret file pointer,
                            secret file size and image file pointers.

    Return      : Status
                e_success - Secret file data encoded successfully.
    ============================================================================
    */
    char secret_buffer;
    for(unsigned long i=0;i<encInfo->size_secret_file;i++)
    {
        if(fread(&secret_buffer,1,1,encInfo->fptr_secret)!=1)
        {
            display_error("Reading from secret file failed");
            exit(e_failure);
        }
        encode_data_to_image(sizeof(secret_buffer),&secret_buffer,encInfo);  
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest,EncodeInfo *encInfo)
{
    /*
    ============================================================================
    Function    : copy_remaining_img_data
    Description : Copies the remaining unmodified source image data
                into the destination stego image after encoding.

    Logic       :
                1. Calculates the remaining image size by subtracting
                    the current source file position from size_image_file.
                2. Allocates memory dynamically using malloc().
                3. If memory allocation succeeds:
                    a. Reads the remaining image data into the buffer.
                    b. Checks whether the complete data was read.
                    c. Writes the remaining data into the destination.
                    d. Checks whether the complete data was written.
                    e. Frees the allocated memory.
                4. If memory allocation fails:
                    a. Declares a temporary buffer of 256 bytes.
                    b. Reads the remaining image data in chunks.
                    c. Writes each chunk into the destination image.
                    d. Checks for source file read errors.
                5. Returns e_success.

    Benefit     : Preserves the remaining image data that was not used
                for encoding, ensuring that the output image contains
                the complete image data.

    Parameters  :
                fptr_src  - File pointer of the source BMP image.
                fptr_dest - File pointer of the destination stego image.
                encInfo   - Structure containing the original image size.

    Return      : Status
                e_success - Remaining image data copied successfully.
    ============================================================================
    */
    unsigned long remining_size= encInfo->size_image_file-((unsigned)ftell(fptr_src));
    char *buffer=malloc(remining_size);
    if(buffer!=NULL)
    {   
        if(fread(buffer,1,remining_size,fptr_src)!=remining_size)
        {
            display_error("Reading from source file failed");
            free(buffer);
            exit(e_failure);
        }
        if(fwrite(buffer,1,remining_size,fptr_dest)!=remining_size)
        {
            display_error("Writing to output file failed");
            free(buffer);
            exit(e_failure);
        }
        free(buffer);
    }
    else
    {
        char temp[256];
        size_t n;
        while((n=fread(temp,1,sizeof(temp),fptr_src))>0)
        {
            if(fwrite(temp,1,n,fptr_dest)!=n)
            {
                display_error("Writing to output file failed");
                exit(e_failure);
            }
        }
        if (ferror(fptr_src))
        {
            perror("ERROR : ");
            display_error("Reading source file Failed");
            exit(e_failure);
        }

    }
    return e_success;  
}

void close_encfile(EncodeInfo *encInfo)
{
    /*
    ============================================================================
    Function    : close_encfile
    Description : Closes all files opened during the encoding operation.

    Benefit     : Releases file resources after the encoding operation
                and ensures that buffered output is flushed when files
                are closed normally.

    Parameters  :
                encInfo - Structure containing all encoding file pointers.

    Return      : void
                Does not return any value.
    ============================================================================
    */
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);
}

void display_error(char *error)
{
    /*
    ============================================================================
    Function    : display_error
    Description : Displays an encoding-related error message along
                with the encoding failure and program termination messages.

    Logic       :
                1. Receives an error description through the argument.
                2. Prints the error description.
                3. Displays the encoding failure message.
                4. Displays the program termination message.

    Benefit     : Provides a common error display mechanism for
                different encoding functions.

    Parameters  :
                error - String containing the error description.

    Return      : void
                Does not return any value.
    ============================================================================
    */
    printf("\n%-9s : %s !!\n","[ERROR]",error);
    printf("\n**************** Encoding FAILED ****************\n");
    printf("************** Programm Terminated **************\n\n");
}