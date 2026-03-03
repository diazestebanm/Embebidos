#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <DHT.h>
#include "pinout.h"

// ── DHT ───────────────────────────────────────────────────────
#define TIPO_DHT        DHT11

// ── Variables de lectura (acceso externo) ─────────────────────
extern float temperatura;
extern float humedad;
extern int   luzRaw;
extern bool flamaActiva;

// ── API pública ───────────────────────────────────────────────
void sensor_init(void);
bool sensor_leerDHT(void);   // false si DHT11 no responde
void sensor_leerLuz(void);   // actualiza luzRaw desde ADC
void sensor_leerFlama(void);  // actualiza flamaActiva (DO activo LOW)

#endif // SENSOR_H
