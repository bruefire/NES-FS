//
//  main.cpp
//  s3som
//
//  Copyright © 2019年 bf. All rights reserved.
//

#include <glew.h>
#include <iostream>
#include "engine3dOSX.h"
using namespace std;

void disp(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POLYGON);
    glColor3d(1.0, 0.0, 0.0);
    glVertex2d(-0.9, -0.9);
    glColor3d(0.0, 1.0, 0.0);
    glVertex2d(0.9, -0.9);
    glColor3d(0.0, 0.0, 1.0);
    glVertex2d(0.9, 0.9);
    glColor3d(1.0, 1.0, 0.0);
    glVertex2d(-0.9, 0.9);
    glEnd();
    glFlush();
    
}

int main(int argc, const char * argv[])
{

    
    //
    
    engine3dOSX s3sim;
    if(s3sim.init())
    {
        s3sim.update();
    }
    s3sim.dispose();
    
    
    

    return 0;
}
