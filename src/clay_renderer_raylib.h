#include "raylib.h"
#include "raymath.h"
#include "stdint.h"
#include "guy.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#define CLAY_RECTANGLE_TO_RAYLIB_RECTANGLE(rectangle) (RL_Rectangle) { .x = rectangle.x, .y = rectangle.y, .width = rectangle.width, .height = rectangle.height }
#define CLAY_COLOR_TO_RAYLIB_COLOR(color) (RL_Color) { .r = (unsigned char)roundf(color.r), .g = (unsigned char)roundf(color.g), .b = (unsigned char)roundf(color.b), .a = (unsigned char)roundf(color.a) }

/* TODO: generational indexing here so we can tell if guy was dealloced since
 * rendering commands created */
typedef guy_Guy CustomLayoutElement;

const char *overlayShaderCode = "#version 330\n"
                                "\n"
                                "in vec2 fragTexCoord;\n"
                                "in vec4 fragColor;\n"
                                "\n"
                                "uniform sampler2D texture0;\n"
                                "uniform vec4 overlayColor;\n"
                                "\n"
                                "out vec4 finalColor;\n"
                                "\n"
                                "void main()\n"
                                "{\n"
                                "    vec4 texelColor = texture(texture0, fragTexCoord) * fragColor;\n"
                                "\n"
                                "    vec3 blendedRGB = mix(texelColor.rgb, overlayColor.rgb, overlayColor.a);\n"
                                "\n"
                                "    finalColor = vec4(blendedRGB, texelColor.a);\n"
                                "}";

RL_Shader overlayShader;
int colorLoc;
bool overlayEnabled = false;

void InitOverlay() {
    overlayShader = RL_LoadShaderFromMemory(0, overlayShaderCode);
    colorLoc = RL_GetShaderLocation(overlayShader, "overlayColor");
}

void SetColorOverlay(RL_Color color) {
    overlayEnabled = true;
    float colorFloat[4] = {
        (float)color.r/255.0f,
        (float)color.g/255.0f,
        (float)color.b/255.0f,
        (float)color.a/255.0f,
    };

    RL_SetShaderValue(overlayShader, colorLoc, colorFloat, SHADER_UNIFORM_VEC4);
    RL_BeginShaderMode(overlayShader);
}

void DisableColorOverlay() {
    if (overlayEnabled) {
        RL_EndShaderMode();
        overlayEnabled = false;
    }
}

static inline Clay_Dimensions Raylib_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData) {
    // Measure string size for Font
    Clay_Dimensions textSize = { 0 };

    float maxTextWidth = 0.0f;
    float lineTextWidth = 0;
    int maxLineCharCount = 0;
    int lineCharCount = 0;

    float textHeight = config->fontSize;
    RL_Font* fonts = (RL_Font*)userData;
    RL_Font fontToUse = fonts[config->fontId];
    // Font failed to load, likely the fonts are in the wrong place relative to the execution dir.
    // RayLib ships with a default font, so we can continue with that built in one. 
    if (!fontToUse.glyphs) {
        fontToUse = RL_GetFontDefault();
    }

    float scaleFactor = config->fontSize/(float)fontToUse.baseSize;

    for (int i = 0; i < text.length; ++i, lineCharCount++)
    {
        if (text.chars[i] == '\n') {
            maxTextWidth = fmax(maxTextWidth, lineTextWidth);
            maxLineCharCount = CLAY__MAX(maxLineCharCount, lineCharCount);
            lineTextWidth = 0;
            lineCharCount = 0;
            continue;
        }
        int index = text.chars[i] - 32;
        if (fontToUse.glyphs[index].advanceX != 0) lineTextWidth += fontToUse.glyphs[index].advanceX;
        else lineTextWidth += (fontToUse.recs[index].width + fontToUse.glyphs[index].offsetX);
    }

    maxTextWidth = fmax(maxTextWidth, lineTextWidth);
    maxLineCharCount = CLAY__MAX(maxLineCharCount, lineCharCount);

    textSize.width = maxTextWidth * scaleFactor + (lineCharCount * config->letterSpacing);
    textSize.height = textHeight;

    return textSize;
}

void Clay_Raylib_Initialize(void) {
    InitOverlay();
//    EnableEventWaiting();
}

// A MALLOC'd buffer, that we keep modifying inorder to save from so many Malloc and Free Calls.
// Call Clay_Raylib_Close() to free
static char *temp_render_buffer = NULL;
static int temp_render_buffer_len = 0;

// Call after closing the window to clean up the render buffer
void Clay_Raylib_Close()
{
    if(temp_render_buffer) free(temp_render_buffer);
    temp_render_buffer_len = 0;

    RL_CloseWindow();
}


void Clay_Raylib_Render(Clay_RenderCommandArray renderCommands, RL_Font* fonts)
{
    for (int j = 0; j < renderCommands.length; j++)
    {
        Clay_RenderCommand *renderCommand = Clay_RenderCommandArray_Get(&renderCommands, j);
        Clay_BoundingBox boundingBox = {renderCommand->boundingBox.x, renderCommand->boundingBox.y, renderCommand->boundingBox.width, renderCommand->boundingBox.height};
        switch (renderCommand->commandType)
        {
            case CLAY_RENDER_COMMAND_TYPE_TEXT: {
                Clay_TextRenderData *textData = &renderCommand->renderData.text;
                RL_Font fontToUse = fonts[textData->fontId];
    
                int strlen = textData->stringContents.length + 1;
    
                if(strlen > temp_render_buffer_len) {
                    // Grow the temp buffer if we need a larger string
                    if(temp_render_buffer) free(temp_render_buffer);
                    temp_render_buffer = (char *) malloc(strlen);
                    temp_render_buffer_len = strlen;
                }
    
                // Raylib uses standard C strings so isn't compatible with cheap slices, we need to clone the string to append null terminator
                memcpy(temp_render_buffer, textData->stringContents.chars, textData->stringContents.length);
                temp_render_buffer[textData->stringContents.length] = '\0';
                RL_DrawTextEx(fontToUse, temp_render_buffer, (RL_Vector2){boundingBox.x, boundingBox.y}, (float)textData->fontSize, (float)textData->letterSpacing, CLAY_COLOR_TO_RAYLIB_COLOR(textData->textColor));
    
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_IMAGE: {
                RL_Texture2D imageTexture = *(RL_Texture2D *)renderCommand->renderData.image.imageData;
                Clay_Color tintColor = renderCommand->renderData.image.backgroundColor;
                if (tintColor.r == 0 && tintColor.g == 0 && tintColor.b == 0 && tintColor.a == 0) {
                    tintColor = (Clay_Color) { 255, 255, 255, 255 };
                }
                RL_DrawTexturePro(
                    imageTexture,
                    (RL_Rectangle) { 0, 0, imageTexture.width, imageTexture.height },
                    (RL_Rectangle){boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height},
                    (RL_Vector2) {},
                    0,
                    CLAY_COLOR_TO_RAYLIB_COLOR(tintColor));
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START: {
                RL_BeginScissorMode((int)roundf(boundingBox.x), (int)roundf(boundingBox.y), (int)roundf(boundingBox.width), (int)roundf(boundingBox.height));
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END: {
                RL_EndScissorMode();
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_OVERLAY_COLOR_START: {
                SetColorOverlay(CLAY_COLOR_TO_RAYLIB_COLOR(renderCommand->renderData.overlayColor.color));
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_OVERLAY_COLOR_END: {
                DisableColorOverlay();
            }
            case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
                Clay_RectangleRenderData *config = &renderCommand->renderData.rectangle;
                if (config->cornerRadius.topLeft > 0) {
                    float radius = (config->cornerRadius.topLeft * 2) / (float)((boundingBox.width > boundingBox.height) ? boundingBox.height : boundingBox.width);
                    RL_DrawRectangleRounded((RL_Rectangle) { boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height }, radius, 8, CLAY_COLOR_TO_RAYLIB_COLOR(config->backgroundColor));
                } else {
                    RL_DrawRectangle(boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height, CLAY_COLOR_TO_RAYLIB_COLOR(config->backgroundColor));
                }
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_BORDER: {
                Clay_BorderRenderData *config = &renderCommand->renderData.border;
                // Left border
                if (config->width.left > 0) {
                    RL_DrawRectangleV((RL_Vector2) { boundingBox.x, boundingBox.y + config->cornerRadius.topLeft }, (RL_Vector2) { config->width.left, boundingBox.height - config->cornerRadius.topLeft - config->cornerRadius.bottomLeft }, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                // Right border
                if (config->width.right > 0) {
                    RL_DrawRectangleV((RL_Vector2) { boundingBox.x + boundingBox.width - config->width.right, boundingBox.y + config->cornerRadius.topRight }, (RL_Vector2) { config->width.right, boundingBox.height - config->cornerRadius.topRight - config->cornerRadius.bottomRight }, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                // Top border
                if (config->width.top > 0) {
                    RL_DrawRectangleV((RL_Vector2) { boundingBox.x + config->cornerRadius.topLeft, boundingBox.y }, (RL_Vector2) { boundingBox.width - config->cornerRadius.topLeft - config->cornerRadius.topRight, (int)config->width.top }, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                // Bottom border
                if (config->width.bottom > 0) {
                    RL_DrawRectangleV((RL_Vector2) { boundingBox.x + config->cornerRadius.bottomLeft, boundingBox.y + boundingBox.height - config->width.bottom }, (RL_Vector2) { boundingBox.width - config->cornerRadius.bottomLeft - config->cornerRadius.bottomRight, (int)config->width.bottom }, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                if (config->cornerRadius.topLeft > 0) {
                    RL_DrawRing((RL_Vector2) { roundf(boundingBox.x + config->cornerRadius.topLeft), roundf(boundingBox.y + config->cornerRadius.topLeft) }, roundf(config->cornerRadius.topLeft - config->width.top), config->cornerRadius.topLeft, 180, 270, 10, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                if (config->cornerRadius.topRight > 0) {
                    RL_DrawRing((RL_Vector2) { roundf(boundingBox.x + boundingBox.width - config->cornerRadius.topRight), roundf(boundingBox.y + config->cornerRadius.topRight) }, roundf(config->cornerRadius.topRight - config->width.top), config->cornerRadius.topRight, 270, 360, 10, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                if (config->cornerRadius.bottomLeft > 0) {
                    RL_DrawRing((RL_Vector2) { roundf(boundingBox.x + config->cornerRadius.bottomLeft), roundf(boundingBox.y + boundingBox.height - config->cornerRadius.bottomLeft) }, roundf(config->cornerRadius.bottomLeft - config->width.bottom), config->cornerRadius.bottomLeft, 90, 180, 10, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                if (config->cornerRadius.bottomRight > 0) {
                    RL_DrawRing((RL_Vector2) { roundf(boundingBox.x + boundingBox.width - config->cornerRadius.bottomRight), roundf(boundingBox.y + boundingBox.height - config->cornerRadius.bottomRight) }, roundf(config->cornerRadius.bottomRight - config->width.bottom), config->cornerRadius.bottomRight, 0.1, 90, 10, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_CUSTOM: {
                Clay_CustomRenderData *config = &renderCommand->renderData.custom;
                CustomLayoutElement *customElement = (CustomLayoutElement *)config->customData;
                if (!customElement) continue;

                guy_draw(
                    customElement,
                    renderCommand->boundingBox.x + renderCommand->boundingBox.width/2,
                    renderCommand->boundingBox.y + renderCommand->boundingBox.height/2
                );
                break;
            }
            default: {
                printf("Error: unhandled render command.");
                exit(1);
            }
        }
    }
}
