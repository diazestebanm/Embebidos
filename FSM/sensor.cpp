#include "sensor.h"

// DHT como variable interna (static = encapsulada en este módulo)
static DHT dht(PIN_DHT, TIPO_DHT);

// Variables exportadas
float temperatura = 0.0f;
float humedad     = 0.0f;
int   luzRaw      = 0;
bool flamaActiva = false;

void sensor_init(void) {
    dht.begin();

    // ── Parche LDR: configurar ADC explícitamente ─────────────
    analogReadResolution(12);                              // 12-bit: 0–4095
    analogSetPinAttenuation(PIN_LDR, ADC_11db);    // seguir midiendo 0–3.3 V,
                                                           // rango 0–3.3 V, 12-bit: 0–4095
    pinMode(PIN_FLAMA, INPUT_PULLUP);  // pull-up interno → sin llama = HIGH                                                       
}

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

void sensor_leerLuz(void) {
    uint32_t suma = 0;
    for (uint8_t i = 0; i < 4; i++) {
        suma += analogRead(PIN_LDR);
    }
    luzRaw = (int)(suma >> 2);  // promedio de 4
}

void sensor_leerFlama(void) {
    flamaActiva = (digitalRead(PIN_FLAMA) == HIGH);
}
