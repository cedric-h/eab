#include "base.h"

typedef struct {
    float x, y;
} svg_AspectRatio;

typedef struct {
    struct { float x, y; } pos;
    Color color;
} svg_Vertex;

typedef struct {
    uint16_t a, b, c;
} svg_Tri;

typedef struct {
    svg_AspectRatio  aspect;
    svg_Vertex      *verts;
    size_t           verts_count;
    svg_Tri         *tris;
    size_t           tris_count;
} svg_Svg;

typedef struct {
    float min_x, min_y, max_x, max_y;
} svg_Rect;
void svg_draw(svg_Svg *svg, svg_Rect dst, Color color);
