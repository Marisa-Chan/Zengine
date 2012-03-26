#ifndef MFILE_H_INCLUDED
#define MFILE_H_INCLUDED

struct mfile
{
    char *buf;
    int32_t size;
    int32_t pos;
};

mfile *mfopen(const char *file);
void mfclose(mfile *fil);
bool mfeof(mfile *fil);
char * mfgets(char * str, int32_t num, mfile * stream);
mfile *mfopen_mem(void *buf, int32_t size);
void m_wide_to_utf8(mfile *file);

#endif // MFILE_H_INCLUDED
