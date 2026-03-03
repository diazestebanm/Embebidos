#include "tasks.h"
#include "fsm.h"    // para: extern Input input

// ═══════════════════════════════════════════════════════════════
// AsyncTasks: Blinks de LEDs
// ═══════════════════════════════════════════════════════════════
AsyncTask tareaVerdeOn (200, false, []() { digitalWrite(PIN_LED_VERDE, HIGH); });
AsyncTask tareaVerdeOff(200, false, []() { digitalWrite(PIN_LED_VERDE, LOW);  });

AsyncTask tareaAzulOn (500, false, []() { digitalWrite(PIN_LED_AZUL,  HIGH); });
AsyncTask tareaAzulOff(900, false, []() { digitalWrite(PIN_LED_AZUL,  LOW);  });

AsyncTask tareaRojoOn (100, false, []() { digitalWrite(PIN_LED_ROJO,  HIGH); });
AsyncTask tareaRojoOff(300, false, []() { digitalWrite(PIN_LED_ROJO,  LOW);  });

// ═══════════════════════════════════════════════════════════════
// AsyncTasks: Timeouts (escriben en input global de FSM)
// ═══════════════════════════════════════════════════════════════
AsyncTask timeout2s    (2000, false, []() { if (input == NONE) input = TIMEOUT_2S;     });
AsyncTask timeoutTH    (4000, false, []() { if (input == NONE) input = TIMEOUT_4S;     }); // 4s TEMP→HUM
AsyncTask timeoutHT    (3000, false, []() { if (input == NONE) input = TIMEOUT_3S;     }); // 3s HUM→TEMP
AsyncTask timeoutAlerta(3000, false, []() { if (input == NONE) input = TIMEOUT_ALERTA; }); // 3s ALERTA→TEMP
AsyncTask timeout5s    (5000, false, []() { if (input == NONE) input = TIMEOUT_5S;     });

// ═══════════════════════════════════════════════════════════════
// LEDs
// ═══════════════════════════════════════════════════════════════
void leds_init(void) {
    pinMode(PIN_LED_VERDE, OUTPUT);
    pinMode(PIN_LED_AZUL,  OUTPUT);
    pinMode(PIN_LED_ROJO,  OUTPUT);
    leds_apagar();
}

void leds_apagar(void) {
    digitalWrite(PIN_LED_VERDE, LOW);
    digitalWrite(PIN_LED_AZUL,  LOW);
    digitalWrite(PIN_LED_ROJO,  LOW);
}

void blinks_detener(void) {
    tareaVerdeOn.Stop();  tareaVerdeOff.Stop();
    tareaAzulOn.Stop();   tareaAzulOff.Stop();
    tareaRojoOn.Stop();   tareaRojoOff.Stop();
}

// ═══════════════════════════════════════════════════════════════
// Botón con debounce por millis() — sin delay()
// ═══════════════════════════════════════════════════════════════
void button_read(void) {
    static bool          prevHigh     = true;
    static unsigned long pressTime    = 0;
    static bool          esperaSoltar = false;

    if (input != NONE) return;

    bool ahoraHigh = (digitalRead(PIN_BOTON) == HIGH);

    if (esperaSoltar) {
        if (ahoraHigh) esperaSoltar = false;
        prevHigh = ahoraHigh;
        return;
    }

    if (!ahoraHigh && prevHigh) {
        pressTime = millis();
    } else if (!ahoraHigh && !prevHigh) {
        if ((millis() - pressTime) >= DEBOUNCE_MS) {
            input        = BTN_PRESS;
            esperaSoltar = true;
        }
    }
    prevHigh = ahoraHigh;
}

void flama_read(void) {
    sensor_leerFlama(); // solo actualiza flamaActiva; la FSM la lee directo
}

// ═══════════════════════════════════════════════════════════════
// Update centralizado
// ═══════════════════════════════════════════════════════════════
void tasks_update(void) {
    tareaVerdeOn.Update(tareaVerdeOff);
    tareaVerdeOff.Update(tareaVerdeOn);
    tareaAzulOn.Update(tareaAzulOff);
    tareaAzulOff.Update(tareaAzulOn);
    tareaRojoOn.Update(tareaRojoOff);
    tareaRojoOff.Update(tareaRojoOn);

    timeout2s.Update();
    timeoutTH.Update();
    timeoutHT.Update();
    timeout5s.Update();
    timeoutAlerta.Update();
    flama_read();
}
