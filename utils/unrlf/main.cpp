#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>

using namespace std;


struct Header
{
    uint32_t magic; //FELR 0x524C4546
    uint32_t size;// from begin
    uint32_t unk1;
    uint32_t unk2;
    uint32_t frames; //number of frames
};

struct Cinf
{
    uint32_t magic; //FNIC
    uint32_t size;
    uint32_t unk1;
    uint32_t unk2;
    uint32_t unk3;
    char VRLE[0x18];
    uint32_t LRVD;
    uint32_t unk4;
    char HRLE[0x18];
    uint32_t ELHD;
    uint32_t unk5;
    char HKEY[0x18];
    uint32_t ELRH;
};

struct MinF
{
    uint32_t magic; //FNIM
    uint32_t size;
    uint32_t OEDV; //OEDV
    uint32_t unk1;
    uint32_t unk2;
    uint32_t unk3;
    uint32_t width;
    uint32_t height;
};

struct Time
{
    uint32_t magic; //EMIT
    uint32_t size;
    uint32_t unk1;
    uint32_t unk2;
};

struct Frame
{
    uint32_t magic; //MARF
    uint32_t size;
    uint32_t unk1;
    uint32_t unk2;
    uint32_t TYPE; // ELRH or  ELHD
    uint32_t offset; //from begin of frame to data
    uint32_t unk3;
};




void DHLE(int8_t *dst, int8_t *src,int32_t size)
{
    int8_t tmp;
    int32_t off1, off2,off3;
    int16_t tmp2;
    off1=0;
    off2=0;
    off3=0;

    while (off1 <= size)
    {
        tmp = src[off1];
        off1++;

        if (tmp < 0)
        {
            off3-=tmp;
            if (tmp < 0)
            {
                tmp=abs(tmp);
                while (tmp!=0)
                {
                    tmp2=*(int16_t *)(src + off1);
                    off2+=2;
                    *(int16_t *)&dst[off2-2] = tmp2;
                    off1+=2;
                    tmp--;
                }
            }
        }
        else
        {
            off2+=tmp*2 + 2;
        }


    }
}

void HRLE(int8_t *dst, int8_t *src,int32_t size)
{
    int8_t tmp;
    int32_t off1, off2,off3;
    int16_t tmp2;
    int32_t tmp3,tmp4;
    off1=0;
    off2=0;
    off3=0;

    while (off1 <= size)
    {
        tmp = src[off1];
        off1++;
        tmp3 = tmp;

        if (tmp < 0)
        {
            off3-=tmp;
            if (tmp < 0)
            {
                tmp=abs(tmp);
                while (1)
                {
                    tmp2=*(int16_t *)(src + off1);
                    off2+=2;
                    *(int16_t *)&dst[off2-2] = tmp2;
                    off1+=2;
                    tmp--;
                    if (tmp==0)
                        break;
                }
            }
        }
        else
        {
            tmp3 += 2;
            off1 += 2;
            tmp2 = *(int16_t *)&src[off1 - 2];
            if (tmp>-2)
            {
                tmp4=tmp+2;
                while (tmp4>0)
                {
                    *(int16_t *)&dst[off2] = tmp2;
                    off2+=2;
                    tmp4--;
                }
            }
        }


    }
}


void Transpose(uint16_t *src, uint16_t *dst, int width, int height) //width and hei of original
{
    for (int j=0; j<height; j++)
        for (int i=0; i<width; i++)
        {
            dst[height*i+j] = src[width*j+i];
        }
}


int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage:\n\nunrlf rotate infile.rlf outfile.tga outfile.anm\n\nrotate\n1 - rotate image\n0 - don't rotate\n");
        return 0;
    }

    FILE *f=fopen(argv[2],"rb");
    Header hd;
    fread(&hd,1,sizeof(Header),f);
    Cinf cin;
    fread(&cin,1,sizeof(Cinf),f);
    MinF mn;
    fread(&mn,1,sizeof(MinF),f);
    Time tm;
    fread(&tm,1,sizeof(Time),f);
    FILE *ff=fopen(argv[3],"wb");

    int tmp=0x020000;
    fwrite(&tmp,3,1,ff);

    tmp=0;
    fwrite(&tmp,4,1,ff);
    fwrite(&tmp,1,1,ff);
    fwrite(&tmp,4,1,ff);

    if (argv[1][0]=='1')
    {
        tmp = mn.height;
        fwrite(&tmp,2,1,ff);
        tmp = mn.width * hd.frames;
        fwrite(&tmp,2,1,ff);
    }
    else
    {
        tmp = mn.width;
        fwrite(&tmp,2,1,ff);
        tmp = mn.height * hd.frames;
        fwrite(&tmp,2,1,ff);
    }


    tmp=16;
    fwrite(&tmp,1,1,ff);
    tmp=0x20; //For nemesis 0x20!
    fwrite(&tmp,1,1,ff);


    Frame frm;
    void *buf2=malloc(mn.height*mn.width*4);
    memset(buf2,0,mn.height*mn.width*2);
    for (uint16_t i=0; i<hd.frames ; i++)
    {
        fread(&frm,1,sizeof(Frame),f);

        void *buf=malloc(frm.size-frm.offset);
        fread(buf,1,frm.size-frm.offset,f);


        //void *buf2=malloc(mn.height*mn.height*4);
        //memset(buf2,0,mn.height*mn.height*2);

        if (frm.TYPE == 0x44484C45)
            DHLE((int8_t *)buf2,(int8_t *)buf,frm.size-frm.offset);
        else
            HRLE((int8_t *)buf2,(int8_t *)buf,frm.size-frm.offset);


        if (argv[1][0]=='1')
        {
            void *buf3=malloc(mn.height*mn.width*2);
            Transpose((uint16_t *)buf2,(uint16_t *)buf3,mn.width,mn.height);
            fwrite(buf3,1,mn.height*mn.width*2,ff);
            free(buf3);
        }
        else
            fwrite(buf2,1,mn.height*mn.width*2,ff);


        free(buf);
        //free(buf2);

    }

    fclose(ff);

    fclose(f);

    ff=fopen(argv[4],"wb");
    if (argv[1][0]=='1')
    {
        fwrite(&mn.height,1,4,ff);
        fwrite(&mn.width,1,4,ff);
    }
    else
    {
        fwrite(&mn.width,1,4,ff);
        fwrite(&mn.height,1,4,ff);
    }

    fwrite(&tm.unk2,1,4,ff);
    fwrite(&hd.frames,1,4,ff);

    fclose(ff);


    return 0;
}
