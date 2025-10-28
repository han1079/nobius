#ifndef KEYCODES_H
#define KEYCODES_H

#include <cstdint>
#include <Qt>
#include <unordered_map>
#include <type_traits>

enum KeyCodes : uint16_t {
    KEY_SPACE =             32,
    KEY_APOSTROPHE =        39,  /* ' */
    KEY_COMMA =             44,  /* , */
    KEY_MINUS =             45,  /* - */
    KEY_PERIOD =            46,  /* . */
    KEY_SLASH =             47,  /* / */
    KEY_0 =                 48,
    KEY_1 =                 49,
    KEY_2 =                 50,
    KEY_3 =                 51,
    KEY_4 =                 52,
    KEY_5 =                 53,
    KEY_6 =                 54,
    KEY_7 =                 55,
    KEY_8 =                 56,
    KEY_9 =                 57,
    KEY_SEMICOLON =         59,  /* ; */
    KEY_EQUAL =             61,  /* = */
    KEY_A =                 65,
    KEY_B =                 66,
    KEY_C =                 67,
    KEY_D =                 68,
    KEY_E =                 69,
    KEY_F =                 70,
    KEY_G =                 71,
    KEY_H =                 72,
    KEY_I =                 73,
    KEY_J =                 74,
    KEY_K =                 75,
    KEY_L =                 76,
    KEY_M =                 77,
    KEY_N =                 78,
    KEY_O =                 79,
    KEY_P =                 80,
    KEY_Q =                 81,
    KEY_R =                 82,
    KEY_S =                 83,
    KEY_T =                 84,
    KEY_U =                 85,
    KEY_V =                 86,
    KEY_W =                 87,
    KEY_X =                 88,
    KEY_Y =                 89,
    KEY_Z =                 90,
    KEY_LEFT_BRACKET =      91,  /* [ */
    KEY_BACKSLASH =         92,  /* \ */
    KEY_RIGHT_BRACKET =     93,  /* ] */
    KEY_GRAVE_ACCENT =      96,  /* ` */
    KEY_WORLD_1 =           161, /* non-US #1 */
    KEY_WORLD_2 =           162, /* non-US #2 */

    /*Function keys */
    KEY_ESCAPE =            256,
    KEY_ENTER =             257,
    KEY_TAB =               258,
    KEY_BACKSPACE =         259,
    KEY_INSERT =            260,
    KEY_DELETE =            261,
    KEY_RIGHT =             262,
    KEY_LEFT =              263,
    KEY_DOWN =              264,
    KEY_UP =                265,
    KEY_PAGE_UP =           266,
    KEY_PAGE_DOWN =         267,
    KEY_HOME =              268,
    KEY_END =               269,
    KEY_CAPS_LOCK =         280,
    KEY_SCROLL_LOCK =       281,
    KEY_NUM_LOCK =          282,
    KEY_PRINT_SCREEN =      283,
    KEY_PAUSE =             284,
    KEY_F1 =                290,
    KEY_F2 =                291,
    KEY_F3 =                292,
    KEY_F4 =                293,
    KEY_F5 =                294,
    KEY_F6 =                295,
    KEY_F7 =                296,
    KEY_F8 =                297,
    KEY_F9 =                298,
    KEY_F10 =               299,
    KEY_F11 =               300,
    KEY_F12 =               301,
    KEY_F13 =               302,
    KEY_F14 =               303,
    KEY_F15 =               304,
    KEY_F16 =               305,
    KEY_F17 =               306,
    KEY_F18 =               307,
    KEY_F19 =               308,
    KEY_F20 =               309,
    KEY_F21 =               310,
    KEY_F22 =               311,
    KEY_F23 =               312,
    KEY_F24 =               313,
    KEY_F25 =               314,
    KEY_KP_0 =              320,
    KEY_KP_1 =              321,
    KEY_KP_2 =              322,
    KEY_KP_3 =              323,
    KEY_KP_4 =              324,
    KEY_KP_5 =              325,
    KEY_KP_6 =              326,
    KEY_KP_7 =              327,
    KEY_KP_8 =              328,
    KEY_KP_9 =              329,
    KEY_KP_DECIMAL =        330,
    KEY_KP_DIVIDE =         331,
    KEY_KP_MULTIPLY =       332,
    KEY_KP_SUBTRACT =       333,
    KEY_KP_ADD =            334,
    KEY_KP_ENTER =          335,
    KEY_KP_EQUAL =          336,
    KEY_LEFT_SHIFT =        340,
    KEY_LEFT_CONTROL =      341,
    KEY_LEFT_ALT =          342,
    KEY_LEFT_SUPER =        343,
    KEY_RIGHT_SHIFT =       344,
    KEY_RIGHT_CONTROL =     345,
    KEY_RIGHT_ALT =         346,
    KEY_RIGHT_SUPER =       347,
    KEY_MENU =              348,
    KEY_LAST =              KEY_MENU
};

enum KeyMods : uint16_t {
    MOD_SHIFT =          0x0001,
    MOD_CONTROL =        0x0002,
    MOD_ALT =            0x0004,
    MOD_SUPER =          0x0008,
    MOD_CAPS_LOCK =      0x0010,
    MOD_NUM_LOCK =       0x0020
};

enum MouseCodes : uint16_t {
    MOUSE_BUTTON_1 =        0,
    MOUSE_BUTTON_2 =        1,
    MOUSE_BUTTON_3 =        2,
    MOUSE_BUTTON_4 =        3,
    MOUSE_BUTTON_5 =        4,
    MOUSE_BUTTON_6 =        5,
    MOUSE_BUTTON_7 =        6,
    MOUSE_BUTTON_8 =        7,
    MOUSE_BUTTON_LAST =     MOUSE_BUTTON_8,
    MOUSE_BUTTON_LEFT =     MOUSE_BUTTON_1,
    MOUSE_BUTTON_RIGHT =    MOUSE_BUTTON_2,
    MOUSE_BUTTON_MIDDLE =   MOUSE_BUTTON_3
};

inline const std::unordered_map<Qt::Key, KeyCodes> qt_to_keycode_map = {
    {Qt::Key_Space, KEY_SPACE},
    {Qt::Key_Apostrophe, KEY_APOSTROPHE},
    {Qt::Key_Comma, KEY_COMMA},
    {Qt::Key_Minus, KEY_MINUS},
    {Qt::Key_Period, KEY_PERIOD},
    {Qt::Key_Slash, KEY_SLASH},
    {Qt::Key_0, KEY_0},
    {Qt::Key_1, KEY_1},
    {Qt::Key_2, KEY_2},
    {Qt::Key_3, KEY_3},
    {Qt::Key_4, KEY_4},
    {Qt::Key_5, KEY_5},
    {Qt::Key_6, KEY_6},
    {Qt::Key_7, KEY_7},
    {Qt::Key_8, KEY_8},
    {Qt::Key_9, KEY_9},
    {Qt::Key_Semicolon, KEY_SEMICOLON},
    {Qt::Key_Equal, KEY_EQUAL},
    {Qt::Key_A, KEY_A},
    {Qt::Key_B, KEY_B},
    {Qt::Key_C, KEY_C},
    {Qt::Key_D, KEY_D},
    {Qt::Key_E, KEY_E},
    {Qt::Key_F, KEY_F},
    {Qt::Key_G, KEY_G},
    {Qt::Key_H, KEY_H},
    {Qt::Key_I, KEY_I},
    {Qt::Key_J, KEY_J},
    {Qt::Key_K, KEY_K},
    {Qt::Key_L, KEY_L},
    {Qt::Key_M, KEY_M},
    {Qt::Key_N, KEY_N},
    {Qt::Key_O, KEY_O},
    {Qt::Key_P, KEY_P},
    {Qt::Key_Q, KEY_Q},
    {Qt::Key_R, KEY_R},
    {Qt::Key_S, KEY_S},
    {Qt::Key_T, KEY_T},
    {Qt::Key_U, KEY_U},
    {Qt::Key_V, KEY_V},
    {Qt::Key_W, KEY_W},
    {Qt::Key_X, KEY_X},
    {Qt::Key_Y, KEY_Y},
    {Qt::Key_Z, KEY_Z},
    {Qt::Key_Escape, KEY_ESCAPE},
    {Qt::Key_Enter, KEY_ENTER},
    {Qt::Key_Tab, KEY_TAB},
    {Qt::Key_Backspace, KEY_BACKSPACE},
    {Qt::Key_Insert, KEY_INSERT},
    {Qt::Key_Delete, KEY_DELETE},
    {Qt::Key_Right, KEY_RIGHT},
    {Qt::Key_Left, KEY_LEFT},
    {Qt::Key_Down, KEY_DOWN},
    {Qt::Key_Up, KEY_UP},
    {Qt::Key_PageUp, KEY_PAGE_UP},
    {Qt::Key_PageDown, KEY_PAGE_DOWN},
    {Qt::Key_Home, KEY_HOME},
    {Qt::Key_End, KEY_END},
    {Qt::Key_CapsLock, KEY_CAPS_LOCK},
    {Qt::Key_ScrollLock, KEY_SCROLL_LOCK},
    {Qt::Key_NumLock, KEY_NUM_LOCK},
    {Qt::Key_Print, KEY_PRINT_SCREEN},
    {Qt::Key_Pause, KEY_PAUSE},
    {Qt::Key_F1, KEY_F1},
    {Qt::Key_F2, KEY_F2},
    {Qt::Key_F3, KEY_F3},
    {Qt::Key_F4, KEY_F4},
    {Qt::Key_F5, KEY_F5},
    {Qt::Key_F6, KEY_F6},
    {Qt::Key_F7, KEY_F7},
    {Qt::Key_F8, KEY_F8},
    {Qt::Key_F9, KEY_F9},
    {Qt::Key_F10, KEY_F10},
    {Qt::Key_F11, KEY_F11},
    {Qt::Key_F12, KEY_F12},
    {Qt::Key_F13, KEY_F13}
};

inline const std::unordered_map<Qt::KeyboardModifier, KeyMods> qt_to_keymod_map = {
    {Qt::ShiftModifier, MOD_SHIFT},
    {Qt::ControlModifier, MOD_CONTROL},
    {Qt::AltModifier, MOD_ALT},
    {Qt::MetaModifier, MOD_SUPER},
    {Qt::KeypadModifier, MOD_NUM_LOCK}
};

inline const std::unordered_map<Qt::MouseButton, MouseCodes> qt_to_mousecode_map = {
    {Qt::LeftButton, MOUSE_BUTTON_LEFT},
    {Qt::RightButton, MOUSE_BUTTON_RIGHT},
    {Qt::MiddleButton, MOUSE_BUTTON_MIDDLE},
    {Qt::XButton1, MOUSE_BUTTON_4},
    {Qt::XButton2, MOUSE_BUTTON_5}
};

template<typename QtType, typename ReturnType>
ReturnType qt2code(QtType qt_val, ReturnType default_val){
    if constexpr (std::is_same_v<QtType, Qt::Key> && std::is_same_v<ReturnType, KeyCodes>) {
        auto it = qt_to_keycode_map.find(qt_val);
        if (it != qt_to_keycode_map.end()) {
            return it->second;
        }
    } else if constexpr (std::is_same_v<QtType, Qt::KeyboardModifier> && std::is_same_v<ReturnType, KeyMods>) {
        auto it = qt_to_keymod_map.find(qt_val);
        if (it != qt_to_keymod_map.end()) {
            return it->second;
        }
    } else if constexpr (std::is_same_v<QtType, Qt::MouseButton> && std::is_same_v<ReturnType, MouseCodes>) {
        auto it = qt_to_mousecode_map.find(qt_val);
        if (it != qt_to_mousecode_map.end()) {
            return it->second;
        }
    }
    return default_val;
}




#endif //KEYCODES_H
