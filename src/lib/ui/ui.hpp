#ifndef SRC_UI_HPP
#define SRC_UI_HPP

#include <stdexcept>
#include <memory>
#include <vector>
#include <cstdint>
#include <optional>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "../chip8/chip8.hpp"

class UI {
private:
    struct PrivateTag {};
    static std::optional<UI> instance;
    
    std::vector<uint32_t> frame_buffer;
    SDL_Window* sdl_window = nullptr;
    SDL_Renderer* sdl_renderer = nullptr;
    SDL_Texture* sdl_texture = nullptr;
    Chip8* chip8 = nullptr;

    int width = 0;
    int height = 0;
    int zoom = 2;
    int run_n_steps = 0;

public:
    UI(const UI&) = delete;
    UI& operator=(const UI&) = delete;
    UI(UI&&) = default;
    UI& operator=(UI&&) = default;

    UI(PrivateTag, const char* title, int w, int h, int zoom, Chip8& chip8) : chip8(&chip8), zoom(zoom), width(w), height(h) {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) throw std::runtime_error("Failed to initialize SDL");
        sdl_window = SDL_CreateWindow(title, w * zoom, h * zoom, 0);
        sdl_renderer = SDL_CreateRenderer(sdl_window, NULL);
        sdl_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);
        if (!sdl_window || !sdl_renderer || !sdl_texture) {
            throw std::runtime_error("Failed to create SDL window, renderer, or texture");
        }
        width = w;
        height = h;
        zoom = zoom;
        frame_buffer.assign(static_cast<size_t>(width) * height, 0xFF000000u);
    }

    ~UI() {
        if (sdl_texture) SDL_DestroyTexture(sdl_texture);
        if (sdl_renderer) SDL_DestroyRenderer(sdl_renderer);
        if (sdl_window) SDL_DestroyWindow(sdl_window);
        SDL_Quit();
    }

    static UI& create(const char* title, int w, int h, int zoom, Chip8& chip8) {
        if (instance) throw std::runtime_error("UI instance already exists. Call destroy() first.");
        instance.emplace(PrivateTag{}, title, w, h, zoom, chip8);
        return *instance;
    }

    static void destroy() {
        instance.reset();
    }

    static UI& get() {
        if (!instance) throw std::runtime_error("UI instance does not exist. Call create() first.");
        return *instance;
    }

    static bool exists() {
        return instance.has_value();
    }

    void display() {
        if (!sdl_texture || frame_buffer.empty()) return;
        void* pix = nullptr;
        int pitch = 0;
        if (SDL_LockTexture(sdl_texture, NULL, &pix, &pitch) != 0) return;
        uint32_t* dst = static_cast<uint32_t*>(pix);
        for (int row = 0; row < height; ++row) {
            for (int col = 0; col < width; ++col) {
                dst[row * pitch + col] = frame_buffer[static_cast<size_t>(row / zoom) * width + (col / zoom)] | 0xFF000000u;
            }
        }
        SDL_UnlockTexture(sdl_texture);
        SDL_RenderTexture(sdl_renderer, sdl_texture, NULL, NULL);
        SDL_RenderPresent(sdl_renderer);
    }

    bool loop() {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                chip8->quit();
                return false;
            }
            if (e.type == SDL_EVENT_KEY_UP) {
                switch (e.key.key) {
                    case SDLK_ESCAPE:
                        chip8->quit();
                        return false;
                    case SDLK_SPACE:
                        run_n_steps = 1;
                        break;
                    case SDLK_KP_ENTER:
                        if (run_n_steps == 0) run_n_steps = -1;
                        if (run_n_steps < 0) run_n_steps = 0;
                        break;
                }
            }
        }
        if (run_n_steps != 0) {
            chip8->step(frame_buffer, width, height);
            run_n_steps -= run_n_steps > 0;
        }
        return true;
    }

    void run() {
        while (loop()) {
            display();
        }
    }
};

inline std::optional<UI> UI::instance;

#endif