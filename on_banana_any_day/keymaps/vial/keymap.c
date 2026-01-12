#include QMK_KEYBOARD_H

// 1. カスタムキーコードの定義
enum custom_keycodes {
    IME_ENTER = QK_KB_0,
    IME_SPACE,
};

// 2. タイマー用の変数
static uint16_t ime_enter_timer = 0;

// タップかホールドかの判定時間 (ミリ秒)
#ifndef TAPPING_TERM
#define TAPPING_TERM 200
#endif

// 3. キーマップ定義 (変更なし)
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_ortho_4x13(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [1] = LAYOUT_ortho_4x13(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
};

// 4. 手動Tap Danceロジック
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case IME_ENTER:
            if (record->event.pressed) {
                // キーを押した瞬間:
                // タイマーを開始し、IME ON (かな) を送信
                ime_enter_timer = timer_read();
                tap_code(KC_LNG1);
            } else {
        
                // 押していた時間をチェック
                if (timer_elapsed(ime_enter_timer) < TAPPING_TERM) {
                    // 短い (タップ): Enter を送信
                    tap_code(KC_ENTER);
                } else {
                    // 長い (ホールド): 
                    // "離したら IME OFF" の仕様通り、英数キーをタップ
                    tap_code(KC_LNG2);
                }
            }
            return false; // 独自の処理をしたので、QMK標準の処理はスキップ
        case IME_SPACE:
            if (record->event.pressed) {
                // キーを押した時: タイマー開始 & IME ON (かな)
                ime_enter_timer = timer_read();
                tap_code(KC_LNG1);
            } else {
                // キーを離した時: まず IME ON を解除
                //tap_code(KC_LNG2);
                
                // 押していた時間を判定
                if (timer_elapsed(ime_enter_timer) < TAPPING_TERM) {
                    // 短押し: Space を送信
                    tap_code(KC_SPACE);
                } else {
                    // 長押し: IME OFF (英数)
                    tap_code(KC_LNG2);
                }
            }
            return false; // QMKの標準処理をスキップ
    }
    return true;
}