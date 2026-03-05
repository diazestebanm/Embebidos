#include "sensor.h"

// DHT como variable interna (static = encapsulada en este módulo)
static DHT dht(PIN_DHT, TIPO_DHT);

// Variables exportadas
float temperatura = 0.0f;
float humedad     = 0.0f;
int   luzRaw      = 0;
bool flamaActiva = false;
/**
 * @brief Inicializa el subsistema de sensores (DHT11, ADC para LDR y pin de flama).
 * @details
 * - Ejecuta dht.begin().
 * - Configura el ADC del ESP32 a 12 bits (0..4095) para la lectura del LDR.
 * - Aplica atenuación ADC_11db para medir hasta ~3.3V.
 * - Configura el pin del sensor de flama como INPUT_PULLUP (sin llama = HIGH).
 */
void sensor_init(void) {
    dht.begin();

    // ── Parche LDR: configurar ADC explícitamente ─────────────
    analogReadResolution(12);                              // 12-bit: 0–4095
    analogSetPinAttenuation(PIN_LDR, ADC_11db);    // seguir midiendo 0–3.3 V,
                                                           // rango 0–3.3 V, 12-bit: 0–4095
    pinMode(PIN_FLAMA, INPUT_PULLUP);  // pull-up interno → sin llama = HIGH                                                       
}
/**
 * @brief Lee temperatura y humedad desde el DHT11 y actualiza variables globales.
 * @details
 * Lee la temperatura y humedad usando la librería DHT. Si la lectura es inválida
 * (NaN), imprime un mensaje de error por Serial y NO actualiza las variables.
 * 
 * @return true Si la lectura fue válida y se actualizaron temperatura y humedad.
 * @return false Si el DHT11 no respondió o devolvió valores inválidos (NaN).
 */
bool sensor_leerDHT(void) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (isnan(t) || isnan(h)) {
        Serial.println("[ERROR] DHT11 no responde");
        return false;
    }
    temperatura = t;
    humedad     = h;
    return true;
}
/**
 * @brief Lee el sensor de luz (LDR) por ADC y actualiza la variable global luzRaw.
 * @details
 * Realiza 4 lecturas consecutivas con analogRead(PIN_LDR) y calcula el promedio
 * (suma >> 2) para reducir ruido.
 * Actualiza: luzRaw (rango típico 0..4095 con resolución de 12 bits).
 */
void sensor_leerLuz(void) {
    uint32_t suma = 0;
    for (uint8_t i = 0; i < 4; i++) {
        suma += analogRead(PIN_LDR);
    }
    luzRaw = (int)(suma >> 2);  // promedio de 4
}
/**
 * @brief Lee el sensor de flama (MD-08 J) y actualiza la variable global flamaActiva.
 * @details
 * El pin DO del sensor es activo en LOW:
 * - LOW  -> llama detectada  (flamaActiva = true)
 * - HIGH -> sin llama        (flamaActiva = false)
 */
void sensor_leerFlama(void) {
    flamaActiva = (digitalRead(PIN_FLAMA) == HIGH);
}

