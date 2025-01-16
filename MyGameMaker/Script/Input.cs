using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;


namespace HawkEngine
{
    class Input
    {
        UIntPtr CplusplusInstance; /* ptr to C++ instance */

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool GetKey(int keyID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool GetKeyDown(int keyID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool GetKeyUp(int keyID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool GetMouseButton(int buttonID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool GetMouseButtonDown(int buttonID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool GetMouseButtonUp(int buttonID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static int GetAxis(string axisName);


    }

    public enum KeyCode
    {
        A = 4,
        B = 5,
        C = 6,
        D = 7,
        E = 8,
        F = 9,
        G = 10,
        H = 11,
        I = 12,
        J = 13,
        K = 14,
        L = 15,
        M = 16,
        N = 17,
        O = 18,
        P = 19,
        Q = 20,
        R = 21,
        S = 22,
        T = 23,
        U = 24,
        V = 25,
        W = 26,
        X = 27,
        Y = 28,
        Z = 29,

        Alpha1 = 30,
        Alpha2 = 31,
        Alpha3 = 32,
        Alpha4 = 33,
        Alpha5 = 34,
        Alpha6 = 35,
        Alpha7 = 36,
        Alpha8 = 37,
        Alpha9 = 38,
        Alpha0 = 39,

        RETURN = 40,
        ESCAPE = 41,
        BACKSPACE = 42,
        TAB = 43,
        SPACE = 44,

        MINUS = 45,
        EQUALS = 46,
        LEFTBRACKET = 47,
        RIGHTBRACKET = 48,
        BACKSLASH = 49,
        NONUSHASH = 50,
        SEMICOLON = 51,
        APOSTROPHE = 52,
        GRAVE = 53,
        COMMA = 54,
        PERIOD = 55,
        SLASH = 56,

        CAPSLOCK = 57,

        F1 = 58,
        F2 = 59,
        F3 = 60,
        F4 = 61,
        F5 = 62,
        F6 = 63,
        F7 = 64,
        F8 = 65,
        F9 = 66,
        F10 = 67,
        F11 = 68,
        F12 = 69,

        PRINTSCREEN = 70,
        SCROLLLOCK = 71,
        PAUSE = 72,
        INSERT = 73,

        HOME = 74,
        PAGEUP = 75,
        DELETE = 76,
        END = 77,
        PAGEDOWN = 78,
        RIGHT = 79,
        LEFT = 80,
        DOWN = 81,
        UP = 82,

        NUMLOCKCLEAR = 83,
    }
}