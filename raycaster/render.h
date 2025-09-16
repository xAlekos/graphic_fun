#ifndef RENDER_H
#define RENDER_H
#include <raylib.h>
#include "utils.h"
#include "input.h"
#include "raycaster.h"
#define MAP_GRID_SCALE 40
#define GRID_OFFSET 5

#define FB_WIDTH 2048
#define FB_HEIGHT 1400

typedef struct framebuffer {
    Color* pixels;
    Texture2D texture;
    int width;
    int height;
} framebuffer;


framebuffer* fb_init(int width, int height);

void set_fb_pixel(framebuffer* fb, int x, int y, Color color);

void draw_fb(framebuffer* fb);

void draw_map(map* grid, float x, float y,float scale ,cam* camera,int screen_width);

void render_loop();
#endif