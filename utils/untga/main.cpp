#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>


void unlzw(uint8_t *dst,uint8_t *src,uint32_t size)
{
    uint8_t     lz[0x1000];
    uint32_t    lz_pos=0x0fee;
    uint32_t    cur=0, d_cur=0, otsk;
    uint8_t     bl,mk,i,j,lw,hi,loops;

    memset(&lz[0],0,0x1000);

    while(cur<size)
    {
        bl=src[cur];
        mk=1;

        for (i=0; i<=7; i++)
        {
            if ((bl & mk)==mk)
            {
                cur++;
                if (cur >= size) break;

                lz[lz_pos]=src[cur];

                dst[d_cur]=src[cur];

                d_cur++;
                lz_pos = (lz_pos+1) & 0xfff;
            }
            else
            {
                cur++;
                if (cur >= size) break;
                lw = src[cur];

                cur++;
                if (cur >= size) break;
                hi = src[cur];

                loops = (hi & 0xf)+2;

                otsk = lw | ((hi & 0xf0)<<4);

                for(j = 0; j <= loops; j++)
                {
                    lz[lz_pos]=lz[(otsk+j) & 0xfff];
                    dst[d_cur]=lz[(otsk+j) & 0xfff];
                    lz_pos=(lz_pos+1) & 0xfff;
                    d_cur++;
                }
            };

            mk=mk << 1;

        }

        cur++;
    };

}

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        printf("Usage:\n untga infile.tga outfile.tga\n");
        exit(0);
    }

    FILE *f=fopen(argv[1],"rb");

    fseek(f,0,SEEK_END);
    int fl_size=ftell(f);
    int pa_size=ftell(f)-16;
    int magic=0;
    fseek(f,0,SEEK_SET);

    fread(&magic,4,1,f);
    if (magic  == 0x005A4754)
    {

        int unp_size;

        fread(&unp_size,4,1,f);

        int wi;
        fread(&wi,4,1,f);

        int hi;
        fread(&hi,4,1,f);

        void * unp=malloc(unp_size);

        void * packed=malloc(pa_size);

        fread(packed,pa_size,1,f);
        fclose(f);

        unlzw((uint8_t *)unp, (uint8_t *)packed, pa_size);


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
        tmp=0x20;  //For nemesis 0x20!
        fwrite(&tmp,1,1,ff);

        fwrite(unp,unp_size,1,ff);

        fclose(ff);


        free(unp);
        free(packed);
    }
    else
    {
	fseek(f,0,SEEK_SET);
        uint8_t * unp=(uint8_t *)malloc(fl_size);
        fread(unp,fl_size,1,f);
        fclose(f);
        FILE *ff=fopen(argv[2],"wb");
        fwrite(unp,fl_size,1,ff);
        fclose(ff);
        free(unp);
    }

    return 0;
}
