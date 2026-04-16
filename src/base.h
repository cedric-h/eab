#ifndef EAB_BASE_IMPL
#define EAB_BASE_IMPL

#include <stdint.h>
#include <string.h>

#define countof(arr) ( sizeof(arr) / sizeof((arr)[0]) )
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define sign(a) (((a) < 0) ? -1 : 1)

extern int eab_mouse_cursor;

typedef struct { char *str; size_t len; } String;
#define STRING(x) ((String) { x, sizeof(x)-1 })

typedef struct { uint8_t r, g, b, a; } Color;

static float lerpf(float start, float end, float amount) {
    return start + amount*(end - start);
}
static float inv_lerpf(float min, float max, float p) {
    return (p - min) / (max - min);
}

static double lerp(double start, double end, double amount) {
    return start + amount*(end - start);
}
static double inv_lerp(double min, double max, double p) {
    return (p - min) / (max - min);
}
#endif
