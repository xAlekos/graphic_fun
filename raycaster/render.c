#include "render.h"


framebuffer* fb_init(){
    
    framebuffer* new_fb = malloc(sizeof(framebuffer));
    memset(new_fb,0,sizeof(framebuffer));
    return new_fb;

}

void draw_pixel(int data,int i , int j){

    switch(data){
        case 0: DrawRectangle(j,i,1,1,RED); break;
        case 1: DrawRectangle(j,i,1,1,BLUE); break;
    }

}

void draw_fb(framebuffer* fb){

    for(int i = 0; i < FB_HEIGHT ; i++){
        for(int j = 0 ; j < FB_WIDTH; j++)
            draw_pixel((*fb)[i][j],i,j);
    }

}

void draw_rays(float x ,float y ,float scale,cam* camera,int screen_width){

    for(int i = 0 ; i<screen_width;i+=300){
        Vector2 ray = get_ray_dir(i,screen_width,camera);
        ray = Vector2Scale(ray, scale);
        
        Vector2 origin = Vector2Add(camera->pos,(Vector2){x,y}); 
        Vector2 line_end = Vector2Add(origin, ray);
        Vector2 line_end_over = Vector2Add(line_end, ray);

        DrawLineEx(origin, line_end,3,RED);
        DrawLineEx(line_end, line_end_over,3,PURPLE);
        
    }
}

void draw_map(float x, float y,float scale ,cam* camera,int screen_width){

    map grid={0};
    grid[0][0] = 1;
    grid[0][1] = 1;
    grid[0][2] = 1;

    Rectangle grid_frame = {.height = MAP_H*(scale + GRID_OFFSET) - GRID_OFFSET, .width = MAP_W*(scale + GRID_OFFSET) - GRID_OFFSET, .x = x, .y = y};
    
    
    //GRID
    for(int i = 0;i<MAP_H;i++){
        for(int j = 0;j<MAP_W;j++){
            switch(grid[i][j]){
            case 0: DrawRectangle(x + j*(scale + GRID_OFFSET) ,y + i*(scale + GRID_OFFSET),scale,scale,WHITE); break;
            case 1: DrawRectangle(x +j*(scale + GRID_OFFSET),y + i*(scale + GRID_OFFSET),scale,scale,YELLOW); break;
            }
        }
    }
    //GRID FRAME
    DrawRectangleLinesEx(grid_frame,4,MAGENTA);

    draw_rays(x,y,scale,camera,screen_width);
    //DrawLine(line1_end.x, line1_end.y, line2_end.x, line2_end.y, GREEN);

    DrawCircle(x + camera->pos.x,y + camera->pos.y, scale/4, MAROON);
}



void render_loop(){

    InitWindow(FB_WIDTH,FB_HEIGHT,"pizzap");
    framebuffer* fb = fb_init();
    cam* camera = camera_init();
    while (!WindowShouldClose())
    {
        ClearBackground(BLACK);
        input(camera);
        BeginDrawing();
        //draw_fb(fb);
        draw_map(1024,15,20,camera,FB_WIDTH);
        DrawText(TextFormat("pos -> x: %f  y: %f", camera->pos.x, camera->pos.y), 200, 80, 20, RED);
        DrawText(TextFormat("dir -> x: %f  y: %f", camera->dir.x, camera->dir.y), 200, 100, 20, MAGENTA);

        EndDrawing();

    }
    free(camera);
    free(fb);


}
