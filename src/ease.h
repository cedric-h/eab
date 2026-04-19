#ifndef __EAB_EASE_IMPL
#define __EAB_EASE_IMPL

float ease_in_back(float x) {
    float c1 = 1.70158;
    float c3 = c1 + 1;
    return c3 * x * x * x - c1 * x * x;
}

#endif
