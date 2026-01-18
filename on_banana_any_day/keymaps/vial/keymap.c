#include QMK_KEYBOARD_H

// 1. カスタムキーコードの定義
enum custom_keycodes {
    IME_ENTER = QK_KB_0,
    IME_SPACE,
};

/// --- SandS方式用の変数定義 ---
static bool ime_space_pressed = false;     // スペースキーが押されているか
static bool ime_space_active = false;      // ホールド中に他のキーが打たれたか（IMEモード発動中か）
static uint16_t ime_timer = 0;       // タップ判定用タイマー

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
                ime_timer = timer_read();
                tap_code(KC_LNG1);
            } else {
        
                // 押していた時間をチェック
                if (timer_elapsed(ime_timer) < TAPPING_TERM) {
                    // 短い (タップ): Enter を送信
                    tap_code(KC_ENTER);
                } else {
                    // 長い (ホールド): 
                    // "離したら IME OFF" の仕様通り、英数キーをタップ
                    tap_code(KC_LNG2);
                }
            }
            return false; // 独自の処理をしたので、QMK標準の処理はスキップ
        // --- IME Space (SandS Like) ---
        case IME_SPACE:
            if (record->event.pressed) {
                // 押下時: フラグを立ててタイマー開始。まだ何もしない。
                ime_space_pressed = true;
                ime_space_active = false;
                ime_timer = timer_read();
            } else {
                // 離脱時:
                if (ime_space_active) {
                    // ホールド中に他の文字を打っていた場合 -> IME OFF (英数) に戻す
                    // ※Windowsでは「英数」キーを押すと未確定文字も確定されるため、Enterは不要な場合が多いですが、
                    // 確実に確定させたい場合は tap_code(KC_ENT) を先頭に追加してください。
                    tap_code(KC_LNG2); // IME OFF
                } else {
                    // 他の文字を打たずに離した場合 -> 単なるスペース
                    tap_code(KC_SPC);
                }
                
                // 状態リセット
                ime_space_pressed = false;
                ime_space_active = false;
            }
            return false; // QMK標準の処理は行わない

        // --- 他の全てのキー入力の監視 ---
        default:
            // IME_SPACEを押しながら、他のキーが押された場合
            if (ime_space_pressed && record->event.pressed) {
                if (!ime_space_active) {
                    // 初回の1回だけ IME ON (かな) を送信
                    tap_code(KC_LNG1); 
                    ime_space_active = true;
                }
            }
            break; // 重要: ここで return false してはいけない（他のキー入力を阻害しないため）
    }
    return true;
}