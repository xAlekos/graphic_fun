#include "input.h"

void input(cam* camera){
    if(IsKeyDown(KEY_W))
        camera->pos.y -= 0.05;
    if(IsKeyDown(KEY_S))
        camera->pos.y += 0.05;
    if(IsKeyDown(KEY_A))
        camera->pos.x -= 0.05;
    if(IsKeyDown(KEY_D))
        camera->pos.x += 0.05;
    if(IsKeyDown(KEY_LEFT)){
        Vector2 old_dir = camera->dir;
        Vector2 old_plane = camera->plane_half_lenght;

        camera->dir.x = cosf(-0.001) * old_dir.x - sinf(-0.001) * old_dir.y;
        camera->dir.y = sinf(-0.001) * old_dir.x + cosf(-0.001) * old_dir.y;

        camera->plane_half_lenght.x = cosf(-0.001) * old_plane.x - sinf(-0.001) * old_plane.y;
        camera->plane_half_lenght.y = sinf(-0.001) * old_plane.x + cosf(-0.001) * old_plane.y;

    }
    if(IsKeyDown(KEY_RIGHT)){
        Vector2 old_dir = camera->dir;
        Vector2 old_plane = camera->plane_half_lenght;

        camera->dir.x = cosf(0.001) * old_dir.x - sinf(0.001) * old_dir.y;
        camera->dir.y = sinf(0.001) * old_dir.x + cosf(0.001) * old_dir.y;

        camera->plane_half_lenght.x = cosf(0.001) * old_plane.x - sinf(0.001) * old_plane.y;
        camera->plane_half_lenght.y = sinf(0.001) * old_plane.x + cosf(0.001) * old_plane.y;
    }

}