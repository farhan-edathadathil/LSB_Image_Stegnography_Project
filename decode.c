#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include "decode.h"
#include "encode.h"
#include"common.h"
#include "types.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    /*
    ============================================================================
    Function    : read_and_validate_decode_args
    Description : Reads and validates the input stego image filename and
                optional output filename supplied through command-line
                arguments.

    Logic       :
                1. Extracts the input image extension using strrchr().
                2. Checks whether the input image has a .bmp extension.
                3. Stores the validated input stego image filename.
                4. Checks whether an optional output filename is provided.
                5. If an output filename is provided:
                    a. Searches for its extension using strrchr().
                    b. Removes the extension if it exists.
                    c. Checks whether a valid filename remains.
                    d. Checks whether the filename exceeds the available
                        output_fname array capacity.
                    e. Stores the validated output filename.
                6. If the output filename is missing or invalid:
                    a. Selects "output" as the default filename.
                7. Returns e_success.

    Benefit     : Validates the input and output filenames before
                beginning the decoding process.

    Parameters  :
                argv    - Array containing command-line arguments.
                decInfo - Structure containing input stego image
                            and output filename information.

    Return      : Status
                e_success - Filename validation completed successfully.
                e_failure - Invalid input image extension.
    ============================================================================
    */
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
    /*
    ============================================================================
    Function    : do_decoding
    Description : Performs the complete decoding process by extracting
                the hidden secret file from the stego BMP image.

    Logic       :
                1. Calls open_file_and_checks() to open and validate
                    the input stego image.
                2. Calls authenticate_magic_str() to verify the
                    user-provided magic string.
                3. If authentication fails, displays an error and
                    terminates the program.
                4. Calls decode_extn_data() to extract the secret
                    file extension length and extension.
                5. Calls create_secret_file() to create the output file.
                6. Calls decode_secret_data_size() to extract the
                    original secret file size.
                7. Calls decode_secret_data() to extract and write
                    the actual secret file contents.
                8. Calls close_decfile() to close all opened files.
                9. Displays the successful decoding message.

    Benefit     : Coordinates all decoding functions and reconstructs
                the original secret file from the stego image.

    Parameters  :
                decInfo - Structure containing input image, output file,
                            magic string and decoding information.

    Return      : Status
                e_success - Decoding completed successfully.
    ============================================================================
    */
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

    decode_secret_data_size(decInfo);
    display_status("Decoding secret data size","Secret data size decoded successfully");

    create_secret_file(decInfo);
    display_status("Creating secret file","Secret file created successfully");

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
    /*
    ============================================================================
    Function    : open_file_and_checks
    Description : Opens the input stego image and validates its basic
                file size and bits-per-pixel information.

    Logic       :
                1. Opens the input stego image in read mode using fopen().
                2. Checks whether the file was opened successfully.
                3. Calculates the input file size using get_file_size().
                4. Checks whether the file size is at least 54 bytes.
                5. If the file size is smaller than 54 bytes, displays
                    an error and terminates the program.
                6. Moves the file pointer to byte position 28.
                7. Reads 2 bytes to obtain the bits-per-pixel value.
                8. Checks whether the image uses 24 bits per pixel.
                9. If the image does not use 24 bpp, displays an error
                    and terminates the program.
                10. Returns e_success.

    Benefit     : Ensures that the input file meets the basic size
                and pixel-depth requirements before decoding begins.

    Parameters  :
                decInfo - Structure containing the input filename,
                            file pointer, file size and bits-per-pixel value.

    Return      : Status
                e_success - Input image passed the implemented checks.
    ============================================================================
    */
    decInfo->fptr_input_stego=fopen(decInfo->input_stego_fname,"rb");
    if(decInfo->fptr_input_stego==NULL)
    {
        printf("\n%-9s : %s file not found in PWD !!\n","[ERROR]",decInfo->input_stego_fname);
        printf("\n**************** Decoding FAILED ****************\n");
        printf("************** Programm Terminated **************\n\n");
        exit(e_failure);
    }
    if((decInfo->size_stego_file=get_file_size(decInfo->fptr_input_stego))<54)
    {
        printf("\n%-9s : %s file is empty or corrupted !!\n","[ERROR]",decInfo->input_stego_fname);
        printf("\n**************** Decoding FAILED ****************\n");
        printf("************** Programm Terminated **************\n\n");
        exit(e_failure);
    }
    fseek(decInfo->fptr_input_stego,28,SEEK_SET);
    fread(&(decInfo->bits_per_pixel),2,1,decInfo->fptr_input_stego);
    if(decInfo->bits_per_pixel != 24)
    {
        display_decode_error("Input file is not RGB(24 bits per pixel) pixel file");
        exit(e_failure);
    }
    return e_success;
}

Status authenticate_magic_str(DecodeInfo *decInfo)
{
    /*
    ============================================================================
    Function    : authenticate_magic_str
    Description : Reads the magic string from the user and compares it
                with the magic string hidden inside the stego image.

    Logic       :
                1. Prompts the user to enter the magic string.
                2. Reads the input using fgets().
                3. Checks whether the input contains a newline character.
                4. If the input is too long, clears the remaining input
                    from stdin and asks the user to enter it again.
                5. Removes the newline character from the input.
                6. Checks whether the magic string contains at least
                    two characters.
                7. Calculates the magic string length using strlen().
                8. Declares a temporary character array to store the
                    decoded magic string.
                9. Moves the input image file pointer to byte position 54.
                10. Calls fsize_check() to check the available file size.
                11. Calls decode_data_from_image() to extract the
                    hidden magic string.
                12. Adds the null terminator to the extracted string.
                13. Compares the user-provided magic string with the
                    extracted magic string using strcmp().
                14. Returns e_failure if both strings do not match.
                15. Returns e_success if authentication succeeds.

    Benefit     : Prevents extraction from proceeding when the
                user-provided magic string does not match the
                identification string stored in the image.

    Parameters  :
                decInfo - Structure containing the input image pointer,
                            user-provided magic string and magic length.

    Return      : Status
                e_success - Magic strings match.
                e_failure - Magic strings do not match.
    ============================================================================
    */
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
        display_decode_error("Something went wrong :- No input for magic string");
        exit(e_failure);
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
    /*
    ============================================================================
    Function    : decode_extn_data
    Description : Extracts the secret file extension length and extension
                from the stego image and constructs the output filename.

    Logic       :
                1. Declares an integer variable to store the extension length.
                2. Calls fsize_check() to verify sufficient data is available.
                3. Calls decode_data_from_image() to extract the
                    extension length.
                4. Checks whether the extension length exceeds 256 bytes.
                5. If it exceeds the limit, displays an error and
                    terminates the program.
                6. Declares a temporary character array based on the
                    extracted extension length.
                7. Calls fsize_check() to verify sufficient data
                    for the extension.
                8. Calls decode_data_from_image() to extract the
                    actual extension characters.
                9. Adds a null terminator to the extracted extension.
                10. If the output filename is the default "output":
                    a. Uses the default filename.
                    b. Appends a dot if the extension does not contain one.
                    c. Appends the extracted extension.
                11. Otherwise:
                    a. Checks whether the combined filename exceeds
                        the output_fname array capacity.
                    b. If too large, switches to the default filename.
                    c. Appends the extracted extension.
                    d. Checks whether the output filename matches
                        the input stego image filename.
                12. Returns e_success after constructing the filename.

    Benefit     : Recovers the original file extension and creates
                an appropriate output filename for the extracted file.

    Parameters  :
                decInfo - Structure containing the input image pointer,
                            output filename and extracted extension information.

    Return      : Status
                e_success - Extension extracted and filename constructed.
    ============================================================================
    */
    int extn_len;
    fsize_check(sizeof(extn_len),decInfo);
    decode_data_from_image(sizeof(extn_len),&extn_len,decInfo);
    if(extn_len <= 0 || extn_len > 248)
    {
        display_decode_error("Unsupported extension length, extension decoding limit is 1 to 248 bytes");
        exit(e_failure);
    }
    char extn[extn_len+1];
    fsize_check(extn_len,decInfo);
    decode_data_from_image(extn_len,extn,decInfo);
    extn[extn_len]='\0';
    char *input = decInfo->input_stego_fname;
    char *output = decInfo->output_fname;
    if(strcmp(decInfo->output_fname,"output")==0)
        goto skip2;
    if((strlen(decInfo->output_fname)+extn_len+2)>sizeof(decInfo->output_fname))
    {
        printf("[INVALID] : Output file name with extension is too large !!\n%-9s : Default output file name choosed !!\n","[FIX]");
        skip :
        strcpy(decInfo->output_fname,"output");
        skip2 :
        if(strchr(extn,'.')==NULL)
            strcat(decInfo->output_fname,".");
        strcat(decInfo->output_fname,extn);
        
        if(strncmp(input, "./", 2) == 0)
            input += 2;
        if(strncmp(output, "./", 2) == 0)
            output += 2;
        if((strcmp(input,output)==0))
        {
            display_error("One of your file name in arguments same as default output file name, So give a output file name");
            exit(e_failure);
        }
        return e_success;
    }
    if(strchr(extn,'.')==NULL)
        strcat(decInfo->output_fname,".");
    strcat(decInfo->output_fname,extn);

    if(strncmp(input, "./", 2) == 0)
            input += 2;
    if(strncmp(output, "./", 2) == 0)
        output += 2;
    if((strcmp(input,output)==0))
    {
        printf("[INVALID] : Two file name can't be same !!\n%-9s : Default output file name choosed !!\n","[FIX]");
        goto skip;
    }
    return e_success;
}

Status create_secret_file(DecodeInfo *decInfo)
{
    /*
    ============================================================================
    Function    : create_secret_file
    Description : Creates the output file in which the extracted secret
                file data will be stored.

    Logic       :
                1. Opens the output filename using fopen() in write mode.
                2. Stores the returned file pointer in fptr_output.
                3. Returns e_success.

    Benefit     : Creates the destination file required to store the
                decoded secret file contents.

    Parameters  :
                decInfo - Structure containing the output filename
                            and output file pointer.

    Return      : Status
                e_success - Returns success after the fopen() call.
                Note: The function does not check whether fopen()
                        returned NULL.
    ============================================================================
    */
    decInfo->fptr_output=fopen(decInfo->output_fname,"wb");
    return e_success;
}

Status decode_secret_data_size(DecodeInfo *decInfo)
{
    /*
    ============================================================================
    Function    : decode_secret_data_size
    Description : Extracts the original secret file size stored inside
                the stego image.

    Logic       :
                1. Calls fsize_check() to verify that sufficient image
                    data is available for the size variable.
                2. Calls decode_data_from_image() to extract the
                    complete size_secret_file variable.
                3. Stores the extracted size in decInfo->size_secret_file.
                4. Returns e_success.

    Benefit     : Determines the exact number of secret data bytes
                that must be extracted from the stego image.

    Parameters  :
                decInfo - Structure containing the input image pointer
                            and secret file size variable.

    Return      : Status
                e_success - Secret file size extracted successfully.
    ============================================================================
    */
    fsize_check(sizeof(decInfo->size_secret_file),decInfo);
    decode_data_from_image(sizeof(decInfo->size_secret_file),&(decInfo->size_secret_file),decInfo);
    fsize_check(decInfo->size_secret_file,decInfo);
    return e_success;
}

Status decode_secret_data(DecodeInfo *decInfo)
{
    /*
    ============================================================================
    Function    : decode_secret_data
    Description : Extracts the actual secret file contents from the
                stego image and writes them into the output file.

    Logic       :
                1. Calls fsize_check() to verify that sufficient image
                    data is available for the secret file.
                2. Declares a temporary buffer of 8 bytes.
                3. Calculates the number of complete 8-byte data blocks
                    using size_secret_file / sizeof(buffer).
                4. Iterates through each complete block.
                5. Calls decode_data_from_image() to extract 8 bytes
                    of secret data.
                6. Writes the extracted 8 bytes into the output file
                    using fwrite().
                7. Checks whether an output file writing error occurred.
                8. Calculates the remaining bytes using the modulus
                    operator.
                9. Extracts the remaining secret data bytes.
                10. Writes the remaining bytes into the output file.
                11. Checks for output file writing errors.
                12. Returns e_success.

    Benefit     : Reconstructs the original secret file by extracting
                all hidden data and storing it in the output file.

    Parameters  :
                decInfo - Structure containing the input stego image,
                            output file pointer and original secret file size.

    Return      : Status
                e_success - Secret file data extracted successfully.
    ============================================================================
    */
    char buffer[8];
    unsigned long count=decInfo->size_secret_file/sizeof(buffer);
    for(unsigned long i=0;i<count;i++)
    {
        decode_data_from_image(sizeof(buffer),buffer,decInfo);
        
        if(fwrite(buffer,1,sizeof(buffer),decInfo->fptr_output)!=sizeof(buffer))
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
   /*
    ============================================================================
    Function    : fsize_check

    Description : Checks whether sufficient bytes remain in the input stego
                image to decode the requested amount of secret data.

    Logic       :
                1. Obtains the current position of the input stego image
                    using ftell().
                2. Checks whether ftell() successfully returned a valid
                    file position.
                3. Calculates the number of bytes remaining in the stego
                    image from the current file position.
                4. Divides the remaining image bytes by 8 because one
                    secret data byte requires 8 image bytes for decoding.
                5. Compares the requested data size with the maximum
                    number of bytes that can be decoded from the remaining
                    image data.
                6. If sufficient image data is not available:
                    a. Displays a corrupted file error message.
                    b. Displays the decoding failure message.
                    c. Terminates the program.
                7. Returns e_success if sufficient data is available.

    Benefit     : Prevents the decoder from reading beyond the available
                stego image data and avoids integer overflow that could
                occur when multiplying a large data size by 8.

    Parameters  :
                cur_data_size - Number of secret data bytes requested
                                for decoding.
                decInfo       - Structure containing the input stego image
                                file pointer, file size and filename.

    Return      : Status
                e_success - Sufficient image data is available for decoding.
    ============================================================================
    */
   long pos = ftell(decInfo->fptr_input_stego);

    if(pos < 0)
    {
        display_decode_error("Unable to determine file position");
        exit(e_failure);
    }

    unsigned long remaining = decInfo->size_stego_file - (unsigned long)pos;

    if(cur_data_size > remaining / 8)
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
    /*
    ============================================================================
    Function    : decode_data_from_image
    Description : Extracts the specified number of secret data bytes
                from the least significant bits of the stego image.

    Logic       :
                1. Declares an 8-byte image buffer.
                2. Iterates through the requested number of data bytes.
                3. Reads 8 bytes from the input stego image for each
                    secret data byte.
                4. Checks whether exactly 8 image bytes were read.
                5. Converts the 8 image bytes into one secret data byte
                    by calling decode_lsb_to_byte().
                6. Stores the extracted byte at the corresponding
                    location in the destination data buffer.
                7. Repeats until all requested bytes are decoded.
                8. Returns e_success.

    Benefit     : Implements the main data extraction mechanism used
                to recover hidden information from the stego image.

    Parameters  :
                size    - Number of secret data bytes to decode.
                data    - Destination memory address where extracted
                            secret data is stored.
                decInfo - Structure containing the input stego image
                            file pointer.

    Return      : Status
                e_success - Requested data extracted successfully.
    ============================================================================
    */
    char image_buffer[8];
    for(int i=0;i<size;i++)
    {
        if(fread(image_buffer,1,8,decInfo->fptr_input_stego)!=8)
        {
            display_decode_error("Reading from source file failed");
            exit(e_failure);
        }
        decode_lsb_to_byte(((char *)data+i),image_buffer);
    }
    return e_success;
}

Status decode_lsb_to_byte(char *data,char *image_buffer)
{
    /*
    ============================================================================
    Function    : decode_lsb_to_byte
    Description : Reconstructs one secret data byte using the least
                significant bits of eight image bytes.

    Logic       :
                1. Initializes the destination data byte to zero.
                2. Iterates through the 8 bytes of the image buffer.
                3. Extracts the LSB of each image byte using bitwise AND
                    with 1.
                4. Shifts the extracted bit to its corresponding bit
                    position using the left shift operator.
                5. Combines the extracted bit with the destination
                    byte using bitwise OR.
                6. Repeats until all 8 bits are reconstructed.
                7. Returns e_success.

    Benefit     : Reconstructs the original secret data byte from the
                LSB values embedded in the image bytes.

    Parameters  :
                data         - Pointer to the destination byte where
                                the reconstructed data is stored.
                image_buffer - Pointer to 8 image bytes containing
                                the embedded LSB values.

    Return      : Status
                e_success - One data byte reconstructed successfully.
    ============================================================================
    */
    *data=0;
    for(int i=0;i<8;i++)
    {
        *data=(*data | ((image_buffer[i] & 1)<<i));
    }
    return e_success;
}

void close_decfile(DecodeInfo *decInfo)
{
    /*
    ============================================================================
    Function    : close_decfile
    Description : Closes the input stego image and output secret file
                after the decoding operation.

    Benefit     : Releases the file resources used during the decoding
                operation and completes normal file handling.

    Parameters  :
                decInfo - Structure containing input and output
                            file pointers.

    Return      : void
                Does not return any value.
    ============================================================================
    */
    fclose(decInfo->fptr_input_stego);
    fclose(decInfo->fptr_output);
}

void display_decode_error(char *error)
{
    /*
    ============================================================================
    Function    : display_decode_error
    Description : Displays a decoding-related error message along with
                the decoding failure and program termination messages.

    Logic       :
                1. Receives an error description through the argument.
                2. Prints the supplied error message.
                3. Displays the decoding failure message.
                4. Displays the program termination message.

    Benefit     : Provides a common error display mechanism for
                different decoding functions.

    Parameters  :
                error - String containing the error description.

    Return      : void
                Does not return any value.
    ============================================================================
    */
    printf("\n%-9s : %s !!\n","[ERROR]",error);
    printf("\n**************** Decoding FAILED ****************\n");
    printf("************** Programm Terminated **************\n\n");
}