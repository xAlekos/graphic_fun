#include "raycaster.h"

cam* camera_init(){

    cam* new_cam = malloc(sizeof(cam));
    new_cam->dir = (Vector2){-1,0};
    new_cam->pos = (Vector2){25,25};
    new_cam->plane_half_lenght = (Vector2){0,0.66};
    return new_cam;
}

Vector2 get_ray_dir(int screen_x,int screen_width,cam* camera){

    float step = 2.0f * (float)screen_x/(float)screen_width - 1.0f; //-1 a 1
    Vector2 scaled_plane = Vector2Scale(camera->plane_half_lenght,step);
    Vector2 ray_dir = Vector2Add(camera->dir,scaled_plane);

    return Vector2Normalize(ray_dir);

}

