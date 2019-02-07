#include <iostream>

#include "interpreter/VM.h"
#include "utils/Utils.h"

using namespace chip8;

int main() {
    auto data = utils::Utils::readChip8Bin(L"/Users/nils/CLionProjects/chip8/games/Tetris [Fran Dachille, 1991].ch8");

    interpreter::VM interpreter(data);

    interpreter.Run();

    return 0;
}