#include "logic.h"
#include "instructions.h"
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <random>

void loadIntoRAM(std::array<uint8_t, RAM_SIZE> &ram, std::string inputFile, uint16_t startAddress) {
    std::ifstream inFile(inputFile, std::ios_base::binary);
    std::vector<uint8_t> buffer;
    // Get size of file
    inFile.seekg(0, inFile.end);
    size_t length = inFile.tellg();
    inFile.seekg(0, inFile.beg);
    if (length > 0) {
        buffer.resize(length);
        inFile.read(reinterpret_cast<char*>(buffer.data()), length);
    }
    for (size_t i = 0; i < buffer.size(); ++i) {
        ram[startAddress + i] = buffer[i];
    }
}

std::string getHexString(uint8_t hex) {
    std::stringstream ss;
    ss << std::hex << hex;
    return ss.str();
}
std::string getHexString(uint16_t hex) {
    std::stringstream ss;
    ss << std::hex << hex;
    return ss.str();
}
std::string getHexString(int hex) {
    std::stringstream ss;
    ss << std::hex << hex;
    return ss.str();
}

systemState state;

std::random_device dev;
std::mt19937 rng(dev());
std::uniform_int_distribution<std::mt19937::result_type> dist8b(0,255);

void execute() {

    bool jump = false; // Overrides incrementing the PC at the end if true

    uint16_t instByteOne = state.ram[state.programCounter];
    state.currentInstruction = (instByteOne << 8) | state.ram[state.programCounter + 0x01];


    uint8_t fn = (state.currentInstruction & 0b1111000000000000) >> 12;
    uint8_t X = (state.currentInstruction & 0b0000111100000000) >> 8;
    uint8_t Y = (state.currentInstruction & 0b0000000011110000) >> 4;
    uint8_t N = (state.currentInstruction & 0b0000000000001111);
    uint8_t NN = (state.currentInstruction & 0b0000000011111111);
    uint16_t NNN = (state.currentInstruction & 0b0000111111111111);

    uint8_t tempFlag = 0x0;

    switch(fn) {

        case 0x0:
            if (NNN == 0x0E0) {
                // 00E0 (CLS)
                ins_CLS(&state);
            } else if (NNN == 0x0EE) {
                // 00EE (RET)
                state.programCounter = state.stack.front();
                state.stack.pop_front();
            }
            break;
        
        case 0x1:
            // 1NNN (JP)
            state.programCounter = NNN;
            jump = true;
            break;

        case 0x2:
            // 2NNN (CALL)
            state.stack.push_front(state.programCounter);
            state.programCounter = NNN;
            jump = true;
            break;

        case 0x3:
            // 3XNN (SE)
            if (state.registers[X] == NN) {
                state.programCounter += 0x2;
            }
            break;

        case 0x4:
            // 4XNN (SNE)
            if (state.registers[X] != NN) {
                state.programCounter += 0x2;
            }
            break;

        case 0x5:
            // 5XY0 (SE)
            if (state.registers[X] == state.registers[Y]) {
                state.programCounter += 0x2;
            }
            break;

        case 0x6:
            // 6XNN (LD)
            state.registers[X] = NN;
            break;

        case 0x7:
            // 7XNN (ADD)
            state.registers[X] = (state.registers[X] + NN) % 256;
            break;

        case 0x8:
            switch (N)
            {
            case 0x0:
                // 8XY0 (LD)
                state.registers[X] = state.registers[Y];
                break;
            case 0x1:
                // 8XY1 (OR)
                state.registers[X] = state.registers[X] | state.registers[Y];
                break;
            case 0x2:
                // 8XY2 (AND)
                state.registers[X] = state.registers[X] & state.registers[Y];
                break;
            case 0x3:
                // 8XY3 (XOR)
                state.registers[X] = state.registers[X] ^ state.registers[Y];
                break;
            case 0x4:
                // 8XY4 (ADD)
                if ((state.registers[X] + state.registers[Y]) > 0xFF) {
                    tempFlag = 0x1;
                } else {
                    tempFlag = 0x0;
                }
                state.registers[X] = static_cast<uint8_t>(state.registers[X] + state.registers[Y]);
                state.registers[0xF] = tempFlag;
                break;
            case 0x5:
                // 8XY5 (SUB)
                if (state.registers[X] >= state.registers[Y]) {
                    tempFlag = 0x1;
                } else {
                    tempFlag = 0x0;
                }
                state.registers[X] -= state.registers[Y];
                state.registers[0xF] = tempFlag;
                break;
            case 0x6:
                // 8XY6 (SHR)
                tempFlag = state.registers[X] & 0b00000001;
                state.registers[X] = state.registers[X] / 2;
                state.registers[0xF] = tempFlag;
                break;
            case 0x7:
                // 8XY7 (SUBN)
                if (state.registers[Y] >= state.registers[X]) {
                    tempFlag = 0x1;
                } else {
                    tempFlag = 0x0;
                }
                state.registers[X] = state.registers[Y] - state.registers[X];
                state.registers[0xF] = tempFlag;
                break;
            case 0xE:
                // 8XYE (SHL)
                tempFlag = (state.registers[X] & 0b10000000) >> 7;
                state.registers[X] = state.registers[X] * 2;
                state.registers[0xF] = tempFlag;
                break;
            default:
                break;
            }
            break;

        case 0x9:
            // 9XY0 (SNE)
            if (state.registers[X] != state.registers[Y]) {
                state.programCounter += 0x2;
            }
            break;

        case 0xA:
            // ANNN (LD)
            state.indexRegister = NNN;
            break;

        case 0xB:
            // BNNN
            state.programCounter = NNN + state.registers[0x0];
            jump = true;
            break;

        case 0xC:
            // CXNN (RND)
            state.registers[X] = dist8b(rng) & NN;
            break;

        case 0xD:
            ins_DRW(&state, &X, &Y, &N);
            break;

        case 0xF:
            switch (NN) {
                case 0x07:
                    // FX07 (LD VX, DT)
                    state.registers[X] = state.delayTimer;
                    break;
                case 0x15:
                    // FX15 (LD DT, VX)
                    state.delayTimer = state.registers[X];
                    break;
                case 0x18:
                    // FX18 (LD ST, VX)
                    state.soundTimer = state.registers[X];
                    break;
                case 0x1E:
                    // FX1E (ADD I, VX)
                    state.indexRegister += state.registers[X];
                    break;
                case 0x33:
                    // FX33 (LD B, VX)
                    state.ram[state.indexRegister]       =  state.registers[X] / 100;
                    state.ram[state.indexRegister + 0x1] = ((state.registers[X] % 100) - (state.registers[X] % 10)) / 10;
                    state.ram[state.indexRegister + 0x2] =  state.registers[X] % 10;
                    break;
                case 0x55:
                    // FX55 (LD [I], VX)
                    for (int index = 0; index <= X; index++) {
                        state.ram[state.indexRegister + index] = state.registers[index];
                    }
                    break;
                case 0x65:
                    // FX65 (LD VX, [I])
                    for (int index = 0; index <= X; index++) {
                        state.registers[index] = state.ram[state.indexRegister + index];
                    }
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }

    if (!jump) {
        state.programCounter += 2;
    }

    state.cycleCount += 1;

}