#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "encode.h"
//#include "types.h"
#include "common.h"

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
unsigned int get_image_size_for_bmp(FILE *fptr_image)
{
    unsigned int width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);
    rewind(fptr_image);

    // Return image capacity
    return width * height * 3; // due to rgb (3) bytes,
}

unsigned int get_file_size(FILE *fptr)
{
    unsigned int count = 0;
    rewind(fptr);
    while (fgetc(fptr) != EOF)
    {
        count++;
    }
    rewind(fptr);
    return count;
}


/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: success or failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    printf("The input .bmp file is opened\n");
    encInfo->src_image_fptr = fopen(encInfo->src_image_fname, "rb");

    if (encInfo->src_image_fptr == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return failure;
    }
    printf("The secret file is opened\n");

    encInfo->secret_fptr = fopen(encInfo->secret_fname, "rb");

    if (encInfo->secret_fptr == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return failure;
    }
    printf("The output .bmp file is opened\n");

    encInfo->output_image_fptr = fopen(encInfo->output_image_fname, "wb");

    if (encInfo->output_image_fptr == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->output_image_fname);

    	return failure;
    }
    printf("The files are successfully opened\n");

    return success;
}

Opr_type check_operation(char *option)
{
    if(strcmp(option,"-e")==0)
    {
        return encode;
    }
    else if(strcmp(option,"-d")==0)
    {
        return decode;
    }
    else
    {
        return unsupported;
    }
}

Status validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // Check source image exten 
    printf("Validation for ecoding arguments started...\n");
    char *extn = strrchr(argv[2], '.');
    if (extn == NULL || strcmp(extn, ".bmp") != 0)
    {
        printf("Error -> Source image must be a .bmp file\n");
        return failure;
    }

    encInfo->src_image_fname = argv[2];

    // Check secret file exten
    extn = strrchr(argv[3], '.');

    if (extn == NULL)
    {
        printf("Error -> Secret file has no extension\n");
        return failure;
    }

    if (strcmp(extn, ".txt") != 0 &&strcmp(extn, ".c") != 0 && strcmp(extn, ".sh")!= 0 &&
        strcmp(extn, ".py") != 0 && strcmp(extn, ".c++")!= 0 &&strcmp(extn, ".cpp")!= 0)
    {
        printf("Error -> Invalid secret file extension\n");
        return failure;
    }

    // Store secret filename and exten
    encInfo->secret_fname = argv[3];

    strcpy(encInfo->secret_extn, extn);
    encInfo->secret_extn_size = strlen(extn);

    // Store output image filename 

    // Store output image filename
if (argv[4] != NULL)
{
    extn = strrchr(argv[4], '.');

    if (extn != NULL && strcmp(extn, ".bmp") == 0)
    {
        encInfo->output_image_fname = argv[4];
    }
    else
    {
        printf("Error -> Invalid output image extension\n");
        return failure;
    }
}
else
{
    printf("The output file is empty\n..so the program took 'steg.bmp' as a output filename\n");
    encInfo->output_image_fname = "steg.bmp";
}

    return success;
}

Status do_encoding(char *argv[], EncodeInfo *encInfo)
{
    printf("-------------The Encoding process started---------------\n");
    if(validate_encode_args(argv, encInfo) == failure)
        return failure;

    if(open_files(encInfo) == failure)
        return failure;

    if(check_capacity(encInfo) == failure)
        return failure;

    if(copy_bmp_header(encInfo) == failure)
        return failure;

    if(encode_magic_string(MAGIC_STRING, encInfo) == failure)
        return failure;

    if(encode_secret_file_extn(encInfo) == failure)
        return failure;

    if(encode_secret_file_data(encInfo) == failure)
        return failure;

    if(copy_remaining_img_data(encInfo) == failure)
        return failure;

    fclose(encInfo->src_image_fptr);
    fclose(encInfo->secret_fptr);
    fclose(encInfo->output_image_fptr);

    return success;
}


Status check_capacity(EncodeInfo *encInfo)
{
    printf("Checking cpacity to store the secret file in .bmp file......\n");
    unsigned int image_size;
    unsigned int encoding_size;
    encInfo->secret_file_size = get_file_size(encInfo->secret_fptr);
    image_size = get_image_size_for_bmp(encInfo->src_image_fptr);
    encoding_size = (strlen(MAGIC_STRING) + 4 + encInfo->secret_extn_size + 4 +
                 encInfo->secret_file_size) * 8;

    if (image_size >= encoding_size)
    {
        printf("Capacity is available to store the secret image\n");
        return success;
    }
    else
    {
        printf("Error--> Image does not have enough capacity\n");
        return failure;
    }
}

Status encode_1byte_to_lsb(char data, char *buffer_8)
{
        for(int i=7;i>=0;i--)  // # passward will read from last in system so i--  //after 54th bit the changes happens that we are getting from this logic 
        {
                int bit = data & (1 << i);  //getting bit   data is # value
                bit = bit >> i;  //moving bit to lsb
                int mask = (1<<0);// mask by 1
                buffer_8[7-i] = buffer_8[7-i] & ~mask; //clearing  [lsb] bit
                buffer_8[7-i] = buffer_8[7-i] | bit;  //replacing [lsb] bit
        }

        return success;
    }
    Status encode_4byte_to_lsb(int data, char *buffer_32)    // same like # this is for *
{
    for(int i=31;i>=0;i--)
        {
                int bit = data & (1 << i);
                bit = bit >> i;
                int mask = (1<<0);
                buffer_32[31-i] = buffer_32[31-i] & ~mask;
                buffer_32[31-i] = buffer_32[31-i] | bit;
        }
        return success;

}

Status copy_bmp_header(EncodeInfo *encInfo)
{
    printf("Copying .bmp header......\n");  // taking the 54 bytes and copying to output file
    char buffer[54];             
    fread(buffer, 54 , 1, encInfo->src_image_fptr);
    fwrite(buffer, 54 , 1 , encInfo->output_image_fptr);
    printf("copying sucess...\n");
    return success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    printf("The encoding started for magic string '#*'......\n"); 
    char img_byte[8];   
    for(int i=0;i<(strlen(magic_string));i++)
    {
        fread(img_byte, 8, 1, encInfo->src_image_fptr);
        encode_1byte_to_lsb(magic_string[i], img_byte);
        fwrite(img_byte, 8 , 1 , encInfo->output_image_fptr);

    }
    printf("Encoding magic string success\n");
    return success;
}

Status encode_secret_file_extn(EncodeInfo *encInfo)
{
    // size of extn is 4 bytes .txt so 4*8  1 byte of data 8bytes *size ofinteger 4
    printf("The encoding started for secret file extension size......\n");
    char img_byte[32];
    fread(img_byte, 32, 1, encInfo->src_image_fptr);
    encode_4byte_to_lsb(encInfo->secret_extn_size, img_byte);
    fwrite(img_byte, 32, 1, encInfo->output_image_fptr);
    printf("Encoding secret file extension size success\n");
    // file
    char img_byte8[8];
    printf("The encoding started for secret file extension......\n");
    for(int i=0;i<encInfo->secret_extn_size;i++)
    {
        fread(img_byte8, 8, 1, encInfo->src_image_fptr);
        encode_1byte_to_lsb(encInfo->secret_extn[i], img_byte8);
        fwrite(img_byte8, 8 , 1 , encInfo->output_image_fptr);
    }
     printf("Encoding secret file extension  success\n");
    return success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)  
{
    printf("The encoding started for secret file data size......\n");
    char img_byte[32],ch;
    fread(img_byte, 32, 1, encInfo->src_image_fptr);
    encode_4byte_to_lsb(encInfo->secret_file_size, img_byte);
    fwrite(img_byte, 32, 1, encInfo->output_image_fptr);
    printf("Encoding secret file extension data size success\n");
    // file
    char img_byte8[8];  //this .txt . will be having ascii value it will be placed in image file after 54bytes
    printf("The encoding started for file data ......\n");
    for(int i=0;i<encInfo->secret_file_size;i++)
    {
        ch = fgetc(encInfo->secret_fptr);
        fread(img_byte8, 8, 1, encInfo->src_image_fptr);
        encode_1byte_to_lsb(ch, img_byte8);
        fwrite(img_byte8, 8 , 1 , encInfo->output_image_fptr);
    }
    printf("Encoding  file data successfully completed\n");
    return success;

}

Status copy_remaining_img_data(EncodeInfo *encInfo)
{
    
    printf("Copying the remaining image data...\n");
    printf(" %ld\n", ftell(encInfo->src_image_fptr));
    printf(" %ld\n", ftell(encInfo->output_image_fptr));
    int ch;
    while((ch = fgetc(encInfo->src_image_fptr)) != EOF)
    {
        fputc(ch, encInfo->output_image_fptr);
    }
    printf("%ld\n", ftell(encInfo->src_image_fptr));
printf("%ld\n", ftell(encInfo->output_image_fptr));
    printf("Successfully copied the remaining data\n");
    return success;
}