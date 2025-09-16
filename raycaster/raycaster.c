#include "raycaster.h"

cam* camera_init(){

    cam* new_cam = malloc(sizeof(cam));
    new_cam->dir = (Vector2){-1,0};
    new_cam->pos = (Vector2){10,10};
    new_cam->plane_half_lenght = (Vector2){0,0.66};
    return new_cam;
}

Vector2 get_ray_dir(int screen_x,int screen_width,cam* camera){

    float step = 2.0f * (float)screen_x/(float)screen_width - 1.0f; //-1 a 1
    Vector2 scaled_plane = Vector2Scale(camera->plane_half_lenght,step);
    Vector2 ray_dir = Vector2Add(camera->dir,scaled_plane);

    return Vector2Normalize(ray_dir);

}

void dda_ray_direction(Vector2 ray_dir,Vector2 ray_origin,map* grid ,int scale){

    int i = 0;
    int x_s = ray_origin.x;
    int y_s = ray_origin.y;
    float s_x = sqrtf(1 + (ray_dir.y/ray_dir.x) * (ray_dir.y/ray_dir.x));
    float s_y = sqrtf(1 + (ray_dir.x / ray_dir.y) * (ray_dir.x / ray_dir.y));
    float len_x = s_x;
    float len_y = s_y;
    int step_x = ray_dir.x > 0 ? 1 : -1;
    int step_y = ray_dir.y > 0 ? 1 : -1;
    (*grid)[y_s][x_s]=1;
    

    while(i < 8){
        i++;
        if(len_x > len_y){
            y_s += step_y;
            len_y += s_y;
        }
        else{
            x_s += step_x;
            len_x += s_x;
        }
        (*grid)[y_s][x_s]=1;
    }


    
}