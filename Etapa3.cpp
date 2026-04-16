const int LED_RED    = 8;
const int LED_YELLOW = 9;
const int LED_GREEN  = 10;
const int BTN_PIN    = 7;
const int POT_PIN    = A0;

bool modoNoturno = false;
int  ciclos      = 0;

// ── Helpers ──────────────────────────────────────────────

void apagaTodos() {
  digitalWrite(LED_RED,    LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN,  LOW);
}

void logSerial(String msg) {
  Serial.print("[");
  Serial.print(millis());
  Serial.print("ms] ");
  Serial.println(msg);
}

// ── Detecção de pressionamento longo (≥ 2s) ─────────────
// Lógica invertida: INPUT_PULLUP → pressionado = LOW

bool pressionamentoLongo() {
  if (digitalRead(BTN_PIN) == HIGH) return false;   // Botão solto

  unsigned long inicio = millis();
  while (digitalRead(BTN_PIN) == LOW) {              // Enquanto pressionado
    if (millis() - inicio >= 2000) {
      while (digitalRead(BTN_PIN) == LOW) delay(10); // Espera soltar
      return true;
    }
    delay(10);
  }
  return false;
}

// ── Pressionamento curto (pausa) ─────────────────────────

bool pressionamentoCurto() {
  return (digitalRead(BTN_PIN) == LOW);  // Pressionado = LOW com pull-up
}

// ── Espera responsiva (verifica botão a cada 50ms) ───────

// Retorna: 0 = tempo completo, 1 = modo trocado, 2 = pausado
int esperaComVerificacao(int tempoTotal) {
  int passos = tempoTotal / 50;

  for (int i = 0; i < passos; i++) {
    if (pressionamentoLongo()) {
      modoNoturno = !modoNoturno;
      apagaTodos();
      if (modoNoturno) {
        logSerial("Modo noturno ATIVADO");
      } else {
        logSerial("Modo semaforo RETOMADO");
      }
      return 1;
    }

    if (pressionamentoCurto()) {
      apagaTodos();
      logSerial("PAUSADO");
      // Espera soltar o botão (debounce simples)
      while (digitalRead(BTN_PIN) == LOW) delay(10);
      delay(200); // Debounce extra
      // Espera próximo pressionamento curto para retomar
      logSerial("Aguardando retomar...");
      while (true) {
        if (pressionamentoLongo()) {
          modoNoturno = !modoNoturno;
          apagaTodos();
          if (modoNoturno) {
            logSerial("Modo noturno ATIVADO");
          } else {
            logSerial("Modo semaforo RETOMADO");
          }
          return 1;
        }
        if (digitalRead(BTN_PIN) == LOW) {
          while (digitalRead(BTN_PIN) == LOW) delay(10);
          delay(200);
          logSerial("RETOMADO");
          return 2;
        }
        delay(10);
      }
    }

    delay(50);
  }
  return 0;
}

// ── Setup ────────────────────────────────────────────────

void setup() {
  pinMode(LED_RED,    OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN,  OUTPUT);
  pinMode(BTN_PIN,    INPUT_PULLUP);  // ✅ Pull-up interno ativado
  Serial.begin(9600);
  logSerial("Sistema iniciado");
}

// ── Loop principal ───────────────────────────────────────

void loop() {
  // Verifica troca de modo
  if (pressionamentoLongo()) {
    modoNoturno = !modoNoturno;
    apagaTodos();
    if (modoNoturno) {
      logSerial("Modo noturno ATIVADO");
    } else {
      logSerial("Modo semaforo RETOMADO");
    }
    return;
  }

  // ── Modo Noturno ──────────────────────────────────────

  if (modoNoturno) {
    Serial.println("MODO NOTURNO");

    digitalWrite(LED_YELLOW, HIGH);
    if (esperaComVerificacao(500) != 0) return;

    digitalWrite(LED_YELLOW, LOW);
    if (esperaComVerificacao(500) != 0) return;

    return;
  }

  // ── Modo Semáforo ─────────────────────────────────────

  // VERDE
  int intervalo = map(analogRead(POT_PIN), 0, 1023, 200, 2000);
  logSerial("Verde | Intervalo: " + String(intervalo) + "ms");
  apagaTodos();
  digitalWrite(LED_GREEN, HIGH);          // ✅ Verde correto
  if (esperaComVerificacao(intervalo) != 0) return;

  // AMARELO
  intervalo = map(analogRead(POT_PIN), 0, 1023, 200, 2000);
  logSerial("Amarelo | Intervalo: " + String(intervalo) + "ms");
  apagaTodos();
  digitalWrite(LED_YELLOW, HIGH);
  if (esperaComVerificacao(intervalo) != 0) return;

  // VERMELHO
  intervalo = map(analogRead(POT_PIN), 0, 1023, 200, 2000);
  logSerial("Vermelho | Intervalo: " + String(intervalo) + "ms");
  apagaTodos();
  digitalWrite(LED_RED, HIGH);            // ✅ Vermelho correto
  if (esperaComVerificacao(intervalo) != 0) return;

  // Contagem de ciclos
  ciclos++;
  logSerial("Ciclos completos: " + String(ciclos));
  if (ciclos % 5 == 0) {
    logSerial("*** ALERTA: " + String(ciclos) + " ciclos completados! ***");
  }
}
