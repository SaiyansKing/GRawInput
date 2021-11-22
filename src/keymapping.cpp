#include "keymapping.h"

#include <map>

std::map<USHORT, DWORD> VK_Keys_Map = {
	{VK_CANCEL, 0},//??
	{VK_BACK, 0x0E},
	{VK_TAB, 0x0F},
	{VK_CLEAR, 0},//??
	{VK_RETURN, 0x1C},
	{VK_SHIFT, 0x2A},//??
	{VK_LSHIFT, 0x2A},
	{VK_RSHIFT, 0x36},
	{VK_CONTROL, 0x1D},//??
	{VK_LCONTROL, 0x1D},
	{VK_RCONTROL, 0x9D},
	{VK_MENU, 0x38},//??
	{VK_LMENU, 0x38},
	{VK_RMENU, 0xB8},
	{VK_PAUSE, 0xC5},
	{VK_CAPITAL, 0x3A},
	{VK_KANA, 0x70},
	{VK_HANGEUL, 0},//??
	{VK_HANGUL, 0},//??
	{VK_HANGUL, 0},//??
	{VK_IME_ON, 0},//??
	{VK_JUNJA, 0},//??
	{VK_FINAL, 0},//??
	{VK_HANJA, 0},//??
	{VK_KANJI, 0x94},
	{VK_IME_OFF, 0},//??
	{VK_ESCAPE, 0x01},
	{VK_CONVERT, 0x79},
	{VK_NONCONVERT, 0x7B},
	{VK_ACCEPT, 0},//??
	{VK_MODECHANGE, 0},//??
	{VK_SPACE, 0x39},
	{VK_PRIOR, 0xC9},
	{VK_NEXT, 0xD1},
	{VK_END, 0xCF},
	{VK_HOME, 0xC7},
	{VK_LEFT, 0xCB},
	{VK_UP, 0xC8},
	{VK_RIGHT, 0xCD},
	{VK_DOWN, 0xD0},
	{VK_SELECT, 0},//??
	{VK_PRINT, 0},//??
	{VK_EXECUTE, 0},//??
	{VK_SNAPSHOT, 0},//??
	{VK_INSERT, 0xD2},
	{VK_DELETE, 0xD3},
	{VK_HELP, 0},//??
	{0x30, 0x0B},
	{0x31, 0x02},
	{0x32, 0x03},
	{0x33, 0x04},
	{0x34, 0x05},
	{0x35, 0x06},
	{0x36, 0x07},
	{0x37, 0x08},
	{0x38, 0x09},
	{0x39, 0x0A},
	{0x41, 0x1E},
	{0x42, 0x30},
	{0x43, 0x2E},
	{0x44, 0x20},
	{0x45, 0x12},
	{0x46, 0x21},
	{0x47, 0x22},
	{0x48, 0x23},
	{0x49, 0x17},
	{0x4A, 0x24},
	{0x4B, 0x25},
	{0x4C, 0x26},
	{0x4D, 0x32},
	{0x4E, 0x31},
	{0x4F, 0x18},
	{0x50, 0x19},
	{0x51, 0x10},
	{0x52, 0x13},
	{0x53, 0x1F},
	{0x54, 0x14},
	{0x55, 0x16},
	{0x56, 0x2F},
	{0x57, 0x11},
	{0x58, 0x2D},
	{0x59, 0x15},
	{0x5A, 0x2C},
	{VK_LWIN, 0xDB},
	{VK_RWIN, 0xDC},
	{VK_APPS, 0xDD},
	{VK_SLEEP, 0xDF},
	{VK_NUMPAD0, 0x52},
	{VK_NUMPAD1, 0x4F},
	{VK_NUMPAD2, 0x50},
	{VK_NUMPAD3, 0x51},
	{VK_NUMPAD4, 0x4B},
	{VK_NUMPAD5, 0x4C},
	{VK_NUMPAD6, 0x4D},
	{VK_NUMPAD7, 0x47},
	{VK_NUMPAD8, 0x48},
	{VK_NUMPAD9, 0x49},
	{VK_MULTIPLY, 0x37},
	{VK_ADD, 0x4E},
	{VK_SEPARATOR, 0},//??
	{VK_SUBTRACT, 0x4A},
	{VK_DECIMAL, 0x53},
	{VK_DIVIDE, 0xB5},
	{VK_F1, 0x3B},
	{VK_F2, 0x3C},
	{VK_F3, 0x3D},
	{VK_F4, 0x3E},
	{VK_F5, 0x3F},
	{VK_F6, 0x40},
	{VK_F7, 0x41},
	{VK_F8, 0x42},
	{VK_F9, 0x43},
	{VK_F10, 0x44},
	{VK_F11, 0x57},
	{VK_F12, 0x58},
	{VK_F13, 0x64},
	{VK_F14, 0x65},
	{VK_F15, 0x66},
	{VK_F16, 0},//??
	{VK_F17, 0},//??
	{VK_F18, 0},//??
	{VK_F19, 0},//??
	{VK_F20, 0},//??
	{VK_F21, 0},//??
	{VK_F22, 0},//??
	{VK_F23, 0},//??
	{VK_F24, 0},//??
	{VK_NUMLOCK, 0x45},
	{VK_SCROLL, 0x46},
	{VK_BROWSER_BACK, 0xEA},
	{VK_BROWSER_FORWARD, 0xE9},
	{VK_BROWSER_REFRESH, 0xE7},
	{VK_BROWSER_STOP, 0xE8},
	{VK_BROWSER_SEARCH, 0xE5},
	{VK_BROWSER_FAVORITES, 0xE6},
	{VK_BROWSER_HOME, 0xB2},
	{VK_VOLUME_MUTE, 0xA0},
	{VK_VOLUME_DOWN, 0xAE},
	{VK_VOLUME_UP, 0xB0},
	{VK_MEDIA_NEXT_TRACK, 0x99},
	{VK_MEDIA_PREV_TRACK, 0x90},
	{VK_MEDIA_STOP, 0xA4},
	{VK_MEDIA_PLAY_PAUSE, 0xA2},
	{VK_LAUNCH_MAIL, 0xEC},
	{VK_LAUNCH_MEDIA_SELECT, 0xED},
	{VK_LAUNCH_APP1, 0},//??
	{VK_LAUNCH_APP2, 0},//??
	{VK_OEM_1, 0x27},
	{VK_OEM_PLUS, 0x0D},
	{VK_OEM_COMMA, 0x33},
	{VK_OEM_MINUS, 0x0C},
	{VK_OEM_PERIOD, 0x34},
	{VK_OEM_2, 0x35},
	{VK_OEM_3, 0x29},
	{VK_OEM_4, 0x1A},
	{VK_OEM_5, 0x2B},
	{VK_OEM_6, 0x1B},
	{VK_OEM_7, 0x28},
	{VK_OEM_8, 0},//??
	{VK_OEM_AX, 0x96},
	{VK_OEM_102, 0x56},
	{VK_PROCESSKEY, 0},//??
	{VK_PACKET, 0},//??
	{0xE8, 0x9C},
	{VK_ATTN, 0},//??
	{VK_CRSEL, 0},//??
	{VK_EXSEL, 0},//??
	{VK_EREOF, 0},//??
	{VK_PLAY, 0},//??
	{VK_ZOOM, 0},//??
	{VK_NONAME, 0},//??
	{VK_PA1, 0},//??
	{VK_OEM_CLEAR, 0},//??
};

// Unsupported
#define DIK_ABNT_C1         0x73    /* /? on Brazilian keyboard */
#define DIK_YEN             0x7D    /* (Japanese keyboard)            */
#define DIK_ABNT_C2         0x7E    /* Numpad . on Brazilian keyboard */
#define DIK_NUMPADEQUALS    0x8D    /* = on numeric keypad (NEC PC98) */
#define DIK_AT              0x91    /*                     (NEC PC98) */
#define DIK_COLON           0x92    /*                     (NEC PC98) */
#define DIK_UNDERLINE       0x93    /*                     (NEC PC98) */
#define DIK_STOP            0x95    /*                     (NEC PC98) */
#define DIK_UNLABELED       0x97    /*                        (J3100) */
#define DIK_CALCULATOR      0xA1    /* Calculator */
#define DIK_NUMPADCOMMA     0xB3    /* , on numeric keypad (NEC PC98) */
#define DIK_SYSRQ           0xB7
#define DIK_POWER           0xDE    /* System Power */
#define DIK_SLEEP           0xDF    /* System Sleep */
#define DIK_WAKE            0xE3    /* System Wake */
#define DIK_MYCOMPUTER      0xEB    /* My Computer */
