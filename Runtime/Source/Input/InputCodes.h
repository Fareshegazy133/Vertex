// Vertex

#pragma once

#include "raylib.h"

namespace VInput
{
using VKeyCode = KeyboardKey;
using VMouseButtonCode = MouseButton;

struct VKey
{
	VKey() = delete;
	
	// Null
	static constexpr VKeyCode Null = KEY_NULL;
	
	// Letters
	static constexpr VKeyCode A = KEY_A;
	static constexpr VKeyCode B = KEY_B;
    static constexpr VKeyCode C = KEY_C;
    static constexpr VKeyCode D = KEY_D;
    static constexpr VKeyCode E = KEY_E;
    static constexpr VKeyCode F = KEY_F;
    static constexpr VKeyCode G = KEY_G;
    static constexpr VKeyCode H = KEY_H;
    static constexpr VKeyCode I = KEY_I;
    static constexpr VKeyCode J = KEY_J;
    static constexpr VKeyCode K = KEY_K;
    static constexpr VKeyCode L = KEY_L;
    static constexpr VKeyCode M = KEY_M;
    static constexpr VKeyCode N = KEY_N;
    static constexpr VKeyCode O = KEY_O;
    static constexpr VKeyCode P = KEY_P;
    static constexpr VKeyCode Q = KEY_Q;
    static constexpr VKeyCode R = KEY_R;
    static constexpr VKeyCode S = KEY_S;
    static constexpr VKeyCode T = KEY_T;
    static constexpr VKeyCode U = KEY_U;
    static constexpr VKeyCode V = KEY_V;
    static constexpr VKeyCode W = KEY_W;
    static constexpr VKeyCode X = KEY_X;
    static constexpr VKeyCode Y = KEY_Y;
    static constexpr VKeyCode Z = KEY_Z;

    // Numbers
    static constexpr VKeyCode Zero = KEY_ZERO;
    static constexpr VKeyCode One = KEY_ONE;
    static constexpr VKeyCode Two = KEY_TWO;
    static constexpr VKeyCode Three = KEY_THREE;
    static constexpr VKeyCode Four = KEY_FOUR;
    static constexpr VKeyCode Five = KEY_FIVE;
    static constexpr VKeyCode Six = KEY_SIX;
    static constexpr VKeyCode Seven = KEY_SEVEN;
    static constexpr VKeyCode Eight = KEY_EIGHT;
    static constexpr VKeyCode Nine = KEY_NINE;

    // Function keys
    static constexpr VKeyCode F1 = KEY_F1;
    static constexpr VKeyCode F2 = KEY_F2;
    static constexpr VKeyCode F3 = KEY_F3;
    static constexpr VKeyCode F4 = KEY_F4;
    static constexpr VKeyCode F5 = KEY_F5;
    static constexpr VKeyCode F6 = KEY_F6;
    static constexpr VKeyCode F7 = KEY_F7;
    static constexpr VKeyCode F8 = KEY_F8;
    static constexpr VKeyCode F9 = KEY_F9;
    static constexpr VKeyCode F10 = KEY_F10;
    static constexpr VKeyCode F11 = KEY_F11;
    static constexpr VKeyCode F12 = KEY_F12;

    // Control
    static constexpr VKeyCode Space = KEY_SPACE;
    static constexpr VKeyCode Escape = KEY_ESCAPE;
    static constexpr VKeyCode Enter = KEY_ENTER;
    static constexpr VKeyCode Tab = KEY_TAB;
    static constexpr VKeyCode Backspace = KEY_BACKSPACE;
    static constexpr VKeyCode Insert = KEY_INSERT;
    static constexpr VKeyCode Delete = KEY_DELETE;

    // Modifiers
    static constexpr VKeyCode LeftShift = KEY_LEFT_SHIFT;
    static constexpr VKeyCode RightShift = KEY_RIGHT_SHIFT;
    static constexpr VKeyCode LeftControl = KEY_LEFT_CONTROL;
    static constexpr VKeyCode RightControl = KEY_RIGHT_CONTROL;
    static constexpr VKeyCode LeftAlt = KEY_LEFT_ALT;
    static constexpr VKeyCode RightAlt = KEY_RIGHT_ALT;

    // Arrow keys
    static constexpr VKeyCode Up = KEY_UP;
    static constexpr VKeyCode Down = KEY_DOWN;
    static constexpr VKeyCode Left = KEY_LEFT;
    static constexpr VKeyCode Right = KEY_RIGHT;

    // Navigation
    static constexpr VKeyCode Home = KEY_HOME;
    static constexpr VKeyCode End = KEY_END;
    static constexpr VKeyCode PageUp = KEY_PAGE_UP;
    static constexpr VKeyCode PageDown = KEY_PAGE_DOWN;

    // Numpad
    static constexpr VKeyCode KP0 = KEY_KP_0;
    static constexpr VKeyCode KP1 = KEY_KP_1;
    static constexpr VKeyCode KP2 = KEY_KP_2;
    static constexpr VKeyCode KP3 = KEY_KP_3;
    static constexpr VKeyCode KP4 = KEY_KP_4;
    static constexpr VKeyCode KP5 = KEY_KP_5;
    static constexpr VKeyCode KP6 = KEY_KP_6;
    static constexpr VKeyCode KP7 = KEY_KP_7;
    static constexpr VKeyCode KP8 = KEY_KP_8;
    static constexpr VKeyCode KP9 = KEY_KP_9;

    static constexpr VKeyCode KPDecimal = KEY_KP_DECIMAL;
    static constexpr VKeyCode KPDivide = KEY_KP_DIVIDE;
    static constexpr VKeyCode KPMultiply = KEY_KP_MULTIPLY;
    static constexpr VKeyCode KPSubtract = KEY_KP_SUBTRACT;
    static constexpr VKeyCode KPAdd = KEY_KP_ADD;
    static constexpr VKeyCode KPEnter = KEY_KP_ENTER;
};

struct VMouseButton
{
	VMouseButton() = delete;

	static constexpr VMouseButtonCode Left = MOUSE_BUTTON_LEFT;
	static constexpr VMouseButtonCode Right = MOUSE_BUTTON_RIGHT;
	static constexpr VMouseButtonCode Middle = MOUSE_BUTTON_MIDDLE;
	static constexpr VMouseButtonCode Side = MOUSE_BUTTON_SIDE;
	static constexpr VMouseButtonCode Extra = MOUSE_BUTTON_EXTRA;
	static constexpr VMouseButtonCode Forward = MOUSE_BUTTON_FORWARD;
	static constexpr VMouseButtonCode Back = MOUSE_BUTTON_BACK;
};
}
