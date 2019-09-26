/*
 * projectile-motion.c
 *
 * This program shows how to compute projectile motion using both
 * analytical and numerical integration of the equations of motion.
 *
 * $Id: projectile-motion.c,v 1.9 2019/03/22 02:53:51 gl Exp gl $
 *
 */
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <GLUT/GLUT.h>
#include <iostream>
#include <vector>

float deg2rad(float deg) {
    return deg * M_PI / 180;
}
bool restart = false;
typedef struct { float x, y; } vec2f;
typedef struct { float speed, angle; } vec2fPolar;

typedef struct { vec2f r0, v0, r, v; } state;
vec2fPolar initVel = {0.5, 60};
state projectile = {
    { 0.0, 0.0 },
    { initVel.speed * cos(deg2rad(initVel.angle)), initVel.speed * sin(deg2rad(initVel.angle)) },
    { 0.0, 0.0 },
    { 0.0, 0.0 }
};

/* Use a low gravity value, rather than true Earth gravity of 9.8,
 * to slow down projectile acceleration, otherwise too fast for
 * nice projectile motion aesthetics.
 */
const float g = -0.25;
const int milli = 1000;

float lastX = 0;
float lastY = 0;

std::vector<vec2f> points;

typedef enum { analytical, numerical } integrationMode;

typedef struct {
    bool debug;
    bool go;
    float startTime;
    integrationMode integrateMode;
    bool OSD;
    int frames;
    float frameRate;
    float frameRateInterval;
    float lastFrameRateT;
} global_t;

global_t global = { false, false, 0.0, numerical, true, 0, 0.0, 0.2, 0.0 };


float magnitude(float x, float y) {
    return sqrt(x * x + y * y);
}

void drawVector(float x, float y, float a, float b, float s, bool normalize, float red, float green, float blue) {
    glBegin(GL_LINES);
    glColor3f(red, green, blue);
    glVertex2f(x, y);
    glVertex2f(x + a / magnitude(a, b) * 0.5, y + b /magnitude(a, b) * 0.5);
    glEnd();
}

//Xt = Vx0*t + X0
//Yt = -0.5 * g * t^2 + Vy0 * t + y0
//parametric equations for projectile motion
//Time of Flight = 2 * V0 * sin(theta) / g;
void drawTrajectoryAnalytical(){
    float timeOfFlight = abs(2 * initVel.speed * sinf(deg2rad(initVel.angle)) / g);
    float timeSegament = 1000;
    float dt = timeOfFlight / timeSegament;
    
    float vx0 = initVel.speed * cosf(deg2rad(initVel.angle));
    float vy0 = initVel.speed * sinf(deg2rad(initVel.angle));
    
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 1, 1);
    for (int i = 0; i <= timeSegament; i++) {
        float t = dt * i;
        glVertex2f(vx0 * t, 0.5 * g * t * t + vy0 * t);
    }
   glEnd();
}

void drawAxes(float length) {
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(length, 0, 0);
    glEnd();
    
    glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, length, 0);
    glEnd();
    
    glBegin(GL_LINES);
    glColor3f(0, 0, 1.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, length);
    glEnd();
}

void drawLine(float a, float b, float x, float y) {
    glBegin(GL_LINES);
    glColor3f(1, 0.65, 0);
    glVertex3f(a, b, 0);
    glVertex3f(x, y, 0);
    glEnd();
}

void updateProjectileStateAnalytical(float t)
{
    vec2f r0, v0;
    
    r0 = projectile.r0;
    v0 = projectile.v0;
    
    projectile.r.x = v0.x * t + r0.x;
    projectile.r.y = 1.0 / 2.0 * g * t * t + v0.y * t + r0.y;
    /*
     * The above statement can alternatively be written as below, saving
     * a few arithmetic operations, which may or may not be worthwhile
     * and which can obscure code and introduce hard to find bugs:
     *
     * 1) Precalculate 1.0 / 2.0 as 0.5 to save a division
     * projectileState.r.y = 0.5 * g * t * t + v0.y * t + r0.y;
     *
     * 2) Factorise to save a multiplication
     * projectileState.r.y = t * (0.5 * g * t + v0.y) + r0.y;
     */
}

void updateProjectileStateNumerical(float dt)
{
    // Euler integration
    
    // Position
    projectile.r.x += projectile.v.x * dt;
    projectile.r.y += projectile.v.y * dt;
    
    // Velocity
    projectile.v.y += g * dt;
}

void updateProjectileState(float t, float dt)
{
    if (global.debug)
        printf("global.integrateMode: %d\n", global.integrateMode);
    if (global.integrateMode == analytical)
        updateProjectileStateAnalytical(t);
    else
        updateProjectileStateNumerical(dt);
}


void displayProjectile(void)
{
    drawAxes(1.0);
    glColor3f(0.5, 0.5, 0.5);
    glPushMatrix();
        glTranslatef(projectile.r.x, projectile.r.y, 0);
        glutSolidSphere(0.02, 40, 16);
    glPopMatrix();
    
    //keep storing current x and y for drawing the trail
    points.push_back({projectile.r.x, projectile.r.y});
    //draws a trail of the moving sphere
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < points.size(); i++) {
        glVertex3f(points.at(i).x, points.at(i).y, 0);
    }
    glEnd();
    drawVector(0, 0, initVel.speed * cosf(deg2rad(initVel.angle)), initVel.speed * sinf(deg2rad(initVel.angle)), 0.5, true, 1.0, 0.65, 0);
    drawTrajectoryAnalytical();
    //drawLine(0, 0, 0.5 * cos(deg2rad(initVel.angle)), 0.5 * sin(deg2rad(initVel.angle)));
}

// Idle callback for animation
void update(void)
{
    projectile.v0.x = initVel.speed * cos(deg2rad(initVel.angle));
    projectile.v0.y = initVel.speed * sin(deg2rad(initVel.angle));
    static float lastT = -1.0;
    float t, dt;
    
    if (!global.go)
        return;
    
    t = glutGet(GLUT_ELAPSED_TIME) / (float)milli - global.startTime;
    
    if (lastT < 0.0) {
        lastT = t;
        return;
    }
    
    dt = t - lastT;
    if (projectile.r.y <= 0)
        printf("%f %f\n", t, dt);
    //for restart purpose, otherwise we get negative -dt as lastT is set to -1.0!!
    if (dt > 0)
        updateProjectileState(t, dt);
    lastT = t;
    
    /* Frame rate */
    dt = t - global.lastFrameRateT;
    if (dt > global.frameRateInterval) {
        global.frameRate = global.frames / dt;
        global.lastFrameRateT = t;
        global.frames = 0;
    }
    glutPostRedisplay();
}

void displayOSD()
{
    char buffer[30];
    char *bufp;
    int w, h;
    
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    /* Set up orthographic coordinate system to match the
     window, i.e. (0,0)-(w,h) */
    w = glutGet(GLUT_WINDOW_WIDTH);
    h = glutGet(GLUT_WINDOW_HEIGHT);
    glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    /* Frame rate */
    glColor3f(1.0, 1.0, 0.0);
    glRasterPos2i(10, 60);
    snprintf(buffer, sizeof buffer, "fr (f/s): %6.0f", global.frameRate);
    for (bufp = buffer; *bufp; bufp++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
    
    /* Time per frame */
    glColor3f(1.0, 1.0, 0.0);
    glRasterPos2i(10, 40);
    snprintf(buffer, sizeof buffer, "ft (ms/f): %5.0f", 1.0 / global.frameRate * 1000.0);
    for (bufp = buffer; *bufp; bufp++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
    
    /* Pop modelview */
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    
    /* Pop projection */
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    /* Pop attributes */
    glPopAttrib();
}

void display(void)
{
    GLenum err;
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glColor3f (0.8, 0.8, 0.8);
    
    // Display projectile
    displayProjectile();
   // std::cout << initVel.speed << std::endl;
    // Display OSD
    if (global.OSD)
        displayOSD();
    
    glPopMatrix();
    
    glutSwapBuffers();
    
    global.frames++;
    
    // Check for errors
    while ((err = glGetError()) != GL_NO_ERROR)
        printf("%s\n",gluErrorString(err));
}

void myinit (void)
{
}

void reset(){
    
    points.clear();
    points.push_back({0.0, 0.0});
    projectile.v.x = 0;
    projectile.v.y = 0;
    projectile.r.x = 0;
    projectile.r.y = 0;
    global.go = false;
    global.startTime = 0;
   
   // restart = true;
    projectile.v0.x = initVel.speed * cos(deg2rad(initVel.angle));
    projectile.v0.y = initVel.speed * sin(deg2rad(initVel.angle));
     std::cout << "after pressed reset init speed " << projectile.v0.x << ", angle " << projectile.v0.y << std::endl;
   
   
}

void keyboardCB(unsigned char key, int x, int y)
{
    switch (key) {
        case 27:
        case 'q':
            exit(EXIT_SUCCESS);
            break;
        case 'a':
            initVel.angle += 1;
            std::cout << "A pressed " << initVel.angle << std::endl;
            break;
        case 'd':
            initVel.angle -= 1;
            std::cout << "D pressed " << initVel.angle << std::endl;
           // global.debug = !global.debug;
            break;
        case 'w':
            initVel.speed += 0.05;
            std::cout << "W pressed " << initVel.speed << std::endl;
            break;
        case 'r':
            reset();
            break;
        case 's':
            initVel.speed -= 0.05;
            std::cout << "S pressed " << initVel.speed << std::endl;
            // global.debug = !global.debug;
            break;
        case 'i':
            if (global.integrateMode == analytical)
                global.integrateMode = numerical;
            else
                global.integrateMode = analytical;
            break;
        case 'o':
            global.OSD = !global.OSD;
            break;
        case ' ':
            drawTrajectoryAnalytical();
            restart = false;
            if (!global.go) {
                global.startTime = glutGet(GLUT_ELAPSED_TIME) / (float)milli;
                global.go = true;
                projectile.v.x = projectile.v0.x;
                projectile.v.y = projectile.v0.y;
            }
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void myReshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/*  Main Loop
 *  Open window with initial window size, title bar,
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(500, 500);
    glutCreateWindow("Projectile Motion");
    glutKeyboardFunc(keyboardCB);
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutIdleFunc(update);
    
    myinit();
    
    glutMainLoop();
}
