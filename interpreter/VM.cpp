//
// Created by Nils van Eijk on 23-10-18.
//

#include "VM.h"
#include <unistd.h>
#include <chrono>

namespace chip8::interpreter {
    using namespace std::chrono;

    ReturnCode VM::Run() {

        while (true) {
            if (!window.Update()) {
                return 0;
            }

            unsigned short data = *reinterpret_cast<unsigned short*>(&memory[registers.PC]);
            auto hi = getHi(data);
            unsigned char instruction = hi >> 4;

            milliseconds currentTime = duration_cast< milliseconds >(
                    system_clock::now().time_since_epoch()
            );

            if (currentTime.count() - lastUpdate.count() > 16) {
                if (registers.DELAY > 0) {
                    registers.DELAY--;
                }

                if (registers.SOUND > 0) {
                    registers.SOUND--;
                }

                lastUpdate = currentTime;
            }

//            unsigned char instruction = getInstruction(data);

            bool increaseInstructionCounter = true;

            switch (instruction) {
                case 0x00: {
                    if (getY(data) == 0x0e) {
                        if (getZ(data) == 0x0e) {
                            // return
                            registers.PC = registers.stack[registers.SP];
                            registers.SP--;
                        } else if (getZ(data) == 0) {
                            // cls
                            window.clearScreen();
                        }
                    } else {
                    }
                    break;
                }
                case 0x01: { // Gecheckt
                    // JMP
                    registers.PC = getXYZ(data);
                    increaseInstructionCounter = false;
                    break;
                }
                case 0x02: {
                    // Call
                    unsigned short address = getXYZ(data);
                    registers.SP++;
                    registers.stack[registers.SP] = registers.PC;
                    registers.PC = address;
                    increaseInstructionCounter = false;
                    break;
                }

                case 0x03: {
                    unsigned char val = registers.V[getX(data)];
                    if (val == getLo(data)) {
                        registers.PC += 4;
                        increaseInstructionCounter = false;
                    }
                    break;
                }

                case 0x04: {
                    unsigned char val = registers.V[getX(data)];
                    if (val != getLo(data)) {
                        registers.PC += 4;
                        increaseInstructionCounter = false;
                    }
                    break;
                }
                case 0x05: {
                    unsigned char x = registers.V[getX(data)];
                    unsigned char y = registers.V[getY(data)];

                    if (x == y) {
                        registers.PC += 4;
                        increaseInstructionCounter = false;
                    }

                    break;
                }
                case 0x06: { // Gecheckt
                    unsigned char reg = getX(data);
                    registers.V[reg] = getLo(data);
                    break;
                }
                case 0x07: { // Gecheckt
                    unsigned char reg = getX(data);
                    registers.V[reg] += getLo(data);

                    break;
                }
                case 0x08: {
                    unsigned char instructionNum = getZ(data);

                    unsigned char x = getX(data);
                    unsigned char y = getY(data);

                    switch (instructionNum) {
                        case 0:
                            registers.V[y] = registers.V[x];
                            break;
                        case 1:
                            registers.V[x] = registers.V[x] | registers.V[y];
                            break;
                        case 2:
                            registers.V[x] = registers.V[x] & registers.V[y];
                            break;
                        case 3:
                            registers.V[x] = registers.V[x] ^ registers.V[y];
                            break;
                        case 4:
                            registers.V[0xF] = (((unsigned int)registers.V[x] + (unsigned int)registers.V[y]) > 255);
                            registers.V[x] += registers.V[y];
                            break;
                        case 5:
                            registers.V[0xF] = (((unsigned int)registers.V[x] + (unsigned int)registers.V[y]) > 255);
                            registers.V[x] -= registers.V[y];
                            break;
                        case 6:
                            registers.V[0xF] = registers.V[x] & 0xF;
                            registers.V[x] /= 2;
                        case 7:
                            registers.V[0xF] = registers.V[y] > registers.V[x];
                            registers.V[x] = registers.V[y] - registers.V[x];
                        case 0xe:
                            registers.V[0xF] = registers.V[x] << sizeof(unsigned char) * 8;
                            registers.V[x] *= 2;
                            break;
                    }
                    break;
                }
                case 0x09: {
                    if (registers.V[getX(data) != registers.V[getY(data)]]) {
                        registers.PC += 4;
                        increaseInstructionCounter = false;
                    }
                    break;
                }
                case 0x0a: { // Gecheckt
                    registers.I = getXYZ(data);
                    break;
                }
                case 0x0b: {
                    registers.PC = registers.V[0] + getXYZ(data);
                    break;
                }
                case 0x0c: {
                    registers.V[getX(data)] = rand() % (0xFF + 1) & getLo(data);
                    break;
                }
                case 0x0d: {
                    unsigned char startX = registers.V[getX(data)];
                    unsigned char startY = registers.V[getY(data)];
                    unsigned char height = getZ(data);

                    bool deactivatedPixel = false;

                    for (unsigned int y = 0; y < height; y++) {
                        unsigned char sprite = memory[registers.I + y];
                        for (unsigned int bitPos = 0; bitPos < 8; bitPos++) {
                            bool active = (sprite & (1 << 7 - bitPos)) != 0;
                            if (active) {
                                active = true;
                            }
                            if (window.flipPixel(startX + bitPos, startY + y, active)) {
                                deactivatedPixel = true;
                            }
                        }
                    }

                    registers.V[0xF] = deactivatedPixel;

                    break;
                }
                case 0x0e: {
                    auto y = getY(data);
                    switch (y) {
                        case 9: {
                            auto key  = registers.V[getX(data)];
                            if (window.isKeyPressed(key)) {
                                registers.PC += 4;
                                increaseInstructionCounter = false;
                            }
                            break; // TODO
                        }
                        case 0x0a: {
                            auto key  = registers.V[getX(data)];
                            if (!window.isKeyPressed(key)) {
                                registers.PC += 4;
                                increaseInstructionCounter = false;
                            }
                            break;
                        }
                    }
                    break;
                }
                case 0x0f: {
                    auto x = getX(data);
                    auto y = getY(data);
                    auto z = getZ(data);

                    switch (y) {
                        case 0: {
                            switch (z) {
                                case 0x07:
                                    registers.V[x] = registers.DELAY;
                                    break;
                                case 0x0a:
                                    utils::Utils::Log("erm");
                                    break;
                            }
                            break;
                        }
                        case 1: {
                            switch (z) {
                                case 0x05: {
                                    registers.DELAY = registers.V[x];
                                    break;
                                }
                                case 0x08: {
                                    registers.SOUND = registers.V[x];
                                    break;
                                }
                                case 0x0e: {
                                    registers.I += registers.V[getX(data)];
                                    break;
                                }
                            }
                            break;
                        }
                        case 2: {
                            // TODO
                            auto sprite = registers.V[getX(data)];
                            registers.I = sprite * 5;
                            break;
                        }
                        case 3: {
                            // BCD formatter
                            auto val = registers.V[getX(data)];
                            unsigned short hundreds = val / 100;
                            unsigned short tens = (val - hundreds) / 10;
                            unsigned short ones = (val - hundreds - tens);

                            memory[registers.I] = hundreds;
                            memory[registers.I + 1] = tens;
                            memory[registers.I + 2] = ones;

                            break;
                        }
                        case 5: {
                            auto len = getX(data);
                            for (auto i = 0; i <= len; i++) {
                                memory[registers.I + i] = registers.V[registers.I + i];
                            }
//                            registers.I += len;
                            break;
                        }
                        case 6: {
                            auto len = getX(data);
                            for (auto i = 0; i <= len; i++) {
                                registers.V[i] = memory[registers.I + i];
                            }
//                            registers.I += len;
                            break;
                        }
                        default: {
                            utils::Utils::Log("niet geimplementeerd");
                            break;
                        }
                    }

                    break;
                }
                default: {
                    break;
                }
            }

            if (increaseInstructionCounter) {
                registers.PC += 2;
            }
        }
    }
}