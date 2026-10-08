#include <stdio.h>
#include <stdint.h> 
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"



/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint32_t width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(uint32_t), 1, fptr_image);
    

    // Read the height (an int)
    fread(&height, sizeof(uint32_t), 1, fptr_image);
    

    fseek(fptr_image, 0, SEEK_SET); 

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

uint get_file_size(FILE *fptr)
{
    uint size;

    // Move file pointer to end
    fseek(fptr, 0, SEEK_END);

    // Get position = size
    size = ftell(fptr);

    // Reset file pointer to beginning
    fseek(fptr, 0, SEEK_SET);

    return size;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        // Clear LSB of image byte
        image_buffer[i] &= 0xFE;

        // Set LSB with MSB of data
        image_buffer[i] |= (data >> (7 - i)) & 1;
    }
    return e_success;
}

Status encode_size_to_lsb(uint32_t size, char *image_buffer)
{
    for (int i = 0; i < 32; i++)
    {
        // Clear LSB
        image_buffer[i] &= 0xFE;

        // Take bit from MSB to LSB
        image_buffer[i] |= (size >> (31 - i)) & 1;
    }
    return e_success;
}
Status encode_secret_file_size(EncodeInfo *encInfo)
{
    char image_buffer[32];

    // Read 32 bytes from source image
    fread(image_buffer, 1, 32, encInfo->fptr_src_image);

    // Encode secret file size
    encode_size_to_lsb(encInfo->secret_file_size, image_buffer);

    // Write to stego image
    fwrite(image_buffer, 1, 32, encInfo->fptr_stego_image);

    return e_success;
}


Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char image_buffer[8];

    for (int i = 0; magic_string[i] != '\0'; i++)
    {
        // Read 8 bytes from source image
        fread(image_buffer, 1, 8, encInfo->fptr_src_image);

        // Encode 1 character
        encode_byte_to_lsb(magic_string[i], image_buffer);

        // Write back to stego image
        fwrite(image_buffer, 1, 8, encInfo->fptr_stego_image);
    }
    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char image_buffer[8];
    char secret_char;

    // Read secret file byte-by-byte
    for (uint i = 0; i < encInfo->secret_file_size; i++)
    {
        // Read one byte from secret file
        fread(&secret_char, 1, 1, encInfo->fptr_secret);

        // Read 8 bytes from source image
        fread(image_buffer, 1, 8, encInfo->fptr_src_image);

        // Encode secret byte into image LSBs
        encode_byte_to_lsb(secret_char, image_buffer);

        // Write encoded bytes to stego image
        fwrite(image_buffer, 1, 8, encInfo->fptr_stego_image);
    }

    return e_success;
}
Status copy_remaining_image_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer;
    
    // Copy till end of file
    while (fread(&buffer, 1, 1, fptr_src_image) > 0)
    {
        fwrite(&buffer, 1, 1, fptr_stego_image);
    }

    return e_success;
}



Status do_encoding(EncodeInfo *encInfo)
{
    // Step 1: Open all required files
    if (open_files(encInfo) == e_failure)
    {
        printf("ERROR: Failed to open files\n");
        return e_failure;
    }

    // Step 2: Check capacity
    if (check_capacity(encInfo) == e_failure)
    {
        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);
        return e_failure;
    }

    // Step 3: Copy BMP header (54 bytes)
    if (copy_bmp_header(encInfo->fptr_src_image,
                        encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
   {
      printf("ERROR: Magic string encoding failed\n");
      return e_failure;
   }
   if (encode_secret_file_size(encInfo) == e_failure)
   {
      printf("ERROR: Encoding secret file size failed\n");
      return e_failure;
   }
   if (encode_secret_file_data(encInfo) == e_failure)
   {
      printf("ERROR: Encoding secret file data failed\n");
      return e_failure;
   }
   if (copy_remaining_image_data(encInfo->fptr_src_image,
                              encInfo->fptr_stego_image) == e_failure)
   {
       printf("ERROR: Copying remaining image data failed\n");
       return e_failure;
   }





    // Remaining steps will be added later
    // encode magic string
    // encode secret extension
    // encode secret file size
    // encode secret data
    // copy remaining image bytes

    return e_success;
}
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity =
        get_image_size_for_bmp(encInfo->fptr_src_image);

    encInfo->secret_file_size =
        get_file_size(encInfo->fptr_secret);

    uint required_capacity =
    (strlen(MAGIC_STRING) * 8) +
    (sizeof(uint32_t) * 8) +          // secret size
    (encInfo->secret_file_size * 8);


    if (encInfo->image_capacity < required_capacity)
    {
        return e_failure;
    }

    return e_success;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char header[54];

    // Move source file pointer to beginning
    fseek(fptr_src_image, 0, SEEK_SET);

    // Read 54 bytes (BMP header)
    if (fread(header, sizeof(char), 54, fptr_src_image) != 54)
    {
        return e_failure;
    }

    // Write 54 bytes to stego image
    if (fwrite(header, sizeof(char), 54, fptr_dest_image) != 54)
    {
        return e_failure;
    }

    return e_success;
}



