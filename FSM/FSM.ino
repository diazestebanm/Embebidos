// ================================================================
// FSM 6 Estados — ESP32 WROOM-32
// Estilo Luis Llamas: StateMachine + AsyncTask | SIN delays
// ================================================================
#include "pinout.h"
#include "types.h"
#include "sensor.h"
#include "tasks.h"
#include "fsm.h"

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
