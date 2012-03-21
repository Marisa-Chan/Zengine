#ifndef LOADER_H_INCLUDED
#define LOADER_H_INCLUDED

Mix_Chunk *loader_LoadFile(const char *file);
SDL_Surface *loader_LoadFile(const char *file, int8_t transpose);
SDL_Surface *loader_LoadFile(const char *file, int8_t transpose,uint32_t key);

#endif // LOADER_H_INCLUDED
