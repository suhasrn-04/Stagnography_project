#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "common.h"

typedef struct DecodeInfo
{
    /* Stego Image Info */
    char steg_name[30];       // Store the steg_image filename
    FILE *steg_fptr;          // File pointer for steg_image

    /* Secret message file */
    char output_image_fname[30];  // Store the output_file filename
    FILE *output_image_fptr;      // File pointer for output_file

} DecodeInfo;

/* Read and validate Decode args from argv */
Status validate_Decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(char *argv[], DecodeInfo *decInfo);

/* Get file pointers for output files */
Status open_file(DecodeInfo *decInfo);

/* Copy bmp image header */

/* Store Magic String */
Status Decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Decode secret file extension */
Status Decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file data */
Status Decode_secret_file_data(DecodeInfo *decInfo);

/* Decode 1 byte into LSB of image data array */
char Decode_1byte_to_lsb(char *buffer_8);

/* Decode 4 byte (int) into LSB of image data array */
int Decode_4byte_to_lsb(char *buffer_32);

#endif