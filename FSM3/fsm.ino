// ================================================================
// FSM 6 Estados — ESP32 WROOM-32
// Estilo Luis Llamas: StateMachine + AsyncTask | SIN delays
// ================================================================
#include "pinout.h"
#include "types.h"
#include "sensor.h"
#include "tasks.h"
#include "fsm.h"
/**
 * @brief Inicializa el sistema: Serial, GPIO, sensores, tareas y la FSM.
 * @details
 * - Inicia el puerto Serial a 115200 y muestra un banner.
 * - Inicializa los pines del LED RGB.
 * - Configura el botón como INPUT_PULLUP.
 * - Inicializa los sensores (DHT11, LDR ADC, flama).
 * - Configura la máquina de estados (transiciones + callbacks) y fuerza el estado INICIO.
 */
void setup() {
    Serial.begin(115200);
    Serial.println("╔══════════════════════════════╗");
    Serial.println("║   FSM 6 Estados — ESP32      ║");
    Serial.println("╚══════════════════════════════╝");

    leds_init();
    pinMode(PIN_BOTON, INPUT_PULLUP);
    sensor_init();

    fsm_setup();
    maquina.SetState(INICIO, false, true);
}
/**
 * @brief Bucle principal cooperativo (sin delay) que actualiza tareas y la FSM.
 * @details
 * - Lee el botón con antirrebote (publica BTN_PRESS en la variable global input).
 * - Actualiza todas las tareas asincrónicas (blinks, timeouts y lectura de flama).
 * - Ejecuta maquina.Update() y permite encadenar transiciones si un callback cambia input.
 * - Al final limpia input para evitar re-procesar el mismo evento en el siguiente ciclo.
 */
void loop() {
    button_read();
    tasks_update();

    Input prevInput;
    do {
        prevInput = input;
        maquina.Update();
    } while (input != NONE && input != prevInput);

    input = NONE;
}
