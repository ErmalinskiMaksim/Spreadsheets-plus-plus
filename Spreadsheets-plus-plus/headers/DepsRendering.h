#ifndef DEPS_RENDERING_H
#define DEPS_RENDERING_H

#include "SDL3_ttf/SDL_ttf.h"
#include <memory>

using Rect      = SDL_FRect;
using Point     = SDL_FPoint;
using Color4    = SDL_Color;
using Color1    = Uint8;

using WindowPtrType     = SDL_Window*;
using RendererPtrType   = SDL_Renderer*;
using TexturePtrType    = SDL_Texture*;
using FontPtrType = TTF_Font*;
using WindowType    = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
using RendererType  = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
using TextureType   = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
using FontType = std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)>;

#endif // !
