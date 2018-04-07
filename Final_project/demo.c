#include <stdlib.h>
#include <stdio.h>
#include <GLUT/glut.h>
#include <math.h>
/* macros */

#define IX(i,j) ((i)+(N+2)*(j))

/* external definitions (from solver.c) */

extern void dens_temp_step ( int N, float * x, float * x0, float * u, float * v, float diff, float dt ,float *T, float * T0);
extern void dens_step ( int N, float * x, float * x0, float * u, float * v, float diff, float dt );
extern void vel_step ( int N, float * u, float * v, float * u0, float * v0, float visc, float dt );

/* global variables */

static int N;
static float dt, diff, visc;
static float force, source;
static float src_heat;
static int dvel;
static double buoyancy ;
static float * u, * v, * u_prev, * v_prev;
static float * dens, * dens_prev;
static float * heat, * heat_prev;

static int win_id;
static int win_x, win_y;
static int mouse_down[3];
static int omx, omy, mx, my;


/*
 ----------------------------------------------------------------------
 free/clear/allocate simulation data
 ----------------------------------------------------------------------
 */


static void free_data ( void )
{
    if ( u ) free ( u );
    if ( v ) free ( v );
    if ( u_prev ) free ( u_prev );
    if ( v_prev ) free ( v_prev );
    if ( dens ) free ( dens );
    if ( dens_prev ) free ( dens_prev );
    if ( heat ) free ( heat );
    if ( heat_prev ) free ( heat_prev );
}

static void clear_data ( void )
{
    int i, size=(N+2)*(N+2);
    
    for ( i=0 ; i<size ; i++ ) {
        u[i] = v[i] = u_prev[i] = v_prev[i] = dens[i] = dens_prev[i] = heat[i] = heat_prev[i] = 0.0f;
    }
}

static int allocate_data ( void )
{
    int size = (N+2)*(N+2);
    
    u			= (float *) malloc ( size*sizeof(float) );
    v			= (float *) malloc ( size*sizeof(float) );
    u_prev		= (float *) malloc ( size*sizeof(float) );
    v_prev		= (float *) malloc ( size*sizeof(float) );
    dens		= (float *) malloc ( size*sizeof(float) );
    dens_prev	= (float *) malloc ( size*sizeof(float) );
    heat        = (float *) malloc ( size*sizeof(float) );
    heat_prev    = (float *) malloc ( size*sizeof(float) );
    
    if ( !u || !v || !u_prev || !v_prev || !dens || !dens_prev ) {
        fprintf ( stderr, "cannot allocate data\n" );
        return ( 0 );
    }
    
    return ( 1 );
}


/*
 ----------------------------------------------------------------------
 OpenGL specific drawing routines
 ----------------------------------------------------------------------
 */

static void pre_display ( void )
{
    glViewport ( 0, 0, win_x, win_y );
    glMatrixMode ( GL_PROJECTION );
    glLoadIdentity ();
    gluOrtho2D ( 0.0, 1.0, 0.0, 1.0 );
    glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear ( GL_COLOR_BUFFER_BIT );
    
}

static void post_display ( void )
{
    glutSwapBuffers ();
}

static void draw_velocity ( void )
{
    int i, j;
    float x, y, h;
    printf("draw_velocity\n");
    h = 1.0f/N;
    
    glColor3f ( 1.0f, 1.0f, 1.0f );
    glLineWidth ( 1.0f );
    
    glBegin ( GL_LINES );
    
    for ( i=1 ; i<=N ; i++ ) {
        x = (i-0.5f)*h;
        for ( j=1 ; j<=N ; j++ ) {
            y = (j-0.5f)*h;
            
            glVertex2f ( x, y );
            glVertex2f ( x+u[IX(i,j)], y+v[IX(i,j)] );
        }
    }
    
    glEnd ();
}

// draw的時候用density來決定顏色
// 用溫度？？？？
static void draw_density ( void )
{
    int i, j;
    float x, y, h, d00, d01, d10, d11;
    float d00b,d01b, d10b, d11b;
    
    h = 1.0f/N;
    
    //printf("draw_density\n");
    
    glBegin ( GL_QUADS );
    
    for ( i=0 ; i<=N ; i++ ) {
        x = (i-0.5f)*h;
        for ( j=0 ; j<=N ; j++ ) {
            y = (j-0.5f)*h;
            
            d00 = d00b = dens[IX(i,j)];
            d01 = d01b = dens[IX(i,j+1)];
            d10 = d10b = dens[IX(i+1,j)];
            d11 = d11b = dens[IX(i+1,j+1)];
            
            int s = (int)((       i /(float)win_x)*N+1);
            int t = (int)(((j-400)/(float)j)*N+1);
            //float speed = sqrt(v[IX(i,j)]*v[IX(i,j)]+u[IX(i,j)]*u[IX(i,j)]);
            //if(speed>0){
            //if(v[IX(i,j)]>=0.001 && u[IX(i,j)]>=0.001){
           // printf("color:[%f,%f,%f,%f]\n",d00,d10,d11,d01);
            float temp = (heat[IX(i,j)]+heat[IX(i,j+1)]+heat[IX(i+1,j)]+heat[IX(i+1,j+1)])/4;

            if(heat[IX(i,j)]>0.95 ) // white
                
            {glColor3f ( 1, 1, 0.8 ); glVertex2f ( x, y );}
            else if(heat[IX(i,j) ] >0.9){ // yellow white
                glColor3f ( 1, 1, 0 ); glVertex2f ( x, y );
            }
            else if(heat[IX(i,j) ] >0.7) { // yellow
                glColor3f ( 1, 0.8, 0 ); glVertex2f ( x, y );
            }
            else if(heat[IX(i,j) ] >0.5) { // orange
                glColor3f ( 1, 0.5, 0 ); glVertex2f ( x, y );
            }
            else if(heat[IX(i,j) ] >0.4) { // orange red
                glColor3f ( 1, 0.3, 0 ); glVertex2f ( x, y );
            }
            else if(heat[IX(i,j) ] >0.3) { // red
                glColor3f ( 1, 0, 0 ); glVertex2f ( x, y );
            }
            else if(heat[IX(i,j) ] >0.2) { // dark red
                glColor3f ( 0.5, 0, 0 ); glVertex2f ( x, y );
            }
            else {glColor3f ( 0, 0, 0 ); glVertex2f ( x, y );} //black
            
         
            
            
            
            
            if(heat[IX(i+1,j)]>0.999 ) // white
                
            {glColor3f ( 1, 1, 0.8 ); glVertex2f ( x+h, y );}
            else if(heat[IX(i+1,j) ] >0.9){ // jin bai
                glColor3f ( 1, 1, 0 ); glVertex2f ( x+h, y );
            }
            else if(heat[IX(i+1,j) ] >0.7) { // jin huang
                glColor3f ( 1, 0.8, 0 ); glVertex2f ( x+h, y );
            }
            else if(heat[IX(i+1,j) ] >0.5) { // orange
                glColor3f ( 1, 0.5, 0 ); glVertex2f ( x+h, y );
            }
            else if(heat[IX(i+1,j) ] >0.4) { // ju hong
                glColor3f ( 1, 0.3, 0 ); glVertex2f ( x+h, y );
            }
            else if(heat[IX(i+1,j) ] >0.3) { // shen hong
                glColor3f ( 1, 0, 0 ); glVertex2f ( x+h, y );
            }
            else if(heat[IX(i+1,j) ] >0.2) { // an hong
                glColor3f ( 0.5, 0, 0 ); glVertex2f ( x+h, y );
            }
            else {glColor3f ( 0, 0, 0 ); glVertex2f ( x+h, y );} //black

            
            
            
            if(heat[IX(i+1,j+1)]>0.999 ) // white
                
            {glColor3f ( 1, 1, 0.8 ); glVertex2f ( x+h, y+h );}
            else if(heat[IX(i+1,j+1) ] >0.9){ // jin bai
                glColor3f ( 1, 1, 0 ); glVertex2f ( x+h, y+h );
            }
            else if(heat[IX(i+1,j+1) ] >0.7) { // jin huang
                glColor3f ( 1, 0.8, 0 ); glVertex2f ( x+h, y+h );
            }
            else if(heat[IX(i+1,j+1) ] >0.5) { // orange
                glColor3f ( 1, 0.5, 0 ); glVertex2f ( x+h, y+h );
            }
            else if(heat[IX(i+1,j+1) ] >0.4) { // ju hong
                glColor3f ( 1, 0.3, 0 ); glVertex2f ( x+h, y+h );
            }
            else if(heat[IX(i+1,j+1) ] >0.3) { // shen hong
                glColor3f ( 1, 0, 0 ); glVertex2f ( x+h, y+h );
            }
            else if(heat[IX(i+1,j+1) ] >0.2) { // an hong
                glColor3f ( 0.5, 0, 0 ); glVertex2f ( x+h, y+h );
            }
            else {glColor3f ( 0, 0, 0 ); glVertex2f ( x+h, y+h );} //black
            
            
            
            
            
            if(heat[IX(i,j+1)]>0.999 ) // white
                
            {glColor3f ( 1, 1, 0.8 ); glVertex2f ( x, y+h );}
            else if(heat[IX(i,j+1) ] >0.9){ // jin bai
                glColor3f ( 1, 1, 0 ); glVertex2f ( x, y+h );
            }
            else if(heat[IX(i,j+1) ] >0.7) { // jin huang
                glColor3f ( 1, 0.8, 0 ); glVertex2f ( x, y+h );
            }
            else if(heat[IX(i,j+1) ] >0.5) { // orange
                glColor3f ( 1, 0.5, 0 ); glVertex2f ( x, y+h );
            }
            else if(heat[IX(i,j+1) ] >0.4) { // ju hong
                glColor3f ( 1, 0.3, 0 ); glVertex2f ( x, y+h );
            }
            else if(heat[IX(i,j+1) ] >0.3) { // shen hong
                glColor3f ( 1, 0, 0 ); glVertex2f ( x, y+h );
            }
            else if(heat[IX(i,j+1) ] >0.2) { // an hong
                glColor3f ( 0.5, 0, 0 ); glVertex2f ( x, y+h );
            }
            else {glColor3f ( 0, 0, 0 ); glVertex2f ( x, y+h );} //black
            
        }
    }
    
    glEnd ();
}

/*
 ----------------------------------------------------------------------
 relates mouse movements to forces sources
 ----------------------------------------------------------------------
 */

//一直跑
static void get_from_UI ( float * d, float * u, float * v ,float * h)
{
    int i, j, size = (N+2)*(N+2);
    
    for ( i=0 ; i<size ; i++ ) {
        u[i] = v[i] = d[i] = 0.0f;
    }
    //for(int s=0;s<50;i++){
    //    d[IX(s+200,300)] = source;
    //}
    //printf("add src\n");
    //if ( !mouse_down[0] && !mouse_down[2] ) return;
    
    i = (int)((       mx /(float)win_x)*N+1);
    j = (int)(((win_y-my)/(float)win_y)*N+1);
    
    for(int p=150;p<351;p++){
        int s = (int)((       p /(float)win_x)*N+1);
        int t = (int)(((win_y-450)/(float)win_y)*N+1);
        d[IX(s,t)] = source*100;
        h[IX(s,t)] = src_heat*10;
        s = (int)((       p /(float)win_x)*N+1);
        t = (int)(((win_y-420)/(float)win_y)*N+1);
        //u[IX(s,t)] = force * 10;
        //v 是垂直的速度
        v[IX(s,t)] = force*2;
    }
    
    if ( i<1 || i>N || j<1 || j>N ) return;
    
    //if(1){
    if ( mouse_down[0] ) {
        
        for(int p=200;p<201;p++){
            int s = (int)((       p /(float)win_x)*N+1);
            int t = (int)(((win_y-450)/(float)win_y)*N+1);
            //d[IX(s,t)] = source;
            //s = (int)((       p /(float)win_x)*N+1);
            //t = (int)(((win_y-420)/(float)win_y)*N+1);
            //u[IX(s,t)] = force * 10;
            //v 是垂直的速度
            v[IX(s,t)] = force*0.5;
        }
        
        //u[IX(i,j)] = force * (mx-omx);
        //v[IX(i,j)] = force * (omy-my);
        printf("add force:[%f,%f]\n",u[IX(i,j)],v[IX(i,j)]);
        /*for(int p=200;p<300;p++){
         int s = (int)((       p /(float)win_x)*N+1);
         int t = (int)(((win_y-450)/(float)win_y)*N+1);
         d[IX(s,t)] += source;
         s = (int)((       p /(float)win_x)*N+1);
         t = (int)(((win_y-250)/(float)win_y)*N+1);
         u[IX(s,t)] = force * 5;
         v[IX(s,t)] = force * 5;
         }*/
        
        //u[IX(200,150)] = 100;//force * (200-omx);
        //v[IX(200,150)] = 100;//force * (omy-150);
        // 右鍵，加一個力
        printf("get_from_UI:m0 [%d,%d]\n",mx,my);
    }
    
    //if(1){
    if ( mouse_down[2] ) {
        // 左鍵，加source
        //d[IX(200,200)] = source;
        d[IX(i,j)] = source;
        printf("get_from_UI:m2 [%d,%d]\n",mx,my);
    }
    
    omx = mx;
    omy = my;
    
    //omx=200;
    //omy=200;
    
    return;
}

/*
 ----------------------------------------------------------------------
 GLUT callback routines
 ----------------------------------------------------------------------
 */
// 設置的callback function key是按鍵， xy是滑鼠座標
static void key_func ( unsigned char key, int x, int y )
{
    switch ( key )
    {
        case 'c':
        case 'C':
            clear_data ();
            break;
            
        case 'q':
        case 'Q':
            free_data ();
            exit ( 0 );
            break;
            
        case 'v':
        case 'V':
            dvel = !dvel;
            break;
    }
}

// button 左鍵是0 右鍵是1
// state 按下滑鼠是0 放開是1
//xy 是滑鼠座標
static void mouse_func ( int button, int state, int x, int y )
{
    omx = mx = x;
    omx = my = y;
    
    mouse_down[button] = state == GLUT_DOWN;
}

//滑鼠按下去而且移動的時候的座標
static void motion_func ( int x, int y )
{
    mx = x;
    my = y;
}

//指定視窗大小
//只跑一次
static void reshape_func ( int width, int height )
{
    glutSetWindow ( win_id );
    glutReshapeWindow ( width, height );
    
    win_x = width;
    win_y = height;
    //printf("reshape_func\n");
}

//當視窗事件還未接收到動作，連續的動畫繪製
//一直跑
static void idle_func ( void )
{
    
    //printf("idle_func\n");
    //dens_prev[IX(i,j)] = source;
    //initial_flame
    
    get_from_UI ( dens_prev, u_prev, v_prev ,heat_prev);
    //always here
    //printf("idle_func: got UI\n");
    vel_step ( N, u, v, u_prev, v_prev, visc, dt );
    dens_temp_step ( N, dens, dens_prev, u, v, diff, dt ,heat, heat_prev);
    //dens_step ( N, dens, dens_prev, u, v, diff, dt );
    //dens_step ( N, heat, heat_prev, u, v, diff, dt );
    
    glutSetWindow ( win_id );
    glutPostRedisplay ();
}

//繪圖
static void display_func ( void )
{
    pre_display ();
    //printf("display\n");
    if ( dvel ) draw_velocity ();
    else		draw_density ();
    
    post_display ();
}

void increment(int t){
    printf("inc timer\n");
    //glutPostRedisplay();
    for(int p=200;p<300;p++){
        int s = (int)((       p /(float)win_x)*N+1);
        int t = (int)(((win_y-450)/(float)win_y)*N+1);
        dens[IX(s,t)] += source;
        s = (int)((       p /(float)win_x)*N+1);
        t = (int)(((win_y-250)/(float)win_y)*N+1);
        u[IX(s,t)] = force * 10;
        v_prev[IX(s,t)] = force * 0.1;
    }
    glutTimerFunc(50,increment,0);
}
/*
 ----------------------------------------------------------------------
 open_glut_window --- open a glut compatible window and set callbacks
 ----------------------------------------------------------------------
 */

static void open_glut_window ( void )
{
    glutInitDisplayMode ( GLUT_RGBA | GLUT_DOUBLE );
    
    glutInitWindowPosition ( 0, 0 );
    glutInitWindowSize ( win_x, win_y );
    win_id = glutCreateWindow ( "Alias | wavefront" );
    
    glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear ( GL_COLOR_BUFFER_BIT );
    glutSwapBuffers ();
    glClear ( GL_COLOR_BUFFER_BIT );
    glutSwapBuffers ();
    
    pre_display ();
    
    glutKeyboardFunc ( key_func );
    glutMouseFunc ( mouse_func );
    glutMotionFunc ( motion_func );
    glutReshapeFunc ( reshape_func );
    glutIdleFunc ( idle_func );
    glutDisplayFunc ( display_func );
    //printf("open_glut_window\n");
    //glutTimerFunc(50,increment,0);
}


/*
 ----------------------------------------------------------------------
 main --- main routine
 ----------------------------------------------------------------------
 */

int main ( int argc, char ** argv )
{
    glutInit ( &argc, argv );
    
    if ( argc != 1 && argc != 6 ) {
        fprintf ( stderr, "usage : %s N dt diff visc force source\n", argv[0] );
        fprintf ( stderr, "where:\n" );\
        fprintf ( stderr, "\t N      : grid resolution\n" );
        fprintf ( stderr, "\t dt     : time step\n" );
        fprintf ( stderr, "\t diff   : diffusion rate of the density\n" );
        fprintf ( stderr, "\t visc   : viscosity of the fluid\n" );
        fprintf ( stderr, "\t force  : scales the mouse movement that generate a force\n" );
        fprintf ( stderr, "\t source : amount of density that will be deposited\n" );
        exit ( 1 );
    }
    
    if ( argc == 1 ) {
        N = 120;
        dt = 0.02f;
        diff = 0.0f;
        visc = 0.0f;
        force = 15.0f;
        source = 50.0f;
        src_heat = 270.0f;
        buoyancy = 0.5;
        fprintf ( stderr, "Using defaults : N=%d dt=%g diff=%g visc=%g force = %g source=%g\n",
                 N, dt, diff, visc, force, source );
    } else {
        N = atoi(argv[1]);
        dt = atof(argv[2]);
        diff = atof(argv[3]);
        visc = atof(argv[4]);
        force = atof(argv[5]);
        source = atof(argv[6]);
    }
    /*
    printf ( "\n\nHow to use this demo:\n\n" );
    printf ( "\t Add densities with the right mouse button\n" );
    printf ( "\t Add velocities with the left mouse button and dragging the mouse\n" );
    printf ( "\t Toggle density/velocity display with the 'v' key\n" );
    printf ( "\t Clear the simulation by pressing the 'c' key\n" );
    printf ( "\t Quit by pressing the 'q' key\n" );
    */
    dvel = 0;
    
    if ( !allocate_data () ) exit ( 1 );
    clear_data ();
    
    win_x = 512;
    win_y = 512;
    open_glut_window ();
    //glutTimerFunc(50,increment,0);
    glutMainLoop ();
    
    exit ( 0 );
}
