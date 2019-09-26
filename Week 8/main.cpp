#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <GLUT/GLUT.h>
#include <OpenGL/glu.h>

typedef struct {
    float lastX, lastY, zoomFactor;
    bool zoomed;
    float scale;
} Camera;

typedef struct {
    float a;
    float kx;
    float kz;
    float w;
    
} Sinewave;

float dx = 45;
float dy = -45;
float rotateY = 0;
float rotateX = 0;
float movingX = 0;
float scale = 1;
float a = 0;
float r = 0.3;
float t = 0;
bool isWireframe = false;
Sinewave sw = { 0.5, M_PI, 2 * M_PI, 0.25 * M_PI };
Sinewave sw1 = {0.25, M_PI, 2 * M_PI,  M_PI };

Camera camera { false, 0.3 };


float magnitude(float x, float y, float z) {
    return sqrt(x * x + y * y + z * z);
}

//work out what tangent vector is and draw it
//tangent vector takes the average of two vectors, prev to curr and curr to next
void drawVector(float x, float y, float z, float a, float b, float c, float s, bool normalize, float red, float green, float blue) {
    
    glBegin(GL_LINES);
    glColor3f(red, green, blue);
    glVertex3f(x, y, z);
    glVertex3f(x + a / magnitude(a, b, c) * 0.1, y + b /magnitude(a, b, c) * 0.1, z + c / magnitude(a, b, c));
    glEnd();
}

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
}

void reshape(int width, int height) {
    std::cout << width << ", " << height << std::endl;
}

void mouseClicked(int button, int state, int x, int y) {
    
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            std::cout << "x " << x << ", y " << std::endl;
            camera.lastX = x;
            camera.lastY = y;
            camera.zoomed = true;
        } else {
            camera.zoomed = false;
        }
        
        std::cout << dx << std::endl;
    }
}

void draw_polygon(float x, float y) {
    glBegin(GL_TRIANGLES);
    glVertex3f(x, y, 0);
    glVertex3f(x + 0.5, y - 0.5, 0);
    glVertex3f(x - 0.5, y - 0.5, 0);
    
    glEnd();
}

void draw_teapot() {
    glPushMatrix();
    glScalef(0.3, 0.3, 0.3);
    
  //  glRotatef(45, 0, 1, 0);
    glTranslatef(1, 0, 0);
    glRotatef(dx, 0, 1, 0);
    glRotatef(dy, 1, 0, 0);
    drawAxes(1);
    glColor3f(1, 1, 1);
    glutWireTeapot(0.5);
    
    glPopMatrix();
}

void draw_sphere() {
    float x, y, z;
    float theta, phi;
    glColor3f(0, 1, 1);
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

float calculateSin(float A, float x, float t) {
    float waveLen = 2;
    return A * sin(2 * M_PI / waveLen * x + M_PI / 4 * t);
}

float calcSineWave(Sinewave sw, float x, float z, float t) {
    return sw.a * sinf(sw.kx * x + sw.kz * z + sw.w * t);
}

float fourierSinewave(Sinewave sw, float x, float z, int n, float t) {
    float result = 0;
    for (int i = 0; i < n; i++) {
        if (i % 2 == 1) {
            result += sinf(sw.kx * i * x + sw.kz * i * z+ sw.w * i * t * 2) / i;
        }
    }
    return result;
}

void draw_rectangle(float x, float y, float z){
    glColor3f(1, 1, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_POLYGON);
    glVertex3f(x, y, z);
    glVertex3f(x, y, z + 0.1);
    glVertex3f(x + 0.1, y, z + 0.1);
    glVertex3f(x, y, z);
    glVertex3f(x + 0.1, y, z);
    glVertex3f(x + 0.1, y, z + 0.1);
    glEnd();
    
//    drawVector(x, y, z, 0, 0.5, 0, 0.05, false, 1.0, 1.0, 0.0);
//    drawVector(x, y, z + 0.1, 0, 0.5, 0, 0.05, false, 1.0, 1.0, 0.0);
//    drawVector(x + 0.1, y, z, 0, 0.5, 0, 0.05, false, 1.0, 1.0, 0.0);
//    drawVector(x + 0.1, y, z + 0.1, 0, 0.5, 0, 0.05, false, 1.0, 1.0, 0.0);
}

void draw_plane(float x, float y, float z) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            float x = -0.5 + i * 0.1;
            float z = -0.5 + j * 0.1;
            draw_rectangle(x, y, z);
        }
    }
}

void draw_water(){
    float n = 100;
    float xStep = 10.0 / n;
    float zStep = 5.0 / n;
    float x, y, z;
    
    glColor3f(0, 1, 1);
    if (isWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    for (int j = 0; j < n; j++) {
        glBegin(GL_TRIANGLE_STRIP);
        z = -2.5 + j * zStep;
        
        for (int i = 0; i <= n; i++) {
            x = -5.0 + i * xStep;
          //y = fourierSinewave(sw, x, 0, 200, t);
            y = calcSineWave(sw, x, 0, t);
          //  y = calcSineWave(sw, x, z, t) + calcSineWave(sw1, x, z, 0);
            glVertex3f(x, y, z);
            glVertex3f(x, y, z + zStep);
        }
        glEnd();
    }
//    glBegin(GL_TRIANGLE_STRIP);
//    glVertex3f(0, 0, 0);
//    glVertex3f(0, 0, 0.25);
//
//    glVertex3f(0.25, 0, 0);
//    glVertex3d(0.25, 0, 0.25);
//
//    glEnd();
}

void idle() {
    t += 0.1;
    glutPostRedisplay();
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0, 0, -1);
    glScalef(scale, scale, scale);
    glRotatef(-dy, 1, 0, 0);
    glRotatef(-dx, 0, 1, 0);
    draw_water();
    glPushMatrix();
        drawAxes(0.8);
      //  draw_plane(0, 0, 0);
    
        glTranslatef(movingX, 0, 0);
    
        glScalef(scale, scale, scale);
      //  draw_sphere();
    
    
        glColor3f(1, 1, 1);
    
    glPopMatrix();
    
   // glLoadIdentity();
  //  draw_plane(0, 0.2, 0);
    
    

    int err;
    while ((err = glGetError()) != GL_NO_ERROR)
        printf("display: %s\n", gluErrorString(err));
    
    glutSwapBuffers();
}


void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
        case 'a':
            rotateY -= 2;
            movingX -= 0.1;
          //  z -= 0.1;
         //   std::cout << "z: " << z << std::endl;
            break;
        case 'd':
            rotateY += 2;
            movingX += 0.1;
           // z += 0.1;
         //   std::cout << "z: " << z << std::endl;
            break;
        case 'w':
            rotateX += 2;
            scale += 0.05;
            break;
        case 's':
            rotateX -= 2;
            scale -= 0.05;
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
        
        default:
            break;
    }
    glutPostRedisplay();
}

void init(){
    glMatrixMode(GL_PROJECTION);    // Set the matrix we want to manipulate
    glLoadIdentity();            // Overwrite the contents with the identity
    gluPerspective(75, 1, 0.01, 100);        // Multiply the current matrix with a generated perspective matrix
    glMatrixMode(GL_MODELVIEW);
    glutSwapBuffers();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode((GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH));
    glutInitWindowSize(600, 600);
    glutCreateWindow("Tutorial 1");

    init();
    glutReshapeFunc(reshape);
    glutMotionFunc(mouseMotion);
    //glutMouseFunc(mouseClicked);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutMainLoop();
    
    return EXIT_SUCCESS;
}


