#pragma once

#include <corgi/inputs/Keys.h>

#include <vector>

namespace corgi
{
class Window;

/*!
	 * @brief	Stores the keyboard state
	 *
	 *			Use key_down and key up will only trigger once, while key while trigger
	 *			every frame the key has been hold down
	 */
class Keyboard
{
public:
    // Friends

    friend class Inputs;
    friend class Window;
    friend class Game;

    // Lifecycle
    //
    // Since I don't want the user to accidentally copy the keyboard
    // everything is deleted

    Keyboard(const Keyboard& other) = delete;
    Keyboard(Keyboard&& other)      = delete;

    Keyboard& operator=(const Keyboard& other) = delete;
    Keyboard& operator=(Keyboard&& other)      = delete;

    // Functions

    /*!
    	 * @brief	Returns true if the key is being pressed
    	 */
    [[nodiscard]] bool key(Key k) const;

    /*!
    	 * @brief	Returns true the frame the key was pressed
    	 */
    [[nodiscard]] bool key_down(Key k) const;

    /*!
    	 * @brief	Returns true the frame the key was released
    	 */
    [[nodiscard]] bool key_up(Key k) const;

    /*!
    	 * @brief	Returns true if a key was pressed between the current and previous frame
    	 */
    [[nodiscard]] bool is_key_pressed() const;

    /**
         * @brief Get the Key Modifiers
         * 
         *        Use || and && operators with the return value
         * 
         * @return int 
         */
    int getKeyModifiers() const;

    /**
         * @brief Checks the state of the given modifier
         * 
         *        KeyModifiers are Ctrl/Alt/Num/Caps/Special/Shift
         * 
         * @param keyModifier 
         * @return true 
         * @return false 
         */
    [[nodiscard]] bool getModifierState(KeyModifiers keyModifier) const;

    [[nodiscard]] const std::vector<Key>& keyPressed() const;

private:
    // Lifecycle
    Keyboard();
    ~Keyboard() = default;

    // Functions
    void initKey(Key k);
    void update();

    // It's just a bit convenient for now to actually do that
    // The actual problem is that I have really high integer values
    // for things like left ctrl, F1-12 keys etc

    // I could have only vectors
    // a std::vector<int> with the key value

    std::vector<Key> keyPressed_;

    int mKeyModifiers;
    int _key_pressed_count {0};

    // We use 512 here because that's the size of SDL_ScanCode

    bool mKeys[512];
    bool mKeysDown[512];
    bool mKeysUp[512];
};
}    // namespace corgi
