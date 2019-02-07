//
// Created by Nils van Eijk on 23-10-18.
//

#ifndef CHIP8_TEST_H
#define CHIP8_TEST_H

#include "../graphics/Window.h"
#include "../utils/Utils.h"

#include <memory>
#include <vector>
#include <err.h>
#include <chrono>

#define MEMORY_SIZE 0xFFF
#define PROGRAM_START 0x200

namespace chip8::interpreter {

    struct Registers {
        unsigned short PC; // Program counter / Instruction pointer
        unsigned short SP; // Stack pointer

        unsigned short I;
        unsigned char DELAY;
        unsigned char SOUND;

        std::vector<unsigned char> V;
        std::vector<unsigned short> stack;

        Registers() : PC(0), SP(0), I(0), DELAY(0), SOUND(0) {
            stack.resize(0xf);
            V.resize(0xf);
        }
    };

    typedef unsigned int ReturnCode;
    class VM {

    private:
        Registers registers;
        std::vector<unsigned char> memory;
        graphics::Window window;

        std::chrono::milliseconds lastUpdate = std::chrono::milliseconds::zero();

        static inline unsigned char getLo(unsigned short data) { return (data >> 8) & 0xFF; };
        static inline unsigned char getHi(unsigned short data) { return data & 0xFF; };
        static inline unsigned char getInstruction(unsigned short data) { return (getHi(data) & 0xF0) >> 4; };
        static inline unsigned char getX(unsigned short data) { return getHi(data) & 0x0F; };
        static inline unsigned char getY(unsigned short data) { return (getLo(data) & 0xF0) >> 4; };
        static inline unsigned char getZ(unsigned short data) { return getLo(data) & 0x0F; };

        typedef union {
            unsigned short val;
            unsigned char bytes[2];
        } short_bytes;

        static inline unsigned short getXYZ(unsigned short data) {
            auto xyz = data & 0x0fff;
            auto hi = getHi(data) & 0x0f;
            auto lo = getLo(data);

            short_bytes res;
            res.bytes[0] = lo;
            res.bytes[1] = hi;
            return res.val;
        }
    public:
        explicit VM(std::vector<unsigned char> &_data) {
            if (_data.size() > MEMORY_SIZE) {
                utils::Utils::throwError("Application too large!");
            }

            static unsigned char chip8_fontset[] =
            {
                0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                0x20, 0x60, 0x20, 0x20, 0x70, // 1
                0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                0xF0, 0x80, 0xF0, 0x80, 0x80  // F
            };

            registers.PC = PROGRAM_START;

            memory.resize(MEMORY_SIZE);

            std::copy(_data.begin(), _data.end(), memory.begin() + PROGRAM_START);
            memcpy(&memory[0], &chip8_fontset[0], sizeof(chip8_fontset));
        }

        ReturnCode Run();
    };
}


#endif //CHIP8_VM_H
