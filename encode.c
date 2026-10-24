#include <stdio.h>
#include "encode.h"
#include "types.h"

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
l
    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
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
    encInfo->src_image_fptr = fopen(encInfo->src_image_fname, "r");

    if (encInfo->src_image_fptr == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return failure;
    }

    encInfo->secret_fptr = fopen(encInfo->secret_fname, "r");

    if (encInfo->secret_fptr == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return failure;
    }

    encInfo->output_image_fptr = fopen(encInfo->output_image_fname, "w");

    if (encInfo->output_image_fptr == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->output_image_fname);

    	return failure;
    }

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
    char *extn = strrchr(argv[2], '.');
    if (extn == NULL || strcmp(extn, ".bmp") != 0)
    {
        printf("Error -> Source image must be a .bmp file\n");
        return failure;
    }

    encInfo->src_image_fname = argv[2];
    //validate_encode_args() checks and stores the filename, while open_files() actually opens the file. fopen to read

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

    if (argv[4] != NULL )
    {
        extn = strrchr(argv[4], '.');
        if(strcmp(extn,".bmp")==0)
        {
        encInfo->output_image_fname = argv[4];
        }
        else
        {
            printf("Error -> Invalid extension\n");
            return failure;
        }
    }
    else
    {
        encInfo->output_image_fname = "stego.bmp";
    }

    return success;
}
Status do_encoding(char *argv[], EncodeInfo *encInfo)
{
    if (validate_encode_args(argv, encInfo) == failure)
        return failure;

    if (open_files(encInfo) == failure)
        return failure;

    if (check_capacity(encInfo) == failure)
        return failure;

    return success;
}


Status check_capacity(EncodeInfo *encInfo)
{
    unsigned int image_size;
    unsigned int encoding_size;
    encInfo->secret_file_size = get_file_size(encInfo->secret_fptr);
    image_size = get_image_size_for_bmp(encInfo->src_image_fptr);
    encoding_size =  (strlen(MAGIC_STRING) + 4 + strlen(encInfo->secret_extn) + encInfo->secret_file_size) * 8;

    if (image_size >= encoding_size)
    {
        return success;
    }
    else
    {
        printf("Error: Image does not have enough capacity\n");
        return failure;
    }
}

Status encode_1byte_to_lsb(char data, char *buffer_8)
{
        for(int i=7;i>=0;i--)
        {
                int bit = data & (1 << i);
                bit = bit >> i;
                int mask = (1<<0);
                buffer_8[7-i] = buffer_8[7-i] & ~mask;
                buffer_8[7-i] = buffer_8[7-i] | bit;
        }

        return success;
    }

