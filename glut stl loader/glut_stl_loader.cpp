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
    point pp[3];
};

vec p[500000];
bool state = 0;
double voxsize = 0.1;
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

            p[i].pp[0].px = *((float*) fx1 );
            p[i].pp[0].py = *((float*) fy1 );
            p[i].pp[0].pz = *((float*) fz1 );

            p[i].pp[1].px = *((float*) fx2 );
            p[i].pp[1].py = *((float*) fy2 );
            p[i].pp[1].pz = *((float*) fz2 );

            p[i].pp[2].px = *((float*) fx3 );
            p[i].pp[2].py = *((float*) fy3 );
            p[i].pp[2].pz = *((float*) fz3 );

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
                            // p[fn].p1.px = f;
                            vn++;
                        }
                        else if(vn==1)
                        {
                            //p[fn].p1.py = f;
                            vn++;
                        }
                        else if(vn==2)
                        {
                            //p[fn].p1.pz = f;
                            vn=0;
                            vv++;
                            //cout << p[fn].p1.px << " , " << p[fn].p1.py << " , " << p[fn].p1.pz << endl;

                        }
                    }
                    else if(vv==1)
                    {
                        if(vn==0)
                        {
                            //p[fn].p2.px = f;
                            vn++;
                        }
                        else if(vn==1)
                        {
                            //p[fn].p2.py = f;
                            vn++;
                        }
                        else if(vn==2)
                        {
                            //p[fn].p2.pz = f;
                            vn=0;
                            vv++;
                            //cout << p[fn].p2.px << " , " << p[fn].p2.py << " , " << p[fn].p2.pz << endl;
                        }
                    }
                    else if(vv==2)
                    {
                        if(vn==0)
                        {
                            //p[fn].p3.px = f;
                            vn++;
                        }
                        else if(vn==1)
                        {
                            //p[fn].p3.py = f;
                            vn++;
                        }
                        else if(vn==2)
                        {
                            //p[fn].p3.pz = f;
                            vn=0;
                            vv=0;
                            //cout << p[fn].p3.px << " , " << p[fn].p3.py << " , " << p[fn].p3.pz << endl;
                            //cout << fn << endl;
                        }
                    }

                }
            }

        }
    }
    cout << "facet: " << fn << endl;


}
void drawvoxel(point p1,double vsize)
{
    glBegin(GL_QUADS);        // Draw The Cube Using quads
            glColor3f(0.0f,0.0f,1.0f);    // Color Blue
            glVertex3f(p1.px+vsize,p1.py+vsize,p1.pz-vsize);    // Top Right Of The Quad (Top)
            glVertex3f(p1.px-vsize,p1.py+vsize,p1.pz-vsize);    // Top Left Of The Quad (Top)
            glVertex3f(p1.px-vsize,p1.py+vsize,p1.pz+vsize);    // Bottom Left Of The Quad (Top)
            glVertex3f(p1.px+vsize,p1.py+vsize,p1.pz+vsize);    // Bottom Right Of The Quad (Top)
            //glColor3f(1.0f,0.5f,0.0f);    // Color Orange
            glVertex3f(p1.px+vsize,p1.py-vsize,p1.pz+vsize);    // Top Right Of The Quad (Bottom)
            glVertex3f(p1.px-vsize,p1.py-vsize,p1.pz+vsize);    // Top Left Of The Quad (Bottom)
            glVertex3f(p1.px-vsize,p1.py-vsize,p1.pz-vsize);    // Bottom Left Of The Quad (Bottom)
            glVertex3f(p1.px+vsize,p1.py-vsize,p1.pz-vsize);    // Bottom Right Of The Quad (Bottom)
            //glColor3f(1.0f,0.0f,0.0f);    // Color Red
            glVertex3f(p1.px+vsize,p1.py+vsize,p1.pz+vsize);    // Top Right Of The Quad (Front)
            glVertex3f(p1.px-vsize,p1.py+vsize,p1.pz+vsize);    // Top Left Of The Quad (Front)
            glVertex3f(p1.px-vsize,p1.py-vsize,p1.pz+vsize);    // Bottom Left Of The Quad (Front)
            glVertex3f(p1.px+vsize,p1.py-vsize,p1.pz+vsize);    // Bottom Right Of The Quad (Front)
            //glColor3f(1.0f,1.0f,0.0f);    // Color Yellow
            glVertex3f(p1.px+vsize,p1.py-vsize,p1.pz-vsize);    // Top Right Of The Quad (Back)
            glVertex3f(p1.px-vsize,p1.py-vsize,p1.pz-vsize);    // Top Left Of The Quad (Back)
            glVertex3f(p1.px-vsize,p1.py+vsize,p1.pz-vsize);    // Bottom Left Of The Quad (Back)
            glVertex3f(p1.px+vsize,p1.py+vsize,p1.pz-vsize);    // Bottom Right Of The Quad (Back)
            //glColor3f(0.0f,0.0f,1.0f);    // Color Blue
            glVertex3f(p1.px-vsize,p1.py+vsize,p1.pz+vsize);    // Top Right Of The Quad (Left)
            glVertex3f(p1.px-vsize,p1.py+vsize,p1.pz-vsize);    // Top Left Of The Quad (Left)
            glVertex3f(p1.px-vsize,p1.py-vsize,p1.pz-vsize);    // Bottom Left Of The Quad (Left)
            glVertex3f(p1.px-vsize,p1.py-vsize,p1.pz+vsize);    // Bottom Right Of The Quad (Left)
            //glColor3f(1.0f,0.0f,1.0f);    // Color Violet
            glVertex3f(p1.px+vsize,p1.py+vsize,p1.pz-vsize);    // Top Right Of The Quad (Right)
            glVertex3f(p1.px+vsize,p1.py+vsize,p1.pz+vsize);    // Top Left Of The Quad (Right)
            glVertex3f(p1.px+vsize,p1.py-vsize,p1.pz+vsize);    // Bottom Left Of The Quad (Right)
            glVertex3f(p1.px+vsize,p1.py-vsize,p1.pz-vsize);    // Bottom Right Of The Quad (Right)
            glEnd();            // End Drawing The Cube - See more at: http://www.codemiles.com/c-opengl-examples/draw-3d-cube-using-opengl-t9018.html#sthash.FDVsWoyj.dpuf

}
void Bresenham(int x0,int y0,int z0,int x1,int y1,int z1)
{
    if((z1-z0)==0)
    {
        bool steep = abs(y1-y0) > abs(x1-x0);
        if(steep)
        {
            swap(x0,y0);
            swap(x1,y1);
        }
        if(x0>x1)
        {
            swap(x0,x1);
            swap(y0,y1);
        }
        int deltax = x1-x0;
        int deltay = abs(y1-y0);
        float error = 0;
        float deltaerr = deltay/deltax;
        int ystep;
        int y = y0;
        if(y0<y1)ystep = 1;
        else ystep = -1;
        point temp;
        temp.pz = z0;
        for(int i=x0; i<=x1; i++)
        {
            if(steep)
            {
                temp.px = y;
                temp.py = i;
            }
            else
            {
                temp.px = i;
                temp.py = y;
            }
            error += deltaerr;
            if(error >= 0.5)
            {
                y += ystep;
                error -= 1.0;
            }
            drawvoxel(temp,0.5);
        }
    }
    else if((x1-x0)==0)
    {
        bool steep = abs(z1-z0) > abs(y1-y0);
        if(steep)
        {
            swap(y0,z0);
            swap(y1,z1);
        }
        if(y0>y1)
        {
            swap(y0,y1);
            swap(z0,z1);
        }
        int deltay = y1-y0;
        int deltaz = abs(z1-z0);
        float error = 0;
        float deltaerr = deltaz/deltay;
        int zstep;
        int z = z0;
        if(z0<z1)zstep = 1;
        else zstep = -1;
        point temp;
        temp.px = x0;
        for(int i=y0; i<=y1; i++)
        {
            if(steep)
            {
                temp.py = z;
                temp.pz = i;
            }
            else
            {
                temp.py = i;
                temp.pz = z;
            }
            error += deltaerr;
            if(error >= 0.5)
            {
                z += zstep;
                error -= 1.0;
            }
            drawvoxel(temp,0.5);
        }
    }
    /*else if((y1-y0)==0)
    {
        bool steep = abs(z1-z0) > abs(x1-x0);
        if(steep)
        {
            swap(x0,z0);
            swap(x1,z1);
        }
        if(x0>x1)
        {
            swap(x0,x1);
            swap(z0,z1);
        }
        int deltax = x1-x0;
        int deltaz = abs(z1-z0);
        float error = 0;
        float deltaerr = deltaz/deltax;
        int zstep;
        int z = z0;
        if(z0<z1)zstep = 1;
        else zstep = -1;
        point temp;
        temp.py = y0;
        for(int i=x0; i<=x1; i++)
        {
            if(steep)
            {
                temp.px = z;
                temp.pz = i;
            }
            else
            {
                temp.px = i;
                temp.pz = z;
            }
            error += deltaerr;
            if(error >= 0.5)
            {
                z += zstep;
                error -= 1.0;
            }
            drawvoxel(temp,0.5);
        }
    }*/
}
void voxelize(void)
{
    for(int i = 0; i < int(nTriLong); i++)
    {

        point v21,v31,v32;
        v21.px = p[i].pp[1].px-p[i].pp[0].px;
        v21.py = p[i].pp[1].py-p[i].pp[0].py;
        v21.pz = p[i].pp[1].pz-p[i].pp[0].pz;

        v31.px = p[i].pp[2].px-p[i].pp[0].px;
        v31.py = p[i].pp[2].py-p[i].pp[0].py;
        v31.pz = p[i].pp[2].pz-p[i].pp[0].pz;

        v32.px = p[i].pp[2].px-p[i].pp[1].px;
        v32.py = p[i].pp[2].py-p[i].pp[1].py;
        v32.pz = p[i].pp[2].pz-p[i].pp[1].pz;


        for(int j=0; j<3; j++)
        {
            if(j==2)Bresenham(p[i].pp[j].px,p[i].pp[j].py,p[i].pp[j].pz,p[i].pp[0].px,p[i].pp[0].py,p[i].pp[0].pz);
            else Bresenham(p[i].pp[j].px,p[i].pp[j].py,p[i].pp[j].pz,p[i].pp[j+1].px,p[i].pp[j+1].py,p[i].pp[j+1].pz);
        }

        /*cout << v21.px << "," << v21.py << "," << v21.pz << endl;
        cout << v31.px << "," << v31.py << "," << v31.pz << endl;
        cout << v32.px << "," << v32.py << "," << v32.pz << endl;*/
    }





    /*for(int i = 0; i < int(nTriLong); i++)
    {
        for(int j = 0; j < 3; j++)
        {
            drawvoxel(p[i].pp[j],voxsize);
        }
    }*/
}
point cenPoint(vec p[])
{
    point avg,cenP;
    cenP.px = 0;
    cenP.py = 0;
    cenP.pz = 0;
    for(int i = 0; i < int(nTriLong); i++)
    {
        avg.px = 0;
        avg.py = 0;
        avg.pz = 0;
        for(int j = 0; j < 3; j++)
        {
            avg.px += p[i].pp[j].px;
            avg.py += p[i].pp[j].py;
            avg.pz += p[i].pp[j].pz;
        }
        cenP.px += avg.px/3;
        cenP.py += avg.py/3;
        cenP.pz += avg.pz/3;
    }
    cenP.px /= int(nTriLong);
    cenP.py /= int(nTriLong);
    cenP.pz /= int(nTriLong);
    return cenP;
}

float minZ(vec p[])
{
    float minZ = 100;
    for(int i = 0; i < int(nTriLong); i++)
    {
        for(int j = 0; j < 3; j++)
        {
            if(p[i].pp[j].pz < minZ)minZ = p[i].pp[j].pz;
        }
    }
    return minZ;
}
point firstcenP(vec p[])
{
    int count = 0;
    point fcp;
    fcp.px = 0;
    fcp.py = 0;
    fcp.pz = 0;
    for(int i = 0; i < int(nTriLong); i++)
    {
        for(int j = 0; j < 3; j++)
        {
            if(p[i].pp[j].pz == minZ(p))
            {
                fcp.px += p[i].pp[j].px;
                fcp.py += p[i].pp[j].py;
                count++;
            }
        }
    }
    fcp.px /= count;
    fcp.py /= count;
    fcp.pz = minZ(p);
    cout << count << endl;
    return fcp;
}
void drawCenPoint(point pc)
{

    glPushMatrix();
    glColor3f(1,0,0);
    GLUquadric *quad = gluNewQuadric();
    glTranslatef(pc.px,pc.py,minZ(p));
    gluSphere(quad,1,100,20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1,1,0);
    GLUquadric *quad1 = gluNewQuadric();
    glTranslatef(firstcenP(p).px,firstcenP(p).py,firstcenP(p).pz);
    gluSphere(quad1,1,100,20);
    glPopMatrix();

}
void DrawModel()
{
    glPushMatrix();
    glTranslatef(0.0f, -0.5f, 0.0f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glScaled (0.01, 0.01, 0.01);
    if(!state)
    {

        for(int i = 0; i < int(nTriLong); i++)
        {
            glBegin(GL_TRIANGLES);
            for(int j = 0; j < 3; j++)
                glVertex3f(p[i].pp[j].px, p[i].pp[j].py, p[i].pp[j].pz);
            glEnd();
        }

    }
    else
    {
        //voxelize();
        for(int i = 0; i < int(nTriLong); i++)
        {
            glBegin(GL_LINES);
            glVertex3f(p[i].pp[0].px, p[i].pp[0].py, p[i].pp[0].pz);
            glVertex3f(p[i].pp[1].px, p[i].pp[1].py, p[i].pp[1].pz);
            glEnd();
            glBegin(GL_LINES);
            glVertex3f(p[i].pp[1].px, p[i].pp[1].py, p[i].pp[1].pz);
            glVertex3f(p[i].pp[2].px, p[i].pp[2].py, p[i].pp[2].pz);
            glEnd();
            glBegin(GL_LINES);
            glVertex3f(p[i].pp[0].px, p[i].pp[0].py, p[i].pp[0].pz);
            glVertex3f(p[i].pp[2].px, p[i].pp[2].py, p[i].pp[2].pz);
            glEnd();
        }
        drawCenPoint(cenPoint(p));

    }
    glPopMatrix();
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
        yRot += 0.05f;
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

    //glRotatef(yRot, 0.0f, 1.0f, 0.0f);
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
    if(key == 'z')
    {
        if(state)state = 0;
        else state = 1;
    }
    if(key == 'a')voxsize+=0.1;
    if(key == 's')voxsize-=0.1;
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
    read_binary_stl("b1.stl");
    //read_ascii_stl("a0.stl");
    SetupRC();
    glutTimerFunc(33, TimerFunction, 1);

    glutMainLoop();

    return 0;
}
