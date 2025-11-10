#include "logic.h"
#include "instructions.h"
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

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

    switch(fn) {

        case 0x0:
            if (NNN == 0x0E0) {
                ins_CLS(&state);
            }
            break;
        
        case 0x1:
            // 1NNN (jump)
            state.programCounter = NNN;
            jump = true;
            break;

        case 0x6:
            state.registers[X] = NN;
            break;

        case 0x7:
            state.registers[X] = (state.registers[X] + NN) % 256;
            break;

        case 0xA:
            state.indexRegister = NNN;
            break;

        case 0xD:
            ins_DRW(&state, &X, &Y, &N);
            break;

        default:
            break;
    }

    if (!jump) {
        state.programCounter += 2;
    }

    state.cycleCount += 1;

}