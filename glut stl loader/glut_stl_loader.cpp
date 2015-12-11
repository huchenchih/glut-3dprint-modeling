// SphereWorld.cpp
// OpenGL SuperBible
// Demonstrates an immersive 3D environment using actors
// and a camera. This version adds lights and material properties
// and shadows.
// Program by Richard S. Wright Jr.

#include "../shared/gltools.h"	// OpenGL toolkit
#include "../shared/math3d.h"
#include "../shared/glframe.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#define NUM_SPHERES      30
using namespace std;
GLFrame    spheres[NUM_SPHERES];
GLFrame    frameCamera;

// Light and material Data
GLfloat fLightPos[4]   = { -100.0f, 100.0f, 50.0f, 1.0f };  // Point source
GLfloat fNoLight[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat fLowLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat fBrightLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

M3DMatrix44f mShadowMatrix;
unsigned long nTriLong;
int fn=0;
struct point
{
    float px;
    float py;
    float pz;
};
struct vec
{
    point p1;
    point p2;
    point p3;
};

vec p[500000];


void read_binary_stl(string fname)
{
    cout <<"This is binary stl file !" << endl;
    //!!
    //don't forget ios::binary
    //!!
    ifstream myFile (fname.c_str(), ios::in | ios::binary);

    char header_info[80] = "";
    char nTri[4];


    //read 80 byte header
    if (myFile)
    {
        myFile.read (header_info, 80);
        cout <<"header: " << header_info << endl;
    }
    else
    {
        cout << "error" << endl;
    }

    //read 4-byte ulong
    if (myFile)
    {
        myFile.read (nTri, 4);
        nTriLong = *((unsigned long*)nTri) ;
        cout <<"n Tri: " << nTriLong << endl;
    }
    else
    {
        cout << "error" << endl;
    }

    //now read in all the triangles
    for(int i = 0; i < int(nTriLong); i++)
    {

        char facet[50];

        if (myFile)
        {

            //read one 50-byte triangle
            myFile.read (facet, 50);
            char fx1[4] = {facet[12],facet[13],facet[14],facet[15]};

            char fy1[4] = {facet[16],facet[17],facet[18],facet[19]};

            char fz1[4] = {facet[20],facet[21],facet[22],facet[23]};

            char fx2[4] = {facet[24],facet[25],facet[26],facet[27]};

            char fy2[4] = {facet[28],facet[29],facet[30],facet[31]};

            char fz2[4] = {facet[32],facet[33],facet[34],facet[35]};

            char fx3[4] = {facet[36],facet[37],facet[38],facet[39]};

            char fy3[4] = {facet[40],facet[41],facet[42],facet[43]};

            char fz3[4] = {facet[44],facet[45],facet[46],facet[47]};

            p[i].p1.px = *((float*) fx1 );
            p[i].p1.py = *((float*) fy1 );
            p[i].p1.pz = *((float*) fz1 );

            p[i].p2.px = *((float*) fx2 );
            p[i].p2.py = *((float*) fy2 );
            p[i].p2.pz = *((float*) fz2 );

            p[i].p3.px = *((float*) fx3 );
            p[i].p3.py = *((float*) fy3 );
            p[i].p3.pz = *((float*) fz3 );

        }
    }
    /*for(int i = 0; i < int(nTriLong); i++){
        cout << "facet" << i << "   p1:   " << p[i].p1.px << "," << p[i].p1.py << "," << p[i].p1.pz ;
        cout << "   p2:   " << p[i].p2.px << "," << p[i].p2.py << "," << p[i].p2.pz ;
        cout << "   p3:   " << p[i].p3.px << "," << p[i].p3.py << "," << p[i].p3.pz ;
        cout << endl;
    }*/

}

void read_ascii_stl(string fname)
{
    string line;
    cout <<"This is ASCII stl file !" << endl;
    //!!
    //don't forget ios::binary
    //!!
    ifstream myFile (fname.c_str(), ios::in | ios::binary);

    string num;
    int vn=0,vv=0;
    while (!myFile.eof())
    {
        getline (myFile,line);
        cout << line << endl;

        int index=0,cnt=0;
        while(line.c_str()[index]!='\0')
        {
            if(line.c_str()[index]== ' ')cnt++;
            index++;
            if(line.c_str()[index]!= ' ')break;
        }
        //header
        if(line.c_str()[cnt]=='s')cout << line << endl;

        //count facet
        if(line.c_str()[cnt]=='f')fn++;
        //vertex
        if(line.c_str()[cnt]=='v')
        {
            int head=0,tail=0;
            for(int i=0; i<int(line.size()); i++)
            {
                if(line.c_str()[i]=='v')i+=6;
                else if(line.c_str()[i]!=' ')
                {
                    head = i;
                    while(line.c_str()[i]!=' '&& line.c_str()[i]!='\0')i++;
                    tail = i;
                    // Vx,Vy,Vz
                    num.assign(line,head,tail-head);
                    float f = ::strtof(num.c_str(), 0);
                    if(vv==0)
                    {
                        if(vn==0)
                        {
                            p[fn].p1.px = f;
                            vn++;
                        }
                        else if(vn==1)
                        {
                            p[fn].p1.py = f;
                            vn++;
                        }
                        else if(vn==2)
                        {
                            p[fn].p1.pz = f;
                            vn=0;
                            vv++;
                            cout << p[fn].p1.px << " , " << p[fn].p1.py << " , " << p[fn].p1.pz << endl;

                        }
                    }
                    else if(vv==1)
                    {
                        if(vn==0)
                        {
                            p[fn].p2.px = f;
                            vn++;
                        }
                        else if(vn==1)
                        {
                            p[fn].p2.py = f;
                            vn++;
                        }
                        else if(vn==2)
                        {
                            p[fn].p2.pz = f;
                            vn=0;
                            vv++;
                            cout << p[fn].p2.px << " , " << p[fn].p2.py << " , " << p[fn].p2.pz << endl;
                        }
                    }
                    else if(vv==2)
                    {
                        if(vn==0)
                        {
                            p[fn].p3.px = f;
                            vn++;
                        }
                        else if(vn==1)
                        {
                            p[fn].p3.py = f;
                            vn++;
                        }
                        else if(vn==2)
                        {
                            p[fn].p3.pz = f;
                            vn=0;
                            vv=0;
                            cout << p[fn].p3.px << " , " << p[fn].p3.py << " , " << p[fn].p3.pz << endl;
                            cout << fn << endl;
                        }
                    }

                }
            }

        }
    }
    cout << "facet: " << fn << endl;


}

void DrawModel()
{
    /*for(int i = 0; i < fn; i++)
        {

           cout << p[i].p1.px << " , " << p[i].p1.py << " , " <<  p[i].p1.pz << endl;
           cout << p[i].p2.px << " , " << p[i].p2.py << " , " <<  p[i].p2.pz << endl;
           cout << p[i].p3.px << " , " << p[i].p3.py << " , " <<  p[i].p3.pz << endl;

        }*/
    if(!fn)
    {
        glPushMatrix();
        glTranslatef(0.0f, -0.5f, 0.0f);
        glRotatef(-90, 1.0f, 0.0f, 0.0f);
        glScaled (0.01, 0.01, 0.01);
        for(int i = 0; i < int(nTriLong); i++)
        {
            glBegin(GL_TRIANGLES);

            glVertex3f(p[i].p1.px, p[i].p1.py, p[i].p1.pz);
            glVertex3f(p[i].p2.px, p[i].p2.py, p[i].p2.pz);
            glVertex3f(p[i].p3.px, p[i].p3.py, p[i].p3.pz);

            glEnd();
        }
        glPopMatrix();
    }
    else
    {

        glTranslatef(0.0f, -0.5f, 0.0f);
        glRotatef(-90, 1.0f, 0.0f, 0.0f);
        //glScaled (0.01, 0.01, 0.01);
        for(int i = 1; i < fn+1; i++)
        {
            glBegin(GL_TRIANGLES);

            glVertex3f(p[i].p1.px, p[i].p1.py, p[i].p1.pz);
            glVertex3f(p[i].p2.px, p[i].p2.py, p[i].p2.pz);
            glVertex3f(p[i].p3.px, p[i].p3.py, p[i].p3.pz);

            glEnd();
        }
    }
}
//////////////////////////////////////////////////////////////////
// This function does any needed initialization on the rendering
// context.
void SetupRC()
{
    int iSphere;

    // Calculate shadow matrix
    M3DVector3f vPoints[3] = {{ 0.0f, -0.4f, 0.0f },
        { 10.0f, -0.4f, 0.0f },
        { 5.0f, -0.4f, -5.0f }
    };

    // Grayish background
    glClearColor(fLowLight[0], fLowLight[1], fLowLight[2], fLowLight[3]);

    // Cull backs of polygons
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // Setup light parameters
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fNoLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, fLowLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, fBrightLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, fBrightLight);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);


    // Get the plane equation from three points on the ground
    M3DVector4f vPlaneEquation;
    m3dGetPlaneEquation(vPlaneEquation, vPoints[0], vPoints[1], vPoints[2]);

    // Calculate projection matrix to draw shadow on the ground
    m3dMakePlanarShadowMatrix(mShadowMatrix, vPlaneEquation, fLightPos);


    // Mostly use material tracking
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glMateriali(GL_FRONT, GL_SHININESS, 128);

    // Randomly place the sphere inhabitants
    for(iSphere = 0; iSphere < NUM_SPHERES; iSphere++)
    {
        spheres[iSphere].SetOrigin((float)((rand() % 400) - 200) * 0.1f, 0.0f,
                                   (float)((rand() % 400) - 200) * 0.1f);
    }
}


///////////////////////////////////////////////////////////
// Draw the ground as a series of triangle strips
void DrawGround(void)
{
    GLfloat fExtent = 20.0f;
    GLfloat fStep = 1.0f;
    GLfloat y = -0.4f;
    GLint iStrip, iRun;

    for(iStrip = -fExtent; iStrip <= fExtent; iStrip += fStep)
    {
        glBegin(GL_TRIANGLE_STRIP);
        glNormal3f(0.0f, 1.0f, 0.0f);   // All Point up

        for(iRun = fExtent; iRun >= -fExtent; iRun -= fStep)
        {
            glVertex3f(iStrip, y, iRun);
            glVertex3f(iStrip + fStep, y, iRun);
        }
        glEnd();
    }
}

///////////////////////////////////////////////////////////////////////
// Draw random inhabitants and the rotating torus/sphere duo
void DrawInhabitants(GLint nShadow)
{
    static GLfloat yRot = 0.0f;         // Rotation angle for animation


    if(nShadow == 0)
        yRot += 0.5f;
    else
        glColor3f(0.0f, 0.0f, 0.0f);

    glPushMatrix();
    glTranslatef(0.0f, 0.1f, -2.5f);

    if(nShadow == 0)
    {
        // Torus alone will be specular
        glColor3f(1.0f, 0.0f, 1.0f);
        glMaterialfv(GL_FRONT, GL_SPECULAR, fBrightLight);
    }

    glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    DrawModel();

    glMaterialfv(GL_FRONT, GL_SPECULAR, fNoLight);
    glPopMatrix();
}


// Called to draw scene
void RenderScene(void)
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    frameCamera.ApplyCameraTransform();

    // Position light before any other transformations
    glLightfv(GL_LIGHT0, GL_POSITION, fLightPos);

    // Draw the ground
    glColor3f(0.60f, .40f, .10f);
    DrawGround();

    // Draw shadows first
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glMultMatrixf(mShadowMatrix);
    DrawInhabitants(1);
    glPopMatrix();
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    // Draw inhabitants normally
    DrawInhabitants(0);

    glPopMatrix();

    // Do the buffer Swap
    glutSwapBuffers();
    glutPostRedisplay();
}



// Respond to arrow keys by moving the camera frame of reference
void SpecialKeys(int key, int x, int y)
{
    if(key == GLUT_KEY_UP)
        frameCamera.MoveForward(0.1f);

    if(key == GLUT_KEY_DOWN)
        frameCamera.MoveForward(-0.1f);

    if(key == GLUT_KEY_LEFT)
        frameCamera.RotateLocalY(0.1);

    if(key == GLUT_KEY_RIGHT)
        frameCamera.RotateLocalY(-0.1);

    // Refresh the Window
    glutPostRedisplay();
}
void KeyboardFunc(unsigned char key, int x, int y)
{
    if(key == 'q')exit(0);

    // Refresh the Window
    glutPostRedisplay();
}

///////////////////////////////////////////////////////////
// Called by GLUT library when idle (window not being
// resized or moved)
void TimerFunction(int value)
{
    // Redraw the scene with new coordinates
    glutPostRedisplay();
    glutTimerFunc(3,TimerFunction, 1);
}

void ChangeSize(int w, int h)
{
    GLfloat fAspect;

    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0)
        h = 1;

    glViewport(0, 0, w, h);

    fAspect = (GLfloat)w / (GLfloat)h;

    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set the clipping volume
    gluPerspective(35.0f, fAspect, 1.0f, 50.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800,600);
    glutCreateWindow("glut model loader");
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutSpecialFunc(SpecialKeys);
    glutKeyboardFunc(KeyboardFunc);
    read_binary_stl("b2.stl");
    //read_ascii_stl("a0.stl");
    SetupRC();
    glutTimerFunc(33, TimerFunction, 1);

    glutMainLoop();

    return 0;
}
