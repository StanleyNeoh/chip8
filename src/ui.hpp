#ifndef SRC_UI_HPP
#define SRC_UI_HPP

#include <memory>
#include <vector>
#include <cstdint>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

class UI {
public:
    int width = 0;
    int height = 0;
    int done = 0;
    uint32_t fg_color = 0xFFFFFFFFu;
    uint32_t bg_color = 0x00000000u;

    static UI& instance() {
        static UI ui;
        return ui;
    }
    UI(const UI& ui) = delete;
    UI(const UI&& ui) = delete;
    UI& operator=(const UI& ui) = delete;
    UI& operator=(const UI&& ui) = delete;

    int init(const char* title, int w, int h) {
        if (!sdl_inited) {
            if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
                return -1;
            }
            sdl_inited = true;
        }
        if (!sdl_obj_inited) {
            sdl_window = SDL_CreateWindow(title, w, h, 0);
            if (!sdl_window) return -1;
            sdl_renderer = SDL_CreateRenderer(sdl_window, NULL);
            if (!sdl_renderer) return -1;
            sdl_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);
            if (!sdl_texture) return -1;
            sdl_obj_inited = true;
        }
        width = w;
        height = h;
        frame_buffer.assign(static_cast<size_t>(width) * height, bg_color);
        done = 0;
        // Optional logical scaling (commented out for compatibility with some SDL3 builds)
        // SDL_SetRenderLogicalPresentation(sdl_renderer, width, height, SDL_LOGICAL_PRESENTATION_STRETCH, SDL_SCALEMODE_NEAREST);
        return 0;
    }

    inline void draw(int r, int c) {
        if ((unsigned)r < (unsigned)height && (unsigned)c < (unsigned)width)
            frame_buffer[static_cast<size_t>(r) * width + c] = fg_color;
    }

    // toggle pixel, return true if collision (pixel changed from set to unset)
    inline bool toggle(int r, int c) {
        if ((unsigned)r >= (unsigned)height || (unsigned)c >= (unsigned)width) return false;
        size_t idx = static_cast<size_t>(r) * width + c;
        uint32_t prev = frame_buffer[idx];
        bool collision = (prev == fg_color);
        frame_buffer[idx] = collision ? bg_color : fg_color;
        return collision;
    }

    inline uint32_t getPixel(int r, int c) const {
        if ((unsigned)r >= (unsigned)height || (unsigned)c >= (unsigned)width) return bg_color;
        return frame_buffer[static_cast<size_t>(r) * width + c];
    }

    inline void clear() {
        std::fill(frame_buffer.begin(), frame_buffer.end(), bg_color);
    }

    void display() {
        if (!sdl_texture || frame_buffer.empty()) return;
        void* pix = nullptr;
        int pitch = 0;
        if (SDL_LockTexture(sdl_texture, NULL, &pix, &pitch) != 0) return;
        unsigned char* dst = static_cast<unsigned char*>(pix);
        const uint8_t* src = reinterpret_cast<const uint8_t*>(frame_buffer.data());
        for (int row = 0; row < height; ++row) {
            memcpy(dst + row * pitch, src + static_cast<size_t>(row) * width * 4, width * 4);
        }
        SDL_UnlockTexture(sdl_texture);
        SDL_RenderTexture(sdl_renderer, sdl_texture, NULL, NULL);
        SDL_RenderPresent(sdl_renderer);
    }
    
    bool loop() {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                return false;
            }
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_ESCAPE) {
                return false;
            }
        }
        SDL_Delay(1);
        return true;
    }

private:
    std::vector<uint32_t> frame_buffer;
    SDL_Window* sdl_window = nullptr;
    SDL_Renderer* sdl_renderer = nullptr;
    SDL_Texture* sdl_texture = nullptr;
    bool sdl_inited = false;
    bool sdl_obj_inited = false;

    UI() = default;
    ~UI() {
        if (sdl_texture) SDL_DestroyTexture(sdl_texture);
        if (sdl_renderer) SDL_DestroyRenderer(sdl_renderer);
        if (sdl_window) SDL_DestroyWindow(sdl_window);
        SDL_Quit();
    }
};

#endif