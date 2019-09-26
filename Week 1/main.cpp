#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <GLUT/GLUT.h>
#include <OpenGL/glu.h>

float x = -1.0, y = 0.0;
float v = 1;

void draw_polygon(float x, float y) {
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y);
    glVertex2f(x + 0.5, y - 0.5);
    glVertex2f(x - 0.5, y - 0.5);
   
    glEnd();
}

void idle() {
    float t, dt;
    static float tLast = 0.0;

    /* Get elapsed time and convert to s */
    t = glutGet(GLUT_ELAPSED_TIME);
    std::cout << t << std::endl;
    t /= 1000;
    /* calculate delta t */
    dt = t - tLast;

    /* Update velocity and position */
    x += v * dt;
    if (x > 1.0)
        x = -1.0;

    tLast = t;
   
    /* ask glut to schedule call to display function */
    glutPostRedisplay();
}

void display() {
     std::cout << x << std::endl;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    
    //x += 0.1;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw_polygon(x, y);
   // glutSwapBuffers();
  
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
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
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


