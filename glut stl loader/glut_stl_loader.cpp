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
    float volume;
    float a;
    float b;
    float c;
    float d;
};

vec p[500000];
bool state = 0;
bool voxstate = 0;
bool voxline = 0;
double voxsize = 10;
point barycenter;
float Volume1=0,Volume2=0;
static GLfloat yRot = 0.0f;         // Rotation angle for animation
static GLfloat zRot = 0.0f;         // Rotation angle for animation
point calVec(point a,point b)
{
    point ab;
    ab.px = b.px - a.px;
    ab.py = b.py - a.py;
    ab.pz = b.pz - a.pz;
    return ab;
}
void cross2Product(point a, point b,int index,point c)
{
    p[index].a = a.py*b.pz - a.pz*b.py;
    p[index].b = a.pz*b.px - a.px*b.pz;
    p[index].c = a.px*b.py - a.py*b.px;
    p[index].d = p[index].a*c.px + p[index].b*c.py + p[index].c*c.pz;
}
float cross3Product(point a, point b, point c)
{
    float product;
    product = abs(  a.px*b.py*c.pz + a.py*b.pz*c.px + a.pz*b.px*c.py
                  - a.pz*b.py*c.px - a.py*b.px*c.pz - a.px*b.pz*c.py);
    return product;
}
float Atriangle(vec tri)
{
    point pq,pr;
    pq = calVec(tri.pp[0],tri.pp[1]);
    pr = calVec(tri.pp[0],tri.pp[2]);
    float area = sqrtf(pow(pq.py*pr.pz-pr.py*pq.pz,2)+pow(pq.pz*pr.px-pr.pz*pq.px,2)+pow(pq.px*pr.py-pr.px*pq.py,2))/2;
    return area;
}
float Vtetrahedron(vec tri)
{
    point pq,pr,ps;
    pq = calVec(barycenter,tri.pp[0]);
    pr = calVec(barycenter,tri.pp[1]);
    ps = calVec(barycenter,tri.pp[2]);
    float volume = cross3Product(pq,pr,ps)/6;
    return volume;
}
void calVoxvolume(int numVox)
{
    Volume2 = numVox * pow(voxsize,3);
    //cout << volume << endl;
}
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

    // initial volume to 1;
    for(int i = 0; i < int(nTriLong); i++)
    {
        p[i].volume = 1;
    }
    //calculate inner point
    for(int i = 0; i < int(nTriLong); i++)
    {
        for(int j = 0; j < 3; j++)
        {
            barycenter.px += p[i].pp[j].px;
            barycenter.py += p[i].pp[j].py;
            barycenter.pz += p[i].pp[j].pz;
        }
    }
    barycenter.px /= float(int(nTriLong)*3);
    barycenter.py /= float(int(nTriLong)*3);
    barycenter.pz /= float(int(nTriLong)*3);
    //calculate volume from barycenter to triangle
    for(int i = 0; i < int(nTriLong); i++)
    {
        p[i].volume = Vtetrahedron(p[i]);
    }
    //calculate triangle function
    for(int i = 0; i < int(nTriLong); i++)
    {
        point a = calVec(p[i].pp[0],p[i].pp[1]);
        point b = calVec(p[i].pp[0],p[i].pp[2]);
        cross2Product(a,b,i,p[i].pp[0]);
    }
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

void draw_cube(int x,int y,int z)
{
    float v = voxsize/2;
    glPushMatrix();
    glColor3f(1,0,0);
    glBegin(GL_POLYGON);
    glVertex3f(x+v, y+v, z+v);  glVertex3f(x-v, y+v, z+v); glVertex3f(x-v, y-v, z+v); glVertex3f(x+v, y-v, z+v);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3f(x+v, y+v, z+v);  glVertex3f(x+v, y+v, z-v); glVertex3f(x+v, y-v, z-v); glVertex3f(x+v, y-v, z+v);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3f(x+v, y+v, z+v);  glVertex3f(x+v, y+v, z-v); glVertex3f(x-v, y+v, z-v); glVertex3f(x-v, y+v, z+v);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3f(x-v, y+v, z+v);  glVertex3f(x-v, y+v, z-v); glVertex3f(x-v, y-v, z-v); glVertex3f(x-v, y-v, z+v);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3f(x+v, y-v, z+v);  glVertex3f(x-v, y-v, z+v); glVertex3f(x-v, y-v, z-v); glVertex3f(x+v, y-v, z-v);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3f(x+v, y+v, z-v);  glVertex3f(x-v, y+v, z-v); glVertex3f(x-v, y-v, z-v); glVertex3f(x+v, y-v, z-v);
    glEnd();
    glPopMatrix();

}
void calVolrate(float v1,float v2)
{
    float rate = abs(v1-v2)/v1;
    int ratioV = int(rate*100);
    cout << "Volume similarity:" << ratioV << "%" << endl;
}
int find_in(int xp,int yp,int zp, int zn)
{
    int start=-1,ending=-1;
    int d,f;
    point project;
    for(int i = 0; i < int(nTriLong); i++)
    {
        for(int k=0; k<1; k++)
        {
            d = abs(p[i].a*xp + p[i].b*yp + p[i].c*(zp+k*voxsize) + p[i].d)
                /sqrtf(pow(p[i].a,2)+pow(p[i].b,2)+pow(p[i].c,2));

            f = (p[i].a*xp + p[i].b*yp + p[i].c*(zp+k*voxsize) + p[i].d)
                /(pow(p[i].a,2)+pow(p[i].b,2)+pow(p[i].c,2));

            project.px = xp + f*p[i].a;
            project.py = yp + f*p[i].b;
            project.pz = (zp + k*voxsize) + f*p[i].c;

            if((d<(sqrtf(3)*voxsize/2))
               &&(project.px>(xp-voxsize/2))
               &&(project.px<(xp+voxsize/2))
               &&(project.py>(yp-voxsize/2))
               &&(project.py<(yp+voxsize/2)))
            {
                if(start<0)start = k;
                else
                {
                    ending = k;
                    break;
                }
            }
        }
    }

    int temp;
    if(start > ending)
    {
        if(ending!=-1)
        {
            temp = ending;
            ending = start;
            start = temp;
        }
    }

    if((start==-1)&&(ending!=-1))
    {
        if(ending!=-1)
        {
            temp = ending;
            ending = start;
            start = temp;
        }
    }

    //cout << start << "," << ending << endl;
    //draw cube

        if(start > ending)
        {
            for(int i=start; i<zn; i++)
            {
                draw_cube(xp,yp,zp+i*voxsize);
            }
            return zn - start;
        }
    else if((start==-1)&&(ending==-1))
    {
    }
    else
    {
        for(int i=start; i<=ending; i++)
        {
            draw_cube(xp,yp,zp+i*voxsize);
        }
        return ending - start +1;
    }

}
void line(int x,int y, int z)
{
    float v = voxsize/2;
    glBegin(GL_LINES);  glVertex3f(x+v, y+v, z+v);  glVertex3f(x-v, y+v, z+v);  glEnd();
    glBegin(GL_LINES);  glVertex3f(x-v, y+v, z+v);  glVertex3f(x-v, y-v, z+v);  glEnd();
    glBegin(GL_LINES);  glVertex3f(x-v, y-v, z+v);  glVertex3f(x+v, y-v, z+v);  glEnd();
    glBegin(GL_LINES);  glVertex3f(x+v, y-v, z+v);  glVertex3f(x+v, y+v, z+v);  glEnd();

    glBegin(GL_LINES);  glVertex3f(x+v, y+v, z+v);  glVertex3f(x+v, y+v, z-v);  glEnd();
    glBegin(GL_LINES);  glVertex3f(x-v, y+v, z+v);  glVertex3f(x-v, y+v, z-v);  glEnd();
    glBegin(GL_LINES);  glVertex3f(x+v, y-v, z+v);  glVertex3f(x+v, y-v, z-v);  glEnd();
    glBegin(GL_LINES);  glVertex3f(x-v, y-v, z+v);  glVertex3f(x-v, y-v, z-v);  glEnd();

    glBegin(GL_LINES);  glVertex3f(x+v, y+v, z-v);  glVertex3f(x-v, y+v, z-v);  glEnd();
    glBegin(GL_LINES);  glVertex3f(x-v, y+v, z-v);  glVertex3f(x-v, y-v, z-v);  glEnd();
    glBegin(GL_LINES);  glVertex3f(x-v, y-v, z-v);  glVertex3f(x+v, y-v, z-v);  glEnd();
    glBegin(GL_LINES);  glVertex3f(x+v, y-v, z-v);  glVertex3f(x+v, y+v, z-v);  glEnd();
}
void drawvoxel(float xMax, float yMax, float zMax, float xMin, float yMin, float zMin)
{
    int x0=int(xMin),y0=int(yMin),z0=int(zMin);
    //cout << x0 << "," << y0 << "," << z0 << endl;
    int xn=int((xMax-x0)/voxsize)+1,yn=int((yMax-y0)/voxsize)+1,zn=int((zMax-z0)/voxsize)+1;
    //cout << xn << "," << yn << "," << zn << endl;
    int xp = x0 + voxsize/2, yp = y0 + voxsize/2, zp = z0 + voxsize/2;
    int count =0;
    //cout << xn*yn*zn << endl;
    //draw all voxel line
    for(int i=0; i<xn; i++)
    {
        for(int j=0; j<yn; j++)
        {
            for(int k=0; k<zn; k++)
            {
                line(xp+i*voxsize,yp+j*voxsize,zp+k*voxsize);
                //draw_cube(xp+i*voxsize,yp+j*voxsize,zp+k*voxsize);
            }
        }
    }
    //draw voxel
    if(voxstate)
    {
        for(int i=0; i<xn; i++)
        {
            for(int j=0; j<yn; j++)
            {
                count += find_in(xp+i*voxsize,yp+j*voxsize,zp,zn);
            }
        }
    }
    calVoxvolume(count);
}

void voxelize(void)
{
    float xMax=0,yMax=0,zMax=0,xMin=100,yMin=100,zMin=100;
    for(int i = 0; i < int(nTriLong); i++)
    {
        for(int j = 0; j < 3; j++)
        {
            if(p[i].pp[j].px > xMax)xMax = p[i].pp[j].px;
            if(p[i].pp[j].py > yMax)yMax = p[i].pp[j].py;
            if(p[i].pp[j].pz > zMax)zMax = p[i].pp[j].pz;
            if(p[i].pp[j].px < xMin)xMin = p[i].pp[j].px;
            if(p[i].pp[j].py < yMin)yMin = p[i].pp[j].py;
            if(p[i].pp[j].pz < zMin)zMin = p[i].pp[j].pz;
        }
    }
    for(int i = 0; i < int(nTriLong); i++)Volume1 += p[i].volume;
    if(voxline)drawvoxel(xMax,yMax,zMax,xMin,yMin,zMin);
}
point cenPoint(vec p[])
{
    float Tweight = 0;
    point avg,cenP;
    cenP.px = 0;
    cenP.py = 0;
    cenP.pz = 0;
    for(int i = 0; i < int(nTriLong); i++)
    {
        avg.px = barycenter.px;
        avg.py = barycenter.py;
        avg.pz = barycenter.pz;
        for(int j = 0; j < 3; j++)
        {
            avg.px += p[i].pp[j].px;
            avg.py += p[i].pp[j].py;
            avg.pz += p[i].pp[j].pz;
        }
        cenP.px += p[i].volume*avg.px/4;
        cenP.py += p[i].volume*avg.py/4;
        cenP.pz += p[i].volume*avg.pz/4;
        Tweight += p[i].volume;
    }
    cenP.px /= Tweight;
    cenP.py /= Tweight;
    cenP.pz /= Tweight;
    return cenP;
}

point firstcenP(vec p[])
{
    float Tweight = 0;
    point avg,cenP,fcp;
    cenP.px = 0;
    cenP.py = 0;
    cenP.pz = 0;
    float area;
    float minz = 100;
    for(int i = 0; i < int(nTriLong); i++)
    {
        for(int j = 0; j < 3; j++)
        {
            if(p[i].pp[j].pz < minz)minz = p[i].pp[j].pz;
        }
    }

    for(int i = 0; i < int(nTriLong); i++)
    {
        avg.px = 0;
        avg.py = 0;
        avg.pz = 0;
        area = 0;
        if(abs(minz*3-p[i].pp[0].pz-p[i].pp[1].pz-p[i].pp[2].pz)<3)
        {
            avg.px = (p[i].pp[0].px+p[i].pp[1].px+p[i].pp[2].px)/3;
            avg.py = (p[i].pp[0].py+p[i].pp[1].py+p[i].pp[2].py)/3;
            avg.pz = (p[i].pp[0].pz+p[i].pp[1].pz+p[i].pp[2].pz)/3;
            area = Atriangle(p[i]);
            cenP.px += avg.px*area;
            cenP.py += avg.py*area;
            cenP.pz += avg.pz*area;
            Tweight += area;
        }
    }
    fcp.px = cenP.px/Tweight;
    fcp.py = cenP.py/Tweight;
    fcp.pz = cenP.pz/Tweight;
    return fcp;
}
void drawCenPoint(point pc)
{

    glPushMatrix();
    glColor3f(1,0,0);
    GLUquadric *quad = gluNewQuadric();
    glTranslatef(pc.px,pc.py,pc.pz);
    gluSphere(quad,1,100,20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1,1,0);
    GLUquadric *quad1 = gluNewQuadric();
    point pp = firstcenP(p);
    glTranslatef(pp.px,pp.py,pp.pz);
    gluSphere(quad1,1,100,20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1,0,1);
    glBegin(GL_LINES);
    glVertex3f(pc.px,pc.py,pc.pz);
    glVertex3f(pp.px,pp.py,pp.pz);
    glEnd();
    glPopMatrix();
}
void DrawModel()
{
    glPushMatrix();
    glTranslatef(0.0f, -0.5f, 0.0f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glScaled (0.01, 0.01, 0.01);
    glColor3f(0.5f,0.5f,0.5f);
    if(!state)
    {

        for(int i = 0; i < int(nTriLong); i++)
        {
            glBegin(GL_TRIANGLES);
            for(int j = 0; j < 3; j++)
                glVertex3f(p[i].pp[j].px, p[i].pp[j].py, p[i].pp[j].pz);
            glEnd();
        }
        voxelize();

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



    /*if(nShadow == 0)
        yRot += 0.05f;
    else
        glColor3f(0.0f, 0.0f, 0.0f);*/

    glPushMatrix();
    glTranslatef(0.0f, 0.1f, -2.5f);

    /*if(nShadow == 0)
    {
        // Torus alone will be specular
        glColor3f(1.0f, 0.0f, 1.0f);
        glMaterialfv(GL_FRONT, GL_SPECULAR, fBrightLight);
    }*/

    glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    glRotatef(zRot, 0.0f, 0.0f, 1.0f);
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
    //glMultMatrixf(mShadowMatrix);
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
    if(key == 'a')voxsize+=0.5;
    if(key == 's')voxsize-=0.5;
    if(key == 'x')
    {
        if(voxline)voxline = 0;
        else voxline = 1;
    }
    if(key == 'c')
    {
        if(voxstate)voxstate = 0;
        else
        {
            voxstate = 1;
            calVolrate(Volume1,Volume2);
        }
    }
    if(key == 'j')yRot+=5;
    if(key == 'l')yRot-=5;
    if(key == 'i')zRot+=5;
    if(key == 'k')zRot-=5;
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
