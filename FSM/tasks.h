#ifndef TASKS_H
#define TASKS_H

#include <Arduino.h>
#include <AsyncTaskLib.h>
#include "pinout.h"
#include "types.h"
#include "sensor.h"

// ── AsyncTasks: Blinks ────────────────────────────────────────
extern AsyncTask tareaVerdeOn,  tareaVerdeOff;
extern AsyncTask tareaAzulOn,   tareaAzulOff;
extern AsyncTask tareaRojoOn,   tareaRojoOff;

// ── AsyncTasks: Timeouts ──────────────────────────────────────
extern AsyncTask timeout2s;
extern AsyncTask timeoutTH;    // Temp → Hum : 4 s
extern AsyncTask timeoutHT;    // Hum  → Temp: 3 s
extern AsyncTask timeout5s;
extern AsyncTask timeoutAlerta;

// ── LEDs ──────────────────────────────────────────────────────
void leds_init(void);
void leds_apagar(void);
void blinks_detener(void);

// ── Botón con debounce (sin delay) ────────────────────────────
void button_read(void);
void flama_read(void);   // polling MD-08 J sin delay

// ── Update centralizado de todas las tasks ────────────────────
void tasks_update(void);

#endif // TASKS_H
