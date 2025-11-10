#include "frontend.h"
#include "logic.h"
#include <SDL.h>
#include <stdio.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_memory_editor.h"

SDL_Window* window = NULL;

SDL_Renderer* renderer = NULL;

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

bool quit = false;

void frontendInit() {
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        throw;
    }

    #ifdef SDL_HINT_IME_SHOW_UI
        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
    #endif

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, window_flags);
    if(window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        throw;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        throw;
    }


    // Set up ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    
    

}

void frontendUpdate() {

    ImGuiIO&io = ImGui::GetIO();
    (void)io;

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL2_ProcessEvent(&e);
        if (e.type == SDL_QUIT) quit = true;
    }


    

    

    // New ImGui Frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
    SDL_RenderClear(renderer);


    if (state.screenUpdate == true) {
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 64; x++) {
                SDL_Rect rect;
                rect.x = x * SCALE_FACTOR;
                rect.y = y * SCALE_FACTOR;
                rect.w = SCALE_FACTOR;
                rect.h = SCALE_FACTOR;
                if (state.displayBuffer[y][x] == true) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderFillRect(renderer, &rect);
                } else {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }
    }

    // Memory Editor window
    static MemoryEditor mem_edit_1;
    mem_edit_1.DrawWindow("Memory Editor", (void*)state.ram.data(), RAM_SIZE);
    mem_edit_1.HighlightMin = state.programCounter;
    mem_edit_1.HighlightMax = state.programCounter + 0x2;

    // Emulation control window
    ImGui::Begin("Emulation Control");
    if (ImGui::Button("Start/Stop"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        state.running = !state.running;
    ImGui::BeginDisabled(state.running);
    if (ImGui::Button("Step")) {
        
        execute();
    }
    if (ImGui::Button("Reset")) {
        state.running = false;
        state = systemState();
    }
    if (ImGui::Button("Load ROM")) {
        loadIntoRAM(state.ram, "IBM Logo.ch8", 0x200);
    }
    
    ImGui::EndDisabled();

    ImGui::End();

    // Emulation info window
    ImGui::Begin("Emulation info");
    ImGui::Text("Instruction:  %04x", state.currentInstruction);
    ImGui::Text("Program Counter:  %04x", state.programCounter);
    ImGui::Text("Cycle count: %d", state.cycleCount);
    ImGui::Text("Index Register:  %04x", state.indexRegister);
    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    ImGui::Text("Registers:");
    for (int i = 0; i <= 0xF; i++) {
        ImGui::Text("%02x: %04x", i , state.registers[i]);
    }
    ImGui::End();

    // Render
    ImGui::Render();

    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
}


void frontendClose() {
    //Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();
}