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

typedef int framebuffer[FB_HEIGHT][FB_WIDTH];

framebuffer* fb_init();

void draw_pixel(int data,int i , int j);

void draw_fb(framebuffer* fb);

void draw_map(float x, float y,float scale,cam* camera,int screen_width);

void render_loop();
#endif