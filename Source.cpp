#include "GL\freeglut.h"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include "FreeImage.h"
#include <windows.h>
#include<mmsystem.h>
#pragma comment(lib,"freeglut.lib")
#pragma comment(lib,"freeimage.lib")

using namespace std;
int cloudoff=1;
float ang = 0;
int texnum;
float sun=180,moon=180;
float R=0,G=0.6,B=0.8,A=0.5;
float movecarX=40,movecarZ=81,movecarY=0.66666667;
float RS=1,GS=0.64,BS=0;
float sunX = 2000, sunY = 1, sunZ = -1350; 
float CameraX = 600, CameraY = 40, CameraZ = 1350;
int stopcar=0;
float HeadingX=0, HeadingZ=-1,HeadingY=0;

float StepsDirection = 0;	
char TexturesLocation[5][50];
GLuint Texturesused[5];
GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat position[] = { 1.0f, 1.0f, 1.0f, 1.0f };
int loadTexture(const char* filename)
{
	GLuint texture;

	FIBITMAP* originalImage = FreeImage_Load(FreeImage_GetFileType(filename, 0), filename);	//Load original image from the storage device
	FIBITMAP* finalImage = FreeImage_ConvertTo32Bits(originalImage);	//Unify the image format to 32 Bits

	int width = FreeImage_GetWidth(finalImage);
	int height = FreeImage_GetHeight(finalImage);

	//OpenGL related routines

	//Let OpenGL know that we need to generate 1 texture
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);	//Set 'texture' as the active texture

	//Set the MAG and MIN Filters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Min Filter, use interpolation
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//Mag Filter, use interpolation

	//Convert from image to a texture
	//GL_BGRA_EXT is used because Freeimage API uses the big endian format (bytes are in reversed order)
	//GL_UNSIGNED_BYTE means that each channel is rep. using a value from 0~255
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, FreeImage_GetBits(finalImage));

	return texture;

}

//Particle Engine Data

class MiniCloudEngine
{
public:
	//Create a Particle Data Structure (Cloud)
	struct Cloud
	{
		GLuint texture;
		float x, y, z;
		float lifeTime;
		float fading;
		float scale;
		bool isAlive;
	};

	Cloud* clouds;	//Dynamic allocation for the required number of clouds
	int count;	//Number of Particles
	int  skyLevel;
	int regionSize;
	float windX;
	char CloudsTexturesLocation[8][50];	//Max of 8 textures. 50 is the max path size
	GLuint CloudTextures[8];


	//Constructor for the Cloud Engine
	MiniCloudEngine(int numberOfParticles, int skyL, int region)
	{
		//Initialize Textures Locations
		strcpy_s(CloudsTexturesLocation[0], "cloud0.jpg");
		strcpy_s(CloudsTexturesLocation[1], "cloud1.jpg");
		strcpy_s(CloudsTexturesLocation[2], "cloud2.jpg");
		strcpy_s(CloudsTexturesLocation[3], "cloud3.jpg");
		strcpy_s(CloudsTexturesLocation[4], "cloud4.jpg");
		strcpy_s(CloudsTexturesLocation[5], "cloud5.jpg");
		strcpy_s(CloudsTexturesLocation[6], "cloud6.jpg");
		strcpy_s(CloudsTexturesLocation[7], "cloud7.jpg");


		count = numberOfParticles;
		skyLevel = skyL;
		
		regionSize = region;

		clouds = new Cloud[count];

		windX = -0.1;
	}

	void initializeAcloud(int n)
	{
		clouds[n].y = skyLevel;
		clouds[n].x = rand() % (regionSize + 1) - regionSize / 2.0f;
		clouds[n].z = rand() % ((regionSize+250) + 1) - (regionSize+1150) / 2.0f;
		clouds[n].scale = (rand() % 50) / 50.0f + 0.5f; //(rand()%50 / 50) + minSize



		int tIndex = rand() % 8;
		clouds[n].texture = CloudTextures[tIndex];
		clouds[n].fading = (rand() % 10 / 1000.0f) + 0.0001;
		clouds[n].isAlive = true;
		clouds[n].lifeTime = 10.0f;



	}

	void update()
	{
		for (int i = 0; i < count; i++)
		{
			if (clouds[i].isAlive)
			{
				clouds[i].x += windX * (rand() % 5);
				clouds[i].lifeTime -= clouds[i].fading;
				if (clouds[i].lifeTime <= 0 || clouds[i].x <= -1000)
				{
					clouds[i].isAlive = false;
				}
			}
			else
			{
				initializeAcloud(i);
			}
		}
	}

	void render()
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_BLEND);



		glDisable( GL_DEPTH_TEST );
		for (int i = 0; i < count; i++)
		{
			if (clouds[i].isAlive)
			{
				

				glPushMatrix();

				glColor4f(1, 1, 1, clouds[i].lifeTime/10.0f);
				glScalef(clouds[i].scale, clouds[i].scale, clouds[i].scale);
				glTranslatef(clouds[i].x, clouds[i].y, clouds[i].z);

				glBindTexture(GL_TEXTURE_2D, clouds[i].texture);

				glBegin(GL_QUADS);

				glTexCoord2d(0, 0);	glVertex3f(-100, -25,0 );
				glTexCoord2d(0, 1);	glVertex3f(-100, 25, 0);
				glTexCoord2d(1, 1);	glVertex3f(100, 25, 0);
				glTexCoord2d(1, 0);	glVertex3f(100, -25, 0);

				glEnd();

				glPopMatrix();

			}
		}
		glDisable(GL_BLEND);
		glEnable( GL_DEPTH_TEST );


	}
	void initialize()
	{
		//Load Textures
		for (int i = 0; i < 8; i++)
			CloudTextures[i] = loadTexture(CloudsTexturesLocation[i]);

		//Initialize Clouds
		for (int i = 0; i < count; i++)
			initializeAcloud(i);
	}
};
MiniCloudEngine cloudEngine(100,640, 2000);
void manageAsyncKeyPresses()
{
	if (GetAsyncKeyState('1') != 0)	//User is pressing left
	{
		stopcar = 0;
		CameraX = 600;
		CameraY = 40;
		CameraZ = 1350;
		HeadingY = 0;
		HeadingZ = -1;
		HeadingX = 0;
		movecarX = 40;
		movecarZ = 81;
		movecarY = 0.66666667;
		cloudoff=1;
	}
	if (GetAsyncKeyState('2') != 0)	//User is pressing left
	{
		stopcar = 1;
		CameraX =600;
		CameraY = 14;
		CameraZ = 1225;
		HeadingY = 0;
		HeadingZ = -1;
		HeadingX = 0;
		movecarX = 40;
		movecarZ = 81;
		movecarY = 0.66666667;
				cloudoff=0;
	}
	if (GetAsyncKeyState('3') != 0)	//User is pressing left
	{
		stopcar = 1;
		CameraX = 377;
		CameraY = 2474;
		CameraZ = 180;
		HeadingY = -12;
		HeadingZ = 0;
		HeadingX = -1.5;
		cloudoff=0;
		
	}
	
	if (((GetAsyncKeyState(VK_LEFT) != 0 && GetAsyncKeyState(VK_UP) != 0) || (GetAsyncKeyState(VK_LEFT) != 0 && GetAsyncKeyState(VK_DOWN) != 0)) && stopcar == 0)	//User is pressing left
	{
		if(movecarX>35.129)
		{
		
		
		movecarX -= 1 * 0.0666;
	}
	}
	if (((GetAsyncKeyState(VK_RIGHT) != 0 && GetAsyncKeyState(VK_UP) != 0) || (GetAsyncKeyState(VK_RIGHT) != 0 && GetAsyncKeyState(VK_DOWN) != 0)) && stopcar == 0)
	{
		if(movecarX<44.129){
		
		
		movecarX += 1 * 0.0666;
		}
		
	}
	if ((GetAsyncKeyState(VK_UP) != 0) && stopcar == 0)
	{
		
		if(movecarZ>-77.842)
		{
		movecarX += HeadingX * 5 * 0.0666;
		movecarZ += HeadingZ * 5 * 0.0666;
		CameraX += HeadingX * 5;
		CameraZ += HeadingZ * 5;
		}
		
	}
	if ((GetAsyncKeyState(VK_DOWN) != 0) && stopcar == 0)
	{
		if(movecarZ<81.666)
		{
		movecarX -= HeadingX * 5 * 0.0666;
		movecarZ -= HeadingZ * 5 * 0.0666;
		CameraX -= HeadingX * 5;
		CameraZ -= HeadingZ * 5;
		}
		
	}


}


int face3dbulding(int p)
{
	if( p<140)
		p=0;
	else if (139<p && p<576)
		p=1;
	else if (575<p && p<924)
		p=2;
	else if (923<p && p<1064)
		p=3;
	else if (1063<p&&p<1500)
		p=4;
	else if (1499<p&&p<2208)
		p=5;
	else if (2207<p&&p<2556)
		p=6;
	else if (2555<p&&p<2632)
		p=7;
	else if (2631<p&&p<2828)
		p=8;
	else if (2827<p&&p<3008)
		p=9;
	else if (3007<p&&p<3768)
		p=10;
	else if (3767<p&&p<4564)
		p=11;
	else if (4563<p&&p<6166)
		p=12;
	else if (6165<p&&p<9200)
		p=13;
	else if (9199<p&&p<11004)
		p=14;
	else if (10003<p&&p<11436)
		p=15;
	else if (11435<p&&p<12580)
		p=16;
	else if (12579<p&&p<13604)
		p=17;
	else if (13603<p&&p<14400)
		p=18;
return p;
}
struct Vertex
{
	float x, y, z;
	Vertex()
	{
		x = y = z = -1;
	}
};
class Object3D
{
	struct Face
	{
		Vertex a, b, c, d;
		Vertex an, bn, cn, dn;
		Vertex at, bt, ct, dt;

		int type;
		int texIndex;

		Face()
		{
			type = 1;	//Default: Triangle
		}
	};
public:
	vector<Vertex> vertices;
	vector<Vertex> normals;
	vector<Vertex> textures;
	int file;
	vector<Face> faces;
	vector<GLuint> tex;

	bool loadObject(const char*filename){
		int texLocation = -1;
		FILE* in = fopen(filename, "r");
		if (!in)
			return false;

		char header[100];
		while (fscanf(in, "%s", header) != EOF)
		{
			if (!strcmp(header, "v"))
			{
				Vertex pt;
				fscanf(in, "%f%f%f", &pt.x, &pt.y, &pt.z);
				vertices.push_back(pt);
			}
			if (!strcmp(header, "vt"))
			{
				Vertex pt;
				fscanf(in, "%f%f%f", &pt.x, &pt.y, &pt.z);
				textures.push_back(pt);
			}
			if (!strcmp(header, "vn"))
			{
				Vertex pt;
				fscanf(in, "%f%f%f", &pt.x, &pt.y, &pt.z);
				normals.push_back(pt);
			}
			if (!strcmp(header, "o"))
			{
				char tName[40];
				fscanf(in, "%s", tName);
				GLuint t = loadTexture(tName);
				tex.push_back(t); 	

				texLocation++;
			}
			if (!strcmp(header, "f"))
			{
				Face f;

				int av = -1, bv = -1, cv = -1, dv = -1;
				int at = -1, bt = -1, ct = -1, dt = -1;
				int an = -1, bn = -1, cn = -1, dn = -1;

				char fLine[100];
				fgets(fLine, 99, in);

				sscanf(fLine, " %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &av, &at,
					&an, &bv, &bt, &bn, &cv, &ct, &cn, &dv, &dt, &dn);
				if (bv == -1)
				{
					sscanf(fLine, " %d/%d %d/%d %d/%d %d/%d", &av, &at,
						&bv, &bt, &cv, &ct, &dv, &dt);
					an = bn = cn = dn = 0;
				}

				f.a = vertices[av - 1];
				f.b = vertices[bv - 1];
				f.c = vertices[cv - 1];

				f.at = textures[at - 1];
				f.bt = textures[bt - 1];
				f.ct = textures[ct - 1];

				if (normals.size() > 0)
				{
					f.an = normals[an - 1];
					f.bn = normals[bn - 1];
					f.cn = normals[cn - 1];

				}

				if (dv != -1)
				{
					f.type = 2;
					f.d = vertices[dv - 1];
					f.dt = textures[dt - 1];
					if (normals.size() > 0)
					{
						f.dn = normals[dn - 1];
					}
				}
				f.texIndex = texLocation;
				faces.push_back(f);

			}

		}
	}
	void render(int file){
		for (int i = 0; i < faces.size(); i++)
		{
			if(file==1){
				texnum=face3dbulding(i);
			glBindTexture(GL_TEXTURE_2D,tex[texnum]);
			}
			else if (file==2)
			{
			glBindTexture(GL_TEXTURE_2D,tex[faces[i].texIndex]);
			}
			else if(file==3)
			{
			
			glBindTexture(GL_TEXTURE_2D, Texturesused[3]);
			}
			if (faces[i].type == 1)
			{
				glBegin(GL_TRIANGLES);

				glTexCoord2d(faces[i].at.x, faces[i].at.y);
				glNormal3f(faces[i].an.x, faces[i].an.y, faces[i].an.z);
				glVertex3f(faces[i].a.x, faces[i].a.y , faces[i].a.z);

				glTexCoord2d(faces[i].bt.x, faces[i].bt.y);
				glNormal3f(faces[i].bn.x, faces[i].bn.y, faces[i].bn.z);
				glVertex3f(faces[i].b.x , faces[i].b.y , faces[i].b.z);

				glTexCoord2d(faces[i].ct.x, faces[i].ct.y);
				glNormal3f(faces[i].cn.x, faces[i].cn.y, faces[i].cn.z);
				glVertex3f(faces[i].c.x , faces[i].c.y , faces[i].c.z);


				glEnd();
			}
			else
			{
				glBegin(GL_QUADS);
				glTexCoord2d(faces[i].at.x, faces[i].at.y);
				glNormal3f(faces[i].an.x, faces[i].an.y, faces[i].an.z);
				glVertex3f(faces[i].a.x, faces[i].a.y , faces[i].a.z);

				glTexCoord2d(faces[i].bt.x, faces[i].bt.y);
				glNormal3f(faces[i].bn.x, faces[i].bn.y, faces[i].bn.z);
				glVertex3f(faces[i].b.x , faces[i].b.y , faces[i].b.z);

				glTexCoord2d(faces[i].ct.x, faces[i].ct.y);
				glNormal3f(faces[i].cn.x, faces[i].cn.y, faces[i].cn.z);
				glVertex3f(faces[i].c.x , faces[i].c.y , faces[i].c.z);

				glTexCoord2d(faces[i].dt.x, faces[i].dt.y);
				glNormal3f(faces[i].dn.x, faces[i].dn.y, faces[i].dn.z);
				glVertex3f(faces[i].d.x , faces[i].d.y , faces[i].d.z);

				glEnd();
			}
		}

	}
};

Object3D bulding, carp, carv, gas_station, egyptM ,cairo_tower,pers_house,ka_statue,AA,S,T,AD_AAST1,AD_AAST2,AD_AAST3;

void reshapeFunc(int w, int h)
{

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	
	gluPerspective(45.0f, (float)w / h, 1, 5000); 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


}

void DrawFunction()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(CameraX, CameraY, CameraZ, CameraX + HeadingX, CameraY +HeadingY, CameraZ + HeadingZ, 0, 1, 0);

	
	glPushMatrix(); // sun-moon
	glDisable( GL_TEXTURE_2D );
	glTranslatef(sunX,sunY,sunZ);
	glColor3f(RS,GS,BS);
	glutSolidSphere(50,36,18);
	glColor3f(1,1,1);
	glPopMatrix();
	glEnable( GL_TEXTURE_2D );
    //grass
	glBindTexture(GL_TEXTURE_2D, Texturesused[0]);
	glCallList(3);


	glPushMatrix(); //Ad_aast1.1
		
		glScalef(0.5,0.5,0.5);
		glTranslatef(950, 7,-2000);
		glRotatef(45,0,1,0);
		glCallList(16);
		glPopMatrix();
		glPushMatrix(); //Ad_aast1.2
		
		glScalef(0.5,0.5,0.5);
		glTranslatef(1450, 7,-2000);
		glRotatef(-45,0,1,0);
		glCallList(16);
		glPopMatrix();
		glPushMatrix(); //Ad_aast2.1
		
		glScalef(0.5,0.5,0.5);
		glTranslatef(950, 7,-500);
		glRotatef(45,0,1,0);
		glCallList(17);
		glPopMatrix();
		glPushMatrix(); //Ad_aast2.2
		
		glScalef(0.5,0.5,0.5);
		glTranslatef(1450, 7,-500);
		glRotatef(-45,0,1,0);
		glCallList(17);
		glPopMatrix();
		glPushMatrix(); //Ad_aast3.1
		
		glScalef(0.5,0.5,0.5);
		glTranslatef(950, 7,1000);
		glRotatef(45,0,1,0);
		glCallList(16);
		glPopMatrix();
		glPushMatrix(); //Ad_aast3.2
		
		glScalef(0.5,0.5,0.5);
		glTranslatef(1450, 7,1000);
		glRotatef(-45,0,1,0);
		glCallList(17);
		glPopMatrix();
		glPushMatrix();//A    
		glScalef(15,15,15);
		glRotatef(90,0,1,0);
		glTranslatef(80, 0,-80);
		glCallList(13);
		glPopMatrix();
			glPushMatrix(); //A2
		
		glScalef(15,15,15);
		glRotatef(90,0,1,0);
		glTranslatef(105, 0,-80);
		glCallList(13);
		glPopMatrix();
			glPushMatrix(); //S
		glScalef(15,15,15);
		glRotatef(90,0,1,0);
		glTranslatef(130, 0,-80);
		glCallList(14);
		glPopMatrix();
			glPushMatrix(); //T
		glScalef(15,15,15);
		glRotatef(90,0,1,0);
		glTranslatef(155, 0,-80);
		glCallList(15);
		glPopMatrix();

		glPushMatrix(); //street
		glBindTexture(GL_TEXTURE_2D, Texturesused[1]); 
		glTranslatef(600, 0,0);
		glCallList(2);
		glPopMatrix();
		glPushMatrix(); //buildings 
		glScalef(0.05, 0.05, 0.05);
		glTranslatef(18000,3250,-1000);
		glCallList(1);
		glPopMatrix();
		glPushMatrix(); //buildings2 
		glScalef(0.05, 0.05, 0.05);
		glTranslatef(-17000,3250,-1000);
		glCallList(1);
		glPopMatrix();
		glPushMatrix(); // pers_house
		glScalef(20, 20, 20);
		glRotatef(180, 0, 1, 0);
		glTranslatef(-8,4.1,-38);
		glCallList(11);
		glPopMatrix();
		glPushMatrix(); //EGYPTM ka_statue
		glScalef(1, 1, 1);
		glRotatef(90, 0, 1, 0);
		glTranslatef(-1300,3, 400);
		glCallList(9);
		glPopMatrix();
		glPushMatrix(); // ka_statue
		glScalef(0.5, 0.5, 0.5);
		glTranslatef(-450,100, -1340);
		glRotatef(-135, 0, 1, 0);
		glCallList(12);
		glPopMatrix();
		glPushMatrix(); // ka_statue
		glScalef(0.5, 0.5, 0.5);
		glTranslatef(-650,100, -850);
		glRotatef(-135, 0, 1, 0);
		glCallList(12);
		glPopMatrix();
		glPushMatrix(); //cairo tower
		glScalef(0.03, 0.03, 0.03);
		glTranslatef(-10000,0,-19000);
		glCallList(7);
		glPopMatrix();
		glPushMatrix(); //car Porsche_911
		glScalef(15, 15, 15);
		//glTranslatef(40,0.6,81);
		glTranslatef(movecarX,movecarY,movecarZ);
		glCallList(4);
		glPopMatrix();
		glPushMatrix(); //car vw beetle
		glTranslatef(280,0,-700);
		glScalef(0.5, 0.5, 0.5);
		glRotatef(270.0,0,1,0);
		
		glCallList(6);
		glPopMatrix();
		glPushMatrix(); //gas_staion
		glScalef(20, 20, 20);
		glTranslatef(20,-0,-45);
		glRotatef(90,0,1,0);
		glCallList(5);
		glPopMatrix();

		glPushMatrix(); //raseef
		glBindTexture(GL_TEXTURE_2D, Texturesused[2]);
		glTranslatef(600, 0, 0);
		glCallList(8);
		glPopMatrix();
		glPushMatrix(); //raseef
		glBindTexture(GL_TEXTURE_2D, Texturesused[2]);
		glTranslatef(350, 0, 0);
		glCallList(8);
		glPopMatrix();
		if(cloudoff==1)
		{
		cloudEngine.render();
		}
		glutSwapBuffers();
}
void myTimer(int t)
{
	glutPostRedisplay();
	manageAsyncKeyPresses();
	cloudEngine.update();
	glClearColor(R,G,B,A);
		sunX -= 5;
		sunY = (-0.0009 * sunX * sunX) + 1000;
	if (sunX >= 1055)
	{
		R = 0;
		G = 0;
		B = 0;
		RS = 0.8;
		GS = 0.33;
		BS = 0;
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHTING);
	}
	else if (sunX < 1055 && sunX>0)
	{
		R += 0.00002;
		G += 0.0002;
		B += 0.0035;
		GS += 0.00035;
		BS += 0.0002;
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
	}
	else if (sunX <= -50 && sunX >= -1128)
	{
		R -= 0.00002;
		G -= 0.0002;
		B -= 0.0035;
		GS -= 0.00035;
		BS -= 0.0002;
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
	}
	else if (sunX <= -1128 && sunY<=-145)
	{
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHTING);
	}
	glutTimerFunc(16, myTimer, 16);
}
void drawlist()
{
	glNewList(16, GL_COMPILE); //AD_AAST1
	AD_AAST1.render(2);
	glEndList();
	glNewList(17, GL_COMPILE); //AD_AAST2
	AD_AAST2.render(2);
	glEndList();
	//glNewList(18, GL_COMPILE); //AD_AAST3
	//	AD_AAST2.render(2);
	//glEndList();
	glNewList(13, GL_COMPILE); //A 
	AA.render(2);
	glEndList();
	glNewList(14, GL_COMPILE); //S 
	S.render(2);
	glEndList();
	glNewList(15, GL_COMPILE); //T 
	T.render(2);
	glEndList();
	glNewList(1, GL_COMPILE); //building
	bulding.render(1);
	glEndList();
	glNewList(11, GL_COMPILE); //pers_house
	pers_house.render(2);
	glEndList();
	glNewList(9, GL_COMPILE); //egyptM
	egyptM.render(2);
	glEndList();
	glNewList(12, GL_COMPILE); //ka_statue
	ka_statue.render(2);
	glEndList();
	glNewList(4, GL_COMPILE); //car Porsche_911
	carp.render(2);
	glEndList();
	glNewList(6, GL_COMPILE); //carvw beetle
	carv.render(2);
	glEndList();
	glNewList(7, GL_COMPILE); //cairo tower
	cairo_tower.render(3);
	glEndList();
	glNewList(5, GL_COMPILE); //gas_staion
	gas_station.render(2);
	glEndList();
	glNewList(2, GL_COMPILE); //street
	for(int streetZ = 0; streetZ < 250*10; streetZ+=250)
	{
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3f(-100, 0.15, 1250-streetZ);
		glTexCoord2d(1, 0); glVertex3f(100, 0.15, 1250-streetZ);
		glTexCoord2d(1, 1); glVertex3f(100, 0.15, 1000-streetZ);
		glTexCoord2d(0, 1); glVertex3f(-100, 0.15, 1000-streetZ);
		glEnd();
	}
	glEndList();
	
	glNewList(3, GL_COMPILE); //grass
		for(int gz = 0; gz<=99*25; gz+=25)
		for(int gx = 0; gx<=80*25; gx+=25)
		{
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex3f(-1000+gx, -0.12, 1250-gz);
			glTexCoord2d(1, 0); glVertex3f(-975+gx, -0.12, 1250-gz);
			glTexCoord2d(1, 1); glVertex3f(-975+gx, -0.12, 1225-gz);
			glTexCoord2d(0, 1); glVertex3f(-1000+gx, -0.12, 1225-gz);
			glEnd();

		}

	glEndList();

	glNewList(8, GL_COMPILE); //raseef
	for (int gz = 0; gz <= 99 * 25; gz += 25)
		for (int gx = 0; gx <= 80 * 25; gx += 25)
		{
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex3f(100, 2.5, 1250 - gz);
			glTexCoord2d(1, 0); glVertex3f(150 , 2.5, 1250 - gz);
			glTexCoord2d(1, 1); glVertex3f(150, 2.5, 1225 - gz);
			glTexCoord2d(0, 1); glVertex3f(100 , 2.5, 1225-gz);
			glEnd();
			glPushMatrix();
			glTranslatef(100, -5, 0);
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex3f(0, 5, 1250 - gz);
			glTexCoord2d(1, 0); glVertex3f(0, 7.5, 1250 - gz);
			glTexCoord2d(1, 1); glVertex3f(0, 7.5, 1225 - gz);
			glTexCoord2d(0, 1); glVertex3f(0, 5, 1225 - gz);
			glEnd();
			glPopMatrix();
			glPushMatrix();
			glTranslatef(100, -5, 0);
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex3f(50, 5, 1250 - gz);
			glTexCoord2d(1, 0); glVertex3f(50, 7.5, 1250 - gz);
			glTexCoord2d(1, 1); glVertex3f(50, 7.5, 1225 - gz);
			glTexCoord2d(0, 1); glVertex3f(50, 5, 1225 - gz);
			glEnd();
			glPopMatrix();
			glPushMatrix();
			glRotatef(90,0,1, 0);
			glTranslatef(-1400, 0, -1125);
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex3f(150, 0, 1275 );
			glTexCoord2d(1, 0); glVertex3f(150, 2.5, 1275);
			glTexCoord2d(1, 1); glVertex3f(150, 2.5, 1225 );
			glTexCoord2d(0, 1); glVertex3f(150, 0, 1225 );
			glEnd();
			glPopMatrix();

			glPushMatrix();
			glRotatef(90, 0, 1, 0);
			glTranslatef(1100, 0, -1125);
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex3f(150, 0, 1275);
			glTexCoord2d(1, 0); glVertex3f(150, 2.5, 1275);
			glTexCoord2d(1, 1); glVertex3f(150, 2.5, 1225);
			glTexCoord2d(0, 1); glVertex3f(150, 0, 1225);
			glEnd();
			glPopMatrix();
		}

	glEndList();
}


int main(int argc,char* argv[])
{
	glutInit(&argc, argv);    //glutInit will initialize the GLUT library and negotiate a session with the window system
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glShadeModel( GL_SMOOTH );
	glutCreateWindow("GTA Balady");
	
	glutFullScreen();
	
	glutDisplayFunc(DrawFunction);
	glutReshapeFunc(reshapeFunc);
	glutTimerFunc(16, myTimer, 16);
	glEnable(GL_DEPTH_TEST);
	glShadeModel( GL_SMOOTH );
	glEnable(GL_TEXTURE_2D);	//Use textures
	strcpy_s(TexturesLocation[0], "grass4.jpg");
	strcpy_s(TexturesLocation[1], "street0.jpg");
	strcpy_s(TexturesLocation[3], "brown.jpg");
	strcpy_s(TexturesLocation[2], "sidewalk.jpg");
	for (int i = 0; i < 5; i++)
		Texturesused[i] = loadTexture(TexturesLocation[i]);
	cloudEngine.initialize();
	bulding.loadObject("low poly buildings2.obj");
	carp.loadObject("Porsche_911_GT2.obj");
	carv.loadObject("vw beetle2.obj");
	gas_station.loadObject("GASOLINE STATION obj.obj");
	egyptM.loadObject("egyptM.obj");
	cairo_tower.loadObject("Cairo Tower1.obj");
	pers_house.loadObject("new_house.obj");
	ka_statue.loadObject("kastatue.obj");
	AA.loadObject("A.obj");
	S.loadObject("S.obj");
	T.loadObject("T.obj");
	AD_AAST1.loadObject("AD AAST1.obj");
	AD_AAST2.loadObject("AD AAST2.obj");
	drawlist();
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glutMainLoop();


	return 0;
}