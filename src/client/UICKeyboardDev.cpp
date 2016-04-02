#include "UICKeyboardDev.h"

#include <uis/device/keysym.h>
#include <uis/device/kbd-layout.h>

extern "C" {
uint16_t keycode_to_symcode(uint32_t keycode, int shift) {
	uint16_t k = 0;

	switch (keycode) {
		case KEYCODE_TAB:
			k = XK_Tab;
			break;
		case KEYCODE_CLEAR:
			k = XK_Clear;
			break;
		case KEYCODE_ENTER:
			k = XK_Return;
			break;

		case KEYCODE_ESCAPE:
		case KEYCODE_BACK:
			k = XK_Escape;
			break;
		case KEYCODE_SPACE:
			k = XK_space;
			break;
		case KEYCODE_DEL:
			k = XK_BackSpace;
			break;
			// case SDLK_KP0: k = XK_KP_0; break;
			// case SDLK_KP1: k = XK_KP_1; break;
			// case SDLK_KP2: k = XK_KP_2; break;
			// case SDLK_KP3: k = XK_KP_3; break;
			// case SDLK_KP4: k = XK_KP_4; break;
			// case SDLK_KP5: k = XK_KP_5; break;
			// case SDLK_KP6: k = XK_KP_6; break;
			// case SDLK_KP7: k = XK_KP_7; break;
			// case SDLK_KP8: k = XK_KP_8; break;
			// case SDLK_KP9: k = XK_KP_9; break;
			// case SDLK_KP_PERIOD: k = XK_KP_Decimal; break;
			// case SDLK_KP_DIVIDE: k = XK_KP_Divide; break;
			// case SDLK_KP_MULTIPLY: k = XK_KP_Multiply; break;
			// case SDLK_KP_MINUS: k = XK_KP_Subtract; break;
			// case SDLK_KP_PLUS: k = XK_KP_Add; break;
			// case SDLK_KP_ENTER: k = XK_KP_Enter; break;
			// case SDLK_KP_EQUALS: k = XK_KP_Equal; break;
		case KEYCODE_DPAD_UP:
			k = XK_Up;
			break;
		case KEYCODE_DPAD_DOWN:
			k = XK_Down;
			break;
		case KEYCODE_DPAD_RIGHT:
			k = XK_Right;
			break;
		case KEYCODE_DPAD_LEFT:
			k = XK_Left;
			break;

		case KEYCODE_HOME:
			k = XK_Home;
			break;

		case KEYCODE_ENDCALL:
			k = XK_End;
			break;
		case KEYCODE_PAGE_UP:
			k = XK_Page_Up;
			break;
		case KEYCODE_PAGE_DOWN:
			k = XK_Page_Down;
			break;
			/*
			   case KEYCODE_F1:
			   k = XK_F1;
			   break;
			   case KEYCODE_F2:
			   k = XK_F2;
			   break;
			   case KEYCODE_F3:
			   k = XK_F3;
			   break;
			   case KEYCODE_F4:
			   k = XK_F4;
			   break;
			   case KEYCODE_F5:
			   k = XK_F5;
			   break;
			   case KEYCODE_F6:
			   k = XK_F6;
			   break;
			   case KEYCODE_F7:
			   k = XK_F7;
			   break;
			   case KEYCODE_F8:
			   k = XK_F8;
			   break;
			   case KEYCODE_F9:
			   k = XK_F9;
			   break;
			   case KEYCODE_F10:
			   k = XK_F10;
			   break;
			   case KEYCODE_F11:
			   k = XK_F11;
			   break;
			   case KEYCODE_F12:
			   k = XK_F12;
			   break;
			   case KEYCODE_NUM_LOCK:
			   k = XK_Num_Lock;
			   break;
			   case KEYCODE_CAPS_LOCK:
			   k = XK_Caps_Lock;
			   break;
			   case KEYCODE_SCROLL_LOCK:
			   k = XK_Scroll_Lock;
			   break;
			 */
		case KEYCODE_SHIFT_RIGHT:
			k = XK_Shift_R;
			break;
		case KEYCODE_SHIFT_LEFT:
			k = XK_Shift_L;
			break;
			/*
			   case KEYCODE_CTRL_RIGHT:
			   k = XK_Control_R;
			   break;
			   case KEYCODE_CTRL_LEFT:
			   k = XK_Control_L;
			   break;
			 */
		case KEYCODE_ALT_RIGHT:
			k = XK_Alt_R;
			break;
		case KEYCODE_ALT_LEFT:
			k = XK_Alt_L;
			break;
			/*
			   case KEYCODE_META_RIGHT:
			   k = XK_Meta_R;
			   break;
			   case KEYCODE_META_LEFT:
			   k = XK_Meta_L;
			   break;
			 */

		case KEYCODE_WINDOW:
			k = XK_Super_L;
			break; 

		case KEYCODE_MENU:
			k = XK_Menu;
			break; 

		case KEYCODE_SEARCH:
			k = XK_Find;
			break; 

			// case SDLK_RSUPER: k = XK_RSuper; break; /* right "windows" key */
			// case SDLK_COMPOSE: k = XK_Compose; break;

			// case SDLK_MODE: k = XK_Mode_switch; break;
			// case SDLK_HELP: k = XK_Help; break;
			// case SDLK_PRINT: k = XK_Print; break;
			// case SDLK_SYSREQ: k = XK_Sys_Req; break;
			// case SDLK_BREAK: k = XK_Break; break;
		default:
			if (keycode >= KEYCODE_A && keycode <= KEYCODE_Z) {
				k = XK_a + keycode - KEYCODE_A;
				if (shift)
					k &= ~0x20;
			}
			break;
	}

	if (k == 0) {
		if (keycode < 0x100)
			k = keycode;
		else
			LOG_WARN ("cannot translate keycode:%u", keycode);
	}

	LOG_DEBUG ("translate keycode %u to %u", keycode, k);
	return k;
}
}
