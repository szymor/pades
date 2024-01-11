#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>

#define SCREEN_WIDTH        (320)
#define SCREEN_HEIGHT       (240)
#define SCREEN_BPP          (32)
#define MAX_PATTERN_SIZE    (32)
#define MAX_PREVIEW_SIZE    (192)
#define BG_COLOR            (0xaa)

SDL_Surface *screen = NULL;
SDL_Surface *pattern = NULL;
int pattern_size = 4;
int px = 0, py = 0;

void redraw(void);

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetCaption("Pattern Designer", NULL);
    SDL_ShowCursor(SDL_DISABLE);
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT,
        SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);

    pattern = SDL_CreateRGBSurface(SDL_HWSURFACE,
        MAX_PATTERN_SIZE, MAX_PATTERN_SIZE, SCREEN_BPP,
        screen->format->Rmask, screen->format->Gmask,
        screen->format->Bmask, screen->format->Amask);
    SDL_FillRect(pattern, NULL, SDL_MapRGB(pattern->format,
        BG_COLOR, BG_COLOR, BG_COLOR));

    redraw();

    SDL_Event event;
    while (SDL_WaitEvent(&event))
    {
        switch (event.type)
        {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_TAB:
                        --pattern_size;
                        if (pattern_size < 2)
                            pattern_size = 2;
                        if (px >= pattern_size || py >= pattern_size)
                        {
                            px = 0;
                            py = 0;
                        }
                        break;
                    case SDLK_BACKSPACE:
                        ++pattern_size;
                        if (pattern_size > MAX_PATTERN_SIZE)
                            pattern_size = MAX_PATTERN_SIZE;
                        break;
                    case SDLK_SPACE:
                    case SDLK_LCTRL:
                    case SDLK_LALT:
                    case SDLK_LSHIFT:
                        SDL_LockSurface(pattern);
                        Uint8 *p = (Uint8 *)pattern->pixels +
                            py * pattern->pitch +
                            px * pattern->format->BytesPerPixel;
                        // RGBA or BGRA - does not matter
                        // needs to be 32bpp
                        switch (event.key.keysym.sym)
                        {
                            case SDLK_SPACE:
                                if (p[0] == BG_COLOR)
                                    p[0] = p[1] = p[2] = 0xff;
                                else
                                    p[0] = p[1] = p[2] = BG_COLOR;
                                break;
                            case SDLK_LCTRL:
                                if (p[0] == BG_COLOR)
                                    p[0] = p[1] = p[2] = 0x00;
                                else
                                    p[0] = p[1] = p[2] = BG_COLOR;
                                break;
                            case SDLK_LALT:
                            case SDLK_LSHIFT:
                                if (p[0] == BG_COLOR)
                                    p[0] = p[1] = p[2] = 0x55;
                                else
                                    p[0] = p[1] = p[2] = BG_COLOR;
                                break;
                        }
                        SDL_UnlockSurface(pattern);
                        break;
                    case SDLK_LEFT:
                        --px;
                        if (px < 0)
                            px = pattern_size - 1;
                        break;
                    case SDLK_RIGHT:
                        ++px;
                        if (px >= pattern_size)
                            px = 0;
                        break;
                    case SDLK_UP:
                        --py;
                        if (py < 0)
                            py = pattern_size - 1;
                        break;
                    case SDLK_DOWN:
                        ++py;
                        if (py >= pattern_size)
                            py = 0;
                        break;
                    case SDLK_RETURN:
                        SDL_FillRect(pattern, NULL, SDL_MapRGB(pattern->format,
                            BG_COLOR, BG_COLOR, BG_COLOR));
                        break;
                    case SDLK_ESCAPE:
                        exit(0);
                        break;
                }
                break;
            case SDL_QUIT:
                exit(0);
                break;
        }

        // redraw after any event
        redraw();
    }

    SDL_Quit();
    return 0;
}

void redraw(void)
{
    // background
    SDL_Rect src, dst;
    src.x = src.y = 0;
    src.w = src.h = pattern_size;
    for (dst.y = 0; dst.y < SCREEN_HEIGHT; dst.y += pattern_size)
        for (dst.x = 0; dst.x < SCREEN_WIDTH; dst.x += pattern_size)
        {
            SDL_BlitSurface(pattern, &src, screen, &dst);
        }

    // calculate optimal preview pixel size
    dst.w = MAX_PREVIEW_SIZE / pattern_size;
    if (dst.w > 16) dst.w = 16;
    dst.h = dst.w;

    // pattern
    SDL_LockSurface(pattern);
    dst.y = SCREEN_HEIGHT - dst.h * pattern_size;
    for (int i = 0; i < pattern_size; ++i, dst.y += dst.h)
    {
        dst.x = SCREEN_WIDTH - dst.w * pattern_size;
        for (int j = 0; j < pattern_size; ++j, dst.x += dst.w)
        {
            Uint8 *p = (Uint8 *)pattern->pixels + i * pattern->pitch +
                j * pattern->format->BytesPerPixel;
            SDL_FillRect(screen, &dst,
                SDL_MapRGB(pattern->format, p[0], p[0], p[0]));
        }
    }
    SDL_UnlockSurface(pattern);

    dst.y = SCREEN_HEIGHT - dst.h * pattern_size;
    dst.x = SCREEN_WIDTH - dst.w * pattern_size;
    hlineRGBA(screen, dst.x, SCREEN_WIDTH, dst.y, 255, 0, 0, 255);
    vlineRGBA(screen, dst.x, dst.y, SCREEN_HEIGHT, 255, 0, 0, 255);
    dst.y += py * dst.h;
    dst.x += px * dst.w;
    rectangleRGBA(screen, dst.x, dst.y, dst.x + dst.w, dst.y + dst.h,
        255, 0, 0, 255);

    // refresh
    SDL_Flip(screen);
}
