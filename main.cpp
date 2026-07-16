/*
  IL POSTINO - Luis Bacalov
  Adaptación monofónica 8-bit para Arduino Nano
  Basada en la partitura para piano proporcionada por el usuario.

  El Arduino Nano y tone() solo reproducen una frecuencia simultánea,
  por lo que los acordes fueron reducidos a su voz superior o a un
  pequeño arpegio cuando era musicalmente importante.

  CONEXIONES
  ------------------------------------------------------------
  Piezo pasivo:
    D10 -> resistencia de 100 a 220 ohm -> positivo del piezo
    GND -> negativo del piezo

  LED grave:
    D3 -> resistencia de 220 a 330 ohm -> ánodo
    cátodo -> GND

  LED medio:
    D4 -> resistencia de 220 a 330 ohm -> ánodo
    cátodo -> GND

  LED agudo:
    D5 -> resistencia de 220 a 330 ohm -> ánodo
    cátodo -> GND

  RANGOS VISUALES
  ------------------------------------------------------------
  Grave:  notas inferiores a DO5
  Medio:  DO5 hasta SI5
  Agudo:  DO6 en adelante

  REPRODUCCIÓN
  ------------------------------------------------------------
  La obra se reproduce una sola vez al encender el Nano o al
  presionar RESET.
*/

#include <Arduino.h>
#include <avr/pgmspace.h>

// Pines
const uint8_t PIN_PIEZO = 10;
const uint8_t LED_GRAVE = 3;
const uint8_t LED_MEDIO = 4;
const uint8_t LED_AGUDO = 5;

// Silencio
const uint16_t SILENCIO = 0;

// Notas usadas
const uint16_t DO4    = 262;
const uint16_t RE4    = 294;
const uint16_t RE_S4  = 311;  // MI bemol 4
const uint16_t MI4    = 330;
const uint16_t FA4    = 349;
const uint16_t FA_S4  = 370;
const uint16_t SOL4   = 392;
const uint16_t SOL_S4 = 415;  // LA bemol 4
const uint16_t LA4    = 440;
const uint16_t LA_S4  = 466;  // SI bemol 4
const uint16_t SI4    = 494;

const uint16_t DO5    = 523;
const uint16_t DO_S5  = 554;  // DO sostenido / RE bemol 5
const uint16_t RE5    = 587;
const uint16_t RE_S5  = 622;  // MI bemol 5
const uint16_t MI5    = 659;
const uint16_t FA5    = 698;
const uint16_t SOL5   = 784;
const uint16_t SOL_S5 = 831;  // LA bemol 5
const uint16_t LA5    = 880;
const uint16_t LA_S5  = 932;  // SI bemol 5
const uint16_t SI5    = 988;

const uint16_t DO6    = 1047;
const uint16_t RE6    = 1175;
const uint16_t RE_S6  = 1245; // MI bemol 6
const uint16_t MI6    = 1319;
const uint16_t FA6    = 1397;
const uint16_t SOL6   = 1568;
const uint16_t LA6    = 1760;

// Límites de los LEDs
const uint16_t LIMITE_MEDIO = DO5;
const uint16_t LIMITE_AGUDO = DO6;

struct Evento {
  uint16_t frecuencia;
  uint8_t unidades;
};

#define N(nota, duracion) {nota, duracion}

/*
  Duraciones expresadas en semicorcheas:

   1 = semicorchea
   2 = corchea
   4 = negra
   6 = negra con puntillo
   8 = blanca
  10 = blanca ligada a corchea
  12 = blanca con puntillo
  16 = redonda
  20 = redonda ligada a negra
*/

// Compases 1 a 22 - Moderato, negra = 60
const Evento SECCION_A[] PROGMEM = {
  // 1
  N(LA_S4, 8), N(LA4, 8),

  // 2
  N(SOL4, 16),

  // 3
  N(LA_S4, 8), N(LA4, 8),

  // 4
  N(SOL4, 16),

  // 5
  N(LA_S4, 8), N(LA4, 8),

  // 6
  N(SOL4, 16),

  // 7
  N(SILENCIO, 10), N(MI5, 2), N(SOL5, 2), N(MI5, 2),

  // 8
  N(SOL5, 10), N(MI5, 2), N(LA5, 2), N(MI5, 2),

  // 9
  N(SOL5, 10), N(SOL5, 2), N(LA5, 2), N(SOL5, 2),

  // 10
  N(SOL5, 6), N(FA5, 2), N(MI5, 2),
  N(FA5, 2), N(DO6, 2), N(FA5, 2),

  // 11
  N(MI5, 10), N(MI5, 2), N(FA5, 2), N(MI5, 2),

  // 12
  N(MI5, 6), N(RE5, 2), N(DO_S5, 2),
  N(RE5, 2), N(SOL5, 2), N(SI4, 2),

  // 13
  N(RE5, 4), N(DO5, 4),
  N(SI4, 2), N(DO5, 2), N(MI5, 2), N(SOL5, 2),

  // 14
  N(DO6, 4), N(SI5, 2), N(DO6, 2),
  N(LA5, 6), N(SI5, 2),

  // 15
  N(SOL5, 10), N(MI4, 2), N(SOL4, 2), N(MI4, 2),

  // 16
  N(SOL4, 2), N(MI6, 2), N(SOL6, 2), N(MI6, 2),
  N(SOL6, 2), N(MI4, 2), N(LA4, 2), N(MI4, 2),

  // 17
  N(SOL4, 2), N(RE6, 2), N(LA6, 2), N(RE6, 2),
  N(SOL6, 2), N(SOL4, 2), N(LA4, 2), N(SOL4, 2),

  // 18
  N(LA4, 6), N(SOL4, 2), N(FA4, 2),
  N(MI4, 2), N(DO5, 2), N(MI4, 2),

  // 19
  N(FA4, 8), N(SILENCIO, 2),
  N(FA5, 2), N(SOL5, 2), N(FA5, 2),

  // 20
  N(MI5, 2), N(SOL4, 2), N(MI5, 2), N(SOL4, 2),
  N(MI5, 2), N(SOL5, 2), N(MI6, 2), N(SOL5, 2),

  // 21
  N(MI6, 8), N(SILENCIO, 2),
  N(MI5, 2), N(FA5, 2), N(MI5, 2),

  // 22
  N(RE5, 2), N(SOL4, 2), N(RE5, 2), N(SOL4, 2),
  N(RE5, 2), N(SOL5, 2), N(RE6, 2), N(SOL5, 2)
};

// Compás 23 - ritardando
const Evento RITARDANDO_1[] PROGMEM = {
  N(RE6, 4), N(RE5, 4), N(DO5, 4), N(SI4, 4)
};

// Compases 24 a 32 - a tempo
const Evento SECCION_B[] PROGMEM = {
  // 24
  N(DO5, 10), N(DO5, 2), N(RE_S5, 2), N(DO5, 2),

  // 25
  N(RE_S5, 10), N(RE_S5, 2), N(SOL5, 2), N(RE_S5, 2),

  // 26
  N(SOL_S5, 10), N(SOL_S5, 2), N(DO6, 2), N(SOL_S5, 2),

  // 27
  N(RE_S6, 10), N(RE_S5, 2), N(FA5, 2), N(RE_S5, 2),

  // 28 y comienzo del 29:
  // RE bemol queda ligado a través de la barra de compás.
  N(RE_S5, 2), N(DO_S5, 2), N(DO_S5, 2), N(DO5, 2),
  N(DO_S5, 12),

  // Resto del 29
  N(DO5, 4), N(LA_S4, 6), N(SOL_S4, 2),

  // 30
  N(DO5, 16),

  // 31
  N(RE_S4, 4), N(DO6, 4), N(SI5, 4), N(SOL_S5, 4),

  // 32
  N(SOL5, 2), N(FA5, 2), N(FA5, 2), N(MI5, 2),
  N(FA5, 6), N(MI4, 2)
};

// Compases 33 a 35 - segundo ritardando
const Evento RITARDANDO_2[] PROGMEM = {
  // 33
  N(SOL4, 2), N(FA4, 2), N(FA4, 2), N(MI4, 2),
  N(FA4, 6), N(MI5, 2),

  // 34 y comienzo del 35:
  // FA5 queda ligado a través de la barra.
  N(SOL5, 2), N(FA5, 2), N(FA5, 2), N(MI5, 2),
  N(FA5, 12),

  // Resto del 35
  N(MI5, 4), N(SOL5, 6), N(MI5, 2)
};

// Compases 36 a 48 - a tempo
const Evento SECCION_C[] PROGMEM = {
  // 36
  N(SOL5, 16),

  // 37
  N(SOL5, 4), N(MI5, 4), N(LA5, 6), N(MI5, 2),

  // 38
  N(SOL5, 8), N(SILENCIO, 2),
  N(SOL4, 2), N(RE5, 2), N(SOL4, 2),

  // 39
  N(RE5, 4), N(SOL5, 4), N(LA5, 6), N(SOL5, 2),

  // 40
  N(LA5, 12), N(SOL5, 4),

  // 41
  N(FA5, 4), N(MI5, 4), N(DO6, 6), N(MI5, 2),

  // 42 y comienzo del 43:
  // FA5 queda ligado y se sostiene bajo la fermata.
  N(FA5, 20),

  // Resto del 43
  N(FA5, 4), N(SOL5, 4), N(FA5, 4),

  // 44
  N(MI5, 2), N(SOL4, 2), N(MI5, 2), N(SOL4, 2),
  N(MI5, 2), N(SOL5, 2), N(MI6, 2), N(SOL5, 2),

  // 45
  N(MI6, 4), N(MI5, 4), N(FA5, 4), N(MI5, 4),

  // 46
  N(RE5, 2), N(SOL4, 2), N(RE5, 2), N(SOL4, 2),
  N(RE5, 2), N(SOL5, 2), N(RE6, 2), N(SOL5, 2),

  // 47
  N(RE6, 8), N(RE5, 8),

  // 48
  N(DO5, 8), N(SI4, 8)
};

// Compases 49 y 50 - negra = 40
const Evento CODA[] PROGMEM = {
  // 49
  N(DO5, 8),
  N(MI4, 2), N(FA4, 2), N(FA_S4, 2), N(SOL4, 2),

  // 50 - acorde final reducido a su nota superior
  N(MI6, 20)
};

void apagarLeds() {
  digitalWrite(LED_GRAVE, LOW);
  digitalWrite(LED_MEDIO, LOW);
  digitalWrite(LED_AGUDO, LOW);
}

void indicarRango(uint16_t frecuencia) {
  apagarLeds();

  if (frecuencia == SILENCIO) {
    return;
  }

  if (frecuencia < LIMITE_MEDIO) {
    digitalWrite(LED_GRAVE, HIGH);
  }
  else if (frecuencia < LIMITE_AGUDO) {
    digitalWrite(LED_MEDIO, HIGH);
  }
  else {
    digitalWrite(LED_AGUDO, HIGH);
  }
}

uint32_t duracionEnMs(uint8_t unidades, uint16_t bpm) {
  // Una negra contiene cuatro semicorcheas.
  return (60000UL * unidades) / (bpm * 4UL);
}

uint8_t articulacion(uint8_t unidades) {
  /*
    Las notas cortas se separan un poco más para que el piezo
    no las fusione. Las notas largas se sostienen casi enteras.
  */
  if (unidades <= 2) {
    return 82;
  }

  if (unidades <= 4) {
    return 88;
  }

  if (unidades <= 8) {
    return 93;
  }

  return 96;
}

void reproducirEvento(
  uint16_t frecuencia,
  uint8_t unidades,
  uint16_t bpm
) {
  const uint32_t duracionTotal = duracionEnMs(unidades, bpm);

  if (frecuencia == SILENCIO) {
    noTone(PIN_PIEZO);
    apagarLeds();
    delay(duracionTotal);
    return;
  }

  const uint32_t duracionSonido =
    duracionTotal * articulacion(unidades) / 100UL;

  indicarRango(frecuencia);
  tone(PIN_PIEZO, frecuencia);

  delay(duracionSonido);

  noTone(PIN_PIEZO);
  apagarLeds();

  delay(duracionTotal - duracionSonido);
}

template <size_t CANTIDAD>
void reproducirSeccion(
  const Evento (&seccion)[CANTIDAD],
  uint16_t bpm
) {
  for (size_t i = 0; i < CANTIDAD; i++) {
    Evento evento;

    memcpy_P(
      &evento,
      &seccion[i],
      sizeof(Evento)
    );

    reproducirEvento(
      evento.frecuencia,
      evento.unidades,
      bpm
    );
  }
}

void animacionInicial() {
  digitalWrite(LED_GRAVE, HIGH);
  delay(180);

  digitalWrite(LED_MEDIO, HIGH);
  delay(180);

  digitalWrite(LED_AGUDO, HIGH);
  delay(250);

  apagarLeds();
  delay(500);
}

void animacionFinal() {
  for (uint8_t i = 0; i < 3; i++) {
    digitalWrite(LED_GRAVE, HIGH);
    digitalWrite(LED_MEDIO, HIGH);
    digitalWrite(LED_AGUDO, HIGH);

    delay(180);
    apagarLeds();
    delay(180);
  }
}

void tocarIlPostino() {
  reproducirSeccion(SECCION_A, 60);
  reproducirSeccion(RITARDANDO_1, 52);
  reproducirSeccion(SECCION_B, 60);
  reproducirSeccion(RITARDANDO_2, 52);
  reproducirSeccion(SECCION_C, 60);
  reproducirSeccion(CODA, 40);

  noTone(PIN_PIEZO);
  apagarLeds();
}

void setup() {
  pinMode(PIN_PIEZO, OUTPUT);

  pinMode(LED_GRAVE, OUTPUT);
  pinMode(LED_MEDIO, OUTPUT);
  pinMode(LED_AGUDO, OUTPUT);

  noTone(PIN_PIEZO);
  apagarLeds();

  delay(500);

  animacionInicial();
  tocarIlPostino();
  animacionFinal();
}

void loop() {
  // Se reproduce una sola vez.
  // Para volver a escucharla, presionar RESET.
}

