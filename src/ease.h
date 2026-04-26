#ifndef __EAB_EASE_IMPL
#define __EAB_EASE_IMPL

static float ease_in_back(float x) {
    float c1 = 1.70158;
    float c3 = c1 + 1;
    return c3 * x * x * x - c1 * x * x;
}

static float ease_out_circ(float x) {
    return sqrtf(1 - powf(x - 1, 2));
}

#endif
