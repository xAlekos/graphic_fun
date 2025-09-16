#ifndef RAYCASTER_H
#define RAYCASTER_H
#include "utils.h"
#include <raylib.h>
#include <raymath.h>

#define MAP_W 16
#define MAP_H 16
#define MAP_CAM_SCALE 160

typedef struct camera{
    Vector2 dir;
    Vector2 pos;
    Vector2 plane_half_lenght;

}cam;

typedef int map[MAP_H][MAP_W];

cam* camera_init();
Vector2 get_ray_dir(int screen_x,int screen_width,cam* camera);
void dda_ray_direction(Vector2 ray_dir,Vector2 ray_origin,map* grid ,int scale);
#endif