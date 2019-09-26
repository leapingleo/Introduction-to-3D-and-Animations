#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <GLUT/GLUT.h>
#include <OpenGL/glu.h>

typedef struct {
    int t, lastT, dt;
} Global;

Global g;
float dx = 0;


void drawAxes(float length) {
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(length, 0.0, 0.0);
    glEnd();
    
    glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, length, 0);
    glEnd();
    
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, length);
    glEnd();
}

void draw_triangle(float x, float y) {
    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 1.0, 0.0);
    glVertex2f(x, y);
    glVertex2f(x + 0.2, y - 0.2);
    glVertex2f(x - 0.2, y - 0.2);
    
    glEnd();
}

void drawLinearLine(float m, float c, float leftDomain, float rightDomain) {
    glBegin(GL_LINES);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(leftDomain, m * leftDomain + c, 0);
    glVertex3f(rightDomain, m * rightDomain + c, 0);
    glEnd();
}

float calculateSin(float A, float x) {
    float waveLen = 2;
    return A * sin(2 * M_PI / waveLen * x + M_PI / 4 * dx);
}

float calculateSinDx(float A, float x) {
    float waveLen = 2;
    float k = 2 * M_PI / waveLen;
    return cos(k * x + M_PI / 4 * dx) * k;
}

float magnitude(float x, float y) {
    return sqrt(x * x + y * y);
}

//work out what tangent vector is and draw it
//tangent vector takes the average of two vectors, prev to curr and curr to next
void drawVector(float x, float y, float a, float b, float s, bool normalize, float red, float green, float blue) {
    float mag = magnitude(a, b);
    
    glBegin(GL_LINES);
    glColor3f(red, green, blue);
    glVertex2f(x, y);
    glVertex2f(x + a / magnitude(a, b) * 0.1, y + b /magnitude(a, b) * 0.1);
    glEnd();
}


void idle() {
    g = { glutGet(GLUT_ELAPSED_TIME)/500 };
    dx += 0.1;
    if (dx > 5.0)
        dx = -5.0;
    /* ask glut to schedule call to display function */
    glutPostRedisplay();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    /* drawing code here */
    
    int n = 80;
    float left = -5.0;
    float right = 5.0;
    float range = right - left;
    float stepSize = range / (float)n;
    float x, y;
    glBegin(GL_LINE_STRIP);
    glColor3f(1.0, 1.0, 1.0);
    
    //draws a sine function
    for (int i = 0; i <= n; i++){
        x = i * stepSize + left;
        
        y = calculateSin(1.0, x);
        glVertex3f(x, y, 0);
    }
    glEnd();
    
    //draws tangent and normal
    for (int i = 0; i <= n; i++) {
        x = i * stepSize + left;
        y = calculateSin(1.0, x);
        
        float gradient = calculateSinDx(1.0, x);
        drawVector(x, y, 1, gradient, 0.05, false, 1.0, 0.0, 0.0);
        drawVector(x, y, -gradient, 1, 0.05, false, 0.0, 1.0, 0.0);
    }
    
    draw_triangle(-dx, calculateSin(1.0, -dx));
    drawAxes(10.0);
//    drawLinearLine(1, 0, -1, 1);
//    drawLinearLine(2, 1, -1, 1);
//    drawLinearLine(-0.5, 0, -1, 1);
    
    int err;
    while ((err = glGetError()) != GL_NO_ERROR)
        printf("display: %s\n", gluErrorString(err));
    
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
        case 'q':
            exit(EXIT_SUCCESS);
            break;
            
        default:
            break;
    }
}

void init(){
    glMatrixMode(GL_PROJECTION);
    glOrtho(-5.0, 5.0, -5.0, 5.0, -5.0, 5.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode((GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH));
    glutCreateWindow("Tutorial 1");
    
    init();
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutMainLoop();
    
    return EXIT_SUCCESS;
}


