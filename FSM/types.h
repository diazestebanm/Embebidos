#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

// ── Estados FSM ───────────────────────────────────────────────
enum State : uint8_t {
    INICIO      = 0,
    MON_TEMP    = 1,
    MON_HUMEDAD = 2,
    MON_LUZ     = 3,
    ALERTA      = 4,
    ALARMA      = 5
};

// ── Eventos / Entradas ────────────────────────────────────────
enum Input : uint8_t {
    NONE           = 0,
    BTN_PRESS      = 1,
    TEMP_ALTA      = 2,
    TEMP_MUY_ALTA  = 3,
    HUMEDAD_BAJA   = 4,
    LUZ_BAJA       = 5,
    TIMEOUT_2S     = 6,
    TIMEOUT_3S     = 7,
    TIMEOUT_4S     = 8,
    TIMEOUT_5S     = 9,
    TIMEOUT_ALERTA = 10,
    FLAMA_ON       = 11
};

#endif // TYPES_H
