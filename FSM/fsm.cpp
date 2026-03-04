#include "fsm.h"
#include "sensor.h"
#include "tasks.h"

// ═══════════════════════════════════════════════════════════════
// Variables globales del sistema
// ═══════════════════════════════════════════════════════════════
StateMachine maquina(6, 12);
Input input    = NONE;
static int   intentos = 0;

// ═══════════════════════════════════════════════════════════════
// Helpers de formato Serial (internos a fsm.cpp)
// ═══════════════════════════════════════════════════════════════
static void _ln()   { Serial.println(); }
static void _header(const char* estado, bool alarma = false) {
    _ln();
    if (alarma) {
        Serial.print("  \xE2\x95\x94\xE2\x95\x90[ "); Serial.print(estado);
        Serial.println(" ]\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90");
    } else {
        Serial.print("  \xE2\x94\x8C\xE2\x94\x80[ "); Serial.print(estado);
        Serial.println(" ]\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80");
    }
}

static void _fila(const char* lbl, const char* val) {
    Serial.print("  \xE2\x94\x82  ");
    Serial.print(lbl);
    Serial.println(val);
}

static void _footer(bool alarma = false) {
    if (alarma)
        Serial.println("  \xE2\x95\x9A\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90");
    else
        Serial.println("  \xE2\x94\x94\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80");
}

// ═══════════════════════════════════════════════════════════════
// INICIO
// ═══════════════════════════════════════════════════════════════
static void onEnterInicio(void) {
    _header("INICIO");
    _fila("LED     : ", "Verde  200/200 ms");
    _fila("ACCION  : ", "Esperando boton...");
    _footer();
    intentos = 0; 
    leds_apagar(); tareaVerdeOn.Start();
}
static void onLeaveInicio(void) { blinks_detener(); leds_apagar(); }

// ═══════════════════════════════════════════════════════════════
// MON_TEMP
// ═══════════════════════════════════════════════════════════════
static void onEnterMonTemp(void) {
    _header("MON_TEMP");
    sensor_leerFlama();
    if (sensor_leerDHT()) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%5.1f \xC2\xB0""C %s",
                 temperatura,
                 temperatura > TEMP_UMBRAL_ALERTA ? "[!! ALTA > 25]" : "[OK < 25    ]");
        _fila("Temp    : ", buf);
        if (temperatura > TEMP_UMBRAL_ALERTA) {
            _fila("Sig.    : ", ">> ALERTA inmediata");
            _footer(); input = TEMP_ALTA; return;
        }
    } else {
        _fila("Sensor  : ", "[ERROR] DHT11 no responde");
    }
    _fila("Sig.    : ", flamaActiva ? "MON_LUZ en 2s (llama!) | MON_HUM en 4s"
                                    : "MON_HUMEDAD en 4s");
    _footer();
    timeout2s.Start();
    timeoutTH.Start();
}
static void onLeaveMonTemp(void) { timeout2s.Stop(); timeoutTH.Stop(); }

// ═══════════════════════════════════════════════════════════════
// MON_HUMEDAD
// ═══════════════════════════════════════════════════════════════
static void onEnterMonHumedad(void) {
    _header("MON_HUMEDAD");
    if (sensor_leerDHT()) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%5.1f %% %s",
                 humedad,
                 humedad < HUM_UMBRAL_MIN ? "[!! BAJA < 60]" : "[OK > 60    ]");
        _fila("Humedad : ", buf);
        if (humedad < HUM_UMBRAL_MIN) {
            _fila("Sig.    : ", ">> ALERTA inmediata");
            _footer(); input = HUMEDAD_BAJA; return;
        }
    } else {
        _fila("Sensor  : ", "[ERROR] DHT11 no responde");
    }
    _fila("Sig.    : ", "MON_TEMP en 3 s");
    _footer();
    timeoutHT.Start();
}
static void onLeaveMonHumedad(void) { timeoutHT.Stop(); }

// ═══════════════════════════════════════════════════════════════
// MON_LUZ
// ═══════════════════════════════════════════════════════════════
static void onEnterMonLuz(void) {
    _header("MON_LUZ");
    sensor_leerLuz();
    char buf[36];
    snprintf(buf, sizeof(buf), "%4d / 4095 %s",
             luzRaw,
             luzRaw < LUZ_UMBRAL_MIN ? "[!! MUY ILUMINADO  ]" : "[OK poca luz        ]");
    _fila("Luz ADC : ", buf);
    if (luzRaw < LUZ_UMBRAL_MIN) {
        _fila("Sig.    : ", ">> ALERTA inmediata");
        _footer(); input = LUZ_BAJA; return;
    }

    _fila("Sig.    : ", "MON_TEMP en 5 s");
    _footer();
    timeout5s.Start();
}
static void onLeaveMonLuz(void) { timeout5s.Stop(); }

// ═══════════════════════════════════════════════════════════════
// ALERTA
// ═══════════════════════════════════════════════════════════════
static void onEnterAlerta(void) {
    intentos++;
    _header("ALERTA");
    char buf[24];
    snprintf(buf, sizeof(buf), "%d / %d", intentos, MAX_INTENTOS);
    _fila("Intento : ", buf);
    snprintf(buf, sizeof(buf), "%.1f \xC2\xB0""C", temperatura);
    _fila("Temp    : ", buf);
    _fila("LED     : ", "Azul parpadeando");

    leds_apagar(); tareaAzulOn.Start();

    if (intentos >= MAX_INTENTOS && temperatura > TEMP_UMBRAL_ALARMA) {
        _fila("Sig.    : ", ">> ALARMA  (3 intentos + Temp critica)");
        _footer(); input = TEMP_MUY_ALTA; return;
    }
    _fila("Sig.    : ", "MON_TEMP    en 3 s");
    _footer(); timeoutAlerta.Start();
}
static void onLeaveAlerta(void) {
    blinks_detener(); timeoutAlerta.Stop(); leds_apagar();
    if (intentos >= MAX_INTENTOS && input != TEMP_MUY_ALTA) {
        intentos = 0;
        _ln();
        Serial.println("  [INFO] 3 alertas sin temp critica -> contador reseteado");
    }
}

// ═══════════════════════════════════════════════════════════════
// ALARMA
// ═══════════════════════════════════════════════════════════════
static void onEnterAlarma(void) {
    _header("ALARMA", true);           // ← usa bordes dobles ╔═╗
    char buf[20];
    snprintf(buf, sizeof(buf), "%.1f \xC2\xB0""C  [!! > 30]", temperatura);
    _fila("Temp    : ", buf);
    _fila("LED     : ", "Rojo parpadeando");
    _fila("Reset   : ", "Presiona el boton");
    _footer(true);
    leds_apagar(); tareaRojoOn.Start();
}
static void onLeaveAlarma(void) {
    blinks_detener(); leds_apagar(); intentos = 0;
    Serial.println("\n [INFO] Sistema reseteado desde ALARMA");
}

// ═══════════════════════════════════════════════════════════════
// Configuración de transiciones + callbacks
// ═══════════════════════════════════════════════════════════════
void fsm_setup(void) {
    // ── Transiciones ────────────────────────────────────────────
    maquina.AddTransition(INICIO,      MON_TEMP,    []() { return input == BTN_PRESS;      });
    maquina.AddTransition(MON_TEMP,    ALERTA,      []() { return input == TEMP_ALTA;      });
    maquina.AddTransition(MON_TEMP,    MON_HUMEDAD, []() { return input == TIMEOUT_4S; }); // timeoutTH dispara TIMEOUT_4S
    maquina.AddTransition(MON_TEMP, MON_LUZ,  []() { return input == TIMEOUT_2S && flamaActiva; });
    maquina.AddTransition(MON_HUMEDAD, ALERTA,      []() { return input == HUMEDAD_BAJA;   });
    maquina.AddTransition(MON_HUMEDAD, MON_TEMP,    []() { return input == TIMEOUT_3S; }); // timeoutHT dispara TIMEOUT_3S
    maquina.AddTransition(MON_LUZ,     ALERTA,      []() { return input == LUZ_BAJA;       });
    maquina.AddTransition(MON_LUZ,  MON_TEMP, []() { return input == TIMEOUT_5S; });
    maquina.AddTransition(ALERTA,      ALARMA,      []() { return input == TEMP_MUY_ALTA;  });
    maquina.AddTransition(ALERTA,      MON_TEMP,    []() { return input == TIMEOUT_ALERTA; });
    maquina.AddTransition(ALARMA,      INICIO,      []() { return input == BTN_PRESS;      });

    // ── OnEntering ──────────────────────────────────────────────
    maquina.SetOnEntering(INICIO,      onEnterInicio);
    maquina.SetOnEntering(MON_TEMP,    onEnterMonTemp);
    maquina.SetOnEntering(MON_HUMEDAD, onEnterMonHumedad);
    maquina.SetOnEntering(MON_LUZ,     onEnterMonLuz);
    maquina.SetOnEntering(ALERTA,      onEnterAlerta);
    maquina.SetOnEntering(ALARMA,      onEnterAlarma);

    // ── OnLeaving ───────────────────────────────────────────────
    maquina.SetOnLeaving(INICIO,       onLeaveInicio);
    maquina.SetOnLeaving(MON_TEMP,     onLeaveMonTemp);
    maquina.SetOnLeaving(MON_HUMEDAD,  onLeaveMonHumedad);
    maquina.SetOnLeaving(MON_LUZ,      onLeaveMonLuz);
    maquina.SetOnLeaving(ALERTA,       onLeaveAlerta);
    maquina.SetOnLeaving(ALARMA,       onLeaveAlarma);
}
