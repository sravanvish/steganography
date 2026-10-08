#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

/* Decode Info structure */
typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Output Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;

    uint secret_file_size;

} DecodeInfo;

/* Function prototypes */
Status do_decoding(DecodeInfo *decInfo);
Status decode_magic_string(DecodeInfo *decInfo);
Status decode_secret_file_size(DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);
Status decode_lsb_to_byte(char *image_buffer, char *data);

#endif
