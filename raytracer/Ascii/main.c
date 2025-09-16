#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define FRAME_BUFFER_WIDTH 160
#define FRAME_BUFFER_HEIGHT 40 

#define KEY_LEFT 1002
#define KEY_RIGHT 1003


struct termios orig_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON); 
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1; 

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int read_key() {
    char c;
    int n = read(STDIN_FILENO, &c, 1);
    if (n == 0) return -1;

    if (c == '\033') { 
        char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) == 0) return '\033';
        if (read(STDIN_FILENO, &seq[1], 1) == 0) return '\033';

        if (seq[0] == '[') {
            switch (seq[1]) {
                case 'A': return 1000; // su
                case 'B': return 1001; // giù
                case 'C': return KEY_LEFT;
                case 'D': return KEY_RIGHT; 
            }
        }
        return '\033';
    }
    return c;
}

typedef struct vec3{
    float x;
    float y;
    float z;
}vec3;

vec3 vec_add(vec3 a, vec3 b){
    vec3 result = {a.x + b.x, a.y + b.y, a.z + b.z};
    return result;
}

vec3 vec_sub(vec3 a, vec3 b){
    vec3 result = {a.x - b.x, a.y - b.y, a.z - b.z};
    return result;
}

vec3 vec_scalar_product(float scalar,vec3 v){

    vec3 result;
    result.x = scalar * v.x;
    result.y = scalar * v.y;
    result.z = scalar * v.z;
    return result;
}

vec3 vec_cross_product(vec3 a,vec3 b){
    vec3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

float vec_dot_product(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3 vec_normalize(vec3 v){
    vec3 result;
    float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length == 0.0f) {
        return v; 
    }
    result.x = v.x / length;
    result.y = v.y / length;
    result.z = v.z / length;
    return result;
}

float vec_magnitude(vec3 v){

    float mag = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    return mag;
}

typedef char framebuffer[FRAME_BUFFER_HEIGHT][FRAME_BUFFER_WIDTH];

framebuffer* frame_buffer_init(){
    
    framebuffer* fb = (framebuffer*)malloc(sizeof(framebuffer));
    memset(fb,' ',sizeof(framebuffer));
    return fb;
}

void framebuffer_print(framebuffer* fb){
    printf("\x1b[H\x1b[2J"); // pulisce schermo e riporta il cursore in alto a sinistra
    for(int i = 0;i<FRAME_BUFFER_HEIGHT;i++){
        for(int j=0;j<FRAME_BUFFER_WIDTH;j++){
            putchar((*fb)[i][j]);
        } 
        putchar('\n'); 
    }
}

typedef struct cube{
    
    vec3 center;
    float half_edge;

}cube;

cube* cube_init(float cx,float cy,float cz, float half_edge){

    cube* q = (cube*)malloc(sizeof(cube));
    q->center = (vec3){cx,cy,cz};
    q->half_edge = half_edge;
    return q;

}

void cube_print_vertices(cube* q){
    vec3 vertex;
    for(int i=0;i<8;i++){
        vertex.x = (i & 1 << 2) == 0 ? q->center.x - q->half_edge : q->center.x + q->half_edge;
        vertex.y = (i & 1 << 1) == 0 ? q->center.y - q->half_edge : q->center.y + q->half_edge;
        vertex.z = (i & 1 << 0) == 0 ? q->center.z - q->half_edge : q->center.z + q->half_edge;
        printf("V%d -> x:%f y:%f z:%f\n",i,vertex.x,vertex.y,vertex.z);
    }
}

typedef struct viewport{

    vec3 center;
    vec3 right;
    vec3 true_up; //Ricalcolato per assicurarsi dell'ortogonalità
    float width;
    float height;

}viewport;

typedef struct camera{
    vec3 pos;
    vec3 direction;
    vec3 up;
    viewport* vp;
    float focal_lenght;
}camera;


viewport* init_viewport(camera* cam, float width, float height){
    
    viewport* v = (viewport*)malloc(sizeof(viewport));

    vec3 right_vector=vec_cross_product(cam->direction, cam->up);
    if(vec_magnitude(right_vector) < 1e-6f){
        v->true_up = (vec3){1,0,0};
        right_vector=vec_cross_product(cam->direction, v->true_up);
    }
    else
        v->true_up = cam->up;
    v->center = vec_add(cam->pos, vec_scalar_product(cam->focal_lenght,cam->direction));
    v->right = vec_normalize(right_vector);
    v->true_up = vec_normalize(vec_cross_product(v->right, cam->direction));
    v->width = width;
    v->height = height;
    return v;
}

camera* camera_init(float posx, float posy, float posz, float dirx, float diry , float dirz,float focal_lenght, float vp_width , float vp_height){

    camera* cam = (camera*)malloc(sizeof(camera));
    vec3 direction = (vec3){dirx,diry,dirz};
    cam->pos = (vec3){posx,posy,posz};
    cam->direction = vec_normalize(direction);
    cam->up = (vec3){0.0f,1.0f,0.0f};
    cam->focal_lenght = focal_lenght;
    cam->vp = init_viewport(cam, vp_width, vp_height);
    
    return cam;
}

void viewport_update(camera* cam){
    
    vec3 right_vector=vec_cross_product(cam->direction, cam->up);
    if(vec_magnitude(right_vector) < 1e-6f){
        cam->vp->true_up = (vec3){1,0,0};
        right_vector=vec_cross_product(cam->direction, cam->vp->true_up);
    }
    else
        cam->vp->true_up = cam->up;
    cam->vp->center = vec_add(cam->pos, vec_scalar_product(cam->focal_lenght,cam->direction));
    cam->vp->right = vec_normalize(right_vector);
    cam->vp->true_up = vec_normalize(vec_cross_product(cam->vp->right, cam->direction));

}

vec3 framebuffer_to_viewport_point(viewport* vp, int i, int j){
    
    float u = (j + 0.5f)/FRAME_BUFFER_WIDTH;
    float v = (i + 0.5f)/FRAME_BUFFER_HEIGHT;


    vec3 right_offset = vec_scalar_product((u - 0.5f) * vp->width,  vp->right);
    vec3 up_offset    = vec_scalar_product((v - 0.5f) * vp->height, vp->true_up);
    vec3 vp_point = vec_add(vp->center, vec_add(right_offset, up_offset));

    return vp_point;
}

vec3 get_ray_dir(camera* cam, int i, int j){

     vec3 point = framebuffer_to_viewport_point(cam->vp,i,j);
     vec3 ray_dir = vec_normalize(vec_sub(point, cam->pos));
     return ray_dir;
}

vec3 get_ray_dir_generic(camera* cam, vec3 point){
     vec3 ray_dir = vec_normalize(vec_sub(point, cam->pos));
     return ray_dir;
}


float ray_cube_intersection(cube* q,vec3 ray_dir, vec3 ray_origin){

    vec3 cube_min = {q->center.x -q->half_edge,q->center.y -q->half_edge,q->center.z -q->half_edge};
    vec3 cube_max = {q->center.x +q->half_edge,q->center.y +q->half_edge,q->center.z +q->half_edge};

    float t_x_min = (cube_min.x - ray_origin.x)/ray_dir.x;
    float t_x_max = (cube_max.x - ray_origin.x)/ray_dir.x;
    
    if(t_x_min > t_x_max){ //se t min è maggiore di t max la direzione è negativa e bisogna invertire tmin e tmax
        float tmp = t_x_min;
        t_x_min = t_x_max;
        t_x_max = tmp;
    }

    float t_y_min = (cube_min.y - ray_origin.y)/ray_dir.y;
    float t_y_max = (cube_max.y - ray_origin.y)/ray_dir.y;
    if(t_y_min > t_y_max){ 
        float tmp = t_y_min;
        t_y_min = t_y_max;
        t_y_max = tmp;
    }
    float t_z_min = (cube_min.z - ray_origin.z)/ray_dir.z;
    float t_z_max = (cube_max.z - ray_origin.z)/ray_dir.z;
    if(t_z_min > t_z_max){ 
        float tmp = t_z_min;
        t_z_min = t_z_max;
        t_z_max = tmp;
    }

    float t_min = fmax(fmax(t_x_min, t_y_min),t_z_min);
    float t_max = fmin(fmin(t_x_max, t_y_max), t_z_max);

    if(t_min <= t_max && t_max > 0)
        return t_min;
    else
        return 0;
}


void raycast(camera* cam,framebuffer* fb,cube* q){

    for(int i = 0; i<FRAME_BUFFER_HEIGHT;i++){
        for(int j=0;j<FRAME_BUFFER_WIDTH;j++){
            vec3 ray_dir = get_ray_dir(cam,i,j);
            float intersect = ray_cube_intersection(q,ray_dir,cam->pos);
            if(intersect <= 1 && intersect != 0){
                (*fb)[i][j] = '@';
            }
            if(intersect > 1 && intersect < 3){
                (*fb)[i][j] = '#';
            }
            if(intersect >= 3)
                (*fb)[i][j] = '"';
        }
    }

}


void camera_update_interactive(camera* cam, float move_speed, float rot_speed) {
    int ch = read_key();
    if (ch == -1) return;

    vec3 forward = cam->direction;
    vec3 right = vec_cross_product(forward, cam->up);
   
    switch(ch) {
        case 'w': cam->pos = vec_add(cam->pos, vec_scalar_product(move_speed, forward)); break;
        case 's': cam->pos = vec_sub(cam->pos, vec_scalar_product(move_speed, forward)); break;
        case 'a': cam->pos = vec_sub(cam->pos, vec_scalar_product(move_speed, right)); break;
        case 'd': cam->pos = vec_add(cam->pos, vec_scalar_product(move_speed, right)); break;
        case 'q': cam->pos.y -= move_speed; break;
        case 'e': cam->pos.y += move_speed; break;

        case KEY_LEFT:
        {
            float angle = rot_speed;
            float cosA = cosf(angle);
            float sinA = sinf(angle);
            vec3 dir = cam->direction;
            vec3 up = cam->up;
            vec3 right = vec_cross_product(dir, up);

            vec3 new_dir = { //formula di Rodriguez semplificata in quanto il vettore dir è ortogonale all'asse di rotazione (il vettore up)
                dir.x * cosA + right.x * sinA,
                dir.y * cosA + right.y * sinA,
                dir.z * cosA + right.z * sinA
            };
            cam->direction = vec_normalize(new_dir);
            break;
        }
        case KEY_RIGHT: 
        {
            float angle = -rot_speed;
            float cosA = cosf(angle);
            float sinA = sinf(angle);
            vec3 dir = cam->direction;
            vec3 up = cam->up;
            vec3 right = vec_cross_product(dir, up);

            vec3 new_dir = {
                dir.x * cosA + right.x * sinA,
                dir.y * cosA + right.y * sinA,
                dir.z * cosA + right.z * sinA
            };
            cam->direction = vec_normalize(new_dir);
            break;
        }
    }
     viewport_update(cam);
}

void mark_vertices(camera* cam,cube* q,framebuffer* fb){
    vec3 vertices[8];
    
    for(int i=0;i<8;i++){
        vertices[i].x = (i & 1 << 2) == 0 ? q->center.x - q->half_edge : q->center.x + q->half_edge;
        vertices[i].y = (i & 1 << 1) == 0 ? q->center.y - q->half_edge : q->center.y + q->half_edge;
        vertices[i].z = (i & 1 << 0) == 0 ? q->center.z - q->half_edge : q->center.z + q->half_edge;
    }

    vec3 ray_directions[8];
    float ts[8];
    for(int i=0;i<8;i++){
        ray_directions[i]=get_ray_dir_generic(cam, vertices[i]);
        //t = (p0-l0)*n/l*n (trova punto di intersezione raggio piano) in questo caso (cam->vp->center - cam->pos)+*cam->dir/raydir * cam->direction
        ts[i] = vec_dot_product(vec_sub(cam->vp->center, cam->pos),cam->direction)/vec_dot_product(ray_directions[i], cam->direction);
    }
    for(int i = 0; i<8 ;i++){
        vec3 point = vec_add(cam->pos, vec_scalar_product(ts[i], ray_directions[i]));
        vec3 offset = vec_sub(point,cam->vp->center);
        float u = vec_dot_product(offset, cam->vp->right) / cam->vp->width + 0.5f;
        float v = vec_dot_product(offset, cam->vp->true_up) / cam->vp->height + 0.5f;
        int x = u * FRAME_BUFFER_WIDTH;
        int y = v * FRAME_BUFFER_HEIGHT;
        if (x >= 0 && x < FRAME_BUFFER_WIDTH && y >= 0 && y < FRAME_BUFFER_HEIGHT) 
            (*fb)[y][x]='#';
    }

}

int main(){

    enable_raw_mode();

    framebuffer* fb = frame_buffer_init();
    cube* q = cube_init(0.0f, 0.0f, 0.0f, 2.0f);
    camera* cam = camera_init(0, 0, 15, 0.0f, 0, -1, 3.0f,4.0f,2.0f);
    while(1) {
        camera_update_interactive(cam, 1.0f, 0.15f);
        memset(*fb, ' ', sizeof(framebuffer));
        raycast(cam, fb, q);
        mark_vertices(cam, q, fb);
        framebuffer_print(fb);
        printf("Camera Pos: x=%.2f y=%.2f z=%.2f  |  Dir: x=%.2f y=%.2f z=%.2f  \n",
        cam->pos.x, cam->pos.y, cam->pos.z,
        cam->direction.x, cam->direction.y, cam->direction.z
    );
    fflush(stdout);
        usleep(16000); // circa 60 fps
    }

    free(fb);
    free(q);
    free(cam);
    disable_raw_mode();
    return 0;
}