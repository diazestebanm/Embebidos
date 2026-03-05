#ifndef FSM_H
#define FSM_H

#include <Arduino.h>
#include <StateMachineLib.h>
#include "types.h"

// ── Objeto FSM ────────────────────────────────────────────────
extern StateMachine maquina;

// ── Variables de estado compartido ───────────────────────────
extern Input input;

// ── API pública ───────────────────────────────────────────────
void fsm_setup(void);

#endif // FSM_H
