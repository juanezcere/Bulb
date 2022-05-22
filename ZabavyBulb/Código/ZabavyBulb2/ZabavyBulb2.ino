#include "Zabavy.h"

void setup(void) {
  Zabavy.begin();
  Zabavy.start();
}

void loop(void) {
  Zabavy.looper();
}
