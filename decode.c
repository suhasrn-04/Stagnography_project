#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "common.h"

// Validating the command line argument
Status validate_Decode_args(char *argv[], DecodeInfo *decInfo)
{
    printf("Validation for Decoding arguments started...\n");

    char *extn = strrchr(argv[2], '.');

    if (extn == NULL || strcmp(extn, ".bmp") != 0)
    {
        printf("Error -> Source image must be a .bmp file\n");
        return failure;
    }

    strcpy(decInfo->steg_name, argv[2]);

    // Check secret file extension
    if (argv[3] != NULL)
    {
        strcpy(decInfo->output_image_fname, argv[3]);

        extn = strrchr(decInfo->output_image_fname, '.');

        // If user entered with extension remove the extension
        if (extn != NULL)
        {
            *extn = '\0';
        }
    }
    else
    {
        printf("The secret file is not assigned, so it is assigned as 'decoded_message'\n");

        strcpy(decInfo->output_image_fname, "decoded_message");
    }

    printf("Validated successfully..\n");

    return success;
}


// Open the encoded image file
Status open_file(DecodeInfo *decInfo)
{
    printf("----->Image file opened<-----\n");

    decInfo->steg_fptr = fopen(decInfo->steg_name, "rb");

    if (decInfo->steg_fptr == NULL)
    {
        printf("The file is empty or invalid file\n");
        return failure;
    }

    printf("steg file opened successfully...\n");

    return success;
}


// Decoding process functions
Status do_decoding(char *argv[], DecodeInfo *decInfo)
{
    printf("---------------------------------------------\n");
    printf("          THE DECODING PROCESS STARTED       \n");
    printf("---------------------------------------------\n");

    if (validate_Decode_args(argv, decInfo) == failure)
    {
        return failure;
    }

    if (open_file(decInfo) == failure)
    {
        return failure;
    }

    if (Decode_magic_string(MAGIC_STRING, decInfo) == failure)
    {
        return failure;
    }

    if (Decode_secret_file_extn(decInfo) == failure)
    {
        return failure;
    }

    if (Decode_secret_file_data(decInfo) == failure)
    {
        return failure;
    }

    printf("---------------------------------------------\n");
    printf("       THE DECODING PROCESS COMPLETED SUCCESSFULLY\n");
    printf("---------------------------------------------\n");

    fclose(decInfo->steg_fptr);
    fclose(decInfo->output_image_fptr);

    return success;
}


// Validate the magic string
Status Decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    // Skipping the header bytes
    int i;

    printf("Skipped 54 header bits...\n");

    fseek(decInfo->steg_fptr, 54, SEEK_SET);

    printf("Skipped successfully\n");

    printf("In process of fetching the secret password(MAGIC STRING)\n");

    char magic[strlen(magic_string) + 1],img_byte[8];
   // char img_byte[8];

    // Search for magic string
    for (i = 0; i < strlen(magic_string); i++)
    {
        fread(img_byte, 8, 1, decInfo->steg_fptr);

        magic[i] = Decode_1byte_to_lsb(img_byte);
    }

    magic[i] = '\0';

    // Decode the magic string and check with original magic string
    if (strcmp(magic_string, magic) == 0)
    {
        printf("The MAGIC STRING secret password matched...\n");

        return success;
    }
    else
    {
        printf("...PASSWORD mismatch...wrong .bmp file\n");

        return failure;
    }
}


// Decoding the secret file size and type of extension
Status Decode_secret_file_extn(DecodeInfo *decInfo)
{
    // Extract the size of extension
    printf("The Decoding process started for Secret file extension size\n");

    int i;
    char img_byte[32];

    fread(img_byte, 32, 1, decInfo->steg_fptr);

    int extn_size = Decode_4byte_to_lsb(img_byte);

    // Extract the extension
    printf("Extension decoding size process completed...\n");

    printf("The Decoding process started for Secret file extension data\n");

    char extension[extn_size + 1], img_byte8[8];

    for (i = 0; i < extn_size; i++)
    {
        fread(img_byte8, 8, 1, decInfo->steg_fptr);

        extension[i] = Decode_1byte_to_lsb(img_byte8);
    }

    extension[i] = '\0';

    printf("Extension decoding data process completed...\n");

    // Concatenate the extension with filename
    printf("Concatenation of filename with extension started...\n");

    strcat(decInfo->output_image_fname, extension);

    printf("Concatenation success\n");

    printf("Opening the secret file.......\n");

    decInfo->output_image_fptr = fopen(decInfo->output_image_fname, "wb");

    if (decInfo->output_image_fptr == NULL)
    {
        printf("Error in opening output file\n");

        return failure;
    }

    printf("Output File opened successfully...\n");

    return success;
}


// Decoding the secret file size and data
Status Decode_secret_file_data(DecodeInfo *decInfo)
{
    // Extract the size of secret message
    printf("The Decoding process started for Secret file size\n");

    int i;
    char img_byte[32];

    fread(img_byte, 32, 1, decInfo->steg_fptr);

    int secret_file_size = Decode_4byte_to_lsb(img_byte);

    // Extract the text
    printf("Secret file decoding size process completed...\n");

    printf("The Decoding process started for Secret file extension data\n");

    char data[secret_file_size], img_byte8[8];

    for (i = 0; i < secret_file_size; i++)
    {
        fread(img_byte8, 8, 1, decInfo->steg_fptr);

        data[i] = Decode_1byte_to_lsb(img_byte8);

        fwrite(&data[i], 1, 1, decInfo->output_image_fptr);
    }

    printf("The Decoding process completed for Secret file\n");

    return success;
}


// Main 1 byte decoding logic
char Decode_1byte_to_lsb(char *buffer_8)
{
    char ch = 0;

    for (int i = 7; i >= 0; i--)
    {
        buffer_8[7-i] = buffer_8[7-i] & 1;

        ch = (buffer_8[7-i] << i) | ch;
    }

    return ch;
}


// Main 4 byte decoding logic
int Decode_4byte_to_lsb(char *buffer_8)
{
    int ch = 0;

    for (int i = 31; i >= 0; i--)
    {
        buffer_8[31-i] = buffer_8[31-i] & 1;

        ch = (buffer_8[31-i] << i) | ch;
    }

    return ch;
}