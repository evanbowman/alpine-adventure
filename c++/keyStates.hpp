#include <SFML/Window.hpp>
#include <array>

class KeyStates {
public:
    KeyStates() {
        for (auto & state : states_) {
            state = false;
        }
    }

    void pressed(sf::Keyboard::Key k) { states_[static_cast<int>(k)] = true; }

    void released(sf::Keyboard::Key k) { states_[static_cast<int>(k)] = false; }

    bool isPressed(sf::Keyboard::Key k) { return states_[static_cast<int>(k)]; }

private:
    std::array<bool, sf::Keyboard::Key::KeyCount> states_;
};
