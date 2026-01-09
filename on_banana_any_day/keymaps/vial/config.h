#pragma once

/* Vial Config */
#define VIAL_KEYBOARD_UID {0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB}
#define VIAL_UNLOCK_COMBO_ROWS { 0, 0 }
#define VIAL_UNLOCK_COMBO_COLS { 0, 11 }

/* QMK 機能定義 */
// TAP_DANCE_COUNT は不要なので削除
#define VIAL_COMBO_ENTRIES 8
#define MACRO_COUNT 8
#define MACRO_LEN 128
#define DYNAMIC_KEYMAP_LAYER_COUNT 5
