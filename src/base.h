#include <stdint.h>

#define countof(arr) ( sizeof(arr) / sizeof((arr)[0]) )
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

#ifndef EAB_BASE_IMPL
#define EAB_BASE_IMPL

typedef struct { uint8_t r, g, b, a; } Color;

// Calculate linear interpolation between two floats
static float lerp(float start, float end, float amount) {
    return start + amount*(end - start);
}
#endif
