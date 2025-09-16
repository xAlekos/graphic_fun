#include "input.h"

void input(cam* camera){
    float delta = GetFrameTime();
    if(IsKeyDown(KEY_W))
        camera->pos.y -= MOV_SPEED * delta;
    if(IsKeyDown(KEY_S))
        camera->pos.y += MOV_SPEED * delta;
    if(IsKeyDown(KEY_A))
        camera->pos.x -= MOV_SPEED * delta;
    if(IsKeyDown(KEY_D))
        camera->pos.x += MOV_SPEED * delta;
    if(IsKeyDown(KEY_LEFT)){
        Vector2 old_dir = camera->dir;
        Vector2 old_plane = camera->plane_half_lenght;

        camera->dir.x = cosf(-ROT_SPEED * delta) * old_dir.x - sinf(-ROT_SPEED * delta) * old_dir.y;
        camera->dir.y = sinf(-ROT_SPEED * delta) * old_dir.x + cosf(-ROT_SPEED * delta) * old_dir.y;

        camera->plane_half_lenght.x = cosf(-ROT_SPEED * delta) * old_plane.x - sinf(-ROT_SPEED * delta) * old_plane.y;
        camera->plane_half_lenght.y = sinf(-ROT_SPEED * delta) * old_plane.x + cosf(-ROT_SPEED * delta) * old_plane.y;

    }
    if(IsKeyDown(KEY_RIGHT)){
        Vector2 old_dir = camera->dir;
        Vector2 old_plane = camera->plane_half_lenght;

        camera->dir.x = cosf(ROT_SPEED * delta) * old_dir.x - sinf(ROT_SPEED * delta) * old_dir.y;
        camera->dir.y = sinf(ROT_SPEED * delta) * old_dir.x + cosf(ROT_SPEED * delta) * old_dir.y;

        camera->plane_half_lenght.x = cosf(ROT_SPEED * delta) * old_plane.x - sinf(ROT_SPEED * delta) * old_plane.y;
        camera->plane_half_lenght.y = sinf(ROT_SPEED * delta) * old_plane.x + cosf(ROT_SPEED * delta) * old_plane.y;
    }

}