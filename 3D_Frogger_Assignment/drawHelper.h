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

void draw_cylinder(float radius, float width, float x, float y, float z, GLuint texture, bool wireFramed) {
   // float x, y, z = 0, x1, y1, z1 = 1;
    //float width = 1;
    float theta = 0;
    float nVertices = 10;
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
