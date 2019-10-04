//
//  drawHelper.hpp
//  opengl
//
//  Created by Leapingleo on 23/9/19.
//  Copyright © 2019 Leapingleo. All rights reserved.


#include <stdio.h>

#include <math.h>
#include <GLUT/GLUT.h>
#include <OpenGL/glu.h>
#include "SOIL.h"

static GLuint loadTexture(const char *filename)
{
    GLuint tex = SOIL_load_OGL_texture(filename, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
    if (!tex)
        return 0;
    
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return tex;
}

void draw_cylinder(float radius, float width, float x, float y, float z, GLuint texture, bool wireFramed, float nVertices) {
   // float x, y, z = 0, x1, y1, z1 = 1;
    //float width = 1;
    float theta = 0;
   // float nVertices = 10;
    float segmentSize = 1 / nVertices;
    float thetaSize = 2 * M_PI * segmentSize;
    float count = 0;
    glEnable(GL_DEPTH_TEST);
    // glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    if (wireFramed) {
        glLineWidth(5.0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    glColor3f(1, 1, 1);
    glBegin(GL_POLYGON);
    for (int i = 0; i <= nVertices; i++) {
        theta += thetaSize;
        float x1 = radius * cosf(theta);
        float y1 = radius * sinf(theta);
        glTexCoord2f(0.5 * cosf(theta) + 0.5, 0.5 * sinf(theta) + 0.5);
        glVertex3f(x + x1, y + y1, z);
    }
    glEnd();
    
    for (int i = 0; i <= nVertices; i++) {
        if (count == nVertices)
            count = 0;
        
        glBegin(GL_POLYGON);
        theta += thetaSize;
        float x1 = radius * cosf(theta);
        float y1 = radius * sinf(theta);
        float nextX = radius * cosf(theta + thetaSize);
        float nextY = radius * sinf(theta + thetaSize);

        glTexCoord2f(segmentSize * count, 0);
        glVertex3f(x + x1, y + y1, z);

        glTexCoord2f(segmentSize * count, 1);
        glVertex3f(x + x1, y + y1, z + width);

        glTexCoord2f(segmentSize * count + segmentSize, 1);
        glVertex3f(x + nextX, y + nextY, z + width);

        glTexCoord2f(segmentSize * count, 0);
        glVertex3f(x + x1, y + y1, z);

        glTexCoord2f(segmentSize * count + segmentSize, 0);
        glVertex3f(x + nextX, y + nextY, z);

        glTexCoord2f(segmentSize * count + segmentSize, 1);
        glVertex3f(x + nextX, y + nextY, z + width);
        glEnd();
        
        count++;
    }
   
    glBegin(GL_POLYGON);
    for (int i = 0; i <= nVertices; i++) {
        theta += thetaSize;
        float x1 = radius * cosf(theta);
        float y1 = radius * sinf(theta);
        glTexCoord2f(0.5 * cosf(theta) + 0.5, 0.5 * sinf(theta) + 0.5);
        glVertex3f(x + x1, y + y1, z + width);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glLineWidth(1.0);
}

void draw_skybox(GLuint negX, GLuint negY, GLuint negZ, GLuint posX, GLuint posY, GLuint posZ){
    float length = 6;

  //Back side
  //NEG-Z
  //glColor3f( 0.5, 0.5, 1.0 );
  
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, negZ);
    glBegin(GL_POLYGON);
    glTexCoord2f(0, 0);
    glVertex3f(-length, -length, -length);
    glTexCoord2f(1, 0);
    glVertex3f(length, -length, -length);
    glTexCoord2f(1,1);
    glVertex3f(length, length, -length);
    glTexCoord2f(0, 1);
    glVertex3f(-length, length, -length);// P4 is purple
    glEnd();
    
    // White side - Front
    // POS-Z
    glBindTexture(GL_TEXTURE_2D, posZ);
    glBegin(GL_POLYGON);
    glColor3f(1.0, 1.0, 1.0 );
    glTexCoord2f(0, 0);
    glVertex3f(length, -length, length);
    glTexCoord2f(1, 0);
    glVertex3f(-length,  -length, length);
    glTexCoord2f(1, 1);
    glVertex3f(-length, length, length);
    glTexCoord2f(0, 1);
    glVertex3f(length, length, length);
    glEnd();

    // Purple side - RIGHT
    // POS-X
    glBindTexture(GL_TEXTURE_2D, posX);
    glBegin(GL_POLYGON);
    glColor3f(  1.0,  1.0,  1.0 );
    glTexCoord2f(0, 0);
    glVertex3f( length, -length, -length );
    glTexCoord2f(1, 0);
    glVertex3f( length,  -length, length );
    glTexCoord2f(1, 1);
    glVertex3f( length,  length,  length );
    glTexCoord2f(0, 1);
    glVertex3f( length, length,  -length );
    glEnd();

    // Green side - LEFT
    // NEG-X
    glBindTexture(GL_TEXTURE_2D, negX);
    glBegin(GL_POLYGON);
    glColor3f(   1.0,  1.0,  1.0 );
    glTexCoord2f(0, 0);
    glVertex3f( -length, -length,  length );
    glTexCoord2f(1, 0);
    glVertex3f( -length,  -length,  -length );
    glTexCoord2f(1, 1);
    glVertex3f( -length,  length, -length );
    glTexCoord2f(0, 1);
    glVertex3f( -length, length, length );
    glEnd();

    // Blue side - TOP
    //POS-Y
    glBindTexture(GL_TEXTURE_2D, posY);
    glBegin(GL_POLYGON);
    glColor3f(   1.0,  1.0,  1.0 );
    glTexCoord2f(0, 0);
    glVertex3f(  length,  length,  length );
    glTexCoord2f(1, 0);
    glVertex3f(  -length,  length, length );
    glTexCoord2f(1, 1);
    glVertex3f( -length,  length, -length );
    glTexCoord2f(0, 1);
    glVertex3f( length,  length,  -length );
    glEnd();

    // Red side - BOTTOM
    //NEG-Y
    glBindTexture(GL_TEXTURE_2D, posY);
    glBegin(GL_POLYGON);
    glColor3f(   1.0,  1.0,  1.0 );
    glTexCoord2f(0, 0);
    glVertex3f(  length, -length, length );
    glTexCoord2f(1, 0);
    glVertex3f(  -length, -length,  length );
    glTexCoord2f(1, 1);
    glVertex3f( -length, -length,  -length );
    glTexCoord2f(0, 1);
    glVertex3f( length, -length, -length );
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
}

void draw_cube(float length, float r, float g, float b, bool wireFramed){
    if (wireFramed) {
        //glLineWidth(5.0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    GLfloat qaBlack[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat qaGreen[] = {0.0, 1.0, 0.0, 1.0};
    GLfloat qaWhite[] = {1.0, 1.0, 1.0, 1.0};
   // glMaterialfv(GL_FRONT, GL_AMBIENT, qaWhite);
  //  glMaterialfv(GL_FRONT, GL_DIFFUSE, qaGreen);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, qaWhite);
    glMaterialf(GL_FRONT, GL_SHININESS, 60);
    //xy plane z+
   // glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    glNormal3f(0, 0, 1);
    glVertex3f(-length, -length, length);
    glVertex3f(length, -length, length);
    glVertex3f(length, length, length);
    glVertex3f(-length, length, length);
    glEnd();
    
    //xy plane z-
    glBegin(GL_POLYGON);
    glNormal3f(0, 0, -1);
    glVertex3f(-length, -length, -length);
    glVertex3f(length, -length, -length);
    glVertex3f(length, length, -length);
    glVertex3f(-length, length, -length);
    glEnd();
    
    //xz plane y+
    glBegin(GL_POLYGON);
    glNormal3f(0, 1, 0);
    glVertex3f(-length, length, -length);
    glVertex3f(length, length, -length);
    glVertex3f(length, length, length);
    glVertex3f(-length, length, length);
    glEnd();
    
    //xz plane y-
    glBegin(GL_POLYGON);
    glNormal3f(0, -1, 0);
    glVertex3f(-length, -length, -length);
    glVertex3f(length, -length, -length);
    glVertex3f(length, -length, length);
    glVertex3f(-length, -length, length);
    glEnd();
    
    //yz plane x+
    glBegin(GL_POLYGON);
    glNormal3f(1, 0, 0);
    glVertex3f(length, -length, -length);
    glVertex3f(length, -length, length);
    glVertex3f(length, length, length);
    glVertex3f(length, length, -length);
    glEnd();
    
    //yz plane x-
    glBegin(GL_POLYGON);
    glNormal3f(-1, 0, 0);
    glVertex3f(-length, -length, -length);
    glVertex3f(-length, -length, length);
    glVertex3f(-length, length, length);
    glVertex3f(-length, length, -length);
    glEnd();
}

void draw_car(float x, float y, float z, float rotateDeg, GLuint texture, bool wireframed){
    glPushMatrix();
    
    glTranslatef(x, y, z);
    glRotatef(rotateDeg, 0, 1, 0);
        glPushMatrix();
        glScalef(2, 0.6, 1);
        draw_cube(0.1, 0.66, 0, 0.12, wireframed);
        glPopMatrix();
    
        glPushMatrix();
        glTranslatef(0, 0.1, 0);
        glScalef(1.4, 1, 1);
        draw_cube(0.07, 0.1, 0.1, 0.1, wireframed);
        glPopMatrix();
    
        glPushMatrix();
        glTranslatef(0.1, -0.06, 0.1);
        draw_cylinder(0.05, 0.03, 0, 0, 0, texture, wireframed, 20);
        glPopMatrix();
    
        glPushMatrix();
        glTranslatef(0.1, -0.06, -0.13);
        draw_cylinder(0.05, 0.03, 0, 0, 0, texture, wireframed, 20);
        glPopMatrix();
    
        glPushMatrix();
        glTranslatef(-0.1, -0.06, 0.1);
        draw_cylinder(0.05, 0.03, 0, 0, 0, texture, wireframed, 20);
        glPopMatrix();
    
        glPushMatrix();
        glTranslatef(-0.1, -0.06, -0.13);
        draw_cylinder(0.05, 0.03, 0, 0, 0, texture, wireframed, 20);
        glPopMatrix();
    glPopMatrix();
}

void draw_road(float x, float y, float z, float l, float w, GLuint texture, bool wireFramed){
    glEnable(GL_DEPTH_TEST);
    // glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    if (wireFramed) {
        glLineWidth(1.0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    glColor3f(1, 1, 1);
    glBegin(GL_POLYGON);
    
    glTexCoord2f(0, 0); glVertex3f(x + w * 0.5, y, z - l * 0.5);
    glTexCoord2f(1, 0); glVertex3f(x + w * 0.5, y, z + l * 0.5);
    glTexCoord2f(1, 1); glVertex3f(x - w * 0.5, y, z + l * 0.5);
    glTexCoord2f(0, 1); glVertex3f(x - w * 0.5, y, z - l * 0.5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void draw_frog(bool wireFramed, float upper, float mid, float lower) {
    glPushMatrix();
        //torsor
        glPushMatrix();
    glRotatef(10, 0, 0, 1);
        glScalef(1, 0.5, 0.65);
        draw_cube(0.1, 0, 1, 0, wireFramed);
        glPopMatrix();
        
        //head-part
        glPushMatrix();
     glRotatef(10, 0, 0, 1);
            //face
            glPushMatrix();
                glTranslatef(0.1, 0.005, 0);
                glScalef(0.5, 0.55, 0.7);
                draw_cube(0.08, 0, 1, 0, wireFramed);
            glPopMatrix();
    
            //eyes
            glPushMatrix();
                //left eye
                glPushMatrix();
                glTranslatef(0.13, 0.024, -0.028);
                glScalef(0.2, 0.2, 0.2);
                draw_cube(0.12, 1, 1, 1, wireFramed);
                //left eye ball
                glTranslatef(0.06, 0, 0);
                draw_cube(0.08, 0, 0, 0, wireFramed);
                glPopMatrix();
                
                //right eye
                glPushMatrix();
                glTranslatef(0.13, 0.024, 0.028);
                glScalef(0.2, 0.2, 0.2);
                draw_cube(0.12, 1, 1, 1, wireFramed);
                //right eye ball
                glTranslatef(0.06, 0, 0);
                draw_cube(0.08, 0, 0, 0, wireFramed);
                glPopMatrix();
    
                //upper lips
                glPushMatrix();
                   glTranslatef(0.16, -0.01, 0);
                   glScalef(0.45, 0.1, 0.7);
                   draw_cube(0.08, 0, 1, 0, wireFramed);
                glPopMatrix();
    
                //lower lips
                glPushMatrix();
                   glTranslatef(0.16, -0.028, 0);
                   glScalef(0.45, 0.1, 0.7);
                   draw_cube(0.08, 0, 1, 0, wireFramed);
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();

        //right rear leg
        glPushMatrix();
            //upper leg joint
            glTranslatef(-0.1, 0, 0.08);
            glRotatef(-30, 1, 0, 0);
            glRotatef(-30, 0, 1, 0);
            //UPPER ROTATION
            glRotatef(-30, 0, 0, 1);
            //upper leg
            glPushMatrix();
               // glRotatef(30, 0, 0, 1);
                //render upper leg
                glPushMatrix();
                    glScalef(0.5, 0.13, 0.2);
                    //glRotatef(45, 0, 1, 0);
                    glTranslatef(0.1, 0, 0);
                    draw_cube(0.1, 0, 1, 0, wireFramed);
                glPopMatrix();
                //mid leg joint
                
            glTranslatef(0.1, 0, 0);
                //MID ROTATION
                glRotatef(30, 0, 0, 1);
    
                glPushMatrix();
                    glTranslatef(-0.05, 0, 0);
                    
                    glScalef(0.4, 0.1, 0.2);
                    draw_cube(0.11, 0, 1, 0, wireFramed);
                glPopMatrix();
    
                //lower leg
                glPushMatrix();
                    glTranslatef(-0.1, 0, 0);
                    //LOWER ROTATION
                    glRotatef(-30, 0, 0, 1);
                    glScalef(0.5, 0.13, 0.2);
                    glTranslatef(0.1, 0, 0);
                    draw_cube(0.11, 0, 1, 0, wireFramed);
    
                    //foot
                    glPushMatrix();
                        glTranslatef(0.14, 0, 0);
                        glScalef(0.35, 1.1, 1);
                        draw_cube(0.13, 0, 1, 0, wireFramed);
                        //mid toe
                        glPushMatrix();
                        glTranslatef(0.23, 0, 0);
                        glScalef(0.8, 0.5, 0.2);
                        draw_cube(0.13, 0, 1, 0, wireFramed);
                        glPopMatrix();
                        
                        //left toe
                        glPushMatrix();
                        glTranslatef(0.23, 0, -0.1);
                        glRotatef(20, 0, 1, 0);
                        glScalef(0.8, 0.5, 0.2);
                        draw_cube(0.13, 0, 1, 0, wireFramed);
                        glPopMatrix();
    
                        //right toe
                        glPushMatrix();
                        glTranslatef(0.23, 0, 0.1);
                        glRotatef(-20, 0, 1, 0);
                        glScalef(0.8, 0.5, 0.2);
                        draw_cube(0.13, 0, 1, 0, wireFramed);
                        glPopMatrix();
                    glPopMatrix();
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();
    
        //left rear leg
             glPushMatrix();
                 //upper leg joint
                 glTranslatef(-0.1, 0, -0.08);
                 glRotatef(30, 1, 0, 0);
                 glRotatef(30, 0, 1, 0);
                //upper
                 glRotatef(-30, 0, 0, 1);
                 //upper leg
                 glPushMatrix();
                    // glRotatef(30, 0, 0, 1);
                     //render upper leg
                     glPushMatrix();
                         glScalef(0.5, 0.13, 0.2);
                         //glRotatef(45, 0, 1, 0);
                         glTranslatef(0.1, 0, 0);
                         draw_cube(0.1, 0, 1, 0, wireFramed);
                     glPopMatrix();
                     //mid leg joint
                     
                 glTranslatef(0.1, 0, 0);
                     //mid rotation
                     glRotatef(30, 0, 0, 1);
         
                     glPushMatrix();
                         glTranslatef(-0.05, 0, 0);
                         
                         glScalef(0.4, 0.1, 0.2);
                         draw_cube(0.11, 0, 1, 0, wireFramed);
                     glPopMatrix();
         
                     //lower leg
                     glPushMatrix();
                         glTranslatef(-0.1, 0, 0);
                         //lower rotation
                         glRotatef(-30, 0, 0, 1);
                         glScalef(0.5, 0.13, 0.2);
                         glTranslatef(0.1, 0, 0);
                         draw_cube(0.11, 0, 1, 0, wireFramed);
         
                         //foot
                         glPushMatrix();
                             glTranslatef(0.14, 0, 0);
                             glScalef(0.35, 1.1, 1);
                             draw_cube(0.13, 0, 1, 0, wireFramed);
                             //mid toe
                             glPushMatrix();
                             glTranslatef(0.23, 0, 0);
                             glScalef(0.8, 0.5, 0.2);
                             draw_cube(0.13, 0, 1, 0, wireFramed);
                             glPopMatrix();
                             
                             //left toe
                             glPushMatrix();
                             glTranslatef(0.23, 0, -0.1);
                             glRotatef(20, 0, 1, 0);
                             glScalef(0.8, 0.5, 0.2);
                             draw_cube(0.13, 0, 1, 0, wireFramed);
                             glPopMatrix();
         
                             //right toe
                             glPushMatrix();
                             glTranslatef(0.23, 0, 0.1);
                             glRotatef(-20, 0, 1, 0);
                             glScalef(0.8, 0.5, 0.2);
                             draw_cube(0.13, 0, 1, 0, wireFramed);
                             glPopMatrix();
                         glPopMatrix();
                     glPopMatrix();
                 glPopMatrix();
             glPopMatrix();
    
//        //left rear leg
//        glPushMatrix();
//                //upper leg joint
//                glTranslatef(-0.05, 0, -0.1);
//                glRotatef(30, 1, 0, 0);
//                glRotatef(30, 0, 1, 0);
//                glRotatef(-30, 0, 0, 1);
//                //upper leg
//                glPushMatrix();
//                   // glRotatef(30, 0, 0, 1);
//                    //render upper leg
//                    glPushMatrix();
//                        glScalef(0.5, 0.13, 0.2);
//                        //glRotatef(45, 0, 1, 0);
//                        draw_cube(0.1, 0, 1, 0, wireFramed);
//                    glPopMatrix();
//                    //mid leg joint
//
//                    glRotatef(10, 0, 0, 1);
//                    glPushMatrix();
//                        glTranslatef(0, -0.03, 0);
//
//                        glScalef(0.4, 0.1, 0.2);
//                        draw_cube(0.11, 0, 1, 0, wireFramed);
//                    glPopMatrix();
//
//                    //lower leg
//                    glPushMatrix();
//                        glTranslatef(0, -0.06, 0);
//
//                        glRotatef(-10, 0, 0, 1);
//                        glScalef(0.5, 0.13, 0.2);
//                        draw_cube(0.11, 0, 1, 0, wireFramed);
//
//                        //foot
//                        glPushMatrix();
//                            glTranslatef(0.14, 0, 0);
//                            glScalef(0.35, 1.1, 1);
//                            draw_cube(0.13, 0, 1, 0, wireFramed);
//                            //mid toe
//                            glPushMatrix();
//                            glTranslatef(0.23, 0, 0);
//                            glScalef(0.8, 0.5, 0.2);
//                            draw_cube(0.13, 0, 1, 0, wireFramed);
//                            glPopMatrix();
//
//                            //left toe
//                            glPushMatrix();
//                            glTranslatef(0.23, 0, -0.1);
//                            glRotatef(20, 0, 1, 0);
//                            glScalef(0.8, 0.5, 0.2);
//                            draw_cube(0.13, 0, 1, 0, wireFramed);
//                            glPopMatrix();
//
//                            //right toe
//                            glPushMatrix();
//                            glTranslatef(0.23, 0, 0.1);
//                            glRotatef(-20, 0, 1, 0);
//                            glScalef(0.8, 0.5, 0.2);
//                            draw_cube(0.13, 0, 1, 0, wireFramed);
//                            glPopMatrix();
//                        glPopMatrix();
//                    glPopMatrix();
//                glPopMatrix();
//            glPopMatrix();
//
        //front right leg
        glPushMatrix();
            //joint rotation
            glTranslatef(0.1, -0.05, 0.06);
            glRotatef(-30, 0, 1, 0);
            //UPPER ROTATION
            glRotatef(45, 0, 0, 1);

            glPushMatrix();
            //render upper leg
            glPushMatrix();
                glScalef(0.35, 0.15, 0.17);
                    glTranslatef(-0.05, 0, 0);
                    draw_cube(0.11, 0, 1, 0, wireFramed);
                glPopMatrix();
                //lower rotation
                glTranslatef(-0.05, 0, 0);
                glRotatef(-45, 0, 0, 1);
                   
                glPushMatrix();
                    //render lower leg
                    glScalef(0.35, 0.15, 0.17);
                    glTranslatef(0.08,-0.02, 0);
                    draw_cube(0.11, 0, 1, 0, wireFramed);
                   
                    //foot
                    glPushMatrix();
                        glTranslatef(0.14, 0, 0);
                        glScalef(0.4, 1.1, 1);
                        draw_cube(0.13, 0, 1, 0, wireFramed);
                        //mid toe
                        glPushMatrix();
                        glTranslatef(0.23, 0, 0);
                        glScalef(0.8, 0.5, 0.2);
                        draw_cube(0.13, 0, 1, 0, wireFramed);
                        glPopMatrix();

                        //left toe
                        glPushMatrix();
                        glTranslatef(0.23, 0, -0.1);
                        glRotatef(20, 0, 1, 0);
                        glScalef(0.8, 0.5, 0.2);
                        draw_cube(0.13, 0, 1, 0, wireFramed);
                        glPopMatrix();

                        //right toe
                        glPushMatrix();
                        glTranslatef(0.23, 0, 0.1);
                        glRotatef(-20, 0, 1, 0);
                        glScalef(0.8, 0.5, 0.2);
                        draw_cube(0.13, 0, 1, 0, wireFramed);
                        glPopMatrix();
                    glPopMatrix();
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();
    
        //front left leg
        glPushMatrix();
            //joint rotation
            glTranslatef(0.1, -0.05, -0.06);
            glRotatef(30, 0, 1, 0);
            //UPPER ROTATION
            glRotatef(45, 0, 0, 1);

            glPushMatrix();
            //render upper leg
            glPushMatrix();
                glScalef(0.35, 0.15, 0.17);
                    glTranslatef(-0.05, 0, 0);
                    draw_cube(0.11, 0, 1, 0, wireFramed);
                glPopMatrix();
                //lower rotation
                glTranslatef(-0.05, 0, 0);
                glRotatef(-45, 0, 0, 1);
                   
                glPushMatrix();
                    //render lower leg
                    glScalef(0.35, 0.15, 0.17);
                    glTranslatef(0.08,-0.02, 0);
                    draw_cube(0.11, 0, 1, 0, wireFramed);
                   
                    //foot
                    glPushMatrix();
                        glTranslatef(0.14, 0, 0);
                        glScalef(0.4, 1.1, 1);
                        draw_cube(0.13, 0, 1, 0, wireFramed);
                        //mid toe
                        glPushMatrix();
                        glTranslatef(0.23, 0, 0);
                        glScalef(0.8, 0.5, 0.2);
                        draw_cube(0.13, 0, 1, 0, wireFramed);
                        glPopMatrix();

                        //left toe
                        glPushMatrix();
                        glTranslatef(0.23, 0, -0.1);
                        glRotatef(20, 0, 1, 0);
                        glScalef(0.8, 0.5, 0.2);
                        draw_cube(0.13, 0, 1, 0, wireFramed);
                        glPopMatrix();

                        //right toe
                        glPushMatrix();
                        glTranslatef(0.23, 0, 0.1);
                        glRotatef(-20, 0, 1, 0);
                        glScalef(0.8, 0.5, 0.2);
                        draw_cube(0.13, 0, 1, 0, wireFramed);
                        glPopMatrix();
                    glPopMatrix();
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();
        
}
