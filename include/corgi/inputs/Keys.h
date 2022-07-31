#pragma once

namespace corgi
{
    enum class Key : int
    {
        Backspace    = '\b',    // 8
        Return       = '\r',    // 13
        Space        = ' ',     // 32
        Key0         = '0',     // 48
        Key1         = '1',
        Key2         = '2',
        Key3         = '3',
        Key4         = '4',
        Key5         = '5',
        Key6         = '6',
        Key7         = '7',
        Key8         = '8',
        Key9         = '9',
        Colon        = ':',
        SemiColon    = ';',
        Less         = '<',
        Equals       = '=',
        Greater      = '>',
        QuestionMark = '?',
        At           = '@',    //64

        LeftBracket  = '[',     //91
        Backlash     = '\\',    //92
        RightBracket = ']',     //93
        Caret        = '^',     //94
        Underscore   = '_',     //95
        Backquoter   = '`',     //96
        A            = 'a',     //97
        B            = 'b',
        C            = 'c',
        D            = 'd',
        E            = 'e',
        F            = 'f',
        G            = 'g',
        H            = 'h',
        I            = 'i',
        J            = 'j',
        K            = 'k',
        L            = 'l',
        M            = 'm',
        N            = 'n',
        O            = 'o',
        P            = 'p',
        Q            = 'q',
        R            = 'r',
        S            = 's',
        T            = 't',
        U            = 'u',
        V            = 'v',
        W            = 'w',
        X            = 'x',
        Y            = 'y',
        Z            = 'z',    //122

        LeftCtrl  = 123,
        LeftShift = 124,
        CapsLock  = 125,
        Left,
        Right,
        Up,
        Down

        //F5 = 33
    };

    // Mapped SDL Values
    enum class KeyModifiers : int
    {
        None     = 0b0,
        LSHIFT   = 0b1,
        RSHIFT   = 0b10,
        LCTRL    = 0b1000000,
        RCTRL    = 0b10000000,
        LALT     = 0b100000000,
        RALT     = 0b1000000000,
        LSPECIAL = 0b10000000000,
        RSPECIAL = 0b100000000000,
        NUM      = 0b1000000000000,
        CAPS     = 0b10000000000000,
        CTRL     = 0b11000000,
        SHIFT    = 0b11,
        ALT      = 0b1100000000,
        SPECIAL  = 0b110000000000
    };
}    // namespace corgi
