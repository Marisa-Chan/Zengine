#include "System.h"

/*********************************** adpcm_Support *******************************/

static const uint8_t wavHeader[0x2C] =
{
    'R', 'I', 'F', 'F',
    0, 0, 0, 0,
    'W', 'A', 'V', 'E',
    'f', 'm', 't', ' ',
    0x10, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    'd', 'a', 't', 'a',
    0, 0, 0, 0
};
const int32_t t1[] = { -1,-1,-1,1,4,7,10,12 };
const int32_t t2[] = {0x0007,0x0008,0x0009,0x000A,0x000B,0x000C,0x000D,0x000E,
                      0x0010,0x0011,0x0013,0x0015,0x0017,0x0019,0x001C,0x001F,
                      0x0022,0x0025,0x0029,0x002D,0x0032,0x0037,0x003C,0x0042,
                      0x0049,0x0050,0x0058,0x0061,0x006B,0x0076,0x0082,0x008F,
                      0x009D,0x00AD,0x00BE,0x00D1,0x00E6,0x00FD,0x0117,0x0133,
                      0x0151,0x0173,0x0198,0x01C1,0x01EE,0x0220,0x0256,0x0292,
                      0x02D4,0x031C,0x036C,0x03C3,0x0424,0x048E,0x0502,0x0583,
                      0x0610,0x06AB,0x0756,0x0812,0x08E0,0x09C3,0x0ABD,0x0BD0,
                      0x0CFF,0x0E4C,0x0FBA,0x114C,0x1307,0x14EE,0x1706,0x1954,
                      0x1BDC,0x1EA5,0x21B6,0x2515,0x28CA,0x2CDF,0x315B,0x364B,
                      0x3BB9,0x41B2,0x4844,0x4F7E,0x5771,0x602F,0x69CE,0x7462,0x7FFF
                     };

void adpcm8_decode(void *in, void *out, int8_t stereo, int32_t n)
{
    uint8_t *m1;
    uint16_t *m2;
    m1 = (uint8_t *)in;
    m2 = (uint16_t *)out;
    uint32_t a, x, j = 0;
    int32_t b, i, t[4] = {0, 0, 0, 0};

    while (n)
    {
        a = *m1;
        i = t[j+2];
        x = t2[i];
        b = 0;

        if(a & 0x40)
            b += x;
        if(a & 0x20)
            b += x >> 1;
        if(a & 0x10)
            b += x >> 2;
        if(a & 8)
            b += x >> 3;
        if(a & 4)
            b += x >> 4;
        if(a & 2)
            b += x >> 5;
        if(a & 1)
            b += x >> 6;

        if(a & 0x80)
            b = -b;

        b += t[j];

        if(b > 32767)
            b = 32767;
        else if(b < -32768)
            b = -32768;

        i += t1[(a >> 4) & 7];

        if(i < 0)
            i = 0;
        else if(i > 88)
            i = 88;

        t[j] = b;
        t[j+2] = i;
        j = (j + 1) & stereo;
        *m2 = b;

        m1++;
        m2++;
        n--;
    }
}

struct ZGI_SND_PARAMS
{
    int8_t pkd;
    uint16_t freq;
    int8_t bits;
    int8_t stereo;
    uint8_t chr;
};

static const ZGI_SND_PARAMS zg[24]= {{0,0x2B11, 0x8, 0x0, '4'},
    {0,0x2B11, 0x8, 0x1, '5'},
    {0,0x2B11, 0x10,0x0, '6'},
    {0,0x2B11, 0x10,0x1, '7'},
    {0,0x5622, 0x8, 0x0, '8'},
    {0,0x5622, 0x8, 0x1, '9'},
    {0,0x5622, 0x10,0x0, 'a'},
    {0,0x5622, 0x10,0x1, 'b'},
    {0,0xAC44, 0x8, 0x0, 'c'},
    {0,0xAC44, 0x8, 0x1, 'd'},
    {0,0xAC44, 0x10,0x0, 'e'},
    {0,0xAC44, 0x10,0x1, 'f'},
    {1,0x2B11, 0x8, 0x0, 'g'},
    {1,0x2B11, 0x8, 0x1, 'h'},
    {1,0x2B11, 0x10,0x0, 'j'},
    {1,0x2B11, 0x10,0x1, 'k'},
    {1,0x5622, 0x8, 0x0, 'm'},
    {1,0x5622, 0x8, 0x1, 'n'},
    {1,0x5622, 0x10,0x0, 'p'},
    {1,0x5622, 0x10,0x1, 'q'},
    {1,0xAC44, 0x8, 0x0, 'r'},
    {1,0xAC44, 0x8, 0x1, 's'},
    {1,0xAC44, 0x10,0x0, 't'},
    {1,0xAC44, 0x10,0x1, 'u'}
};

Mix_Chunk *Load_ZGI(const char *file, char type)
{
    int32_t freq;
    int32_t bits;
    int32_t stereo;
    int32_t pkd;

    int32_t size,size2;

    int32_t indx= -1;

    char low = tolower(type);

    for (int32_t i=0; i<24; i++)
        if (zg[i].chr == low)
        {
            indx = i;
            freq = zg[i].freq;
            bits = zg[i].bits;
            stereo = zg[i].stereo;
            pkd = zg[i].pkd;

            break;
        }

    if (indx == -1)
        return NULL;

    FILE *f = fopen(file,"rb");
    fseek(f,0,SEEK_END);
    size = ftell(f);
    fseek(f,0,SEEK_SET);

    void *fil = malloc(size);
    fread(fil,size,1,f);
    fclose(f);

    if (pkd == 1)
        size2 = size*2 + 0x2C;
    else
        size2 = size + 0x2C;

    void *raw_w = malloc(size2);

    memcpy(raw_w,wavHeader,0x2C);

    uint32_t *raw_i = (uint32_t *)raw_w;
    raw_i[1] = size2 - 8;
    raw_i[5] = ((stereo+1)<<16) + 1;
    raw_i[6] = freq;
    if (bits == 16)
    {
        raw_i[7] = freq << (stereo+1);
        raw_i[8] = 0x100000 + (stereo + 1) * 2;
    }
    else
    {
        raw_i[7] = freq << stereo;
        raw_i[8] = 0x100000 + stereo * 2;
    }
    raw_i[10] = size2-0x2C;

    if (pkd == 1)
        adpcm8_decode(fil,&raw_i[11],stereo,size);
    else
        memcpy(&raw_i[11],fil,size);

    free(fil);

    return Mix_LoadWAV_RW(SDL_RWFromMem(raw_w,size2),1);
}

int znem_freq[16] = { 0x1F40, 0x1F40, 0x1F40, 0x1F40, 0x2B11, 0x2B11, 0x2B11, 0x2B11, 0x5622, 0x5622, 0x5622, 0x5622, 0xAC44, 0xAC44, 0xAC44, 0xAC44};

int znem_bits[4] =  {  8, 8, 0x10, 0x10};

int znem_stereo[2] = {0, 1};

Mix_Chunk *Load_ZNEM(const char *file,  char type)
{
    int32_t freq;
    int32_t bits;
    int32_t stereo;
    int32_t pkd;

    int32_t size,size2;

    char low = tolower(type);

    if ( low < 'p' )
    {
        if ( low >= 'j' )
            low--;
    }
    else
    {
        low -= 2;
    }
    if ( low > '9' )
        low -= '\'';
    low -= '0';

    char buf[4];
    int32_t t_len = strlen(file);

    buf[0] = file[t_len-3];
    buf[1] = file[t_len-2];
    buf[2] = file[t_len-1];
    buf[3] = 0;

    if (strCMP(buf,"ifp") == 0)
        pkd = 0;
    else
        pkd = 1;

    freq = znem_freq[low % 16];
    bits = znem_bits[low % 4];
    stereo = znem_stereo[low % 2];

    FILE *f = fopen(file,"rb");
    fseek(f,0,SEEK_END);
    size = ftell(f);
    fseek(f,0,SEEK_SET);

    void *fil = malloc(size);
    fread(fil,size,1,f);
    fclose(f);

    if (pkd == 1)
        size2 = size*2 + 0x2C;
    else
        size2 = size + 0x2C;

    void *raw_w = malloc(size2);

    memcpy(raw_w,wavHeader,0x2C);

    uint32_t *raw_i = (uint32_t *)raw_w;
    raw_i[1] = size2 - 8;
    raw_i[5] = ((stereo+1)<<16) + 1;
    raw_i[6] = freq;
    if (bits == 16)
    {
        raw_i[7] = freq << (stereo+1);
        raw_i[8] = 0x100000 + (stereo + 1) * 2;
    }
    else
    {
        raw_i[7] = freq << stereo;
        raw_i[8] = 0x100000 + stereo * 2;
    }
    raw_i[10] = size2-0x2C;

    if (pkd == 1)
        adpcm8_decode(fil,&raw_i[11],stereo,size);
    else
        memcpy(&raw_i[11],fil,size);

    free(fil);

    return Mix_LoadWAV_RW(SDL_RWFromMem(raw_w,size2),1);
}

Mix_Chunk *loader_LoadFile(const char *file)
{
    char buf[4];

    const char *fil = GetExactFilePath(file);

    if (!fil)
        fil = GetFilePath(file);

    if (!fil)
        return NULL;

    int32_t t_len = strlen(fil);

    buf[0] = fil[t_len-3];
    buf[1] = fil[t_len-2];
    buf[2] = fil[t_len-1];
    buf[3] = 0;

    Mix_Chunk *chunk;

    if ((strCMP(buf,"src") == 0) || (strCMP(buf,"raw") == 0) || (strCMP(buf,"ifp") == 0))
    {
#ifdef GAME_ZGI
        chunk = Load_ZGI(fil,fil[t_len-5]);
#endif
#ifdef GAME_NEMESIS
        chunk = Load_ZNEM(fil,fil[t_len-6]);
#endif
    }
    else
    {
        chunk = Mix_LoadWAV(fil);
    }

    return chunk;
}

/*********************************** END adpcm_Support *******************************/


/***********************************TGZ_Support *******************************/

void de_lz(uint8_t *dst,uint8_t *src,uint32_t size,int32_t w, int32_t h, int32_t transpose)
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

                if (transpose == 0)
                    dst[d_cur]=src[cur];
                else
                {
                    int32_t dx = d_cur/2;
                    int32_t ddx = d_cur & 1;
                    int32_t hh = dx % h;
                    int32_t ww = dx / h;
                    //if (((hh*w+ww)*2 + ddx) <h*2*w)
                    dst[(hh*w+ww)*2 + ddx] = src[cur];
                    //else
                    //       printf("asdasdasdas\n");
                }


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
                    if (transpose == 0)
                        dst[d_cur]=lz[(otsk+j) & 0xfff];
                    else
                    {
                        int32_t dx = d_cur/2;
                        int32_t ddx = d_cur & 1;
                        int32_t hh = dx % h;
                        int32_t ww = dx / h;
                        //if (d_cur <h*2*w)
                        dst[(hh*w+ww)*2 + ddx] = lz[(otsk+j) & 0xfff];
                        //else
                        // printf("asdasdasdas\n");
                    }
                    lz_pos=(lz_pos+1) & 0xfff;
                    d_cur++;
                }
            };

            mk=mk << 1;

        }

        cur++;
    };

}



SDL_Surface *loader_Load_GF_File(const char *file, int8_t transpose,int8_t key,uint32_t ckey)
{
    char buf[4];

    const char *fil = GetExactFilePath(file);

    if (!fil)
        fil = GetFilePath(file);

    if (!fil)
        return NULL;

    int32_t t_len = strlen(fil);

    buf[0] = fil[t_len-3];
    buf[1] = fil[t_len-2];
    buf[2] = fil[t_len-1];
    buf[3] = 0;

    SDL_Surface *srf=NULL;

    if ((strCMP(buf,"tga") == 0))
    {
        FILE *f = fopen(fil,"rb");
        uint32_t magic;
        fread(&magic,4,1,f);
        if (magic  == 0x005A4754)
        {

            fseek(f,0,SEEK_END);
            int32_t pksz = ftell(f) - 0x10;
            fseek(f,8,SEEK_SET);

            int32_t wi;
            fread(&wi,4,1,f);

            int32_t hi;
            fread(&hi,4,1,f);

            uint8_t *mem = (uint8_t *)malloc(pksz);
            fread(mem,1,pksz,f);
            fclose(f);

            if (transpose)
            {
                wi ^= hi;
                hi ^= wi;
                wi ^= hi;
            }

            srf = SDL_CreateRGBSurface(SDL_SWSURFACE,wi,hi,16,0x7C00,0x3E0,0x1F,0);

            SDL_LockSurface(srf);



            de_lz((uint8_t *)srf->pixels,mem,pksz,wi,hi,transpose);

            SDL_UnlockSurface(srf);

            free(mem);
        }
        else
        {
            fclose(f);
            srf = IMG_Load(fil);
        }
    }
    else
    {
        srf = IMG_Load(fil);
    }

    if (srf)
        ConvertImage(&srf);

    if (key != 0)
        SDL_SetColorKey(srf,SDL_SRCCOLORKEY,ckey);

    return srf;
}


SDL_Surface *loader_LoadFile(const char *file, int8_t transpose)
{
    return loader_Load_GF_File(file,transpose,0,0);
}
SDL_Surface *loader_LoadFile(const char *file, int8_t transpose,uint32_t key)
{
    return loader_Load_GF_File(file,transpose,1,key);
}



/*********************************** END TGZ_Support *******************************/




/*********************************** RLF ***************************************/


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

struct mTime
{
    uint32_t magic; //EMIT
    uint32_t size;
    uint32_t unk1;
    uint32_t microsecs;
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

void DHLE(int8_t *dst, int8_t *src,int32_t size,int32_t size2)
{
    int8_t tmp;
    int32_t off1, off2;
    int16_t tmp2;
    off1=0;
    off2=0;

    while (off1 < size && off2 < size2)
    {
        tmp = src[off1];
        off1++;

        if (tmp < 0)
        {
            if (tmp < 0)
            {
                tmp=abs(tmp);
                while (tmp!=0)
                {
                    if (off1+1 >= size || off2+1 >= size2)
                        return;
                    tmp2=*(int16_t *)(&src[off1]);

                    *(int16_t *)&dst[off2] = tmp2;
                    off2+=2;
                    off1+=2;
                    tmp--;
                }
            }
        }
        else
            off2+=tmp*2 + 2;
    }
}

void HRLE(int8_t *dst, int8_t *src,int32_t size,int32_t size2)
{
    int8_t tmp;
    int32_t off1, off2;
    int16_t tmp2;
    int32_t tmp4;
    off1=0;
    off2=0;

    while (off1 < size)
    {
        tmp = src[off1];
        off1++;

        if (tmp < 0)
        {
            if (tmp < 0)
            {
                tmp=abs(tmp);
                while (tmp > 0)
                {
                    if (off1+1 >= size || off2+1 >= size2)
                        return;
                    tmp2=*(int16_t *)(src + off1);
                    *(int16_t *)&dst[off2] = tmp2;
                    off2+=2;
                    off1+=2;
                    tmp--;
                }
            }
        }
        else
        {
            if (off1+1 >= size)
                return;

            tmp2 = *(int16_t *)&src[off1];
            off1 += 2;
            tmp4=tmp+2;

            while (tmp4>0)
            {
                if (off2+1 >= size2)
                    return;
                *(int16_t *)&dst[off2] = tmp2;
                off2+=2;
                tmp4--;
            }
        }
    }
}




SDL_Surface *rlf_frame(void *buf, int32_t w, int32_t h, int8_t transpose)
{
    SDL_Surface *srf = SDL_CreateRGBSurface(SDL_SWSURFACE,w,h,16,0x7C00,0x3E0,0x1F,0);

    SDL_LockSurface(srf);

    if (transpose == 1)
    {
        uint16_t *tmp = (uint16_t *)buf;
        uint16_t *tmp2 = (uint16_t *)srf->pixels;

        for (int32_t j=0; j< h; j++)
            for (int32_t i=0; i< w; i++)
            {
                *tmp2 = tmp[i*h+j];
                tmp2++;
            }

    }
    else
        memcpy(srf->pixels,buf,w*h*2);

    SDL_UnlockSurface(srf);

    return srf;
}


anim_surf *loader_LoadRlf(const char *file, int8_t transpose,int32_t mask)
{
#ifdef LOADTRACE
    printf("loader:\trlf\tTry to load %s with ",file);
#endif

    const char *fil = GetExactFilePath(file);

    if (fil == NULL)
        return LoadAnimImage(file,mask); //rollback mechanism

#ifdef LOADTRACE
    printf("RLF-mechanism\n");
#endif

    FILE *f=fopen(fil,"rb");
    if (!f)
        return NULL;

    Header hd;
    fread(&hd,1,sizeof(Header),f);

    if (hd.magic != 0x524C4546) //RLEF
        return NULL;

    Cinf cin;
    fread(&cin,1,sizeof(Cinf),f);
    MinF mn;
    fread(&mn,1,sizeof(MinF),f);
    mTime tm;
    fread(&tm,1,sizeof(mTime),f);


    if (transpose == 1)
    {
        mn.height ^=  mn.width;
        mn.width  ^=  mn.height;
        mn.height ^=  mn.width;
    }

    anim_surf *atmp=new(anim_surf);


    typedef SDL_Surface* PSDL_Surface;

    atmp->info.time=tm.microsecs / 10;
    atmp->info.frames=hd.frames;
    atmp->info.w = mn.width;
    atmp->info.h = mn.height;
    atmp->img=new PSDL_Surface[atmp->info.frames];//frames * sizeof(SDL_Surface *));



    Frame frm;
    int32_t sz_frame = mn.height*mn.width*2;
    void *buf2=malloc(sz_frame);
    memset(buf2,0,sz_frame);

    for (uint16_t i=0; i<hd.frames ; i++)
    {
        if (fread(&frm,1,sizeof(Frame),f) == sizeof(Frame))
            if (frm.size > 0 && frm.size < 0x40000000)
            {

                void *buf=malloc(frm.size-frm.offset);
                if ( fread(buf,1,frm.size-frm.offset,f) ==  frm.size-frm.offset)
                {


                    if (frm.TYPE == 0x44484C45)
                        DHLE((int8_t *)buf2,(int8_t *)buf,frm.size-frm.offset,sz_frame);
                    else if (frm.TYPE == 0x48524C45)
                        HRLE((int8_t *)buf2,(int8_t *)buf,frm.size-frm.offset,sz_frame);
                }

                free(buf);
            }

        atmp->img[i] = rlf_frame(buf2,atmp->info.w,atmp->info.h,transpose);
        ConvertImage(&atmp->img[i]);

        if (mask != 0 && mask != -1)
            SDL_SetColorKey(atmp->img[i],SDL_SRCCOLORKEY ,mask);


    }

    fclose(f);

    free(buf2);

    return atmp;
}

/*********************************** END RLF ***************************************/



void loader_LoadZcr(const char *file, Cursor *cur)
{
#ifdef LOADTRACE
    printf("loader:\tzcr\tTry to load %s with ",file);
#endif
    const char *fil = GetExactFilePath(file);

    if (fil == NULL)
    {
#ifdef LOADTRACE
        printf("fallback-mechanism\n");
#endif
        Mouse_LoadCursor(file,cur);
        return;
    }
#ifdef LOADTRACE
    printf("ZCR-mechanism\n");
#endif

    FILE *f=fopen(fil,"rb");
    if (!f)
        return;

    uint32_t magic=0;
    fread(&magic,4,1,f);
    if (magic  == 0x3152435A)
    {
        uint16_t x,y,w,h;
        fread(&x,2,1,f);
        fread(&y,2,1,f);
        fread(&w,2,1,f);
        fread(&h,2,1,f);

        cur->ox = x;
        cur->oy = y;

        cur->img = SDL_CreateRGBSurface(SDL_SWSURFACE,w,h,16,0x7C00,0x3E0,0x1F,0);

        SDL_LockSurface(cur->img);

        fread(cur->img->pixels,2,w*h,f);

        SDL_UnlockSurface(cur->img);

        ConvertImage(&cur->img);

        SDL_SetColorKey(cur->img,SDL_SRCCOLORKEY,0);
    }
    fclose(f);
}

void loader_LoadMouseCursor(const char *file, Cursor *cur)
{
    if (strcasestr(file,"zcr") != NULL)
        loader_LoadZcr(file,cur);
    else
        Mouse_LoadCursor(file,cur);
}
