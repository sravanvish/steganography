#include <stdio.h>
#include "encode.h"

int main()
{
    EncodeInfo encInfo;

    encInfo.src_image_fname = "beautiful.bmp";
    encInfo.secret_fname = "secret.txt";
    encInfo.stego_image_fname = "stego_img.bmp";

    if (do_encoding(&encInfo) == e_success)
    {
        printf("SUCCESS: Encoding completed\n");
    }
    else
    {
        printf("ERROR: Encoding failed\n");
    }

    return 0;
}
