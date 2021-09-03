#include <stdio.h>
#include "Evan_avatar.c"

int main()
{
    int i;
    printf("static const struct {\n");
    printf(" unsigned int width;\n");
    printf(" unsigned int height;\n");
    printf(" unsigned char pixel_data[64*74*2];\n");
    printf(" } evan_image = {\n");
    printf("  64, 74,\n");
    printf("{\n");

    for(i=0;i<64*74;i++) {
        printf("0x%02X, 0x%02X, ", gimp_image.pixel_data[2*i+1], gimp_image.pixel_data[2*i]);
        if( (i+1)%16 == 0) {
            printf("\n");
        }
    }
    printf("\n}};");


    return 0;
}
