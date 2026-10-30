#ifndef CONTROLS_H
#define CONTROLS_H

#include <SFML/Window/Keyboard.hpp>

struct Controls
{
    struct Keyboard
    {
        sf::Keyboard::Key up;
        sf::Keyboard::Key right;
        sf::Keyboard::Key down;
        sf::Keyboard::Key left;

        explicit constexpr Keyboard(
            sf::Keyboard::Key const _up,
            sf::Keyboard::Key const _right,
            sf::Keyboard::Key const _down,
            sf::Keyboard::Key const _left):
                up(_up), right(_right), down(_down), left(_left) {}
    } keyboard;

    constexpr Controls(Keyboard const &_keyboard):
        keyboard(_keyboard) {}
};

#endif // CONTROLS_H