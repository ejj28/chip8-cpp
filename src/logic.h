#pragma once
#include <array>
#include <list>
#include <string>

const uint16_t START_ADDR = 0x200;

constexpr size_t RAM_SIZE = 4096;

struct systemState {

    // 4096 bytes of RAM
    std::array<uint8_t, RAM_SIZE> ram;

    // 8-bit timers
    uint8_t delayTimer = 0x00;
    uint8_t soundTimer = 0x00;

    // Stack for 12-bit addresses
    std::list<uint16_t> stack;

    // This is technically only 12 bits, since the entirety of the RAM is addressable using only 12 bits
    uint16_t programCounter = START_ADDR;

    // 12-bit index register
    uint16_t indexRegister = 0x000;

    // 16 8-bit general purpose registers
    std::array<uint8_t, 16> registers;

    // Array for 16 keys
    std::array<bool, 16> keys;

    // Display buffer
    std::array<std::array<bool, 64>, 32> displayBuffer;

    // Normal execution
    bool running = false;

    uint16_t currentInstruction = 0x0000;

    // Does the screen need to be re-rendered
    bool screenUpdate = true;

    // Total number of cycles
    uint32_t cycleCount = 0;

};

extern systemState state;


void loadIntoRAM(std::array<uint8_t, RAM_SIZE> &ram, std::string inputFile, uint16_t startAddress);

void execute();