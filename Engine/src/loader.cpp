#include "System.h"


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
	                  0x3BB9,0x41B2,0x4844,0x4F7E,0x5771,0x602F,0x69CE,0x7462,0x7FFF };

void adpcm8_decode(void *in, void *out, uint stereo, uint n)
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
    int16_t freq;
    int8_t bits;
    int8_t stereo;
    uint8_t chr;
};

static const ZGI_SND_PARAMS zg[24]={{0,0x2B11, 0x8, 0x0, '4'},
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
                                    {1,0xAC44, 0x10,0x1, 'u'}};

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

