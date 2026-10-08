#include <stdio.h>      // FILE, fopen, fread, fwrite, fseek
#include <string.h>     // strlen, strcmp
#include <stdint.h>     // uint32_t
#include "decode.h"     // DecodeInfo struct + function prototypes
#include "types.h"      // Status, uint
#include "common.h"     // MAGIC_STRING



Status decode_lsb_to_byte(char *image_buffer, char *data)
{
    *data = 0;

    for (int i = 0; i < 8; i++)
    {
        *data = (*data << 1) | (image_buffer[i] & 1);
    }
    return e_success;
}
Status decode_magic_string(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char decoded_char;
    char magic_buf[10];
    int i;

    for (i = 0; i < strlen(MAGIC_STRING); i++)
    {
        fread(image_buffer, 1, 8, decInfo->fptr_stego_image);
        decode_lsb_to_byte(image_buffer, &decoded_char);
        magic_buf[i] = decoded_char;
    }
    magic_buf[i] = '\0';

    if (strcmp(magic_buf, MAGIC_STRING) != 0)
    {
        return e_failure;
    }

    return e_success;
}


Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char image_buffer[32];
    uint32_t size = 0;

    fread(image_buffer, 1, 32, decInfo->fptr_stego_image);

    for (int i = 0; i < 32; i++)
    {
        size = (size << 1) | (image_buffer[i] & 1);
    }

    decInfo->secret_file_size = size;
    return e_success;
}
Status open_files_dec(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        return e_failure;
    }

    decInfo->fptr_secret = fopen(decInfo->secret_fname, "wb");
    if (decInfo->fptr_secret == NULL)
    {
        perror("fopen");
        return e_failure;
    }

    return e_success;
}
    

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char data;

    for (uint i = 0; i < decInfo->secret_file_size; i++)
    {
        fread(image_buffer, 1, 8, decInfo->fptr_stego_image);
        decode_lsb_to_byte(image_buffer, &data);
        fwrite(&data, 1, 1, decInfo->fptr_secret);
    }

    return e_success;
}
Status do_decoding(DecodeInfo *decInfo)
{
    if (open_files_dec(decInfo) == e_failure)
    {
        return e_failure;
    }

    
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    // Decode and validate magic string
    if (decode_magic_string(decInfo) == e_failure)
    {
        printf("ERROR: Magic string mismatch\n");
        return e_failure;
    }

    decode_secret_file_size(decInfo);
    decode_secret_file_data(decInfo);

    return e_success;
}
