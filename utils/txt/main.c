#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>

int main(int argc, char *argv[])
{
    if (argc<3)
    {
        printf("Usage:\ntxt infile.txt outfile.txt\n");
        return 0;
    }

    FILE *f=fopen(argv[1],"rb");
    FILE *f2=fopen(argv[2],"wb");


    fseek(f,0,SEEK_END);
    int sz=ftell(f);
    fseek(f,0,SEEK_SET);

    char *chr=malloc(sz);
    fread(chr,1,sz,f);

    fclose(f);

    char *chr2=malloc(sz+1);

    memset(chr2,0,sz+1);

    int j=0;
    int i;
    for (i=0; i<sz;i++)
    {
        if (chr[i]!= 0x0)
            {
                chr2[j] = chr[i];
                j++;
            }
    }
    int sz2=j;

    size_t buf1,buf2;


    buf1 = sz2+1;
    buf2 = sz2*2;

    char *chr3 = malloc(sz2*2+1);
    memset(chr3,0,sz2*2+1);
    iconv_t *icnv;


    char *in= chr2;
    char *out=chr3;

    icnv=iconv_open("UTF-8","cp1251");
    iconv(icnv,&in,&buf1,&out,&buf2);
    fwrite(chr3,1,strlen(chr3),f2);
    fclose(f2);

    return 0;
}
