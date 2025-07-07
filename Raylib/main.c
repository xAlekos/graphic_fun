#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 512

#define DEFAULT_UP (Vector3){0,1,0}


struct hit{
    bool result;
    float  t;
};

struct hit rayIntersectsBox(Vector3 O, Vector3 V, Vector3 Pmin, Vector3 Pmax) {
    double tmin_x = MIN((Pmin.x - O.x) / V.x, (Pmax.x - O.x) / V.x);
    double tmax_x = MAX((Pmin.x - O.x) / V.x, (Pmax.x - O.x) / V.x);

    double tmin_y = MIN((Pmin.y - O.y) / V.y, (Pmax.y - O.y) / V.y);
    double tmax_y = MAX((Pmin.y - O.y) / V.y, (Pmax.y - O.y) / V.y);

    double tmin_z = MIN((Pmin.z - O.z) / V.z, (Pmax.z - O.z) / V.z);
    double tmax_z = MAX((Pmin.z - O.z) / V.z, (Pmax.z - O.z) / V.z);

    double entry = MAX(MAX(tmin_x, tmin_y), tmin_z);
    double exit = MIN(MIN(tmax_x, tmax_y), tmax_z);

    if (entry <= exit && exit >= 0.0) {
        return (struct hit){true,entry};
    }
    return (struct hit){false,entry};
}



typedef int framebuffer[WINDOW_HEIGHT][WINDOW_WIDTH];

framebuffer* frame_buffer_init(){
    framebuffer* fb = malloc(sizeof(framebuffer));
    memset(fb, 0, sizeof(framebuffer));
    return fb;
}

void frame_buffer_print(framebuffer* fb){
    for(int y=0; y < WINDOW_HEIGHT; y++){
        for(int x=0; x < WINDOW_WIDTH; x++){
            if((*fb)[y][x] == 1)
                DrawRectangle(x, y, 1, 1, RED);
            else if((*fb)[y][x] == 2)
                DrawRectangle(x, y, 1, 1, GREEN);
            else
                DrawRectangle(x, y, 1, 1, WHITE);
        }
    }
}

typedef struct camera{
    float vp_width;
    float vp_height;
    float focal_lenght;
    Vector3 direction;
    Vector3 pos;
    Vector3 up;
    Vector3 right;

}cam;

cam* init_camera(Vector3 pos,Vector3 dir,float vp_width, float vp_height,float focal_lenght){

    cam* camera = (cam*)malloc(sizeof(cam));
    camera->pos = pos;
    camera->direction = Vector3Normalize(dir);
    camera->vp_width = vp_width;
    camera->vp_height = vp_height;
    camera->focal_lenght = focal_lenght;
    camera->up = DEFAULT_UP; 

    if(fabs(Vector3DotProduct(camera->direction, camera->up)) >= 0.999f )
        camera->up = (Vector3){1,0,0};
    
    camera->right = Vector3Normalize(Vector3CrossProduct(camera->direction, camera->up));
    camera->up = Vector3Normalize(Vector3CrossProduct(camera->direction, camera->right));
    return camera;
};


Vector3 pixel_to_viewport_point(int i, int j,cam* camera){

    float u = (j+0.5)/WINDOW_WIDTH;
    float v = (i+0.5)/WINDOW_HEIGHT;
    
    Vector3 right_offset = Vector3Scale(camera->right, (u-0.5) * camera->vp_width);
    Vector3 up_offset = Vector3Scale(camera->up, (v-0.5) * camera->vp_height);

    Vector3 viewport_point = Vector3Add(camera->pos, right_offset);
    viewport_point = Vector3Add(viewport_point, up_offset);
    viewport_point = Vector3Add(viewport_point, Vector3Scale(camera->direction,camera->focal_lenght));

    return viewport_point;
}

Vector3 get_ray_dir(Vector3 point, Vector3 ray_origin){

    Vector3 dir = Vector3Subtract(point, ray_origin);
    return Vector3Normalize(dir);
}

int color_from_hit(struct hit hit,Vector3 pmin, Vector3 pmax, Vector3 raydir, cam* camera){

    if(hit.result == false)
        return 0;
    int n =0;
    Vector3 hitpoint = Vector3Add(camera->pos, Vector3Scale(raydir, hit.t));
    if(fabs(hitpoint.x - pmin.x) <= 0.1 || fabs(hitpoint.x - pmax.x) <= 0.1)
        n++;
    if(fabs(hitpoint.y - pmin.y) <= 0.1 || fabs(hitpoint.y - pmax.y) <= 0.1)
        n++;
    if(fabs(hitpoint.z - pmin.z) <= 0.1 || fabs(hitpoint.z - pmax.z) <= 0.1)
        n++;
    if(n>=2)
        return 2;
    else
        return 1;
}

void ray_cast(framebuffer* fb, cam* camera){

    struct hit hit;
    for(int i=0;i<WINDOW_HEIGHT;i++)
        for(int j=0;j<WINDOW_WIDTH;j++){

            Vector3 view_port_point=pixel_to_viewport_point(i,j,camera);
            Vector3 ray_dir=get_ray_dir(view_port_point, camera->pos);

            hit = rayIntersectsBox(camera->pos, ray_dir, (Vector3){-5,-5,-5}, (Vector3){5,5,5});
            (*fb)[i][j]= color_from_hit(hit, (Vector3){-5,-5,-5}, (Vector3){5,5,5}, ray_dir,camera);
        }
}

void refresh(framebuffer* fb, cam* camera){
    memset(fb, 0, sizeof(framebuffer));
    
    camera->up = DEFAULT_UP; 
    if(fabs(Vector3DotProduct(camera->direction, camera->up)) >= 0.999f )
        camera->up = (Vector3){1,0,0};
    camera->right = Vector3Normalize(Vector3CrossProduct(camera->direction, camera->up));
    camera->up = Vector3Normalize(Vector3CrossProduct(camera->direction, camera->right));

}

void input(cam* camera,float mov_speed){

    if(IsKeyDown(KEY_A))
        camera->pos = Vector3Subtract(camera->pos, Vector3Scale(camera->right, mov_speed));
    if(IsKeyDown(KEY_D))
        camera->pos = Vector3Add(camera->pos, Vector3Scale(camera->right, mov_speed));
    if(IsKeyDown(KEY_SPACE))
        camera->pos = Vector3Subtract(camera->pos, Vector3Scale(camera->up, mov_speed));
    if(IsKeyDown(KEY_Z))
        camera->pos = Vector3Add(camera->pos, Vector3Scale(camera->up, mov_speed));
    if(IsKeyDown(KEY_S))
        camera->pos = Vector3Subtract(camera->pos, Vector3Scale(camera->direction, mov_speed));
    if(IsKeyDown(KEY_W))
        camera->pos = Vector3Add(camera->pos, Vector3Scale(camera->direction, mov_speed));
    if(IsKeyDown(KEY_RIGHT)){
        camera->direction = Vector3Add(Vector3Scale(camera->direction, cos(0.15)) , Vector3Scale(camera->right, sin(0.15)));
    }
    if(IsKeyDown(KEY_LEFT))
        camera->direction = Vector3Add(Vector3Scale(camera->direction, cos(-0.15)) , Vector3Scale(camera->right, sin(-0.15)));
    if(IsKeyDown(KEY_DOWN)){
        camera->direction = Vector3Add(Vector3Scale(camera->direction, cos(0.15)) , Vector3Scale(camera->up, sin(0.15)));
    }
    if(IsKeyDown(KEY_UP))
        camera->direction = Vector3Add(Vector3Scale(camera->direction, cos(-0.15)) , Vector3Scale(camera->up, sin(-0.15)));
}

int main(){
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raytracerino");
    framebuffer* fb = frame_buffer_init();
    cam* camera = init_camera((Vector3){0,0,-10}, (Vector3){0,0,1}, 4, 2,1);
    printf("init!");
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(WHITE); 
        ray_cast(fb, camera);
        frame_buffer_print(fb);
        EndDrawing();
        input(camera,1);
        refresh(fb, camera);
    }

    free(fb);
    CloseWindow();

    return 0;
}