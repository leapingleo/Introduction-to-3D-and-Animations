#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <GLUT/GLUT.h>
#include <OpenGL/glu.h>
#include "helper.h"
#include "drawHelper.h"

typedef struct { float lastX, lastY, zoomFactor; bool zoomed; float scale;
} Camera;

typedef struct { float x, y;} Vec2f;
typedef struct { float x, y, z; } Vec3f;
typedef struct { Vec3f r0, v0, v; Vec3f r; float rotationX, rotationY; bool isAlive; } Frog;
typedef struct { float radius, width, posX, posY, posZ; } Log;
typedef struct { float groundX, groundZ, riverPosX, riverPosZ, riverSizeX, riverSizeZ, riverHeight; } Scene;

Vec2f initVel = { 0.5, 70 };
Frog frog = {
    { 0.0, 0.0 },
   // { initVel.x * cosf(M_PI * initVel.y / 180) * cosf(deg2rad(frog.rotationY)),
   //   initVel.x * sinf(M_PI * initVel.y / 180), },
    { 0, 0, 0},
    { 0.0, 0.0 },
    //frog x, y, z
    { 0.5, 0.0, 0.0 },
    0, 0, true
};

float dx = 0;
float dy = -45;
float rotateY = 0;
float rotateX = 0;
float movingX = 0, movingZ = -2;
float scale = 1;
float jumping = false;
float a = 0;
float r = 0.03;
float t = 0;
Sinewave water_sw = { 1.0/16.0, M_PI * 4, 2 * M_PI, 0.25 * M_PI };
Sinewave sw1 = {0.25, M_PI, 2 * M_PI,  M_PI };
bool isWireframe = false;
Log logs[5];

Camera camera { false, 0.3 };
Scene scene { 0, 0, -1.5, 0, 3, 6, 0.15 };

static GLuint grassTexture;
static GLuint woodTexture, negXTexture, negYTexture,
              negZTexture, posXTexture, posYTexture, posZTexture;

void drawAxes(float l) {
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(l, 0.0, 0.0);
    glEnd();
    
    glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, l, 0);
    glEnd();
    
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, l);
    glEnd();
}

void mouseMotion(int x, int y) {
    //make sure it doesnt rotate wiredly by large amount when last mouse pos
    //are too far from current mouse pos
    
    if (x - camera.lastX < 5 && y - camera.lastY < 5) {
        dx += x - camera.lastX;
        dy += y - camera.lastY;
    }
    camera.lastX = x;
    camera.lastY = y;
    std::cout << camera.lastX << std::endl;
}

void draw_rectangle(float x, float y, float z, float size){
    glColor3f(0, 1, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_POLYGON);
    glVertex3f(x, y, z);
    glVertex3f(x, y, z + size);
    glVertex3f(x + size, y, z + size);
    glVertex3f(x, y, z);
    glVertex3f(x + size, y, z);
    glVertex3f(x + size, y, z + size);
    glEnd();
    glColor3f(1, 1, 1);
    //    drawVector(x, y, z, 0, 0.5, 0, 0.05, false, 1.0, 1.0, 0.0);
    //    drawVector(x, y, z + 0.1, 0, 0.5, 0, 0.05, false, 1.0, 1.0, 0.0);
    //    drawVector(x + 0.1, y, z, 0, 0.5, 0, 0.05, false, 1.0, 1.0, 0.0);
    //    drawVector(x + 0.1, y, z + 0.1, 0, 0.5, 0, 0.05, false, 1.0, 1.0, 0.0);
}

void draw_plane(float x, float y, float z, float planeSize) {
    if (isWireframe) {
        float n = planeSize * 5;
        float stepSize = planeSize / n;
        for (int i = 0; i < n; i++) {
           for (int j = 0; j < n; j++) {
               float x1 = -planeSize * 0.5 + i * stepSize + x;
               float z1 = -planeSize * 0.5 + j * stepSize + z;
               draw_rectangle(x1, y, z1, stepSize);
           }
        }
    } else {
        float offset = planeSize * 0.5;
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, grassTexture);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glColor3f(1, 1, 1);

        glBegin(GL_QUADS);
            glVertex3f(x-offset, 0,z-offset); glTexCoord2f(0, 0);
            glVertex3f(x+offset, 0, z-offset); glTexCoord2f(1, 0);
            glVertex3f(x+offset, 0, z+offset); glTexCoord2f(1, 1);
            glVertex3f(x-offset, 0, z+offset); glTexCoord2f(0, 1);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
}

void draw_sphere() {
    float x, y, z;
    float theta, phi;
    glColor3f(0, 1, 0);
    glPointSize(2.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    
    for (int i = 0; i < 20; i++){
        phi = i / (float)20 * M_PI;
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < 100; j++) {
            theta = j / (float)100 * 2 * M_PI;
            x = r * sinf(phi) * sinf(theta);
            y = r * cosf(phi);
            z = r * sinf(phi) * cosf(theta);
            glVertex3f(x, y, z);
        }
        glEnd();
    }
    
    for (int i = 0; i < 4; i++) {
        phi = M_PI / 2;
        glRotatef(360 / 8, 0, 1, 0);
        glPushMatrix();
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < 100; j++) {
            theta = j / (float)100 * 2 * M_PI;
            y = r * sinf(phi) * sinf(theta);
            x = r * cosf(phi);
            z = r * sinf(phi) * cosf(theta);
            glVertex3f(x, y, z);
        }
        glEnd();
        glPopMatrix();
    }
}

void draw_water(float sizeX, float sizeZ, float x, float y, float z){
    float n = 50;
    float xStep = sizeX / n;
    float zStep = sizeZ / n;
   
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0, 1, 1, 0.8);
    if (isWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    for (int j = 0; j < n; j++) {
        glBegin(GL_TRIANGLE_STRIP);
        float z1 = sizeZ * -0.5 + j * zStep + z;
        
        for (int i = 0; i <= n; i++) {
            float x1 = sizeX * -0.5 + i * xStep + x;
          //y = fourierSinewave(sw, x, 0, 200, t);
            float y1 = calcSineWave(water_sw, x1, 0, t) + y;
          //  y = calcSineWave(sw, x, z, t) + calcSineWave(sw1, x, z, 0);
            glVertex3f(x1, y1, z1);
            glVertex3f(x1, y1, z1 + zStep);
        }
        glEnd();
    }
}

bool collideWithLog(Log log, float x, float y, float z, bool withFrog) {
    float r = 0;
    
    if (withFrog)
        r = log.radius + 0.03;
    else
        r = log.radius;
    
    return (x - log.posX) * (x - log.posX) + (y - log.posY) * (y - log.posY) <= r * r && z >= log.posZ && z <= log.posZ + log.width;
}

void reset() {
    jumping = false;
    frog.v.x = 0;
    frog.v.y = 0;
    frog.v.z = 0;
  //  frog.r.y = 0;
}

void idle(void) {
    t += 0.1;
    
    if (!frog.isAlive) {
        frog.r.y = calcSineWave(water_sw, frog.r.x, frog.r.z, dx);
        return;
    }
    
    frog.r.x += frog.v.x * 0.1;
    frog.r.y += frog.v.y * 0.1;
    frog.r.z += frog.v.z * 0.1;
    // Velocity
    frog.v.y += -0.25 * 0.1;
    
//    if (isUnderwater(water_sw, frog.r.x, frog.r.y, frog.r.z, dx)){
//        frog.isAlive = false;
//    }
    
    if (frog.r.x >= -1 && frog.r.x <= 1 && frog.r.z >= -1 && frog.r.z <= 1 && frog.r.y <= 0) {
        frog.r.y = 0;
        reset();
    }
    if (frog.r.x >= scene.riverPosX - scene.riverSizeX * 0.5 &&
        frog.r.x <= scene.riverPosX + scene.riverSizeX * 0.5 &&
        frog.r.z >= scene.riverPosZ - scene.riverSizeZ * 0.5 &&
        frog.r.z <= scene.riverPosZ + scene.riverSizeZ * 0.5
        ) {
        for (int i = 0; i < 5; i++) {
            if (collideWithLog(logs[i], frog.r.x, frog.r.y, frog.r.z, true)) {
                frog.r.y = logs[i].posY + logs[i].radius;
                reset();
               // frog.r.x = logs[0].posX;
              //  reset();
            }
        }
    }
    
    glutPostRedisplay();
}



void draw_trajectory(){
    float x = frog.r.x, y = frog.r.y, z = frog.r.z;
    float dt = 0.01;
    int n = 1000;
    Vec3f v;
    
    if (!jumping) {
        v.x = initVel.x * cosf(M_PI * initVel.y / 180) * cosf(M_PI * frog.rotationY / 180);
        v.y = initVel.x * sinf(M_PI * initVel.y / 180);
        v.z = initVel.x * cosf(M_PI * initVel.y / 180) * sinf(M_PI * frog.rotationY / 180);
    } else {
        v = frog.v;
    }
    
    glLineWidth(3.0);
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 1, 1);
    int count = 0;
    
    while (count <= n ) {
        
        x += v.x * dt;
        y += v.y * dt;
        z += v.z * dt;
        v.y += -0.25 * dt;
        
        
        //ground
        if ( x >= -1 && x <= 1 && z >= -1 && z <= 1 && y <= 0){
            break;
        }
        //river
        if (x >= scene.riverPosX - scene.riverSizeX * 0.5 &&
            x <= scene.riverPosX + scene.riverSizeX * 0.5 &&
            z >= scene.riverPosZ - scene.riverSizeZ * 0.5 &&
            z <= scene.riverPosZ + scene.riverSizeZ * 0.5 &&
            y <= -scene.riverHeight) {
            break;
        }
            
        if (collideWithLog(logs[0], x, y, z, false)) {
            break;
        }
     //   if (x >= scene.riverPosX)

        //  if (isCollideWithLogs(x, y))
        //   break;
        glVertex3f(x, y, z);
        count++;
    }
    glEnd();
    glLineWidth(1.0);
    
}

bool horizontalRotate = false;
void draw_Frog(){
//    if (horizontalRotate) {
//        glTranslatef(movingX, 0, 0);
//        glRotatef(frog.rotationY, 0, 1, 0);
//       // glRotatef(frog.rotationX, 1, 0, 0);
//      //  glTranslatef(-movingX, 0, 0);
//    }
//    else {
//        glRotatef(frog.rotationY, 0, 1, 0);
//        glTranslatef(movingX, 0, 0);
//    }
//    glRotatef(frog.rotationX, 1, 0, 0);
//    if (!jumping){
//        glTranslatef(frog.r.x, frog.r.y, 0);
//        glRotatef(frog.rotationY, 0, 1, 0);
//        glTranslatef(-frog.r.x, -frog.r.y, 0);
//    } else {
//        glRotatef(frog.rotationY, 0, 1, 0);
//    }
    draw_trajectory();
    glTranslatef(frog.r.x, frog.r.y, frog.r.z);
    glRotatef(-frog.rotationY, 0, 1, 0);
    glTranslatef(-frog.r.x, -frog.r.y, -frog.r.z);
    
    glTranslatef(frog.r.x, frog.r.y, frog.r.z);
    draw_sphere();
   // draw_cube();
    drawAxes(0.3);
}

void draw_logs(){
    for (int i = 0; i < 5; i++) {
        logs[i].posY = calcSineWave(water_sw, logs[i].posX, 0, t) - scene.riverHeight;
        draw_cylinder(logs[i].radius, logs[i].width, logs[i].posX, logs[i].posY, logs[i].posZ,woodTexture, isWireframe);
    }
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glLoadIdentity();
    
    //make the scene visiable by moving -1 unit in z-axis
    glTranslatef(0, 0, -1);
    //make the pivot at orgin when rotating the camera
    glRotatef(-dy, 1, 0, 0);
    glRotatef(-dx, 0, 1, 0);
    glScalef(scale, scale, scale);
    //shift the whole scene by negative units as the frog moves
    glTranslatef(-frog.r.x, -frog.r.y, -frog.r.z);
   
    draw_plane(1, 0, -3, 2);
    draw_plane(1, 0, -1, 2);
    draw_plane(1, 0, 1, 2);
    draw_plane(1, 0, 3,2 );
    
    draw_plane(3, 0, -3,2);
    draw_plane(3, 0, -1,2);
    draw_plane(3, 0, 1,2);
    draw_plane(3, 0, 3,2);
    
    //river left
   draw_plane(-0.5, 0, 3.5, 1);
   draw_plane(-1.5, 0, 3.5, 1);
   draw_plane(-2.5, 0, 3.5, 1);
    
    //river right
    draw_plane(-0.5, 0, -3.5, 1);
    draw_plane(-1.5, 0, -3.5, 1);
    draw_plane(-2.5, 0, -3.5, 1);
    
    draw_plane(-3.5, 0, -3.5, 1);
    draw_plane(-3.5, 0, -2.5, 1);
    draw_plane(-3.5, 0, -1.5, 1);
    draw_plane(-3.5, 0, -0.5, 1);
    draw_plane(-3.5, 0, 0.5, 1);
    draw_plane(-3.5, 0, 1.5, 1);
    draw_plane(-3.5, 0, 2.5, 1);
    draw_plane(-3.5, 0, 3.5, 1);
    
    
    draw_water(scene.riverSizeX, scene.riverSizeZ, scene.riverPosX, -scene.riverHeight, scene.riverPosZ);
    
    glPushMatrix();
        drawAxes(0.8);
        draw_Frog();
    glPopMatrix();
    
  //  glRotatef(90, 0, 1, 0);
   // glTranslatef(2, -1.0/8.0, 0);
    draw_logs();
  //  glScalef(2, 1, 2);
    glTranslatef(0, 0.5, 0);
    draw_skybox(negXTexture, negYTexture, negZTexture, posXTexture, posYTexture, posZTexture);
  //  glRotatef(90, 1, 0, 0);
  //  drawGrass();
    
    int err;
    while ((err = glGetError()) != GL_NO_ERROR)
        printf("display: %s\n", gluErrorString(err));
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
        case 'a':
            frog.rotationY -= 2;
            std::cout << "degree: " << frog.rotationY << std::endl;
            break;
        case 'd':
            frog.rotationY += 2;
            break;
        case 'w':
            initVel.y += 1;
            break;
        case 's':
            initVel.y -= 1;
            std::cout << "init y : " << initVel.y << std::endl;
            std::cout << "frog.v0.y: " << frog.v0.y << std::endl;
            break;
        case 'q':
            exit(EXIT_SUCCESS);
            break;
        case 'f':
            if (!isWireframe)
                isWireframe = true;
            else
                isWireframe = false;
            //camera.scale += 0.01;
            break;
        case ' ':
            
            if (!jumping) {
                frog.v0.x = initVel.x * cosf(M_PI * initVel.y / 180) * cosf(M_PI * frog.rotationY / 180);
                frog.v0.y = initVel.x * sinf(M_PI * initVel.y / 180);
                frog.v0.z = initVel.x * cosf(M_PI * initVel.y / 180) * sinf(M_PI * frog.rotationY / 180);
                frog.v.x = frog.v0.x;
                frog.v.y = frog.v0.y;
                frog.v.z = frog.v0.z;
            }
            jumping = true;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void SpecialInput(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_UP:
            scale += 0.1;
            break;
        case GLUT_KEY_DOWN:
            scale -= 0.1;
            break;
        case GLUT_KEY_LEFT:
            movingX += 0.1;
            break;
        case GLUT_KEY_RIGHT:
            movingX -= 0.1;
            break;
    }
}

void init(){
    glMatrixMode(GL_PROJECTION);    // Set the matrix we want to manipulate
    glLoadIdentity();            // Overwrite the contents with the identity
    gluPerspective(75, 1, 0.01, 100);        // Multiply the current matrix with a generated perspective matrix
    glMatrixMode(GL_MODELVIEW);
    
    //float radius, width, posX, posY, posZ
    logs[0] = { 0.08, 0.8, -0.3, 0, -0.4 };
    logs[1] = { 0.075, 0.9, -0.7, 0, 0 };
    logs[2] = { 0.07, 1.1, -1.2, 0, -0.2 };
    logs[3] = { 0.1, 1, -1.7, 0, -0.8 };
    logs[4] = { 0.1, 0.9, -2.5, 0, -0.1 };
    
  //  glutSwapBuffers();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode((GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH));
    glutInitWindowSize(600, 600);
    glutCreateWindow("Frogger");
    
    init();
    grassTexture = loadTexture("grass.png");
    woodTexture = loadTexture("wood.png");
    
    negXTexture = loadTexture("negx.png");
    negYTexture = loadTexture("negy.png");
    negZTexture = loadTexture("negz.png");
    posXTexture = loadTexture("posx.png");
    posYTexture = loadTexture("posy.png");
    posZTexture = loadTexture("posz.png");
   // glutReshapeFunc(reshape);
    glutMotionFunc(mouseMotion);
    glutSpecialFunc(SpecialInput);
    //glutMouseFunc(mouseClicked);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutMainLoop();
    
    return EXIT_SUCCESS;
}















































