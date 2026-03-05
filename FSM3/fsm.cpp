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
/**
 * @brief Imprime una línea en blanco en el puerto Serial.
 * @details
 * Helper interno para separar bloques de salida en el monitor serial.
 */
static void _ln()   { Serial.println(); }
/**
 * @brief Imprime el encabezado del bloque de estado en el monitor Serial.
 * @details
 * Muestra el nombre del estado y un marco decorativo. Si alarma=true usa
 * bordes dobles (estilo “crítico”) para el estado ALARMA.
 *
 * @param estado Nombre del estado a imprimir.
 * @param alarma Si true, imprime el encabezado con bordes dobles.
 */
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
/**
 * @brief Imprime una fila con etiqueta y valor en formato de tabla por Serial.
 * @details
 * Helper interno para mostrar pares "label : value" alineados dentro del bloque.
 *
 * @param lbl Etiqueta (por ejemplo "Temp    : ").
 * @param val Valor asociado (por ejemplo "25.0 °C [OK]").
 */
static void _fila(const char* lbl, const char* val) {
    Serial.print("  \xE2\x94\x82  ");
    Serial.print(lbl);
    Serial.println(val);
}
/**
 * @brief Imprime el pie del bloque de estado en el monitor Serial.
 * @details
 * Cierra el marco del bloque. Si alarma=true usa bordes dobles.
 *
 * @param alarma Si true, imprime el pie con bordes dobles.
 */
static void _footer(bool alarma = false) {
    if (alarma)
        Serial.println("  \xE2\x95\x9A\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90");
    else
        Serial.println("  \xE2\x94\x94\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80");
}

// ═══════════════════════════════════════════════════════════════
// INICIO
// ═══════════════════════════════════════════════════════════════
/**
 * @brief Callback OnEntering del estado INICIO.
 * @details
 * - Reinicia el contador de intentos.
 * - Apaga el LED RGB y arranca el parpadeo verde (tareaVerdeOn).
 * - Imprime en Serial que está esperando la pulsación del botón.
 */
static void onEnterInicio(void) {
    _header("INICIO");
    _fila("LED     : ", "Verde  200/200 ms");
    _fila("ACCION  : ", "Esperando boton...");
    _footer();
    intentos = 0; 
    leds_apagar(); tareaVerdeOn.Start();
}
/**
 * @brief Callback OnLeaving del estado INICIO.
 * @details
 * Detiene parpadeos y apaga el LED RGB para dejar limpio el cambio de estado.
 */
static void onLeaveInicio(void) { blinks_detener(); leds_apagar(); }

// ═══════════════════════════════════════════════════════════════
// MON_TEMP
// ═══════════════════════════════════════════════════════════════
/**
 * @brief Callback OnEntering del estado MON_TEMP.
 * @details
 * - Lee flama y DHT11.
 * - Si la temperatura supera TEMP_UMBRAL_ALERTA, publica input = TEMP_ALTA y retorna
 *   para provocar ALERTA inmediata.
 * - Si no hay alerta, arranca timeout2s y timeoutTH para controlar cambios de estado.
 * - Imprime lecturas y próxima acción en Serial.
 *
 * @note
 * Usa variables globales: temperatura, flamaActiva, input.
 */
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
/**
 * @brief Callback OnLeaving del estado MON_TEMP.
 * @details
 * Detiene los temporizadores asociados a MON_TEMP (timeout2s y timeoutTH) para evitar
 * eventos residuales al salir del estado.
 */
static void onLeaveMonTemp(void) { timeout2s.Stop(); timeoutTH.Stop(); }

// ═══════════════════════════════════════════════════════════════
// MON_HUMEDAD
// ═══════════════════════════════════════════════════════════════
/**
 * @brief Callback OnEntering del estado MON_HUMEDAD.
 * @details
 * - Lee el DHT11.
 * - Si la humedad es menor que HUM_UMBRAL_MIN, publica input = HUMEDAD_BAJA y retorna
 *   para provocar ALERTA inmediata.
 * - Si está OK, inicia timeoutHT para volver a MON_TEMP.
 * - Imprime lecturas y próxima acción en Serial.
 *
 * @note
 * Usa variables globales: humedad, input.
 */
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
/**
 * @brief Callback OnLeaving del estado MON_HUMEDAD.
 * @details
 * Detiene timeoutHT para evitar disparos cuando el estado ya no está activo.
 */
static void onLeaveMonHumedad(void) { timeoutHT.Stop(); }

// ═══════════════════════════════════════════════════════════════
// MON_LUZ
// ═══════════════════════════════════════════════════════════════
/**
 * @brief Callback OnEntering del estado MON_LUZ.
 * @details
 * - Lee el ADC del LDR (luzRaw) y el sensor de flama (flamaActiva).
 * - Si luzRaw < LUZ_UMBRAL_MIN o flamaActiva es true, publica input = LUZ_BAJA y retorna
 *   para provocar ALERTA inmediata.
 * - Si está OK, inicia timeout5s para volver a MON_TEMP.
 * - Imprime lecturas y próxima acción en Serial.
 *
 * @note
 * Usa variables globales: luzRaw, flamaActiva, input.
 */
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
/**
 * @brief Callback OnLeaving del estado MON_LUZ.
 * @details
 * Detiene timeout5s para evitar eventos fuera de contexto al cambiar de estado.
 */
static void onLeaveMonLuz(void) { timeout5s.Stop(); }

// ═══════════════════════════════════════════════════════════════
// ALERTA
// ═══════════════════════════════════════════════════════════════
/**
 * @brief Callback OnEntering del estado ALERTA.
 * @details
 * - Incrementa el contador de intentos.
 * - Enciende el parpadeo azul (tareaAzulOn).
 * - Si intentos >= MAX_INTENTOS y temperatura > TEMP_UMBRAL_ALARMA, publica
 *   input = TEMP_MUY_ALTA y retorna para entrar a ALARMA.
 * - Si no, inicia timeoutAlerta para volver a MON_TEMP.
 * - Imprime diagnóstico (intentos, temperatura) y decisión en Serial.
 *
 * @note
 * Usa variables globales: intentos, temperatura, input.
 */
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
/**
 * @brief Callback OnLeaving del estado ALERTA.
 * @details
 * - Detiene parpadeos y timeoutAlerta, y apaga el LED RGB.
 * - Si se alcanzó MAX_INTENTOS pero no se entra a ALARMA, reinicia intentos e imprime
 *   un mensaje informativo por Serial.
 */
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
/**
 * @brief Callback OnEntering del estado ALARMA.
 * @details
 * - Imprime el bloque con bordes dobles (modo crítico).
 * - Enciende parpadeo rojo (tareaRojoOn).
 * - Muestra instrucción de reset por botón.
 *
 * @note
 * Usa variable global temperatura para el mensaje de estado crítico.
 */
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
/**
 * @brief Callback OnLeaving del estado ALARMA.
 * @details
 * - Detiene parpadeos, apaga LED RGB y reinicia el contador de intentos.
 * - Imprime por Serial que el sistema fue reseteado desde ALARMA.
 */
static void onLeaveAlarma(void) {
    blinks_detener(); leds_apagar(); intentos = 0;
    Serial.println("\n [INFO] Sistema reseteado desde ALARMA");
}

// ═══════════════════════════════════════════════════════════════
// Configuración de transiciones + callbacks
// ═══════════════════════════════════════════════════════════════
/**
 * @brief Configura la máquina de estados: transiciones y callbacks de entrada/salida.
 * @details
 * Registra:
 * - Transiciones entre estados evaluando la variable global input (y flamaActiva en MON_TEMP -> MON_LUZ).
 * - Funciones OnEntering/OnLeaving para cada estado.
 *
 * @note
 * Debe llamarse una única vez en setup() antes de ejecutar maquina.SetState().
 */
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
