
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

static const uchar wavHeader[] = {
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

const int t1[] = { -1,-1,-1,1,4,7,10,12 };
const int t2[] = { 0x0007,0x0008,0x0009,0x000A,0x000B,0x000C,0x000D,0x000E,0x0010,0x0011,0x0013,0x0015,0x0017,0x0019,0x001C,0x001F,
	0x0022,0x0025,0x0029,0x002D,0x0032,0x0037,0x003C,0x0042,0x0049,0x0050,0x0058,0x0061,0x006B,0x0076,0x0082,0x008F,
	0x009D,0x00AD,0x00BE,0x00D1,0x00E6,0x00FD,0x0117,0x0133,0x0151,0x0173,0x0198,0x01C1,0x01EE,0x0220,0x0256,0x0292,
	0x02D4,0x031C,0x036C,0x03C3,0x0424,0x048E,0x0502,0x0583,0x0610,0x06AB,0x0756,0x0812,0x08E0,0x09C3,0x0ABD,0x0BD0,
	0x0CFF,0x0E4C,0x0FBA,0x114C,0x1307,0x14EE,0x1706,0x1954,0x1BDC,0x1EA5,0x21B6,0x2515,0x28CA,0x2CDF,0x315B,0x364B,
	0x3BB9,0x41B2,0x4844,0x4F7E,0x5771,0x602F,0x69CE,0x7462,0x7FFF };

int adpcm8_decode(FILE *fi, FILE *fo, uint stereo, uint n) {
	uint a, x, j = 0; int b, i, t[4] = {0, 0, 0, 0};
	if(n) do {
	  if((a = fgetc(fi)) == EOF) return -1;
	  i = t[j+2]; x = t2[i]; b = 0;
	  if(a & 0x40) b += x;
	  if(a & 0x20) b += x >> 1;
	  if(a & 0x10) b += x >> 2;
	  if(a & 8) b += x >> 3;
	  if(a & 4) b += x >> 4;
	  if(a & 2) b += x >> 5;
	  if(a & 1) b += x >> 6;
	  if(a & 0x80) b = -b;
	  b += t[j];
	  if(b > 32767) b = 32767; else if(b < -32768) b = -32768;
	  i += t1[(a >> 4) & 7];
	  if(i < 0) i = 0; else if(i > 88) i = 88;
	  t[j] = b; t[j+2] = i;
	  j = (j + 1) & stereo;
	  fputc(b & 0xFF, fo); fputc(b >> 8, fo);
	} while(--n);
	return 0;
}

int adpcm8_nodecode(FILE *fi, FILE *fo) {
	while (!feof(fi))
	  fputc(fgetc(fi), fo);
}

int process(FILE *fi, FILE *fo, uint adpcm, uint freq, uint stereo, uint bits) {
	int i; uchar buf[0x2C];

	memcpy(buf, wavHeader, 0x2C); 
	fwrite(buf, 1, 0x2C, fo);

	if (adpcm == 1)
	   adpcm8_decode(fi, fo, stereo, -1);
   	else
   	   adpcm8_nodecode(fi, fo);

	stereo++;
	i = ftell(fo);
	*((uint*)(buf+4)) = i - 8;
	*((uint*)(buf+0x14)) = (stereo << 16) + 1;
	*((uint*)(buf+0x18)) = freq;
	*((uint*)(buf+0x1C)) = freq << stereo;
	*((uint*)(buf+0x20)) = 0x100000 + stereo * 2;
	*((uint*)(buf+0x28)) = i - 0x2C;

	fseek(fo, 0, SEEK_SET); fwrite(buf, 1, 0x2C, fo);
	exit(1);
}

int main(int argc, char **argv) {
	FILE *fi, *fo; int freq, adpcm, stereo; char c;

	if(argc != 6) 
	{
	printf("Usage:\nunraw infile.raw outfile.wav compressed freq stereo\n\ncompressed: 0 - raw; 1 - adpcm\nfreq: 11025; 22050; 44100\nstereo: 0 - mono; 1 - stereo\n\n");
	return 1;
	}
	if(sscanf(argv[3], "%u%c", &adpcm, &c) != 1) return 1;
	if(sscanf(argv[4], "%u%c", &freq, &c) != 1) return 1;
	if((freq < 0)||(freq > 96000)) return 1;
	if(sscanf(argv[5], "%u%c", &stereo, &c) != 1) return 1;
	if((stereo < 0)||(stereo > 1)) return 1;

	if(fi = fopen(argv[1], "rb")) {
	  if(fo = fopen(argv[2], "wb")) {
	    process(fi, fo, adpcm, freq, stereo,0);
	    fclose(fo);
	  }
	  fclose(fi);
	}
	return 0;
}

