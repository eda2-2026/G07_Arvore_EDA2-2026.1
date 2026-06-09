#include "tempo.h"

#include <time.h>

double tempo_agora_ms(void) {
#if defined(CLOCK_MONOTONIC)
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
  }
#endif
  return (double)clock() * 1000.0 / (double)CLOCKS_PER_SEC;
}
