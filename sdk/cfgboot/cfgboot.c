#include <stdint.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char **argv)
{
    if(argc != 3)
    {
        printf("Usage: cfgboot [image file] [args]\n");
        return 1;
    }
    FILE *img = fopen(argv[1], "rb+");
    if(!img)
    {
        fprintf(stderr, "Error: can't open file.\n");
        return 1;
    }
    char block[512];
    fread(block, 1, 512, img);
    if(strlen(argv[2]) >= 299)
    {
        fprintf(stderr, "Error: argument size overflow.\n");
        fclose(img);
        return 1;
    }
    strlcpy(&block[100], argv[2], 300);
    fseek(img, 0, SEEK_SET);
    fwrite(block, 1, 512, img);
    fclose(img);
    return 0;
}