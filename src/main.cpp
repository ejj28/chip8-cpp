#include <SDL.h>
#include "frontend.h"
#include "logic.h"
#include <stdio.h>

int main(int argc, char* args[]) {
    printf("starting init\n");
    frontendInit();

    printf("main loop start\n");
    // Main loop
    while (quit == false) {

        if (state.running) {
            // Emulation logic
            execute();
        }
        
        frontendUpdate();
    }
    printf("closing\n");
    frontendClose();

    return 0;
}