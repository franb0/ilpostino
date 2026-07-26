/*
  IL POSTINO - Luis Bacalov
  Version monofonica mejorada para Arduino Nano y piezo pasivo.

  MEJORAS MUSICALES
  ------------------------------------------------------------
  - Tempo principal de 70 BPM, tomado del archivo MIDI.
  - Ritardandos progresivos, sin cambios bruscos de velocidad.
  - Articulacion diferente para notas cortas, normales y largas.
  - Ligados casi continuos en frases seleccionadas.
  - Vibrato muy sutil solamente en notas largas.
  - Fermatas y final mas expresivos.

  CONEXIONES (sin cambios)
  ------------------------------------------------------------
  Piezo pasivo:
    D10 -> resistencia de 100 a 220 ohm -> positivo del piezo
    GND -> negativo del piezo

  LED grave:
    D3 -> resistencia de 220 a 330 ohm -> anodo
    catodo -> GND

  LED medio:
    D4 -> resistencia de 220 a 330 ohm -> anodo
    catodo -> GND

  LED agudo:
    D5 -> resistencia de 220 a 330 ohm -> anodo
    catodo -> GND

  No utiliza potenciometro.
  La obra se reproduce una vez al encender o presionar RESET.
*/

#include <Arduino.h>
#include <avr/pgmspace.h>

// Pines
const uint8_t PIN_PIEZO = 10;
const uint8_t LED_GRAVE = 3;
const uint8_t LED_MEDIO = 4;
const uint8_t LED_AGUDO = 5;

// Tempos
const uint16_t BPM_PRINCIPAL = 70;
const uint16_t BPM_RIT_1_FINAL = 56;
const uint16_t BPM_RIT_2_FINAL = 52;
const uint16_t BPM_CODA_INICIAL = 56;
const uint16_t BPM_CODA_FINAL = 42;

// Silencio
const uint16_t SILENCIO = 0;

// Frecuencias de las notas utilizadas
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
const uint16_t DO_S5  = 554;  // RE bemol 5
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

// Limites visuales de los LEDs
const uint16_t LIMITE_MEDIO = DO5;
const uint16_t LIMITE_AGUDO = DO6;

enum Estilo : uint8_t {
  AUTOMATICO,
  LIGADO,
  VIBRATO,
  MARCATO
};

struct Evento {
  uint16_t frecuencia;
  uint8_t unidades;
  uint8_t estilo;
};

#define N(nota, duracion) {nota, duracion, AUTOMATICO}
#define L(nota, duracion) {nota, duracion, LIGADO}
#define V(nota, duracion) {nota, duracion, VIBRATO}
#define M(nota, duracion) {nota, duracion, MARCATO}

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
  20 = redonda ligada a negra / fermata
*/

// Compases 1 a 22
const Evento SECCION_A[] PROGMEM = {
  // 1
  L(LA_S4, 8), L(LA4, 8),

  // 2
  V(SOL4, 16),

  // 3
  L(LA_S4, 8), L(LA4, 8),

  // 4
  V(SOL4, 16),

  // 5
  L(LA_S4, 8), L(LA4, 8),

  // 6
  V(SOL4, 16),

  // 7
  N(SILENCIO, 10), N(MI5, 2), L(SOL5, 2), L(MI5, 2),

  // 8
  V(SOL5, 10), N(MI5, 2), L(LA5, 2), L(MI5, 2),

  // 9
  V(SOL5, 10), N(SOL5, 2), L(LA5, 2), L(SOL5, 2),

  // 10
  L(SOL5, 6), L(FA5, 2), L(MI5, 2),
  L(FA5, 2), L(DO6, 2), L(FA5, 2),

  // 11
  V(MI5, 10), N(MI5, 2), L(FA5, 2), L(MI5, 2),

  // 12
  L(MI5, 6), L(RE5, 2), L(DO_S5, 2),
  L(RE5, 2), L(SOL5, 2), L(SI4, 2),

  // 13
  L(RE5, 4), L(DO5, 4),
  L(SI4, 2), L(DO5, 2), L(MI5, 2), L(SOL5, 2),

  // 14
  L(DO6, 4), L(SI5, 2), L(DO6, 2),
  V(LA5, 6), L(SI5, 2),

  // 15
  V(SOL5, 10), N(MI4, 2), L(SOL4, 2), L(MI4, 2),

  // 16
  L(SOL4, 2), L(MI6, 2), L(SOL6, 2), L(MI6, 2),
  L(SOL6, 2), L(MI4, 2), L(LA4, 2), L(MI4, 2),

  // 17
  L(SOL4, 2), L(RE6, 2), L(LA6, 2), L(RE6, 2),
  L(SOL6, 2), L(SOL4, 2), L(LA4, 2), L(SOL4, 2),

  // 18
  L(LA4, 6), L(SOL4, 2), L(FA4, 2),
  L(MI4, 2), L(DO5, 2), L(MI4, 2),

  // 19
  V(FA4, 8), N(SILENCIO, 2),
  N(FA5, 2), L(SOL5, 2), L(FA5, 2),

  // 20
  L(MI5, 2), L(SOL4, 2), L(MI5, 2), L(SOL4, 2),
  L(MI5, 2), L(SOL5, 2), L(MI6, 2), L(SOL5, 2),

  // 21
  V(MI6, 8), N(SILENCIO, 2),
  N(MI5, 2), L(FA5, 2), L(MI5, 2),

  // 22
  L(RE5, 2), L(SOL4, 2), L(RE5, 2), L(SOL4, 2),
  L(RE5, 2), L(SOL5, 2), L(RE6, 2), L(SOL5, 2)
};

// Compas 23: ritardando progresivo de 70 a 56 BPM
const Evento RITARDANDO_1[] PROGMEM = {
  L(RE6, 4), L(RE5, 4), L(DO5, 4), V(SI4, 4)
};

// Compases 24 a 32
const Evento SECCION_B[] PROGMEM = {
  // 24
  V(DO5, 10), N(DO5, 2), L(RE_S5, 2), L(DO5, 2),

  // 25
  V(RE_S5, 10), N(RE_S5, 2), L(SOL5, 2), L(RE_S5, 2),

  // 26
  V(SOL_S5, 10), N(SOL_S5, 2), L(DO6, 2), L(SOL_S5, 2),

  // 27
  V(RE_S6, 10), N(RE_S5, 2), L(FA5, 2), L(RE_S5, 2),

  // 28 y comienzo del 29
  L(RE_S5, 2), L(DO_S5, 2), L(DO_S5, 2), L(DO5, 2),
  V(DO_S5, 12),

  // Resto del 29
  L(DO5, 4), V(LA_S4, 6), L(SOL_S4, 2),

  // 30
  V(DO5, 16),

  // 31
  L(RE_S4, 4), L(DO6, 4), L(SI5, 4), L(SOL_S5, 4),

  // 32
  L(SOL5, 2), L(FA5, 2), L(FA5, 2), L(MI5, 2),
  V(FA5, 6), L(MI4, 2)
};

// Compases 33 a 35: ritardando progresivo de 70 a 52 BPM
const Evento RITARDANDO_2[] PROGMEM = {
  // 33
  L(SOL4, 2), L(FA4, 2), L(FA4, 2), L(MI4, 2),
  V(FA4, 6), L(MI5, 2),

  // 34 y comienzo del 35
  L(SOL5, 2), L(FA5, 2), L(FA5, 2), L(MI5, 2),
  V(FA5, 12),

  // Resto del 35
  L(MI5, 4), V(SOL5, 6), L(MI5, 2)
};

// Compases 36 a 48
const Evento SECCION_C[] PROGMEM = {
  // 36
  V(SOL5, 16),

  // 37
  L(SOL5, 4), L(MI5, 4), V(LA5, 6), L(MI5, 2),

  // 38
  V(SOL5, 8), N(SILENCIO, 2),
  N(SOL4, 2), L(RE5, 2), L(SOL4, 2),

  // 39
  L(RE5, 4), L(SOL5, 4), V(LA5, 6), L(SOL5, 2),

  // 40
  V(LA5, 12), L(SOL5, 4),

  // 41
  L(FA5, 4), L(MI5, 4), V(DO6, 6), L(MI5, 2),

  // 42 y comienzo del 43: FA5 sostenido bajo la fermata
  V(FA5, 20),

  // Resto del 43
  L(FA5, 4), L(SOL5, 4), L(FA5, 4),

  // 44
  L(MI5, 2), L(SOL4, 2), L(MI5, 2), L(SOL4, 2),
  L(MI5, 2), L(SOL5, 2), L(MI6, 2), L(SOL5, 2),

  // 45
  L(MI6, 4), L(MI5, 4), L(FA5, 4), L(MI5, 4),

  // 46
  L(RE5, 2), L(SOL4, 2), L(RE5, 2), L(SOL4, 2),
  L(RE5, 2), L(SOL5, 2), L(RE6, 2), L(SOL5, 2),

  // 47
  L(RE6, 8), V(RE5, 8),

  // 48
  L(DO5, 8), V(SI4, 8)
};

// Compases 49 y 50: coda progresivamente mas lenta
const Evento CODA[] PROGMEM = {
  // 49
  L(DO5, 8),
  L(MI4, 2), L(FA4, 2), L(FA_S4, 2), L(SOL4, 2),

  // 50: nota superior del acorde final
  V(MI6, 20)
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

uint8_t porcentajeArticulacion(uint8_t unidades, Estilo estilo) {
  if (estilo == LIGADO) {
    return 99;
  }

  if (estilo == VIBRATO) {
    return 98;
  }

  if (estilo == MARCATO) {
    return 82;
  }

  // Articulacion automatica segun la duracion.
  if (unidades <= 2) {
    return 86;
  }

  if (unidades <= 4) {
    return 91;
  }

  if (unidades <= 8) {
    return 95;
  }

  return 97;
}

void sostenerTono(
  uint16_t frecuencia,
  uint32_t duracion,
  bool usarVibrato
) {
  // Las notas breves quedan limpias y estables.
  if (!usarVibrato || duracion < 500) {
    tone(PIN_PIEZO, frecuencia);
    delay(duracion);
    return;
  }

  // Ataque estable: el vibrato no comienza inmediatamente.
  uint32_t ataque = 150;
  if (ataque > duracion / 3) {
    ataque = duracion / 3;
  }

  tone(PIN_PIEZO, frecuencia);
  delay(ataque);

  uint32_t restante = duracion - ataque;
  const uint8_t PASO_MS = 30;
  const int8_t FORMA_VIBRATO[] = {0, 1, 1, 0, -1, -1};
  const uint8_t CANTIDAD_PASOS =
    sizeof(FORMA_VIBRATO) / sizeof(FORMA_VIBRATO[0]);

  // Desvio aproximado de 0,4 %. Es perceptible pero no desafina.
  uint16_t desvio = frecuencia / 250;
  if (desvio < 1) {
    desvio = 1;
  }

  uint8_t fase = 0;

  while (restante > 0) {
    uint16_t fragmento =
      restante > PASO_MS ? PASO_MS : restante;

    int32_t frecuenciaModulada =
      (int32_t)frecuencia +
      (int32_t)FORMA_VIBRATO[fase] * desvio;

    tone(PIN_PIEZO, (uint16_t)frecuenciaModulada);
    delay(fragmento);

    restante -= fragmento;
    fase = (fase + 1) % CANTIDAD_PASOS;
  }
}

void reproducirEvento(
  uint16_t frecuencia,
  uint8_t unidades,
  Estilo estilo,
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
    duracionTotal *
    porcentajeArticulacion(unidades, estilo) /
    100UL;

  indicarRango(frecuencia);
  sostenerTono(
    frecuencia,
    duracionSonido,
    estilo == VIBRATO
  );

  noTone(PIN_PIEZO);
  apagarLeds();

  delay(duracionTotal - duracionSonido);
}

template <size_t CANTIDAD>
uint16_t contarUnidades(const Evento (&seccion)[CANTIDAD]) {
  uint16_t total = 0;

  for (size_t i = 0; i < CANTIDAD; i++) {
    Evento evento;
    memcpy_P(&evento, &seccion[i], sizeof(Evento));
    total += evento.unidades;
  }

  return total;
}

uint16_t interpolarTempo(
  uint16_t bpmInicial,
  uint16_t bpmFinal,
  uint16_t unidadesTranscurridas,
  uint16_t unidadesTotales
) {
  if (bpmInicial == bpmFinal || unidadesTotales == 0) {
    return bpmInicial;
  }

  const int32_t diferencia =
    (int32_t)bpmFinal - (int32_t)bpmInicial;

  return (uint16_t)(
    (int32_t)bpmInicial +
    diferencia * unidadesTranscurridas / unidadesTotales
  );
}

template <size_t CANTIDAD>
void reproducirSeccion(
  const Evento (&seccion)[CANTIDAD],
  uint16_t bpmInicial,
  uint16_t bpmFinal
) {
  const uint16_t unidadesTotales = contarUnidades(seccion);
  uint16_t unidadesTranscurridas = 0;

  for (size_t i = 0; i < CANTIDAD; i++) {
    Evento evento;
    memcpy_P(&evento, &seccion[i], sizeof(Evento));

    const uint16_t bpmActual = interpolarTempo(
      bpmInicial,
      bpmFinal,
      unidadesTranscurridas,
      unidadesTotales
    );

    reproducirEvento(
      evento.frecuencia,
      evento.unidades,
      (Estilo)evento.estilo,
      bpmActual
    );

    unidadesTranscurridas += evento.unidades;
  }
}

template <size_t CANTIDAD>
void reproducirSeccion(
  const Evento (&seccion)[CANTIDAD],
  uint16_t bpm
) {
  reproducirSeccion(seccion, bpm, bpm);
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
  reproducirSeccion(SECCION_A, BPM_PRINCIPAL);

  reproducirSeccion(
    RITARDANDO_1,
    BPM_PRINCIPAL,
    BPM_RIT_1_FINAL
  );

  reproducirSeccion(SECCION_B, BPM_PRINCIPAL);

  reproducirSeccion(
    RITARDANDO_2,
    BPM_PRINCIPAL,
    BPM_RIT_2_FINAL
  );

  reproducirSeccion(SECCION_C, BPM_PRINCIPAL);

  reproducirSeccion(
    CODA,
    BPM_CODA_INICIAL,
    BPM_CODA_FINAL
  );

  noTone(PIN_PIEZO);
  apagarLeds();
}

const uint32_t TIEMPO_ESPERA =
  7UL * 60UL * 60UL * 1000UL;  // 7 horas = 25.200.000 ms

uint32_t momentoInicio;
bool alarmaReproducida = false;

void setup() {
  pinMode(PIN_PIEZO, OUTPUT);
  pinMode(LED_GRAVE, OUTPUT);
  pinMode(LED_MEDIO, OUTPUT);
  pinMode(LED_AGUDO, OUTPUT);

  noTone(PIN_PIEZO);
  apagarLeds();

  // El temporizador comienza al encender o pulsar RESET.
  momentoInicio = millis();
}

void loop() {
  if (
    !alarmaReproducida &&
    millis() - momentoInicio >= TIEMPO_ESPERA
  ) {
    alarmaReproducida = true;

    animacionInicial();
    tocarIlPostino();
    animacionFinal();
  }
}