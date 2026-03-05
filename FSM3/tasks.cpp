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
/**
 * @brief Inicializa los pines del LED RGB como salidas y apaga el LED.
 * @details
 * Configura PIN_LED_VERDE, PIN_LED_AZUL y PIN_LED_ROJO como OUTPUT.
 * Luego llama a leds_apagar() para dejar el LED RGB apagado al inicio.
 */
void leds_init(void) {
    pinMode(PIN_LED_VERDE, OUTPUT);
    pinMode(PIN_LED_AZUL,  OUTPUT);
    pinMode(PIN_LED_ROJO,  OUTPUT);
    leds_apagar();
}
/**
 * @brief Apaga completamente el LED RGB.
 * @details
 * Escribe LOW en los pines del LED verde, azul y rojo.
 */
void leds_apagar(void) {
    digitalWrite(PIN_LED_VERDE, LOW);
    digitalWrite(PIN_LED_AZUL,  LOW);
    digitalWrite(PIN_LED_ROJO,  LOW);
}
/**
 * @brief Detiene todas las tareas de parpadeo del LED (verde, azul y rojo).
 * @details
 * Llama Stop() sobre las AsyncTask ON/OFF de cada color.
 * Útil al cambiar de estado para evitar parpadeos de estados anteriores.
 */
void blinks_detener(void) {
    tareaVerdeOn.Stop();  tareaVerdeOff.Stop();
    tareaAzulOn.Stop();   tareaAzulOff.Stop();
    tareaRojoOn.Stop();   tareaRojoOff.Stop();
}

// ═══════════════════════════════════════════════════════════════
// Botón con debounce por millis() — sin delay()
// ═══════════════════════════════════════════════════════════════
/**
 * @brief Lee el botón con antirrebote (debounce) sin delay y publica el evento BTN_PRESS.
 * @details
 * Implementa debounce basado en millis():
 * - Detecta flanco de bajada (HIGH -> LOW) y marca el tiempo.
 * - Si el botón permanece presionado al menos DEBOUNCE_MS, coloca input = BTN_PRESS.
 * - Exige soltar el botón (HIGH) antes de permitir otra pulsación.
 * 
 * @note
 * Si input != NONE, la función retorna inmediatamente para no sobrescribir eventos.
 */
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
/**
 * @brief Actualiza la lectura del sensor de flama sin bloquear el loop.
 * @details
 * Realiza polling llamando a sensor_leerFlama() para mantener flamaActiva actualizada.
 * La FSM consulta directamente flamaActiva en sus condiciones/transiciones.
 */
void flama_read(void) {
    sensor_leerFlama(); // solo actualiza flamaActiva; la FSM la lee directo
}

// ═══════════════════════════════════════════════════════════════
// Update centralizado
// ═══════════════════════════════════════════════════════════════
/**
 * @brief Actualiza todas las tareas asincrónicas (blinks, timeouts y flama) en un solo punto.
 * @details
 * - Actualiza los parpadeos del LED RGB alternando tareas ON/OFF (Update con “siguiente”).
 * - Actualiza los timeouts que generan eventos en la FSM (TIMEOUT_* escribiendo en input).
 * - Ejecuta flama_read() al final para mantener flamaActiva actualizada.
 *
 * @note
 * No bloquea la ejecución: está diseñada para llamarse en cada iteración del loop().
 */
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
