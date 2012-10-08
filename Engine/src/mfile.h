#ifndef MFILE_H_INCLUDED
#define MFILE_H_INCLUDED

struct mfile
{
    char *buf;
    int32_t size;
    int32_t pos;
};

mfile *mfopen(const char *file);
mfile *mfopen(FManNode *nod);
int32_t mfsize(FManNode *nod);
void mfclose(mfile *fil);
bool mfeof(mfile *fil);
bool mfread(void *buf, int32_t bytes, mfile *file);
void mfseek(mfile *fil, int32_t pos);
char * mfgets(char * str, int32_t num, mfile * stream);
mfile *mfopen_mem(void *buf, int32_t size);
void m_wide_to_utf8(mfile *file);

#endif // MFILE_H_INCLUDED
