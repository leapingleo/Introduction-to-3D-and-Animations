#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <GLUT/GLUT.h>
#include <iostream>
#include <vector>

typedef struct { float x, y;} Vec2f;
typedef struct { Vec2f r0, v0, r, v; } Frog;
typedef struct { Vec2f pos; float width, height; } Car;
typedef struct { float start; float width, height; } Road;
typedef struct { Vec2f r; float radius; } Log;

Vec2f initVel = { 0.25, 70 };

Frog frog = {
    { 0.0, 0.0 },
    { initVel.x * cosf(M_PI * initVel.y / 180), initVel.x * sinf(M_PI * initVel.y / 180) },
    { -0.1, 0.0 },
    { 0.0, 0.0 }
};

const float g = -0.25;
const int milli = 1000;
float lastX = 0;
float lastY = 0;

float t = 0;
float landingY = 0;
float dx = 0;
bool filled = false;
bool dead = false;
bool isFlying = false;
bool hitCar = false;
Car cars[4];
Log logs[4];
Road road;

typedef struct {
    bool debug;
    bool go;
    float startTime;
    bool OSD;
    int frames;
    float frameRate;
    float frameRateInterval;
    float lastFrameRateT;
} global_t;

global_t global = { false, false, 0.0, true, 0, 0.0, 0.2, 0.0 };

float deg2rad(float deg) {
    return M_PI * deg / 180;
}

float rad2deg(float rad) {
    return 180 * rad / M_PI;
}

float magnitude(float x, float y) {
    return sqrt(x * x + y * y);
}

void drawVector(float x, float y, float a, float b, float s, bool normalize, float red, float green, float blue) {
    glBegin(GL_LINES);
    glColor3f(1, 1, 1);
    glVertex2f(x, y);
    glVertex2f(x + a / magnitude(a, b) * s, y + b /magnitude(a, b) * s);
    glEnd();
}

float getHypotenuse(float a, float b) {
    return sqrt(a * a + b * b);
}

float velocityAngle(float x, float y) {
    return atan2f(y, x);
}

float calculateSine(float x, float dx){
    return -1.0 / 32 + 1.0 / 32 * sin(4 * M_PI * x + M_PI / 4 * dx);
}

bool isCollideWithRoad(float x, float y) {
    return x >= road.start && x <= road.start + road.width && y <= road.height;
}

bool isCollideWithCar(float x, float y, Car car) {
    return x >= car.pos.x - car.width / 2 && x <= car.pos.x + car.width / 2 && y <= car.pos.y + car.height / 2;
}

bool isCollideWithLog(float x, float y, Log log) {
    return ((x - log.r.x) * (x - log.r.x) + (y - log.r.y) * (y - log.r.y)) <= log.radius * log.radius;
}

// at x, find out the exact y
bool isUnderWater(float x, float y, float dx){
    return y <= calculateSine(x, dx);
}

bool isCollideWithCars(float x, float y) {
    for (int i = 0; i < 4; i++) {
        if (isCollideWithCar(x, y, cars[i]))
            return true;
    }
    return false;
}

bool isCollideWithLogs(float x, float y) {
    if (isCollideWithLog(x, y, logs[0])) {
        return true;
    } else if (isCollideWithLog(x, y, logs[1])) {
        return true;
    } else if (isCollideWithLog(x, y, logs[2])) {
        return true;
    } else if (isCollideWithLog(x, y, logs[3])) {
        return true;
    } else {
        return false;
    }
}

Log* findLandingLog(float x, float y){
    for (int i = 0; i < 4; i++) {
        if (isCollideWithLog(x, y, logs[i]))
            return &logs[i];
    }
    return NULL;
}


void drawTrajectoryAnalytical() {
    
    
    float x = frog.r.x, y = frog.r.y;
    float dt = 0.01;
    int n = 1000;
    Vec2f v;
    if (!isFlying) {
        v.x = initVel.x * cos(deg2rad(initVel.y));
        v.y = initVel.x * sin(deg2rad(initVel.y));
    } else
        v = frog.v;
    
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 1, 1);
    int count = 0;
    
    while (count <= n ) {
        x += v.x * dt;
        y += v.y * dt;
        
        v.y += g * dt;
        
        if (x <= 0 && y <= 0){
            break;
        }
        if (x >= 0 && y <= -0.0625){
            break;
        }
        if (isCollideWithCars(x, y))
            break;
        if (isCollideWithRoad(x, y))
            break;
        if (isCollideWithLogs(x, y))
            break;
        if (x >= 0.75 && y <= 0)
            break;
        //  if (isCollideWithLogs(x, y))
        //   break;
        glVertex2f(x, y);
        count++;
    }
    glEnd();
}


void drawRiver() {
    //y = A * sin(k * x + w * t)
    int n = 20;
    float left = 0;
    float right = 0.75;
    float range = right - left;
    float stepSize = range / (float)n;
    float x, y;
    //  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (filled)
        glBegin(GL_QUAD_STRIP);
    else
        glBegin(GL_LINE_STRIP);
    //glVertex2f(x, -1.0 / 32 / 2.0);
    glColor3f(0, 1, 1);
    
    //draws a sine function
    for (int i = 0; i <= n; i++){
        x = i * stepSize + left;
        y = calculateSine(x, dx);
        glVertex2f(x, y);
        if (filled)
            glVertex2f(x, -0.0625);
    }
    
    glEnd();
    glutPostRedisplay();
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

//-0.75, 0.5, 0.025,
void drawLine() {
    glBegin(GL_LINE_STRIP);
    //  glColor3f(1, 0.65, 0);
    glVertex3f(-1, 0, 0);
    //road starts
    glVertex3f(road.start, 0, 0);
    glVertex3f(road.start, road.height, 0);
    glVertex3f(road.start + road.width, road.height, 0);
    glVertex3f(road.start + road.width, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, -1.0 / 16, 0);
    glVertex3f(0.75, -0.0625, 0);
    glVertex3f(0.75, 0, 0);
    glVertex3f(1, 0, 0);
    glEnd();
}

void drawOval(float a, float b) {
    float theta;
    float x, y;
    //change color to red if dead
    if (!dead)
        glColor3f(0, 1, 0);
    else
        glColor3f(1, 0, 0);
    // glLineWidth(2.0);
    if (filled)
        glBegin(GL_POLYGON);
    else
        glBegin(GL_LINE_LOOP);
    
    for (int i = 0; i <= 100; i++) {
        theta = i /(float)100 * 2.0 * M_PI;
        x = a * cosf(theta);
        
        y = b * sinf(theta);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawCircle(float x, float y, float radius) {
    if (filled)
        glBegin(GL_POLYGON);
    else
        glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 100; i++) {
        float theta = i / (float)100 * 2.0 * M_PI;
        float x1 = radius * cosf(theta);
        float y1 = radius * sinf(theta);
        glVertex2f(x + x1, y + y1);
    }
    glEnd();
}

void drawCar(Vec2f pos, float width, float height) {
    if (filled)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(1.0, 1.0, 0);
    glBegin(GL_QUADS);
    glVertex2f(pos.x - width / 2, pos.y + height / 2);
    glVertex2f(pos.x - width / 2, pos.y);
    glVertex2f(pos.x + width / 2, pos.y);
    glVertex2f(pos.x + width / 2, pos.y + height / 2);
    glEnd();
}

void drawCars() {
    for (int i = 0; i < 4; i++) {
        drawCar(cars[i].pos, cars[i].width, cars[i].height);
    }
}

void drawLogs(){
    for (int i = 0; i < 4; i++) {
        float x = logs[i].r.x;
        logs[i].r.y = calculateSine(logs[i].r.x, dx);
        glColor3f(0.8, 0.6, 0.13);
        drawCircle(x, logs[i].r.y, logs[i].radius);
    }
}

void updatePosition(float dt)
{
    // Position
    frog.r.x += frog.v.x * dt;
    frog.r.y += frog.v.y * dt;
    
    // Velocity
    frog.v.y += g * dt;
}


void displayRoad(void)
{
    drawLine();
    // glColor3f(0, 1, 0);
}
//float landingY = 0;
void reset(){
    isFlying = false;
    frog.v.x = 0;
    frog.v.y = 0;
    frog.r.y = landingY;
    global.go = false;
    frog.v0.x = initVel.x * cos(deg2rad(initVel.y));
    frog.v0.y = initVel.x * sin(deg2rad(initVel.y));
}

void update(void)
{
    dx += 0.04;
    
    if (dead) {
        if (!hitCar)
            frog.r.y = calculateSine(frog.r.x, dx);
        return;
    }
    updatePosition(0.05);
    
    if (frog.r.y < 0 && frog.r.x < 0) {
        landingY = 0;
        reset();
    } else if (isCollideWithRoad(frog.r.x, frog.r.y)) {
        landingY = road.height;
        reset();
    } else if (isCollideWithLogs(frog.r.x, frog.r.y)) {
        Log* log = findLandingLog(frog.r.x, frog.r.y);
        frog.r.x = log->r.x;
        //landingX = log->r.x;
        landingY = log->r.y + log->radius;
        reset();
    } else if (frog.r.y < 0 && frog.r.x > 0.75) {
        landingY = 0;
        reset();
    } else if (isUnderWater(frog.r.x, frog.r.y, dx)) {
        dead = true;
    } else if (isCollideWithCars(frog.r.x, frog.r.y)) {
        hitCar = true;
        dead = true;
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
    
    // drawRoad
    displayRoad();
    drawRiver();
    drawLogs();
    
    Vec2f v;
    if (!isFlying){
        v.x = initVel.x * cos(deg2rad(initVel.y));
        v.y = initVel.x * sin(deg2rad(initVel.y));
        //  initVel.x * cos(deg2rad(initVe
    }
    else {
        v.x = frog.v.x;
        v.y = frog.v.y;
    }
    
    glPushMatrix();
    glTranslatef(frog.r.x, frog.r.y, 0);
    drawVector(0, 0, v.x, v.y, 0.2, true, 1.0, 0.65, 0);
    
    glPushMatrix();
    glRotatef(rad2deg(atan2f(v.y, v.x)), 0, 0, 1);
    glTranslatef(0.03, 0, 0);
    
    drawOval(0.03, 0.018);
    glPopMatrix();
    glPopMatrix();
    
    glLoadIdentity();
    drawTrajectoryAnalytical();
    drawCars();
    glPopMatrix();
    drawAxes(1);
    
    
    // std::cout << initVel.speed << std::endl;
    // Display OSD
    if (global.OSD)
        displayOSD();
    
    
    glutSwapBuffers();
    
    global.frames++;
    
    // Check for errors
    while ((err = glGetError()) != GL_NO_ERROR)
        printf("%s\n",gluErrorString(err));
}

void myinit (void)
{
    road = Road { -0.75, 0.5, 0.025 };
    
    cars[0] = Car { {-0.68, road.height}, 0.06, 0.07 };
    cars[1] = Car { {-0.56, road.height}, 0.08, 0.12 };
    cars[2] = Car { {-0.43, road.height}, 0.07, 0.04 };
    cars[3] = Car { {-0.32, road.height}, 0.06, 0.18 };
    //0.025
    logs[0] = Log { {0.15, 0}, 0.025 };
    logs[1] = Log { {0.3, 0}, 0.025 };
    logs[2] = Log { {0.45, 0}, 0.025 };
    logs[3] = Log { {0.6, 0}, 0.025 };
}



void keyboardCB(unsigned char key, int x, int y)
{
    switch (key) {
        case 27:
        case 'f':
            filled = !filled;
            
            break;
        case 'q':
            exit(EXIT_SUCCESS);
            break;
        case 'a':
            initVel.y += 1;
            std::cout << "A pressed " << initVel.y << std::endl;
            break;
        case 'd':
            initVel.y -= 1;
            std::cout << "D pressed " << initVel.y << std::endl;
            // global.debug = !global.debug;
            break;
        case 'w':
            initVel.x += 0.01;
            std::cout << "W pressed " << initVel.x << std::endl;
            break;
        case 'r':
            reset();
            break;
        case 's':
            initVel.x -= 0.01;
            std::cout << "S pressed " << initVel.x << std::endl;
            // global.debug = !global.debug;
            break;
        case 'o':
            global.OSD = !global.OSD;
            break;
        case ' ':
            isFlying = true;
            //   restart = false;
            std::cout << "space pressed " << global.go << std::endl;
            if (!global.go) {
                std::cout << "pressed " << std::endl;
                global.startTime = glutGet(GLUT_ELAPSED_TIME) / (float)milli;
                global.go = true;
                frog.v.x = frog.v0.x;
                frog.v.y = frog.v0.y;
                
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
    glutCreateWindow("Frogger!!!");
    myinit();
    glutKeyboardFunc(keyboardCB);
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutIdleFunc(update);
    glutMainLoop();
}
