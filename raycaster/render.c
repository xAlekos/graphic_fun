#include "render.h"


framebuffer* fb_init(int width, int height) {
    framebuffer* fb = malloc(sizeof(framebuffer));
    fb->width = width;
    fb->height = height;
    fb->pixels = calloc(width * height, sizeof(Color));  // inizializzato a nero

    Image img = GenImageColor(width, height, BLACK);

    fb->texture = LoadTextureFromImage(img); 
    UnloadImage(img);
    return fb;
}

void set_fb_pixel(framebuffer* fb, int x, int y, Color color) {
    if (x >= 0 && x < fb->width && y >= 0 && y < fb->height) {
        fb->pixels[y * fb->width + x] = color;
    }
}

void draw_fb(framebuffer* fb){

    UpdateTexture(fb->texture, fb->pixels);
    DrawTexture(fb->texture, 0, 0, WHITE);

}

void draw_rays(float x ,float y ,float scale,cam* camera,int screen_width){

    for(int i = 0 ; i<screen_width;i+=300){
        Vector2 ray = get_ray_dir(i,screen_width,camera);
        ray = Vector2Scale(ray, scale * 3);
        Vector2 scaled_pos = Vector2Scale(camera->pos,(scale + GRID_OFFSET));
        Vector2 origin = Vector2Add(scaled_pos,(Vector2){x,y}); 
        Vector2 line_end = Vector2Add(origin, ray);
        Vector2 line_end_over = Vector2Add(line_end, ray);

        DrawLineEx(origin, line_end,3,RED);
        DrawLineEx(line_end, line_end_over,3,PURPLE);
        
    }
}

void draw_map(map* grid, float x, float y,float scale ,cam* camera,int screen_width){

    Rectangle grid_frame = {.height = MAP_H*(scale + GRID_OFFSET) - GRID_OFFSET, .width = MAP_W*(scale + GRID_OFFSET) - GRID_OFFSET, .x = x, .y = y};
    
    
    //GRID
    for(int i = 0;i<MAP_H;i++){
        for(int j = 0;j<MAP_W;j++){
            switch((*grid)[i][j]){
            case 0: DrawRectangle(x + j*(scale + GRID_OFFSET) ,y + i*(scale + GRID_OFFSET),scale,scale,WHITE); break;
            case 1: DrawRectangle(x +j*(scale + GRID_OFFSET),y + i*(scale + GRID_OFFSET),scale,scale,YELLOW); break;
            }
        }
    }
    //GRID FRAME
    DrawRectangleLinesEx(grid_frame,4,MAGENTA);

    draw_rays(x,y,scale,camera,screen_width);

    DrawCircle(x + camera->pos.x * (scale + GRID_OFFSET),y + camera->pos.y * (scale + GRID_OFFSET), scale/4, MAROON);
}

void hightlight_path(int len,Vector2* path, map* draw_grid){

    for(int i = 0; i<len; i++)
        (*draw_grid)[(int)(path[i].y)][(int)(path[i].x)]=1;
}

void render_loop(){

    InitWindow(FB_WIDTH,FB_HEIGHT,"pizzapo");
    framebuffer* fb = fb_init(FB_WIDTH,FB_HEIGHT);
    cam* camera = camera_init();
    //levare di qua
    map grid= {0};
    map draw_grid = {0};
    for(int i = 0;i<MAP_W;i++){
        grid[0][i]=1;
        grid[MAP_H - 1][i]=1;
        grid[i][0]=1;
        grid[i][MAP_W - 1]=1;
    }
    //-----
    
    while (!WindowShouldClose())
    {
        input(camera);
        BeginDrawing();
        ClearBackground(BLACK);
        draw_fb(fb);
        memcpy(draw_grid, grid, sizeof(map));
        Vector2 path1[7];
        Vector2 path2[7];
        dda_ray_direction(&grid,path1,get_ray_dir(0,FB_WIDTH,camera),camera->pos);
        dda_ray_direction(&grid,path2,get_ray_dir(FB_WIDTH - 1,FB_WIDTH,camera),camera->pos);
        hightlight_path(7,path1,&draw_grid);
        hightlight_path(7,path2,&draw_grid);
        draw_map(&draw_grid,1024,15,20,camera,FB_WIDTH);
        DrawText(TextFormat("%d FPS", GetFPS()), 200, 60, 20, RED);
        DrawText(TextFormat("pos -> x: %f  y: %f", camera->pos.x, camera->pos.y), 200, 80, 20, RED);
        DrawText(TextFormat("dir -> x: %f  y: %f", camera->dir.x, camera->dir.y), 200, 100, 20, MAGENTA);

        EndDrawing();

    }
    free(camera);
    free(fb->pixels);
    UnloadTexture(fb->texture);
    free(fb);


}
