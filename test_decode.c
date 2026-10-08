#include <stdio.h>
#include "decode.h"

int main()
{
    DecodeInfo decInfo;

    decInfo.stego_image_fname = "stego_img.bmp";
    decInfo.secret_fname = "decoded_secret.txt";

    if (do_decoding(&decInfo) == e_success)
    {
        printf("SUCCESS: Decoding completed\n");
    }
    else
    {
        printf("ERROR: Decoding failed\n");
    }

    return 0;
}
