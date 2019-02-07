//
// Created by Nils van Eijk on 24-10-18.
//

#ifndef CHIP8_GRAPHICS_H
#define CHIP8_GRAPHICS_H

#include <sfml/Graphics.hpp>
#include "../utils/non_copyable.h"
#include <memory>

namespace chip8::graphics {

    class Pixel : public utils::non_copyable {
    private:
        sf::RectangleShape pixel;
        bool active;
    public:
        void setSize(sf::Vector2<float> size) {
            pixel.setSize(size);
        }

        void setPosition(sf::Vector2<float> position) {
            pixel.setPosition(position);
        }

        bool getActive() {
            return active;
        }

        void setActive(bool active) {
            this->active = active;
            if (active) {
                pixel.setFillColor(sf::Color::White);
            } else {
                pixel.setFillColor(sf::Color::Black);
            }
        }

        const sf::RectangleShape &getPixel() const {
            return pixel;
        }
    };

    class Window : public utils::non_copyable {
    private:
        std::unique_ptr<sf::RenderWindow> window;

        static const unsigned int WIDTH = 64;
        static const unsigned int HEIGHT = 32;
        static const unsigned int PIXEL_SIZE = 15;

        Pixel pixelData[WIDTH][HEIGHT];

    public:
        Window();
        void Draw();
        bool Update();
        bool isKeyPressed(unsigned char keyCode);

        bool flipPixel(unsigned int x, unsigned int y, bool active);
        void clearScreen();
    };

}


#endif //CHIP8_GRAPHICS_H
