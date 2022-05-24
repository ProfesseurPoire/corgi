#include "Keyboard.h"

namespace corgi
{

    Keyboard::Keyboard()
    {
        for(int i = 0; i < 130; i++)
        {
            mKeysUp[i]   = false;
            mKeysDown[i] = false;
            mKeys[i]     = false;
        }
    }
    void Keyboard::update()
    {
        keyPressed_.clear();

        for(int i = 0; i < 130; i++)
        {
            mKeysUp[i]   = false;
            mKeysDown[i] = false;
        }
    }
    const Vector<Key>& Keyboard::keyPressed() const { return keyPressed_; }

    int  Keyboard::getKeyModifiers() const { return mKeyModifiers; }
    bool Keyboard::getModifierState(const KeyModifiers keyModifiers) const
    {
        return ((mKeyModifiers & static_cast<int>(keyModifiers)) != 0);
    }

    bool Keyboard::key(Key k) const { return mKeys[static_cast<int>(k)]; }

    bool Keyboard::key_down(Key k) const { return mKeysDown[static_cast<int>(k)]; }

    bool Keyboard::key_up(Key k) const { return mKeysUp[static_cast<int>(k)]; }

    bool Keyboard::is_key_pressed() const { return _key_pressed_count > 0; }

}    // namespace corgi