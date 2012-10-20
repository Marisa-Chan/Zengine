#ifndef SIMPLE_AVI_H_INCLUDED
#define SIMPLE_AVI_H_INCLUDED

#include <SDL/SDL_audio.h>
#include <SDL/SDL_mixer.h>

#define AVI_PLAY    1
#define AVI_STOP    0

struct avi_strm_hdr
{
    uint32_t size;
    uint32_t streamType;
    uint32_t streamHandler;
    uint32_t flags;
    uint16_t priority;
    uint16_t language;
    uint32_t initialFrames;
    uint32_t scale;
    uint32_t rate;
    uint32_t start;
    uint32_t length;
    uint32_t bufferSize;
    uint32_t quality;
    uint32_t sampleSize;
    //Common::Rect frame;
};

struct vid_idx
{
    uint32_t id;
    uint32_t flags;
    uint32_t offset;
    uint32_t size;
};

struct vid_trk
{
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t sizeImage;
    uint32_t xPelsPerMeter;
    uint32_t yPelsPerMeter;
    uint32_t clrUsed;
    uint32_t clrImportant;
    avi_strm_hdr hdr;
};
struct aud_trk
{
    uint16_t tag;
    uint16_t channels;
    uint32_t samplesPerSec;
    uint32_t avgBytesPerSec;
    uint16_t blockAlign;
    uint16_t size;
    avi_strm_hdr hdr;
};

struct vframes
{
    uint32_t fof;
    uint32_t sz;
    bool kfrm;
};

#define MAX_MOVI    10

struct avi_file
{

    int8_t status;
    uint32_t stime;

    FILE *file;
    uint32_t size;
    uint32_t cframe;

    int32_t w,h;

    struct header
    {
        uint32_t size;
        uint32_t mcrSecPframe;
        uint32_t maxbitstream;
        uint32_t padding;
        uint32_t flags;
        uint32_t frames;
        uint32_t iframes;
        uint32_t streams;
        uint32_t buffsize;
        int32_t width;
        int32_t height;
    } header;

    struct movi
    {
        uint32_t fofset;
        uint32_t offset;
        uint32_t ssize;
    } movi [MAX_MOVI];

    uint32_t movi_cnt;

    uint32_t idx_cnt;
    vid_idx *idx;
    vid_trk *vtrk;
    aud_trk *atrk;

    int32_t pix_fmt;

    void *frame;

    void *priv_data;

    vframes *vfrm;
    int32_t vfrm_cnt;

    vframes *achunk;
    int32_t achunk_cnt;

    uint32_t lastrnd;

    void *buf;

    uint8_t translate;
};


avi_file *avi_openfile(const char *fle, uint8_t transl = 0);
void avi_set_dem(avi_file *av, int32_t w, int32_t h);
int8_t avi_renderframe(avi_file *av, int32_t frm);
void avi_play(avi_file *av);
Mix_Chunk *avi_get_audio(avi_file *av);
void avi_to_surf(avi_file *av, SDL_Surface *srf);
void avi_stop(avi_file *av);
void avi_update(avi_file *av);
void avi_close(avi_file *av);

#endif // SIMPLE_AVI_H_INCLUDED
