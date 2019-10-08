#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include "helper.h"
#include "drawHelper.h"
#define NUM_OF_PARTICLES 500

typedef struct { float lastX, lastY, zoomFactor; bool zoomed; float scale;
} Camera;

typedef struct { float x, y;} Vec2f;
typedef struct { float x, y, z; } Vec3f;
typedef struct { Vec3f r0, v0, v; Vec3f r; float rotationX, rotationY; bool isAlive; } Frog;
typedef struct { float radius, width, posX, posY, posZ, dz; } Log;
typedef struct { float posX, posY, posZ; } Car;
typedef struct { float groundX, groundZ, riverPosX, riverPosZ, riverSizeX, riverSizeZ, riverHeight, roadPosX, roadPosZ, roadLength, roadWidth; } Scene;
typedef struct { float posX, posY, posZ; Vec3f direction; } Particle;

Vec2f initVel = { 0.7, 40 };
Frog frog = {
    { 0.0, 0.0 },
   // { initVel.x * cosf(M_PI * initVel.y / 180) * cosf(deg2rad(frog.rotationY)),
   //   initVel.x * sinf(M_PI * initVel.y / 180), },
    { 0, 0, 0},
    { 0.0, 0.0 },
    //frog x, y, z
    { 3.2, 0.0, 0.0 },
    0, 180, true
};


//frog rotation
float upper = upperRotation.min;
float mid = midRotation.min;
float lower = lowerRotation.min;
float torsorAngle = torsorRotation.min;
float frontUpperAngle = frontUpperRotation.min;
float frontLowerAngle = frontLowerRotation.min;

float landingZ = 0;
bool landOnLog = false;
float dx = 110;
float dy = -30;
float rotateY = 0;
float rotateX = 0;
float movingY = 0, movingZ = -2, movingX = 0;
float scale = 1;
float jumping = false;
float a = 0;
float r = 0.03;

//use for animations
float startTime = 0;
float endTime = 1;
float currTime = 0;
float startValue = 0;
float endValue = -60;
float duration = 4;

float timeOfFlight = 0;

float t = 0, dt = 0;
float global_time = 0, last_t = 0, timer = 0, deadTime = 0;

Sinewave water_sw = { 1.0/60.0, M_PI * 4, 2 * M_PI, 0.25 * M_PI };
Sinewave sw1 = {0.1, M_PI, 2 * M_PI,  M_PI };
bool isWireframe = true;

Log logs[5];
Car cars[8];
Camera camera { false, 0.3 };
Scene scene { 0, 0, -1.5, 0, 3, 6, 0.15, 2, 0, 8, 2 };
Particle particles[NUM_OF_PARTICLES];


static GLuint grassTexture;
static GLuint woodTexture, negXTexture, negYTexture,
              negZTexture, posXTexture, posYTexture, posZTexture,
              sandTexture, roadTexture;

void drawAxes(float l) {
    glLineWidth(4.0);
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
    glLineWidth(1.0);
}

void mouseMotion(int x, int y) {
    //make sure it doesnt rotate wiredly by large amount when last mouse pos
    //are too far from current mouse pos
    if (abs(x - camera.lastX) < 10 && abs(y - camera.lastY) < 10) {
     
        dx += x - camera.lastX;
        dy += y - camera.lastY;
    }
    camera.lastX = x;
    camera.lastY = y;

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
    float pondMinX = scene.riverPosX - scene.riverSizeX * 0.5;
    float pondMaxX = scene.riverPosX + scene.riverSizeX * 0.5;
    float pondMinZ = scene.riverPosZ - scene.riverSizeZ * 0.5;
    float pondMaxZ = scene.riverPosZ + scene.riverSizeZ * 0.5;
    
    if (isWireframe) {
        float n = planeSize * 5;
        float stepSize = planeSize / n;
        for (int i = 0; i < n; i++) {
           for (int j = 0; j < n; j++) {
               //so that the pivot is always at the centre
               float x1 = -planeSize * 0.5 + i * stepSize + x;
               float z1 = -planeSize * 0.5 + j * stepSize + z;
               //skip drawing ponds
               if (x1 > pondMinX && x1 < pondMaxX &&
                   z1 > pondMinZ && z1 < pondMaxZ)
                   //the magic number 0.2 = riverbed height lol
                   draw_rectangle(x1, -scene.riverHeight - 0.2, z1, stepSize);
               else
                   draw_rectangle(x1, 0, z1, stepSize);
           }
        }
    } else {
        float offset = planeSize * 0.5;
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glColor3f(1, 1, 1);

//        glBegin(GL_QUADS);
//
//            glVertex3f(x-offset, 0,z-offset); glTexCoord2f(0, 0);
//
//            glVertex3f(x+offset, 0, z-offset); glTexCoord2f(1, 0);
//            glVertex3f(x+offset, 0, z+offset); glTexCoord2f(1, 1);
//
//            glVertex3f(x-offset, 0, z+offset); glTexCoord2f(0, 1);
//        glEnd();
       // glDisable(GL_TEXTURE_2D);
        
        float x1 = -4;
        float z1 = -4;
        
        for (int i = 0; i < 8; i++) {
            x1 = -4;
            for (int j = 0; j < 8; j++) {
                if (x1 < -3 || x1 >= 0 || z1 < -3 || z1 >= 3) {
                    glBegin(GL_QUADS);
                    glVertex3f(x1, 0, z1); glTexCoord2f(0, 0);
                    glVertex3f(x1, 0, z1 + 1); glTexCoord2f(0, 1);
                    glVertex3f(x1 + 1, 0, z1 + 1); glTexCoord2f(1, 1);
                    glVertex3f(x1 + 1, 0, z1); glTexCoord2f(1, 0);
                    glEnd();
                }
                x1 += 1;
            }
            z1 += 1;
        }
//        glBegin(GL_QUADS);
//
//            glVertex3f(0, 0, 0); glTexCoord2f(0, 0);
//
//            glVertex3f(0, 0, 0.2); glTexCoord2f(0, 0.3);
//            glVertex3f(0.2, 0, 0.2); glTexCoord2f(0.3, 0.3);
//
//            glVertex3f(0.2, 0, 0); glTexCoord2f(0.3, 0);
//        glEnd();
        
        //river bed front
        glBegin(GL_QUADS);
            glVertex3f(pondMinX, 0, pondMaxZ); glTexCoord2f(0, 0);
            glVertex3f(pondMinX, 0, pondMinZ); glTexCoord2f(1, 0);
            glVertex3f(pondMinX, -scene.riverHeight - 0.2, pondMinZ); glTexCoord2f(1, 1);
            glVertex3f(pondMinX, -scene.riverHeight - 0.2, pondMaxZ); glTexCoord2f(0, 1);
        glEnd();
        
        //river bed back
        glBegin(GL_QUADS);
            glVertex3f(pondMaxX, 0, pondMaxZ); glTexCoord2f(0, 0);
            glVertex3f(pondMaxX, 0, pondMinZ); glTexCoord2f(1, 0);
            glVertex3f(pondMaxX, -scene.riverHeight - 0.2, pondMinZ); glTexCoord2f(1, 1);
            glVertex3f(pondMaxX, -scene.riverHeight - 0.2, pondMaxZ); glTexCoord2f(0, 1);
        glEnd();
        
        //river bed right
        glBegin(GL_QUADS);
            glVertex3f(pondMinX, 0, pondMinZ); glTexCoord2f(0, 0);
            glVertex3f(pondMaxX, 0, pondMinZ); glTexCoord2f(1, 0);
            glVertex3f(pondMaxX, -scene.riverHeight - 0.2, pondMinZ); glTexCoord2f(1, 1);
            glVertex3f(pondMinX, -scene.riverHeight - 0.2, pondMinZ); glTexCoord2f(0, 1);
        glEnd();
        
        //river bed left
        glBegin(GL_QUADS);
            glVertex3f(pondMinX, 0, pondMaxZ); glTexCoord2f(0, 0);
            glVertex3f(pondMaxX, 0, pondMaxZ); glTexCoord2f(1, 0);
            glVertex3f(pondMaxX, -scene.riverHeight - 0.2, pondMaxZ); glTexCoord2f(1, 1);
            glVertex3f(pondMinX, -scene.riverHeight - 0.2, pondMaxZ); glTexCoord2f(0, 1);
        glEnd();
        
        
        glBindTexture(GL_TEXTURE_2D, sandTexture);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glColor3f(1, 1, 1);

        glBegin(GL_QUADS);
        glVertex3f(pondMinX, -scene.riverHeight - 0.2, pondMinZ); glTexCoord2f(0, 0);
        glVertex3f(pondMinX, -scene.riverHeight - 0.2, pondMaxZ); glTexCoord2f(1, 0);
        glVertex3f(pondMaxX, -scene.riverHeight - 0.2, pondMaxZ); glTexCoord2f(1, 1);
        glVertex3f(pondMaxX, -scene.riverHeight - 0.2, pondMinZ); glTexCoord2f(0, 1);
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
    glColor4f(0, 1, 1, 0.6);
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

bool collideWithTerrain(float x, float y, float z) {
    return x >= -4 && x <= 4 && z >= -4 && z <= 4 && y <= 0
            && (x < scene.riverPosX - scene.riverSizeX * 0.5 ||
                x > scene.riverPosX + scene.riverSizeX * 0.5 ||
                z < scene.riverPosZ - scene.riverSizeZ * 0.5 ||
                z > scene.riverPosZ + scene.riverSizeZ * 0.5);
}

bool inPondArea(float x, float z){
    return x >= scene.riverPosX - scene.riverSizeX * 0.5 && x <= scene.riverPosX + scene.riverSizeX * 0.5 &&
           z >= scene.riverPosZ - scene.riverSizeZ * 0.5 && z <= scene.riverPosZ + scene.riverSizeZ * 0.5;
}

bool collideWithCar(Car car, float x, float y, float z) {
    float dx = car.posX - x;
    float dy = car.posY - y;
    float dz = car.posZ - z;
    //magic number here, needs better definition
    float radius_sum = 0.2 + r;
    return dx * dx + dy * dy + dz * dz <= radius_sum * radius_sum;
}

void resetVelocity() {
    jumping = false;
    currTime = 0;
    timeOfFlight = 0;
  //  dead = false;
    frog.v.x = 0;
    frog.v.y = 0;
    frog.v.z = 0;
    
  //  frog.r.y = 0;
}

void respawn(){
    if (timer > deadTime + 5) {
        for (int i = 0; i < NUM_OF_PARTICLES; i++) {
              float randX = (rand() % 200 - 100) * 0.004;
              float randY = rand() % 100 * 0.009;
              float randZ = (rand() % 200 - 100) * 0.004;
              particles[i] = {0, 0.05, 0, {randX, randY, randZ}};
        }
        frog.isAlive = true;
        frog.r.x = 3.2;
        frog.r.y = 0;
        frog.r.z = 0;
        movingX = -dy;
    }
    currTime = 0;
    timeOfFlight = 0;
    upper = upperRotation.min;
    mid = midRotation.min;
    lower = lowerRotation.min;
    torsorAngle = torsorRotation.min;
    frontUpperAngle = frontUpperRotation.min;
    frontLowerAngle = frontLowerRotation.min;
    
}

void moveCars() {
    for (int i = 0; i < 8; i++) {
        if (i < 4) {
            cars[i].posZ -= 0.05;
            
            if (cars[i].posZ < -3.7)
                cars[i].posZ = 3.7;
        } else {
            cars[i].posZ += 0.05;
            
            if (cars[i].posZ > 3.7)
                cars[i].posZ = -3.7;
        }
    }
}

void draw_particle() {
    
    glPointSize(20.0);
    
   
    for (int i = 0; i < NUM_OF_PARTICLES; i++) {
        float x = rand() % 100;
           if (x > 20)
               glColor3f(1, 0, 0);
           else
               glColor3f(0, 1, 0);
        glBegin(GL_POINTS);
        glVertex3f(frog.r.x + particles[i].posX, particles[i].posY, frog.r.z + particles[i].posZ);
       // glVertex3f(1, 1, 1);
        glEnd();
    }
    glPointSize(1.0);
}

void update_particle_position(){
    
    for (int i = 0; i < NUM_OF_PARTICLES; i++) {
       
        if (particles[i].posY >= 0.05) {
            particles[i].posX += particles[i].direction.x * 0.05;
            particles[i].posY += particles[i].direction.y * 0.05;
            particles[i].posZ += particles[i].direction.z * 0.05;
            
            particles[i].direction.y += -0.5 * 0.05;
        }
    }
}

void update_frog_position(){
    frog.r.x += frog.v.x * dt * 1.5;
    frog.r.y += frog.v.y * dt * 1.5;
    frog.r.z += frog.v.z * dt * 1.5;
    // Velocity
    frog.v.y += gravity * dt * 1.5;
}

void frames(float t){
    upperRotation = updateFrames(upperRotation, t, false);
    midRotation = updateFrames(midRotation, t, false);
    lowerRotation = updateFrames(lowerRotation, t, false);
    torsorRotation = updateFrames(torsorRotation, t, false);
    frontUpperRotation = updateFrames(frontUpperRotation, t, true);
    frontLowerRotation = updateFrames(frontLowerRotation, t, false);
}

void idle(void) {
    if (!frog.isAlive) {
        movingY += 1;
        if (movingX <= 60) {
            std::cout << "moving x: " << movingX << std::endl;
            movingX += 0.618;
        }
    }
    global_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    dt = global_time - last_t;
    timer += dt;
    last_t = global_time;
    
    frames(timeOfFlight);
    
    if (currTime < timeOfFlight) {
        upper = interpolator(upperRotation, currTime);
        mid = interpolator(midRotation, currTime);
        lower = interpolator(lowerRotation, currTime);
        torsorAngle = interpolator(torsorRotation, currTime);
        frontUpperAngle = interpolator(frontUpperRotation, currTime);
        frontLowerAngle = interpolator(frontLowerRotation, currTime);
        currTime += dt;
        if (currTime > timeOfFlight){
            return;
         // currTime = 0;
        }
    }
   // std::cout << "body angle " << torsorAngle << std::endl;

    
    if (!frog.isAlive) {
        respawn();
        update_particle_position();
    } else {
        update_frog_position();
    }
    moveCars();
    t += 0.1;
    
    
    if (collideWithTerrain(frog.r.x, frog.r.y, frog.r.z)) {
        frog.r.y = 0;
        resetVelocity();
    }
    if (inPondArea(frog.r.x, frog.r.z)) {
        //if frog collide with each log
        for (int i = 0; i < 5; i++) {
            if (collideWithLog(logs[i], frog.r.x, frog.r.y, frog.r.z, true)) {
                if (!landOnLog) {
                    landingZ = frog.r.z;
                    landOnLog = true;
                }
                
                frog.r.x = logs[i].posX;
                frog.r.y = logs[i].posY + logs[i].radius;
                frog.r.z = calcSineWave(sw1, frog.r.x, 0, t * 0.2) + landingZ;
                resetVelocity();
              //  reset();
            }
        }
    }
    for (int i = 0; i < 8; i++) {
        if (collideWithCar(cars[i], frog.r.x, frog.r.y, frog.r.z)) {
            if (frog.isAlive) {
                deadTime = global_time;
            }
            frog.isAlive = false;
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
        v.y += gravity * dt;
        
        
        //ground
        if (collideWithTerrain(x, y, z)){
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
        
        glVertex3f(x, y, z);
        count++;
    }
    glEnd();
    glLineWidth(1.0);
    
}



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
   // glRotatef(-180, 0, 1, 0);
    glScalef(0.3, 0.3, 0.3);
    draw_frog(isWireframe, upper, mid, lower, frontUpperAngle, frontLowerAngle, torsorAngle);
   // draw_sphere();
   // draw_cube();
    drawAxes(0.3);
}

void draw_logs(){
    for (int i = 0; i < 5; i++) {
        logs[i].posY = calcSineWave(water_sw, logs[i].posX, 0, t) - scene.riverHeight;
        
        float z = calcSineWave(sw1, logs[i].posX, 0, t * 0.2) + logs[i].posZ;
    //    std::cout << "log 1 z: " << logs[0].posZ << std::endl;
        draw_cylinder(logs[i].radius, logs[i].width, logs[i].posX, logs[i].posY, z ,woodTexture, isWireframe, 10);

    }
}

void draw_cars(){
   // glScalef(1.1, 1.1, 1.1);
    for (int i = 0; i < 8; i++) {
        draw_car(cars[i].posX, 0.12, cars[i].posZ, -90, woodTexture, isWireframe);
 //   draw_car(cars[1].posX, 0.12, cars[1].posZ, -90, woodTexture);
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
    if (!frog.isAlive){
        glRotatef(movingX, 1, 0, 0);
        glRotatef(-movingY, 0, 1, 0);
        glScalef(0.7, 0.7, 0.7);
    } else {
        glRotatef(-dy, 1, 0, 0);
        glRotatef(-dx, 0, 1, 0);
        glScalef(scale, scale, scale);
    }
    
    //shift the whole scene by negative units as the frog moves
    glTranslatef(-frog.r.x, -frog.r.y, -frog.r.z);
    GLfloat qaLightPosition[] = {-6, 6, 6, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, qaLightPosition);
    GLfloat qaLightPosition1[] = {-6, 6, -6, 1.0};
    glLightfv(GL_LIGHT1, GL_POSITION, qaLightPosition1);
   
    if (!frog.isAlive) {
        draw_particle();
    }
    
    draw_road(scene.roadPosX, 0.005, scene.roadPosZ, scene.roadLength, scene.roadWidth, roadTexture, isWireframe);
    draw_plane(0, 0, 0, 8);
    draw_cars();
    draw_water(scene.riverSizeX, scene.riverSizeZ, scene.riverPosX, -scene.riverHeight, scene.riverPosZ);
    
    if (frog.isAlive) {
        glPushMatrix();
            drawAxes(0.8);
            glTranslatef(0, 0.03, 0);
            draw_Frog();
        glPopMatrix();
    }
    
    draw_logs();
    glTranslatef(0, 0.5, 0);
    draw_skybox(negXTexture, negYTexture, negZTexture, posXTexture, posYTexture, posZTexture);

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
            break;
        case 'd':
            frog.rotationY += 2;
            break;
        case 'w':
            initVel.y += 1;
            break;
        case 's':
            initVel.y -= 1;
            break;
        case 'q':
            exit(EXIT_SUCCESS);
            break;
        case 'f':
            if (!isWireframe)
                isWireframe = true;
            else
                isWireframe = false;
            break;
        case ' ':
            timeOfFlight = getTimeOfFlight(initVel.x * sinf(deg2rad(initVel.y))) * 0.66;
            std::cout << "flight time " << timeOfFlight << std::endl;
            if (frog.isAlive && !jumping) {
                frog.v0.x = initVel.x * cosf(M_PI * initVel.y / 180) * cosf(M_PI * frog.rotationY / 180);
                frog.v0.y = initVel.x * sinf(M_PI * initVel.y / 180);
                frog.v0.z = initVel.x * cosf(M_PI * initVel.y / 180) * sinf(M_PI * frog.rotationY / 180);
                frog.v.x = frog.v0.x;
                frog.v.y = frog.v0.y;
                frog.v.z = frog.v0.z;
            }
            jumping = true;
            landOnLog = false;
            break;
        case 'u':
            upper += 2;
            std::cout << "upper " << upper << std::endl;
            break;
        case 'j':
            upper -= 2;
            std::cout << "upper " << upper << std::endl;
            break;
        case 'i':
            mid += 2;
            std::cout << "mid " << mid << std::endl;
            break;
        case 'k':
            mid -= 2;
            std::cout << "mid " << mid << std::endl;
            break;
        case 'o':
            lower += 2;
            std::cout << "lower " << lower << std::endl;
            break;
        case 'l':
            lower -= 2;
            std::cout << "lower " << lower << std::endl;
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
            movingX += 1;
            break;
        case GLUT_KEY_RIGHT:
            movingX -= 1;
            break;
    }
}

void init(){
    glMatrixMode(GL_PROJECTION);    // Set the matrix we want to manipulate
    glLoadIdentity();            // Overwrite the contents with the identity
    gluPerspective(75, 1, 0.01, 100);        // Multiply the current matrix with a generated perspective matrix
    glMatrixMode(GL_MODELVIEW);
 
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
   // glEnable(GL_LIGHT1);
    
    GLfloat qaAmbientLight[] = {0.5, 0.5, 0.5, 1.0};
    GLfloat qaDiffuseLight[] = {0.8, 0.8, 0.8, 1.0};
    GLfloat qaSpecularLight[] = {1.0, 1.0, 1.0, 1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, qaAmbientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, qaDiffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, qaSpecularLight);
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, qaAmbientLight);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, qaDiffuseLight);
    glLightfv(GL_LIGHT1, GL_SPECULAR, qaSpecularLight);
    
  
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    for (int i = 0; i < NUM_OF_PARTICLES; i++) {
        float randX = (rand() % 100 - 50) * 0.003;
        float randY = rand() % 100 * 0.01;
        float randZ = (rand() % 100 - 50) * 0.003;
        std::cout << randY << std::endl;
        particles[i] = {0, 0.05, 0, {randX, randY, randZ}};
    }
    
    //float radius, width, posX, posY, posZ, dz
    logs[0] = { 0.08, 0.8, -0.3, 0, -0.4, 0.1 };
    logs[1] = { 0.075, 0.9, -0.7, 0, 0, 0.1 };
    logs[2] = { 0.07, 1.1, -1.2, 0, -0.2, 0.1 };
    logs[3] = { 0.1, 1, -1.7, 0, -0.8, 0.1 };
    logs[4] = { 0.1, 0.9, -2.3, 0, -0.1, 0.1 };
    
    cars[0] = { 1.25, 0.01, 3.5};
    cars[1] = { 1.25, 0.01, 2.2};
    cars[2] = { 1.75, 0.01, 1.7};
    cars[3] = { 1.75, 0.01, -0.6};
    
    cars[4] = { 2.25, 0.01, -3.4};
    cars[5] = { 2.25, 0.01, -1.9};
    cars[6] = { 2.75, 0.01, 0.5};
    cars[7] = { 2.75, 0.01, 1.3};
    
 //   frames();
//    cars[3] = {}
//    cars[4] = {}
    
  //  glutSwapBuffers();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode((GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH));
    glutInitWindowSize(600, 600);
    glutCreateWindow("OpenGL - Frogger");
    
    init();
    grassTexture = loadTexture("grass.png");
    woodTexture = loadTexture("wood.png");
    roadTexture = loadTexture("road.png");
    
    negXTexture = loadTexture("negx.png");
    negYTexture = loadTexture("negy.png");
    negZTexture = loadTexture("negz.png");
    posXTexture = loadTexture("posx.png");
    posYTexture = loadTexture("posy.png");
    posZTexture = loadTexture("posz.png");
    sandTexture = loadTexture("sand.png");
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























