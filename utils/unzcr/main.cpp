#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("Usage:\nunzcr infile.zcr outfile.tga outfile.point");
        return 0;
    }
    FILE *f=fopen(argv[1],"rb");

    fseek(f,0,SEEK_END);
    int magic=0;
    fseek(f,0,SEEK_SET);
    fread(&magic,4,1,f);
    if (magic  == 0x3152435A)
        {
        uint16_t x,y;
        fread(&x,2,1,f);
        fread(&y,2,1,f);

        uint16_t wi;
        fread(&wi,2,1,f);

        uint16_t hi;
        fread(&hi,2,1,f);

        void * packed=malloc(wi*hi*2);

        fread(packed,wi*hi*2,1,f);
        fclose(f);

        FILE *ff=fopen(argv[2],"wb");

        int tmp=0x020000;
        fwrite(&tmp,3,1,ff);

        tmp=0;
        fwrite(&tmp,4,1,ff);
        fwrite(&tmp,1,1,ff);
        fwrite(&tmp,4,1,ff);
        fwrite(&wi,2,1,ff);
        fwrite(&hi,2,1,ff);
        tmp=16;
        fwrite(&tmp,1,1,ff);
        tmp=0x20;
        fwrite(&tmp,1,1,ff);

        fwrite(packed,wi*hi*2,1,ff);

        fclose(ff);

        ff=fopen(argv[3],"wb");
        fwrite(&x,2,1,ff);
        fwrite(&y,2,1,ff);
        fclose(ff);

        free(packed);
        }

    return 0;
}
