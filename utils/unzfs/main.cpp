#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>


struct header
{
    uint32_t magic;
    uint32_t unk1;
    uint32_t unk2;
    uint32_t files_perblock;
    uint32_t files_cnt;
    uint32_t xor_key;
    uint32_t Offset_files;
};

struct file_header
{
    char        name[0x10];
    uint32_t    offset;
    uint32_t    id;
    uint32_t    size;
    uint32_t    time;
    uint32_t    unk2;
};

void unxor(void *buf,uint32_t size,uint32_t xork)
{
    uint32_t cnt=size >> 2;
    uint32_t cnt2=size & 3;

    uint32_t *px=(uint32_t *)buf;

    for (uint32_t i=0;i<cnt;i++)
    {
        px[i]^=xork;
        //px++;
    }

    uint8_t *px2=(uint8_t *)&px[cnt];

   /* if (cnt2 > 0)
        px2[0]^=(xork & 0xFF);

    if (cnt2 > 1)
        px2[1]^=((xork >> 8) & 0xFF);

    if (cnt2 > 2)
        px2[2]^=((xork >> 16) & 0xFF);
*/
}


int main(int argvc,char **argv)
{
    if (argvc>2)
    {
        FILE *f=fopen(argv[1],"rb");
        header head;
        fread(&head,sizeof(head),1,f);

        uint32_t nextpos;

        nextpos=0x1C;

        while (nextpos!=0)
        {

            fseek(f,nextpos,0);
            fread(&nextpos,4,1,f);
            uint32_t pos=ftell(f);



            for (int i=0; i<head.files_perblock; i++)
            {
                fseek(f,pos,0);
                file_header fil;
                fread(&fil,sizeof(fil),1,f);

                pos+=sizeof(fil);

                if (strlen(fil.name)>0)
                {
                    fseek(f,fil.offset,0);

                    void * bytes=malloc(fil.size);

                    fread(bytes,fil.size,1,f);

                    if (head.xor_key != 0)
                        unxor(bytes,fil.size,head.xor_key);

                    char buff[512];
                    sprintf(buff,"%s/%s",argv[2],fil.name);
                    FILE *ff=fopen(buff,"wb");
                    fwrite(bytes,fil.size,1,ff);
                    fclose(ff);

                    free(bytes);
                }
            }

        }
        fclose(f);
    }
    else
        printf("Specify file and path for extracting.\n");

    return 0;
}
