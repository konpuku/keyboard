#include QMK_KEYBOARD_H

// 1. カスタムキーコードの定義
enum custom_keycodes {
    IME_ENTER = QK_KB_0,
    IME_SPACE,88
};

/// --- SandS方式用の変数定義 ---
static bool ime_space_pressed = false;     // スペースキーが押されているか
static bool ime_space_active = false;      // ホールド中に他のキーが打たれたか（IMEモード発動中か）
static uint16_t ime_timer = 0;       // タップ判定用タイマー

// 3. タップ判定時間
#ifndef TAPPING_TERM
#define TAPPING_TERM 200
#endif

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // Layer 0: Base
    [0] = LAYOUT_ortho_4x10(
        KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
        KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_ENT,
        KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
        KC_LCTL, KC_LGUI, KC_LALT, KC_LSFT, MO(1),   KC_SPC,  KC_BSPC, MO(2),   KC_RALT, IME_ENTER
    ),

    // Layer 1: Lower
    [1] = LAYOUT_ortho_4x10(
        KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, MO(3),   KC_TRNS, KC_TRNS
    ),

    // Layer 2: Raise
    [2] = LAYOUT_ortho_4x10(
        KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, MO(3),   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

    // Layer 3: Adjust (エラーが出ていた箇所を修正: QK_MAGIC_... に変更)
    [3] = LAYOUT_ortho_4x10(
        QK_BOOT, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,
        KC_NO,   QK_MAGIC_SWAP_LCTL_LGUI, KC_NO, QK_MAGIC_UNSWAP_LCTL_LGUI, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_NO,   KC_NO
    )
};

// 4. IME Enter 機能の実装ロジック
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
            return false; // QMKの標準処理をスキップ
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