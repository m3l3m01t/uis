#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <linux/input.h>

#include <uis/device/kbd-dev.h>
#include <uis/device/kbd-layout.h>
#include <uis/device/keysym.h>
#include <uis/debug.h>

typedef struct {
	uint32_t l_shift:1;
	uint32_t r_shift:1;
	uint32_t l_alt:1;
	uint32_t r_alt:1;
	uint32_t l_ctrl:1;
	uint32_t r_ctrl:1;
	uint32_t capslock:1;
} KbdState;

typedef struct {
	BaseEvDev devh;
	KbdState st;
} KbdDev;

BaseEvDev *kbd_dev_new (EvDev *evdev) {
	if ((ev_dev_get_class (evdev) & INPUT_DEVICE_CLASS_KEYBOARD) == 0) {
		return NULL;
	}

	KbdDev *bed = (KbdDev *)malloc (sizeof (KbdDev));
	bed->devh.evdev = ev_dev_ref (evdev);

/*
	if (evdev->data == NULL) {
		KbdState *st = (KbdState *) malloc (sizeof (KbdState));
		memset (st, 0, sizeof (KbdState));
		evdev->data = st;
	}
*/

	return (BaseEvDev *)bed;
}

int kbd_dev_ctrl (BaseEvDev *bed, int32_t cmd, ...) {
	KeySymMsg *msg;
	va_list ap;
	KbdDev *dev = (KbdDev *)bed;

	va_start (ap, cmd);

	switch (cmd) {
		case KEYSYM_SET:
			msg = va_arg (ap, KeySymMsg *);
			break;
	}
	va_end (ap);

	switch (cmd) {
		case KEYSYM_SET:
			for (int i = 0; i < msg->count; i++) {
				KeySym *sym = msg->data + i;
			}
			break;
		default:
			LOG_ERROR ("invalid command %d", cmd);
			return -1;
	}
	
	return 0;
}

void kbd_dev_free (BaseEvDev *bed) {
	int result = ev_dev_unref (bed->evdev);
	free (bed);
}

#ifndef KEY_STAR  
#define KEY_STAR 227
#endif

#ifndef KEY_SHARP
#define KEY_SHARP 229
#endif

#ifndef KEY_SOFT1 
#define KEY_SOFT1 229
#endif

#ifndef KEY_SOFT2 
#define KEY_SOFT2 230
#endif

#ifndef KEY_CENTER
#define KEY_CENTER 232
#endif

uint16_t symcode_to_scancode (KbdDev *dev, const KeyData *data) {
	int scancode = 0;

	uint32_t code = data->sym;
	if (code>='0' && code<='9') {
		scancode = (code & 0xF) - 1;
		if (scancode<0) scancode += 10;
		scancode += KEY_1;
	} else if (code>=0xFF50 && code<=0xFF58) {
		static const uint16_t map[] =
		{ KEY_HOME, KEY_LEFT, KEY_UP, KEY_RIGHT, KEY_DOWN,
			KEY_SOFT1, KEY_SOFT2, KEY_END, 0 };
		scancode = map[code & 0xF];
	} else if (code>=0xFFE1 && code<=0xFFEE) {
		static const uint16_t map[] = { 
			0, KEY_LEFTSHIFT, KEY_LEFTSHIFT,
			KEY_COMPOSE, KEY_COMPOSE,
			KEY_LEFTSHIFT, KEY_LEFTSHIFT,
			0,0,
			KEY_LEFTALT, KEY_RIGHTALT,
			KEY_HOME, 0, 0, 0 
		};
		scancode = map[code & 0xF];
	} else if ((code>='A' && code<='Z') || (code>='a' && code<='z')) {
		static const uint16_t map[] = {
			KEY_A, KEY_B, KEY_C, KEY_D, KEY_E,
			KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
			KEY_K, KEY_L, KEY_M, KEY_N, KEY_O,
			KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
			KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z };
		scancode = map[(code & 0x5F) - 'A'];
	} else {
		switch (code) {
			case 0x0003:    scancode = KEY_CENTER;      break;
			case 0x0020:    scancode = KEY_SPACE;       break;
			case 0x0023:    scancode = KEY_SHARP;       break;
			case 0x0033:    scancode = KEY_SHARP;       break;
			case 0x002C:    scancode = KEY_COMMA;       break;
			case 0x003C:    scancode = KEY_COMMA;       break;
			case 0x002A:    scancode = KEY_STAR;	break;
			case 0x002E:    scancode = KEY_DOT;	 break;
			case 0x003E:    scancode = KEY_DOT;	 break;
			case 0x002F:    scancode = KEY_SLASH;       break;
			case 0x003F:    scancode = KEY_SLASH;       break;
			case 0x0032:    scancode = KEY_EMAIL;       break;
			case 0x0040:    scancode = KEY_EMAIL;       break;
			case 0xFF08:    scancode = KEY_BACKSPACE;   break;
			case 0xFF09:    scancode = KEY_TAB;	 break;
			case 0xFF0D:    scancode = KEY_ENTER;       break;
			case 0xFF1B:    scancode = KEY_BACK;	break;
			case 0xFF66:    scancode = KEY_HOME;	break;
			case XK_Menu:   scancode = KEY_MENU;	break;
			case XK_Find:   scancode = KEY_SEARCH;     break;
			case 0xFFBE:    scancode = KEY_F1;		break; // F1
			case 0xFFBF:    scancode = KEY_F2;	  break; // F2
			case 0xFFC0:    scancode = KEY_F3;		break; // F3
			case 0xFFC5:    scancode = KEY_F4;       	break; // F8
			case 0xFFC8:	break; // F11
			default:
					LOG_WARN ("cannot decode key:%04x", code);
		}
	}

	return (uint16_t)scancode;
}

static int kbd_write_data (BaseEvDev *bed, const KeyData *data) {
	struct input_event ev;
	memset(&ev, 0, sizeof(ev));

	LOG_DEBUG ("key data %p: sym 0x%08x, scancode 0x%08x, value %d", 
			data, data->sym, data->scancode, data->value);
	if (data->scancode) {
		ev.code = data->scancode;
	} else {
		ev.code = symcode_to_scancode (NULL, data);
		LOG_DEBUG ("translated symcode 0x%08x to scancode 0x%08x", data->sym, ev.code);
	}

	if (ev.code != 0) {
		gettimeofday(&ev.time,0);

		ev.type = EV_KEY;
		ev.value = data->value;

		LOG_DEBUG ("writting event");
		return ev_dev_write (bed->evdev, &ev, sizeof(ev));
	}
	else {
		LOG_WARN ("ignore null symcode %u", data->sym);
		return 0;
	}
}

int kbd_dev_write (BaseEvDev *bed, int type, const EvData *evdata) {
	const KeyData *data = (KeyData *)evdata;

	return kbd_write_data (bed, data);
#if 0
	LOG_DEBUG ("evdata %p, data %p: sym 0x%08x, scancode 0x%08x, value %d",evdata, data,
		data->sym, data->scancode, data->value);

	memset(&ev, 0, sizeof(ev));
	if (data->scancode) {
		ev.code = data->scancode;
	} else {
		LOG_DEBUG ("translate symcode to scancode");
		ev.code = symcode_to_scancode (kbd, data);
		LOG_DEBUG ("translated symcode 0x%08x to scancode 0x%08x", data->sym, ev.code);
	}

	if (ev.code != 0) {
		gettimeofday(&ev.time,0);

		ev.type = EV_KEY;
		ev.value = data->value;

		LOG_DEBUG ("writting event");
		return ev_dev_write (bed->evdev, &ev, sizeof(ev));
	}
	else {
		LOG_WARN ("ignore unknown symcode %u", data->sym);
		return 0;
	}
#endif
}

int kbd_dev_write_n (BaseEvDev *bed, int type, const EvData *evdata, int count) {
	int result = 0;

	const KeyData *data = (KeyData *)evdata;

	for (int i = 0; i < count && result == 0; i++) {
		//result = kbd_dev_write (bed, type, (const EvData *)(data + i));
		result = kbd_write_data (bed, data + i);
	}

	return result;
}

#if 0
enum {
	 XK_BackSpace = 0xFF08; /* back space, back char */
	 XK_Tab = 0xFF09;
	 XK_Linefeed = 0xFF0A; /* Linefeed, LF */
	 XK_Clear = 0xFF0B;
	 XK_Return = 0xFF0D; /* Return, enter */
	 XK_Pause = 0xFF13; /* Pause, hold */
	 XK_Scroll_Lock = 0xFF14;
	 XK_Sys_Req = 0xFF15;
	 XK_Escape = 0xFF1B;
	 XK_Delete = 0xFFFF; /* Delete, rubout */

	 XK_Home = 0xFF50;
	 XK_Left = 0xFF51; /* Move left, left arrow */
	 XK_Up = 0xFF52; /* Move up, up arrow */
	 XK_Right = 0xFF53; /* Move right, right arrow */
	 XK_Down = 0xFF54; /* Move down, down arrow */
	 XK_Prior = 0xFF55; /* Prior, previous */
	 XK_Page_Up = 0xFF55;
	 XK_Next = 0xFF56; /* Next */
	 XK_Page_Down = 0xFF56;
	 XK_End = 0xFF57; /* EOL */
	 XK_Begin = 0xFF58; /* BOL */

	/* Misc Functions */

	 XK_Select = 0xFF60; /* Select, mark */
	 XK_Print = 0xFF61;
	 XK_Execute = 0xFF62; /* Execute, run, do */
	 XK_Insert = 0xFF63; /* Insert, insert here */
	 XK_Undo = 0xFF65; /* Undo, oops */
	 XK_Redo = 0xFF66; /* redo, again */
	 XK_Menu = 0xFF67;
	 XK_Find = 0xFF68; /* Find, search */
	 XK_Cancel = 0xFF69; /* Cancel, stop, abort, exit */
	 XK_Help = 0xFF6A; /* Help */
	 XK_Break = 0xFF6B;
	 XK_Mode_switch = 0xFF7E; /* Character set switch */
	 XK_script_switch = 0xFF7E; /* Alias for mode_switch */
	 XK_Num_Lock = 0xFF7F;

	/* Keypad Functions, keypad numbers cleverly chosen to map to ascii */

	 XK_KP_Space = 0xFF80; /* space */
	 XK_KP_Tab = 0xFF89;
	 XK_KP_Enter = 0xFF8D; /* enter */
	 XK_KP_F1 = 0xFF91; /* PF1, KP_A, ... */
	 XK_KP_F2 = 0xFF92;
	 XK_KP_F3 = 0xFF93;
	 XK_KP_F4 = 0xFF94;
	 XK_KP_Home = 0xFF95;
	 XK_KP_Left = 0xFF96;
	 XK_KP_Up = 0xFF97;
	 XK_KP_Right = 0xFF98;
	 XK_KP_Down = 0xFF99;
	 XK_KP_Prior = 0xFF9A;
	 XK_KP_Page_Up = 0xFF9A;
	 XK_KP_Next = 0xFF9B;
	 XK_KP_Page_Down = 0xFF9B;
	 XK_KP_End = 0xFF9C;
	 XK_KP_Begin = 0xFF9D;
	 XK_KP_Insert = 0xFF9E;
	 XK_KP_Delete = 0xFF9F;
	 XK_KP_Equal = 0xFFBD; /* equals */
	 XK_KP_Multiply = 0xFFAA;
	 XK_KP_Add = 0xFFAB;
	 XK_KP_Separator = 0xFFAC; /* separator, often comma */
	 XK_KP_Subtract = 0xFFAD;
	 XK_KP_Decimal = 0xFFAE;
	 XK_KP_Divide = 0xFFAF;

	 XK_KP_0 = 0xFFB0;
	 XK_KP_1 = 0xFFB1;
	 XK_KP_2 = 0xFFB2;
	 XK_KP_3 = 0xFFB3;
	 XK_KP_4 = 0xFFB4;
	 XK_KP_5 = 0xFFB5;
	 XK_KP_6 = 0xFFB6;
	 XK_KP_7 = 0xFFB7;
	 XK_KP_8 = 0xFFB8;
	 XK_KP_9 = 0xFFB9;

	/*
	 * Auxilliary Functions; note the duplicate definitions for left and right
	 * function keys; Sun keyboards and a few other manufactures have such
	 * function key groups on the left and/or right sides of the keyboard. We've
	 * not found a keyboard with more than 35 function keys total.
	 */

	 XK_F1 = 0xFFBE;
	 XK_F2 = 0xFFBF;
	 XK_F3 = 0xFFC0;
	 XK_F4 = 0xFFC1;
	 XK_F5 = 0xFFC2;
	 XK_F6 = 0xFFC3;
	 XK_F7 = 0xFFC4;
	 XK_F8 = 0xFFC5;
	 XK_F9 = 0xFFC6;
	 XK_F10 = 0xFFC7;
	 XK_F11 = 0xFFC8;
	 XK_L1 = 0xFFC8;
	 XK_F12 = 0xFFC9;
	 XK_L2 = 0xFFC9;
	 XK_F13 = 0xFFCA;
	 XK_L3 = 0xFFCA;
	 XK_F14 = 0xFFCB;
	 XK_L4 = 0xFFCB;
	 XK_F15 = 0xFFCC;
	 XK_L5 = 0xFFCC;
	 XK_F16 = 0xFFCD;
	 XK_L6 = 0xFFCD;
	 XK_F17 = 0xFFCE;
	 XK_L7 = 0xFFCE;
	 XK_F18 = 0xFFCF;
	 XK_L8 = 0xFFCF;
	 XK_F19 = 0xFFD0;
	 XK_L9 = 0xFFD0;
	 XK_F20 = 0xFFD1;
	 XK_L10 = 0xFFD1;
	 XK_F21 = 0xFFD2;
	 XK_R1 = 0xFFD2;
	 XK_F22 = 0xFFD3;
	 XK_R2 = 0xFFD3;
	 XK_F23 = 0xFFD4;
	 XK_R3 = 0xFFD4;
	 XK_F24 = 0xFFD5;
	 XK_R4 = 0xFFD5;
	 XK_F25 = 0xFFD6;
	 XK_R5 = 0xFFD6;
	 XK_F26 = 0xFFD7;
	 XK_R6 = 0xFFD7;
	 XK_F27 = 0xFFD8;
	 XK_R7 = 0xFFD8;
	 XK_F28 = 0xFFD9;
	 XK_R8 = 0xFFD9;
	 XK_F29 = 0xFFDA;
	 XK_R9 = 0xFFDA;
	 XK_F30 = 0xFFDB;
	 XK_R10 = 0xFFDB;
	 XK_F31 = 0xFFDC;
	 XK_R11 = 0xFFDC;
	 XK_F32 = 0xFFDD;
	 XK_R12 = 0xFFDD;
	 XK_F33 = 0xFFDE;
	 XK_R13 = 0xFFDE;
	 XK_F34 = 0xFFDF;
	 XK_R14 = 0xFFDF;
	 XK_F35 = 0xFFE0;
	 XK_R15 = 0xFFE0;

	/* Modifiers */

	 XK_Shift_L = 0xFFE1; /* Left shift */
	 XK_Shift_R = 0xFFE2; /* Right shift */
	 XK_Control_L = 0xFFE3; /* Left control */
	 XK_Control_R = 0xFFE4; /* Right control */
	 XK_Caps_Lock = 0xFFE5; /* Caps lock */
	 XK_Shift_Lock = 0xFFE6; /* Shift lock */

	 XK_Meta_L = 0xFFE7; /* Left meta */
	 XK_Meta_R = 0xFFE8; /* Right meta */
	 XK_Alt_L = 0xFFE9; /* Left alt */
	 XK_Alt_R = 0xFFEA; /* Right alt */
	 XK_Super_L = 0xFFEB; /* Left super */
	 XK_Super_R = 0xFFEC; /* Right super */
	 XK_Hyper_L = 0xFFED; /* Left hyper */
	 XK_Hyper_R = 0xFFEE; /* Right hyper */

	/*
	 * Latin 1 Byte 3 = 0
	 */
	 XK_space = 0x020;
	 XK_exclam = 0x021;
	 XK_quotedbl = 0x022;
	 XK_numbersign = 0x023;
	 XK_dollar = 0x024;
	 XK_percent = 0x025;
	 XK_ampersand = 0x026;
	 XK_apostrophe = 0x027;
	 XK_quoteright = 0x027; /* deprecated */
	 XK_parenleft = 0x028;
	 XK_parenright = 0x029;
	 XK_asterisk = 0x02a;
	 XK_plus = 0x02b;
	 XK_comma = 0x02c;
	 XK_minus = 0x02d;
	 XK_period = 0x02e;
	 XK_slash = 0x02f;
	 XK_0 = 0x030;
	 XK_1 = 0x031;
	 XK_2 = 0x032;
	 XK_3 = 0x033;
	 XK_4 = 0x034;
	 XK_5 = 0x035;
	 XK_6 = 0x036;
	 XK_7 = 0x037;
	 XK_8 = 0x038;
	 XK_9 = 0x039;
	 XK_colon = 0x03a;
	 XK_semicolon = 0x03b;
	 XK_less = 0x03c;
	 XK_equal = 0x03d;
	 XK_greater = 0x03e;
	 XK_question = 0x03f;
	 XK_at = 0x040;
	 XK_A = 0x041;
	 XK_B = 0x042;
	 XK_C = 0x043;
	 XK_D = 0x044;
	 XK_E = 0x045;
	 XK_F = 0x046;
	 XK_G = 0x047;
	 XK_H = 0x048;
	 XK_I = 0x049;
	 XK_J = 0x04a;
	 XK_K = 0x04b;
	 XK_L = 0x04c;
	 XK_M = 0x04d;
	 XK_N = 0x04e;
	 XK_O = 0x04f;
	 XK_P = 0x050;
	 XK_Q = 0x051;
	 XK_R = 0x052;
	 XK_S = 0x053;
	 XK_T = 0x054;
	 XK_U = 0x055;
	 XK_V = 0x056;
	 XK_W = 0x057;
	 XK_X = 0x058;
	 XK_Y = 0x059;
	 XK_Z = 0x05a;
	 XK_bracketleft = 0x05b;
	 XK_backslash = 0x05c;
	 XK_bracketright = 0x05d;
	 XK_asciicircum = 0x05e;
	 XK_underscore = 0x05f;
	 XK_grave = 0x060;
	 XK_quoteleft = 0x060; /* deprecated */
	 XK_a = 0x061;
	 XK_b = 0x062;
	 XK_c = 0x063;
	 XK_d = 0x064;
	 XK_e = 0x065;
	 XK_f = 0x066;
	 XK_g = 0x067;
	 XK_h = 0x068;
	 XK_i = 0x069;
	 XK_j = 0x06a;
	 XK_k = 0x06b;
	 XK_l = 0x06c;
	 XK_m = 0x06d;
	 XK_n = 0x06e;
	 XK_o = 0x06f;
	 XK_p = 0x070;
	 XK_q = 0x071;
	 XK_r = 0x072;
	 XK_s = 0x073;
	 XK_t = 0x074;
	 XK_u = 0x075;
	 XK_v = 0x076;
	 XK_w = 0x077;
	 XK_x = 0x078;
	 XK_y = 0x079;
	 XK_z = 0x07a;
	 XK_braceleft = 0x07b;
	 XK_bar = 0x07c;
	 XK_braceright = 0x07d;
	 XK_asciitilde = 0x07e;

	 XK_nobreakspace = 0x0a0;
	 XK_exclamdown = 0x0a1;
	 XK_cent = 0x0a2;
	 XK_sterling = 0x0a3;
	 XK_currency = 0x0a4;
	 XK_yen = 0x0a5;
	 XK_brokenbar = 0x0a6;
	 XK_section = 0x0a7;
	 XK_diaeresis = 0x0a8;
	 XK_copyright = 0x0a9;
	 XK_ordfeminine = 0x0aa;
	 XK_guillemotleft = 0x0ab; /* left angle quotation mark */
	 XK_notsign = 0x0ac;
	 XK_hyphen = 0x0ad;
	 XK_registered = 0x0ae;
	 XK_macron = 0x0af;
	 XK_degree = 0x0b0;
	 XK_plusminus = 0x0b1;
	 XK_twosuperior = 0x0b2;
	 XK_threesuperior = 0x0b3;
	 XK_acute = 0x0b4;
	 XK_mu = 0x0b5;
	 XK_paragraph = 0x0b6;
	 XK_periodcentered = 0x0b7;
	 XK_cedilla = 0x0b8;
	 XK_onesuperior = 0x0b9;
	 XK_masculine = 0x0ba;
	 XK_guillemotright = 0x0bb; /*
														 * right angle quotation
														 * mark
														 */
	 XK_onequarter = 0x0bc;
	 XK_onehalf = 0x0bd;
	 XK_threequarters = 0x0be;
	 XK_questiondown = 0x0bf;
	 XK_Agrave = 0x0c0;
	 XK_Aacute = 0x0c1;
	 XK_Acircumflex = 0x0c2;
	 XK_Atilde = 0x0c3;
	 XK_Adiaeresis = 0x0c4;
	 XK_Aring = 0x0c5;
	 XK_AE = 0x0c6;
	 XK_Ccedilla = 0x0c7;
	 XK_Egrave = 0x0c8;
	 XK_Eacute = 0x0c9;
	 XK_Ecircumflex = 0x0ca;
	 XK_Ediaeresis = 0x0cb;
	 XK_Igrave = 0x0cc;
	 XK_Iacute = 0x0cd;
	 XK_Icircumflex = 0x0ce;
	 XK_Idiaeresis = 0x0cf;
	 XK_ETH = 0x0d0;
	 XK_Eth = 0x0d0; /* deprecated */
	 XK_Ntilde = 0x0d1;
	 XK_Ograve = 0x0d2;
	 XK_Oacute = 0x0d3;
	 XK_Ocircumflex = 0x0d4;
	 XK_Otilde = 0x0d5;
	 XK_Odiaeresis = 0x0d6;
	 XK_multiply = 0x0d7;
	 XK_Ooblique = 0x0d8;
	 XK_Ugrave = 0x0d9;
	 XK_Uacute = 0x0da;
	 XK_Ucircumflex = 0x0db;
	 XK_Udiaeresis = 0x0dc;
	 XK_Yacute = 0x0dd;
	 XK_THORN = 0x0de;
	 XK_Thorn = 0x0de; /* deprecated */
	 XK_ssharp = 0x0df;
	 XK_agrave = 0x0e0;
	 XK_aacute = 0x0e1;
	 XK_acircumflex = 0x0e2;
	 XK_atilde = 0x0e3;
	 XK_adiaeresis = 0x0e4;
	 XK_aring = 0x0e5;
	 XK_ae = 0x0e6;
	 XK_ccedilla = 0x0e7;
	 XK_egrave = 0x0e8;
	 XK_eacute = 0x0e9;
	 XK_ecircumflex = 0x0ea;
	 XK_ediaeresis = 0x0eb;
	 XK_igrave = 0x0ec;
	 XK_iacute = 0x0ed;
	 XK_icircumflex = 0x0ee;
	 XK_idiaeresis = 0x0ef;
	 XK_eth = 0x0f0;
	 XK_ntilde = 0x0f1;
	 XK_ograve = 0x0f2;
	 XK_oacute = 0x0f3;
	 XK_ocircumflex = 0x0f4;
	 XK_otilde = 0x0f5;
	 XK_odiaeresis = 0x0f6;
	 XK_division = 0x0f7;
	 XK_oslash = 0x0f8;
	 XK_ugrave = 0x0f9;
	 XK_uacute = 0x0fa;
	 XK_ucircumflex = 0x0fb;
	 XK_udiaeresis = 0x0fc;
	 XK_yacute = 0x0fd;
	 XK_thorn = 0x0fe;
	 XK_ydiaeresis = 0x0ff;

}
#endif


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
