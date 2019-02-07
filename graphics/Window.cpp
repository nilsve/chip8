//
// Created by Nils van Eijk on 24-10-18.
//

#include "Window.h"

#include <sfml/Window/Keyboard.hpp>

#include <cmath>
#include <map>

namespace chip8::graphics {
    chip8::graphics::Window::Window() {
        using namespace std;
        using namespace sf;

        window = make_unique<sf::RenderWindow>(VideoMode(WIDTH * PIXEL_SIZE, HEIGHT * PIXEL_SIZE), "Chip8 VM");

        const auto pixelSize = sf::Vector2((float)PIXEL_SIZE, (float)PIXEL_SIZE);

        for (unsigned int x = 0; x < WIDTH; x++) {
            for (unsigned int y = 0; y < HEIGHT; y++) {
                auto& pixel = pixelData[x][y];
                const auto pos = sf::Vector2((float)PIXEL_SIZE * x, (float)PIXEL_SIZE * y);
                pixel.setPosition(pos);
                pixel.setSize(pixelSize);
                pixel.setActive(false);
            }
        }
    }

    // Returns true if pixel gets deactivated
    bool Window::flipPixel(unsigned int x, unsigned int y, bool active) {
        x = x % WIDTH;
        y = y % HEIGHT;
        bool currentlyActive = pixelData[x][y].getActive();
        bool shouldBeActive = active != currentlyActive;
        pixelData[x][y].setActive(shouldBeActive);
        return !shouldBeActive && currentlyActive != shouldBeActive;
    }

    bool chip8::graphics::Window::Update() {
        if (!window->isOpen()) {
            return false;
        }
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window->pollEvent(event)) {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window->close();
        }

        // clear the window with black color
        window->clear(sf::Color::Black);

        Draw();

        window->display();
        return true;
    }

    void Window::Draw() {
        for (unsigned int x = 0; x < WIDTH; x++) {
            for (unsigned int y = 0; y < HEIGHT; y++) {
                window->draw(pixelData[x][y].getPixel());
            }
        }
    }

    void Window::clearScreen() {
        for (unsigned int x = 0; x < WIDTH; x++) {
            for (unsigned int y = 0; y < HEIGHT; y++) {
                pixelData[x][y].setActive(false);
            }
        }
    }

    bool Window::isKeyPressed(unsigned char keyCode) {
        static std::map<unsigned char, sf::Keyboard::Key> keyMap = {
                {0x1, sf::Keyboard::Num1},{0x2, sf::Keyboard::Num2},{0x3, sf::Keyboard::Num3},{0xc, sf::Keyboard::Num4},
                {0x4, sf::Keyboard::Q},{0x5, sf::Keyboard::W},{0x6, sf::Keyboard::E},{0xd, sf::Keyboard::R},
                {0x7, sf::Keyboard::A},{0x8, sf::Keyboard::S},{0x9, sf::Keyboard::D},{0xe, sf::Keyboard::F},
                {0xa, sf::Keyboard::Z},{0x0, sf::Keyboard::X},{0xb, sf::Keyboard::C},{0xf, sf::Keyboard::V},
        };

        return sf::Keyboard::isKeyPressed(keyMap[keyCode]);
    }
}
