#ifndef EAB_BASE_IMPL
#define EAB_BASE_IMPL

#include <stdint.h>
#include <string.h>

#define countof(arr) ( sizeof(arr) / sizeof((arr)[0]) )
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

typedef struct { char *str; size_t len; } String;
#define STRING(x) ((String) { x, sizeof(x)-1 })

typedef struct { uint8_t r, g, b, a; } Color;

// Calculate linear interpolation between two floats
static float lerp(float start, float end, float amount) {
    return start + amount*(end - start);
}
#endif
