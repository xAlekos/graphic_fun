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

void dda_ray_direction(map* grid,Vector2* path_buf,Vector2 ray_dir,Vector2 ray_origin){

    int i = 0;
    float x_s = ray_origin.x;
    float y_s = ray_origin.y;
    float s_x = sqrtf(1 + (ray_dir.y/ray_dir.x) * (ray_dir.y/ray_dir.x));
    float s_y = sqrtf(1 + (ray_dir.x / ray_dir.y) * (ray_dir.x / ray_dir.y));
    
  
    float first_step_x = ray_dir.x > 0 ? 1 -(x_s - floorf(x_s)) : x_s - floorf(x_s);
    float first_step_y = ray_dir.y > 0 ? 1 -(y_s - floorf(y_s)) : y_s - floorf(y_s);
    int step_x = ray_dir.x > 0 ? 1 : -1;
    int step_y = ray_dir.y > 0 ? 1 : -1;
    
    float len_x = first_step_x * s_x;
    float len_y = first_step_y * s_y;


    path_buf[0]=(Vector2){x_s,y_s};
    

    while(i < 7){
        i++;
        if(len_x > len_y){
            y_s = fmax(fmin(MAP_H - 1, y_s + step_y),0); 
            len_y += s_y;
        }
        else{
            x_s = fmax(fmin(MAP_W - 1, x_s + step_x),0); 
            len_x += s_x;
        }
        path_buf[i] = (Vector2){x_s,y_s};
    }

}