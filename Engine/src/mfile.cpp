#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "System.h"
#include "mfile.h"

mfile *mfopen(const char *file)
{
    FILE *fl = fopen(file,"rb");
    if (!fl)
        return NULL;

    mfile *tmp = new mfile;

    fseek(fl,0,SEEK_END);

    tmp->size = ftell(fl);

    fseek(fl,0,SEEK_SET);

    tmp->pos = 0;

    tmp->buf = (char *)malloc(tmp->size);

    fread(tmp->buf,tmp->size,1,fl);

    fclose(fl);

    return tmp;
}

mfile *mfopen(FManNode *nod)
{
    mfile *tmp = NULL;

    if (nod->zfs)
    {
        tmp = new mfile;
        tmp->buf = (char *)loader_zload(nod->zfs);
        tmp->pos = 0;
        tmp->size = nod->zfs->size;
    }
    else
    {
        FILE *fl = fopen(nod->Path,"rb");
        if (!fl)
            return NULL;

        tmp = new mfile;

        fseek(fl,0,SEEK_END);

        tmp->size = ftell(fl);

        fseek(fl,0,SEEK_SET);

        tmp->pos = 0;

        tmp->buf = (char *)malloc(tmp->size);

        fread(tmp->buf,tmp->size,1,fl);

        fclose(fl);
    }


    return tmp;
}

int32_t mfsize(FManNode *nod)
{
    if (nod->zfs)
        return nod->zfs->size;

    return FileSize(nod->Path);
}

mfile *mfopen_mem(void *buf, int32_t size)
{

    mfile *tmp = new mfile;

    tmp->size = size;
    tmp->pos = 0;

    tmp->buf = (char *)malloc(tmp->size);

    memcpy(tmp->buf,buf,size);
    return tmp;
}

bool mfread(void *buf, int32_t bytes, mfile *file)
{
    if (file->pos + bytes > file->size)
        return false;

    memcpy(buf, file->buf + file->pos, bytes);

    file->pos+=bytes;

    return true;
}

void mfseek(mfile *fil, int32_t pos)
{
    if (pos <= fil->size && pos >= 0)
        fil->pos = pos;
}

void mfclose(mfile *fil)
{
    free(fil->buf);
    delete fil;
}

bool mfeof(mfile *fil)
{
    return (fil->pos >= fil->size);
}

char * mfgets(char * str, int32_t num, mfile * stream)
{
    int32_t copied = 0;
    num--;

    if (mfeof(stream))
        return NULL;

    while((copied < num) && (stream->pos < stream->size) && (stream->buf[stream->pos] != 0x0A) && (stream->buf[stream->pos] != 0x0D))
    {
        str[copied] = stream->buf[stream->pos];
        copied++;
        stream->pos++;
    }

    str[copied]=0;

    if (stream->pos < stream->size)
    {
        if (stream->buf[stream->pos] == 0x0D)
        {
            stream->pos++;

            if (stream->pos < stream->size)
                if (stream->buf[stream->pos] == 0xA)//windows
                    stream->pos++;
        }
        else if (stream->buf[stream->pos] == 0x0D) //unix
            stream->pos++;
    }

    return str;
}

bool m_is_wide_char(mfile *file)
{
    for(int32_t i=0;i<file->size-2;i++)
        if (file->buf[i] == 0 && file->buf[i+2] == 0)
            return true;

    return false;
}

void m_wide_to_utf8(mfile *file)
{
    if (m_is_wide_char(file))
    {
        char *buf = (char *)malloc(file->size*2);
        int32_t pos = 0,size = file->size*2;
        file->pos = 0;
        while (file->pos < file->size && pos < size)
        {
            if (file->buf[file->pos] == 0xD)
            {
                buf[pos] = 0xD;
                pos++;
                file->pos++;
                if (file->pos < file->size)
                    if (file->buf[file->pos] == 0xA)
                    {
                        if (pos < size)
                        {
                            buf[pos] = 0xA;
                            pos++;
                        }
                        file->pos++;
                        if (file->pos < file->size)
                            if (file->buf[file->pos] == 0x0)
                                file->pos++;
                    }
            }
            else
            {
                if (file->pos+1 < file->size)
                {
                    uint16_t ch = (file->buf[file->pos] & 0xFF) | ((file->buf[file->pos+1] << 8) & 0xFF00) ;
                    if (ch < 0x80)
                    {
                        buf[pos] = ch & 0x7F;
                        pos++;
                    }
                    else if (ch >= 0x80 && ch < 0x800)
                    {
                        if (pos + 1 < size)
                        {
                            buf[pos] = 0xC0 | ((ch >>6) & 0x1F);
                            pos++;
                            buf[pos] = 0x80 | ((ch) & 0x3F);
                            pos++;
                        }
                    }
                    else if (ch >= 0x800 && ch < 0x10000)
                    {
                        if (pos + 2 < size)
                        {
                            buf[pos] = 0xE0 | ((ch >>12) & 0xF);
                            pos++;
                            buf[pos] = 0x80 | ((ch >> 6) & 0x3F);
                            pos++;
                            buf[pos] = 0x80 | ((ch) & 0x3F);
                            pos++;
                        }
                    }
                    else if (ch >= 0x10000 && ch < 0x200000)
                    {
                        if (pos + 3 < size)
                        {
                            buf[pos] = 0xF0;
                            pos++;
                            buf[pos] = 0x80 | ((ch >> 12) & 0x3F);
                            pos++;
                            buf[pos] = 0x80 | ((ch >> 6) & 0x3F);
                            pos++;
                            buf[pos] = 0x80 | ((ch) & 0x3F);
                            pos++;
                        }
                    }

                    file->pos+=2;
                }
            }
        }

    free(file->buf);
    file->pos = 0;
    file->size = pos;
    file->buf = (char *)malloc(file->size);
    memcpy(file->buf,buf,file->size);
    free(buf);
    }
}
