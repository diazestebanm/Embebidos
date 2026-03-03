#ifndef PINOUT_H
#define PINOUT_H

// ── LEDs ──────────────────────────────────────────────────────
#define PIN_LED_VERDE   26
#define PIN_LED_AZUL    27
#define PIN_LED_ROJO    25

// ── Entradas ──────────────────────────────────────────────────
#define PIN_BOTON       33    // INPUT_PULLUP, activo en LOW
#define PIN_DHT         23
#define PIN_LDR         34    // ADC1, input-only
#define PIN_FLAMA    32    // DO del MD-08 J — activo LOW, INPUT_PULLUP


// ── Umbrales ambientales ──────────────────────────────────────
#define TEMP_UMBRAL_ALERTA  25.0f
#define TEMP_UMBRAL_ALARMA  30.0f
#define HUM_UMBRAL_MIN      60.0f
#define LUZ_UMBRAL_MIN      60

// ── Parámetros del sistema ────────────────────────────────────
#define MAX_INTENTOS    3
#define DEBOUNCE_MS     50UL

#endif // PINOUT_H
