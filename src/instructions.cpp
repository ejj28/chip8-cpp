#include "logic.h"

void ins_CLS(systemState* state) {
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            state->displayBuffer[y][x] = false;
        }
    }
    state->screenUpdate = true;
}


void ins_DRW(systemState* state, uint8_t* X, uint8_t* Y, uint8_t* N) {
    uint8_t xCoord = state->registers[*X] & 63;
    uint8_t yCoord = state->registers[*Y] & 31;
    state->registers[0xF] = 0x0;
    
    for (int row = 0; row < *N; row++) {
        int count = 7;
        
        if (yCoord >= 32) {
            break;
        }

        uint16_t addr = row + state->indexRegister;
        uint8_t data = state->ram[addr];
        
        uint8_t rowXCoord = xCoord;
        for (int column = 0; column < 8; column++) {
            
            uint8_t pixel = (data >> count) & 0b00000001;
            
            if (xCoord >= 64) {
                break;
            }
            if (pixel == 1) {
                
                if (state->displayBuffer[yCoord][rowXCoord] == true) {
                    state->registers[0xF] = 1;
                    state->displayBuffer[yCoord][rowXCoord] = false;
                } else {
                    state->displayBuffer[yCoord][rowXCoord] = true;
                }
            }
            count -= 1;
            rowXCoord++;
        }
        yCoord++;
    }
    state->screenUpdate = true;
}