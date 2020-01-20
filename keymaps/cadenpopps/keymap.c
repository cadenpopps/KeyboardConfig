#include <ctrl.h>

#define MILLISECONDS_IN_SECOND 1000

#define RGB_DEFAULT_TIME_OUT 300
#define RGB_TIME_OUT_MAX 600
#define RGB_TIME_OUT_MIN 20
#define RGB_TIME_OUT_STEP 20

#define NUM_KEYS 86

#define HSV_LIMIT_VAL 255
#define HSV_LIMIT_SAT 100

#define TAPPING_TOGGLE 2

static uint16_t idle_timer;             // Idle LED timeout timer
static uint8_t idle_second_counter;     // Idle LED seconds counter, counts seconds not milliseconds

static bool stealth_mode;

bool rgb_enabled_flag;                  // Current LED state flag. If false then LED is off.
bool rgb_time_out_enable;               // Idle LED toggle enable. If false then LED will not turn off after idle timeout.
bool rgb_time_out_user_value;           // This holds the toggle value set by user with ROUT_TG. It's necessary as RGB_TOG changes timeout enable.
uint16_t rgb_time_out_seconds;          // Idle LED timeout value, in seconds not milliseconds
uint16_t rgb_time_out_saved_seconds;
led_flags_t rgb_time_out_saved_flag;    // Store LED flag before timeout so it can be restored when LED is turned on again.

enum ctrl_keycodes {
	MD_BOOT = SAFE_RANGE,  //Restart into bootloader after hold timeout
	FN_UTIL,
	LOCK_PC,
	STLTH_TG,
	DBG_TOG,			   //DEBUG Toggle On / Off
	DBG_MTRX,              //DEBUG Toggle Matrix Prints
	DBG_KBD,               //DEBUG Toggle Keyboard Prints
	DBG_MOU,               //DEBUG Toggle Mouse Prints
	ROUT_TG,               //Timeout Toggle. Toggle idle LED time out on or off
	ROUT_VI,               //Timeout Value Increase. Increase idle time out before LED disabled
	ROUT_VD               //Timeout Value Decrease. Decrease idle time out before LED disabled
};

enum layout_names {
	LAYER_BASE = 0,       // Keys Layout: The main keyboard layout that has all the characters
	LAYER_PRACTICE,
	LAYER_NUM_NAV,
	LAYER_MEDIA,
	LAYER_LED,
	/* _FL,         // Function Layout: The function key activated layout with default functions and some added ones */
	/* _ML,         // Mouse Layout: Mouse Keys and mouse movement */
	/* _VL,         // VIM Layout: VIM shorcuts and macros */
	/* _GL,         // GIT Layout: GIT shortcuts and macros */
	/* _YL,         // Yakuake Layout: Yakuake drop-down terminal shortcuts and macros */
	/* _EL,         // KDE Layout: Shortcuts for KDE desktop using default KDE shortcuts settings */
	LAYER_UTILITY,
};

enum COLORS { WHITE = 0, RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, PINK, GREY};
enum CUSTOM_LEDS { CLED_ESC=0, CLED_F1=1, CLED_F2=2, CLED_F3=3, CLED_F4=4, CLED_F5=5, CLED_F6=6, CLED_F7=7, CLED_F8=8, CLED_F9=9, CLED_F10=10, CLED_F11=11, CLED_F12=12, CLED_PSCR=13, CLED_SLCK=14, CLED_PAUS=15, CLED_GRV=16, CLED_1=17, CLED_2=18, CLED_3=19, CLED_4=20, CLED_5=21, CLED_6=22, CLED_7=23, CLED_8=24, CLED_9=25, CLED_0=26, CLED_MINS=27, CLED_EQL=28, CLED_BSPC=29, CLED_INS=30, CLED_HOME=31, CLED_PGUP=32, CLED_TAB=33, CLED_Q=34, CLED_W=35, CLED_E=36, CLED_R=37, CLED_T=38, CLED_Y=39, CLED_U=40, CLED_I=41, CLED_O=42, CLED_P=43, CLED_LBRC=44, CLED_RBRC=45, CLED_BSLS=46, CLED_DEL=47, CLED_END=48, CLED_PGDN=49, CLED_CAPS=50, CLED_A=51, CLED_S=52, CLED_D=53, CLED_F=54, CLED_G=55, CLED_H=56, CLED_J=57, CLED_K=58, CLED_L=59, CLED_SCLN=60, CLED_QUOT=61, CLED_ENT=62, CLED_LSFT=63, CLED_Z=64, CLED_X=65, CLED_C=66, CLED_V=67, CLED_B=68, CLED_N=69, CLED_M=70, CLED_COMM=71, CLED_DOT=72, CLED_SLSH=73, CLED_RSFT=74, CLED_UP=75, CLED_LCTL=76, CLED_LGUI=77, CLED_LALT=78, CLED_SPC=79, CLED_RALT=80, CLED_FN=81, CLED_APP=82, CLED_RCTL=83, CLED_LEFT=84, CLED_DOWN=85, CLED_RGHT=86 };

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

	/* [DEFAULT] = LAYOUT( */
	/* KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,             KC_PSCR, KC_SLCK, KC_PAUS, */
	/* KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,   KC_INS,  KC_HOME, KC_PGUP, */
	/* KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,   KC_DEL,  KC_END,  KC_PGDN, */
	/* KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT, */
	/* KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,                              KC_UP, */
	/* KC_LCTL, KC_LGUI, KC_LALT,                   KC_SPC,                             KC_RALT, MO(1),   KC_APP,  KC_RCTL,            KC_LEFT, KC_DOWN, KC_RGHT */
	/* ), */
	/* [BLANK] = LAYOUT( */
	/* 		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, */
	/* 		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, */
	/* 		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, */
	/* 		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, */
	/* 		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                              _______, */
	/* 		_______, _______, _______,                   _______,                            _______, FN_UTIL, _______, _______,            _______, _______, _______ */
	/* 		),*/

	[LAYER_BASE] = LAYOUT(
			KC_ESC, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_PSCR, KC_SLCK, KC_PAUS,
			KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC, KC_INS, KC_HOME, KC_PGUP,
			KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL, KC_END, KC_PGDN,
			KC_ESC, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT,
			KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_SFTENT, KC_UP,
			KC_LCTL, KC_LGUI, KC_LALT, 				KC_SPC, 		 KC_RALT, FN_UTIL, KC_APP, KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
			),
	[LAYER_PRACTICE] = LAYOUT(
			KC_ESC, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_PSCR, KC_SLCK, KC_PAUS,
			KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC, KC_INS, KC_HOME, KC_PGUP,
			KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL, KC_END, KC_PGDN,
			KC_ESC, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT,
			KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_SFTENT, KC_UP,
			KC_LCTL, KC_LGUI, KC_LALT, 				KC_SPC, 		 KC_RALT, FN_UTIL, KC_APP, KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
			),
	[LAYER_UTILITY] = LAYOUT(
			_______, TO(LAYER_PRACTICE), _______, TO(LAYER_NUM_NAV), TO(LAYER_MEDIA), _______, _______, _______, _______, _______, _______, _______, _______,       MD_BOOT, LOCK_PC, KC_SLEP,
			_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   KC_BRID, _______, KC_BRIU,
			_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   KC_VOLD, KC_MUTE, KC_VOLU,
			_______, _______, STLTH_TG, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
			_______, _______, _______, _______, _______, _______, _______, _______, KC_WBAK, KC_WFWD, KC_WREF, _______,                              		_______,
			_______, _______, _______,                   TO(LAYER_BASE),                     _______, TO(LAYER_BASE), _______, _______,            KC_MPRV, KC_MPLY, KC_MNXT
			),
	[LAYER_NUM_NAV] = LAYOUT(
			_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______,
			_______, LSFT(KC_EQL), KC_MINS, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______,
			_______, KC_7, KC_8, KC_9, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______,
			_______, KC_4, KC_5, KC_6, _______, _______, _______, _______, _______, _______, _______, _______, _______,
			_______, KC_1, KC_2, KC_3, _______, MD_BOOT, _______, _______, _______, _______, _______, _______,                              _______,
			_______, _______, KC_0,                   _______,                            _______, FN_UTIL, _______, _______,            _______, _______, _______
			),
	[LAYER_MEDIA] = LAYOUT(
			_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______,
			_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, KC_MUTE, _______,
			_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   KC_MPRV, KC_MPLY, KC_MNXT,
			_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
			_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                              KC_VOLU,
			_______, _______, _______,                   _______,                            _______, FN_UTIL, _______, _______,            _______, KC_VOLD, _______
			),
	[LAYER_LED] = LAYOUT(
			_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______,
			_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______,
			_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______,
			_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
			_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                              _______,
			_______, _______, _______,                   _______,                            _______, FN_UTIL, _______, _______,            _______, _______, _______
			),
	/* [_FL] = LAYOUT( */
	/* 		_______, DM_PLY1, DM_PLY2, _______,  _______, DM_REC1, DM_REC2, _______,  _______,  DM_RSTP, _______, KC_WAKE, KC_SLEP,            KC_MUTE, _______, _______, */
	/* 		_______, RGB_M_P, _______, _______,  _______, _______, _______, _______,  _______,  _______, ROUT_TG, ROUT_VD, ROUT_VI, _______,   KC_MSTP, KC_MPLY, KC_VOLU, */
	/* 		_______, RGB_SPD, RGB_VAI, RGB_SPI,  RGB_HUI, RGB_SAI, _______, U_T_AUTO, U_T_AGCR, _______, _______, _______, _______, _______,   KC_MPRV, KC_MNXT, KC_VOLD, */
	/* 		_______, RGB_RMOD,RGB_VAD, RGB_MOD,  RGB_HUD, RGB_SAD, _______, _______,  _______,  _______, _______, _______, _______, */
	/* 		_______, RGB_TOG, _______, _______, _______, MD_BOOT, _______, _______,  _______,  _______, _______, _______,                              KC_BRIU, */
	/* 		_______, _______, _______,                    _______,                              _______, _______, _______, _______,            _______, KC_BRID, _______ */
	/* 		), */
};


// Runs just one time when the keyboard initializes.
void matrix_init_user(void) {

	// Enable or disable debugging
	debug_enable=true;
	debug_matrix=true;
	debug_keyboard=true;
	debug_mouse=true;

	idle_second_counter = 0;
	rgb_time_out_seconds = RGB_DEFAULT_TIME_OUT;
	rgb_time_out_enable = true;
	rgb_enabled_flag = true;
	rgb_time_out_user_value = true;
	rgb_time_out_saved_flag = rgb_matrix_get_flags();
};

// Runs constantly in the background, in a loop.
void matrix_scan_user(void) {
	if(rgb_time_out_enable && rgb_enabled_flag) {
		if (timer_elapsed(idle_timer) > MILLISECONDS_IN_SECOND) {
			idle_second_counter++;
			idle_timer = timer_read();
		}

		if (idle_second_counter >= rgb_time_out_seconds) {
			rgb_time_out_saved_flag = rgb_matrix_get_flags();
			rgb_matrix_set_flags(LED_FLAG_NONE);
			rgb_matrix_disable_noeeprom();
			rgb_enabled_flag = false;
			idle_second_counter = 0;
		}
	}
};

/* qk_tap_dance_action_t tap_dance_actions[] = { */
/* 	[FN_UTIL] = ACTION_TAP_DANCE_LAYER_MOVE (KC_F24, LAYER_UTILITY) */
/* }; */


bool process_record_user(uint16_t keycode, keyrecord_t *record) {

	static uint16_t FN_UTIL_TIMER;

	if (rgb_time_out_enable) {
		idle_timer = timer_read();
		idle_second_counter = 0;
		if (!rgb_enabled_flag) {
			rgb_matrix_enable_noeeprom();
			rgb_matrix_set_flags(rgb_time_out_saved_flag);
			rgb_enabled_flag = true;
		}
	}

	switch (keycode) {
		case FN_UTIL:
			if (record->event.pressed) {
				FN_UTIL_TIMER = timer_read();
				layer_on(LAYER_UTILITY);
			}
			else if(timer_elapsed(FN_UTIL_TIMER) > TAPPING_TERM) {
				layer_off(LAYER_UTILITY);
			}
			return false;
		case LOCK_PC:
			if (record->event.pressed) { }
			return false;
		case STLTH_TG:
			if (record->event.pressed) {
				stealth_mode = !stealth_mode;
			}
			return false;
		case MD_BOOT:
			if (record->event.pressed) {
				reset_keyboard();
			}
			return false;
		case RGB_TOG:
			if (record->event.pressed) {
				rgb_time_out_enable = rgb_time_out_user_value;
				switch (rgb_matrix_get_flags()) {
					case LED_FLAG_ALL:
						rgb_matrix_set_flags(LED_FLAG_KEYLIGHT | LED_FLAG_MODIFIER);
						rgb_matrix_set_color_all(0, 0, 0);
						break;
					case LED_FLAG_KEYLIGHT | LED_FLAG_MODIFIER:
						rgb_matrix_set_flags(LED_FLAG_UNDERGLOW);
						rgb_matrix_set_color_all(0, 0, 0);
						break;
					case LED_FLAG_UNDERGLOW:
						// This line is for LED idle timer. It disables the toggle so you can turn off LED completely if you like
						rgb_time_out_enable = false;
						rgb_matrix_set_flags(LED_FLAG_NONE);
						rgb_matrix_disable_noeeprom();
						break;
					default:
						rgb_matrix_set_flags(LED_FLAG_ALL);
						rgb_matrix_enable_noeeprom();
						break;
				}
			}
			return false;
		case ROUT_TG: // Toggle idle LED timeout on or off
			if (record->event.pressed) {
				rgb_time_out_enable = !rgb_time_out_enable;
				rgb_time_out_user_value = rgb_time_out_enable;
			}
			return false;
		case ROUT_VI: // Increase idle LED timeout value in seconds
			if (record->event.pressed) {
				// Only increase if current value is lower than RGB_TIME_OUT_MAX. Don't care what value the result will be
				// Modity RGB_TIME_OUT_STEP for bigger or smaller increments
				if (rgb_time_out_seconds <= RGB_TIME_OUT_MAX) {
					rgb_time_out_seconds += RGB_TIME_OUT_STEP;
				}
			}
			return false;
		case ROUT_VD: // Decrease idle LED timeout value in seconds
			if (record->event.pressed) {
				// Only decrease if current value is higher than minimum value and the result is larger than zero
				// Modity RGB_TIME_OUT_STEP for bigger or smaller decrements
				if (rgb_time_out_seconds > RGB_TIME_OUT_MIN) {
					rgb_time_out_seconds -= RGB_TIME_OUT_STEP;
				}
			}
			return false;
		default:
			return true; //Process all other keycodes normally
	}
}

void customSetLEDRGB(int led, int r, int g, int b) {
	rgb_matrix_set_color(led, r, g, b);
}

void customSetLED(int led, int * color) {
	customSetLEDRGB(led, *(color + 0), *(color + 1), *(color + 2));
}

int * parseColor(int color) {
	static int colorArr[3];
	switch(color) {
		case WHITE:
			colorArr[0] = 250;
			colorArr[1] = 255;
			colorArr[2] = 255;
			return colorArr;
		case RED:
			colorArr[0] = 240;
			colorArr[1] = 0;
			colorArr[2] = 0;
			return colorArr;
		case ORANGE:
			colorArr[0] = 250;
			colorArr[1] = 100;
			colorArr[2] = 0;
			return colorArr;
		case YELLOW:
			colorArr[0] = 240;
			colorArr[1] = 240;
			colorArr[2] = 0;
			return colorArr;
		case GREEN:
			colorArr[0] = 0;
			colorArr[1] = 250;
			colorArr[2] = 100;
			return colorArr;
		case BLUE:
			colorArr[0] = 0;
			colorArr[1] = 100;
			colorArr[2] = 240;
			return colorArr;
		case PURPLE:
			colorArr[0] = 100;
			colorArr[1] = 0;
			colorArr[2] = 240;
			return colorArr;
		case PINK:
			colorArr[0] = 240;
			colorArr[1] = 40;
			colorArr[2] = 100;
			return colorArr;
		case GREY:
			colorArr[0] = 50;
			colorArr[1] = 50;
			colorArr[2] = 50;
			return colorArr;
		default:
			colorArr[0] = 0;
			colorArr[1] = 0;
			colorArr[2] = 0;
			return colorArr;
	}
}

void setAll(int color) {
	int *colorPointer = parseColor(color);
	for(int i = 0; i <= NUM_LEDS; i++) {
		customSetLED(i, colorPointer);
	}
}

void setAllRGB(int r, int g, int b) {
	for(int i = 0; i <= NUM_LEDS; i++) {
		customSetLEDRGB(i, r, g, b);
	}
}

void setKeys(int color) {
	int *colorPointer = parseColor(color);
	for(int i = 0; i <= NUM_KEYS; i++) {
		customSetLED(i, colorPointer);
	}
}

void setKeysRGB(int r, int g, int b) {
	for(int i = 0; i <= NUM_KEYS; i++) {
		customSetLEDRGB(i, r, g, b);
	}
}

void setLED(int led, int color) {
	int *colorPointer = parseColor(color);
	customSetLED(led, colorPointer);
}

void setLEDRGB(int led, int r, int g, int b) {
	customSetLEDRGB(led, r, g, b);
}

void setUnderglow(int color) {
	int *colorPointer = parseColor(color);
	for(int i = NUM_KEYS + 1; i <= NUM_LEDS; i++) {
		customSetLED(i, colorPointer);
	}
}

void setUnderglowRGB(int r, int g, int b) {
	for(int i = NUM_KEYS + 1; i <= NUM_LEDS; i++) {
		customSetLEDRGB(i, r, g, b);
	}
}

void clearAll(void) {
	for(int i = 0; i <= NUM_LEDS; i++) {
		customSetLEDRGB(i, 0, 0, 0);
	}
}

void updateColors(void) {
	clearAll();
	switch (biton32(layer_state)) {
		case LAYER_BASE:
			if(!stealth_mode) {
				setAll(WHITE);
			}
			else {
				setLED(CLED_Q, GREY);
				setLED(CLED_W, GREY);
				setLED(CLED_E, GREY);
				setLED(CLED_R, GREY);
				setLED(CLED_T, GREY);
				setLED(CLED_Y, GREY);
				setLED(CLED_U, GREY);
				setLED(CLED_I, GREY);
				setLED(CLED_O, GREY);
				setLED(CLED_P, GREY);

				setLED(CLED_A, GREY);
				setLED(CLED_S, GREY);
				setLED(CLED_D, GREY);
				setLED(CLED_F, GREY);
				setLED(CLED_G, GREY);
				setLED(CLED_H, GREY);
				setLED(CLED_J, GREY);
				setLED(CLED_K, GREY);
				setLED(CLED_L, GREY);

				setLED(CLED_Z, GREY);
				setLED(CLED_X, GREY);
				setLED(CLED_C, GREY);
				setLED(CLED_V, GREY);
				setLED(CLED_B, GREY);
				setLED(CLED_N, GREY);
				setLED(CLED_M, GREY);
			}
			break;
		case LAYER_PRACTICE:
			for(int i = NUM_KEYS + 1; i <= NUM_LEDS; i++) {
				switch(i % 2) {
					case 0:
						setLEDRGB(i, 240, 20, 0);
						break;
					case 1:
						setLEDRGB(i, 250, 40, 0);
						break;
				}
			}

			setLED(CLED_1, RED);
			setLED(CLED_2, RED);
			setLED(CLED_Q, RED);
			setLED(CLED_A, RED);
			setLED(CLED_Z, RED);

			setLED(CLED_3, ORANGE);
			setLED(CLED_W, ORANGE);
			setLED(CLED_S, ORANGE);
			setLED(CLED_X, ORANGE);

			setLED(CLED_4, YELLOW);
			setLED(CLED_E, YELLOW);
			setLED(CLED_D, YELLOW);
			setLED(CLED_C, YELLOW);

			setLED(CLED_5, GREEN);
			setLED(CLED_6, GREEN);
			setLED(CLED_R, GREEN);
			setLED(CLED_T, GREEN);
			setLED(CLED_F, GREEN);
			setLED(CLED_G, GREEN);
			setLED(CLED_V, GREEN);
			setLED(CLED_B, GREEN);

			setLED(CLED_7, BLUE);
			setLED(CLED_Y, BLUE);
			setLED(CLED_U, BLUE);
			setLED(CLED_H, BLUE);
			setLED(CLED_J, BLUE);
			setLED(CLED_N, BLUE);
			setLED(CLED_M, BLUE);

			setLED(CLED_8, PURPLE);
			setLED(CLED_I, PURPLE);
			setLED(CLED_K, PURPLE);
			setLED(CLED_COMM, PURPLE);

			setLED(CLED_9, PINK);
			setLED(CLED_O, PINK);
			setLED(CLED_L, PINK);
			setLED(CLED_DOT, PINK);

			setLED(CLED_0, RED);
			setLED(CLED_MINS, RED);
			setLED(CLED_EQL, RED);
			setLED(CLED_P, RED);
			setLED(CLED_LBRC, RED);
			setLED(CLED_RBRC, RED);
			setLED(CLED_SCLN, RED);
			setLED(CLED_QUOT, RED);
			setLED(CLED_SLSH, RED);

			break;
		case LAYER_UTILITY:
			for(int i = NUM_KEYS + 1; i <= NUM_LEDS; i++) {
				switch(i % 4) {
					case 0:
						setLEDRGB(i, 240, 10, 140);
						break;
					case 1:
						setLEDRGB(i, 180, 10, 180);
						break;
					case 2:
						setLEDRGB(i, 140, 10, 240);
						break;
					case 3:
						setLEDRGB(i, 180, 10, 180);
						break;
				}
			}

			setKeys(GREY);

			setLED(CLED_F1, RED);
			setLED(CLED_F2, ORANGE);
			setLED(CLED_F3, YELLOW);
			setLED(CLED_F4, GREEN);
			setLED(CLED_F5, BLUE);

			setLED(CLED_PSCR, RED);
			setLED(CLED_PAUS, RED);

			setLED(CLED_INS, ORANGE);
			setLED(CLED_HOME, YELLOW);
			setLED(CLED_PGUP, GREEN);

			setLED(CLED_DEL, ORANGE);
			setLED(CLED_END, YELLOW);
			setLED(CLED_PGDN, GREEN);

			setLED(CLED_LEFT, ORANGE);
			setLED(CLED_DOWN, YELLOW);
			setLED(CLED_RGHT, GREEN);

			setLED(CLED_COMM, ORANGE);
			setLED(CLED_DOT, GREEN);
			setLED(CLED_SLSH, YELLOW);

			break;
		case LAYER_LED:
			setUnderglow(RED);
			break;
		case LAYER_NUM_NAV:
			setUnderglow(YELLOW);
			setLED(CLED_Q, YELLOW);
			setLED(CLED_W, YELLOW);
			setLED(CLED_E, YELLOW);
			setLED(CLED_A, YELLOW);
			setLED(CLED_S, YELLOW);
			setLED(CLED_D, YELLOW);
			setLED(CLED_Z, YELLOW);
			setLED(CLED_X, YELLOW);
			setLED(CLED_C, YELLOW);
			setLED(CLED_LALT, YELLOW);
			setLED(CLED_1, ORANGE);
			setLED(CLED_2, ORANGE);
			setLED(CLED_3, ORANGE);
			break;
		case LAYER_MEDIA:
			setUnderglow(GREEN);

			setLED(CLED_DEL, ORANGE);
			setLED(CLED_END, YELLOW);
			setLED(CLED_PGDN, GREEN);

			setLED(CLED_LEFT, ORANGE);
			setLED(CLED_DOWN, YELLOW);
			setLED(CLED_RGHT, GREEN);

			break;
		default:
			setUnderglowRGB(0, 0, 0);
			break;
	}
	setLED(CLED_FN, WHITE);
}

void rgb_matrix_indicators_user(void) {
	if (!g_suspend_state && rgb_matrix_config.enable) {
		updateColors();
	}
}
