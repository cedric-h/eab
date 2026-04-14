#include "base.h"
#include <string.h>
#include <stdint.h>
#include "raylib.h"
#include "svg.h"

/**
 * Note that the SVG cooker leaves padding on one axis of the
 * SVG so that the aspect ratio is always one, and then provides
 * an "aspect" field that indicates which axis was padded and what
 * portion of the axis is actually used. The tesselate tab of the asset
 * cooker provides a visualization of this.
 *
 * This function doesn't account for this, so it may redundantly pad the SVG.
 **/
void svg_draw(svg_Svg *svg, svg_Rect dst, Color color) {
    /* largest possible space svg could go in */
    float dst_size_x = dst.max_x - dst.min_x;
    float dst_size_y = dst.max_y - dst.min_y;
    float dst_minor_axis = min(dst_size_x, dst_size_y);

    float pad_x = (dst_size_x - dst_minor_axis)*0.5;
    float pad_y = (dst_size_y - dst_minor_axis)*0.5;
    float dst_min_x = dst.min_x + pad_x;
    float dst_max_x = dst.min_x + pad_x + dst_minor_axis;
    float dst_min_y = dst.min_y + pad_y;
    float dst_max_y = dst.min_y + pad_y + dst_minor_axis;

    for (size_t i = 0; i < svg->tris_count; i++) {
        svg_Vertex a = svg->verts[svg->tris[i].a];
        svg_Vertex b = svg->verts[svg->tris[i].b];
        svg_Vertex c = svg->verts[svg->tris[i].c];
        RL_DrawTriangle(
            (RL_Vector2) {
                lerp(dst_min_x, dst_max_x, b.pos.x),
                lerp(dst_min_y, dst_max_y, b.pos.y)
            },
            (RL_Vector2) {
                lerp(dst_min_x, dst_max_x, a.pos.x),
                lerp(dst_min_y, dst_max_y, a.pos.y)
            },
            (RL_Vector2) {
                lerp(dst_min_x, dst_max_x, c.pos.x),
                lerp(dst_min_y, dst_max_y, c.pos.y)
            },
            (RL_Color) { color.r, color.g, color.b, color.a }
        );
    }
}
