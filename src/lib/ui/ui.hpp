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
#include <unordered_map>

const std::unordered_map<uint8_t, uint8_t> key_map = {
    {SDLK_X, 0x0}, {SDLK_1, 0x1}, {SDLK_2, 0x2}, {SDLK_3, 0x3},
    {SDLK_Q, 0x4}, {SDLK_W, 0x5}, {SDLK_E, 0x6}, {SDLK_A, 0x7},
    {SDLK_S, 0x8}, {SDLK_D, 0x9}, {SDLK_Z, 0xA}, {SDLK_C, 0xB},
    {SDLK_4, 0xC}, {SDLK_R, 0xD}, {SDLK_F, 0xE}, {SDLK_V, 0xF}
};

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
    int run_n_steps = 0;
    uint16_t keydown = 0x0000;

public:
    UI(const UI&) = delete;
    UI& operator=(const UI&) = delete;
    UI(UI&&) = default;
    UI& operator=(UI&&) = default;

    UI(PrivateTag, const char* title, int w, int h, Chip8& chip8) : chip8(&chip8), width(w), height(h) {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) throw std::runtime_error("Failed to initialize SDL");
        sdl_window = SDL_CreateWindow(title, w, h, 0);
        sdl_renderer = SDL_CreateRenderer(sdl_window, NULL);
        sdl_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);
        if (!sdl_window || !sdl_renderer || !sdl_texture) {
            throw std::runtime_error("Failed to create SDL window, renderer, or texture");
        }
        frame_buffer.assign(static_cast<size_t>(width) * height, 0xFF000000u);
    }

    ~UI() {
        if (sdl_texture) SDL_DestroyTexture(sdl_texture);
        if (sdl_renderer) SDL_DestroyRenderer(sdl_renderer);
        if (sdl_window) SDL_DestroyWindow(sdl_window);
        SDL_Quit();
    }

    static UI& create(const char* title, int w, int h, Chip8& chip8) {
        if (instance) throw std::runtime_error("UI instance already exists. Call destroy() first.");
        instance.emplace(PrivateTag{}, title, w, h, chip8);
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

        char* pix;
        int pitch;
        SDL_LockTexture(sdl_texture, NULL, (void**)&pix, &pitch);
        for (int i = 0, sp = 0, dp = 0; i < height; i++, dp += width, sp += pitch)
            memcpy(pix + sp, frame_buffer.data() + dp, width * 4);

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
                        std::cerr << "Stepping one instruction\n";
                        run_n_steps = 1;
                        break;
                    case SDLK_RETURN:
                        if (run_n_steps == 0) {
                            std::cerr << "Toggle running\n";
                            run_n_steps = -1;
                        } else if (run_n_steps < 0) {
                            std::cerr << "Pausing execution\n";
                            run_n_steps = 0;
                        }
                        break;
                    default:
                        auto it = key_map.find(e.key.key);
                        if (it != key_map.end()) keydown &= ~(1 << it->second);
                        break;
                }
            }
            if (e.type == SDL_EVENT_KEY_DOWN) {
                auto it = key_map.find(e.key.key);
                if (it != key_map.end()) keydown |= (1 << it->second);
            }
        }
        if (run_n_steps != 0) {
            chip8->step(frame_buffer, width, height, keydown);
            run_n_steps -= run_n_steps > 0;
        }
        SDL_Delay(1); // Prevent CPU hogging
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