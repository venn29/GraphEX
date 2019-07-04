// luweiqi.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
//



#include <stdio.h>
#include<GL/glut.h>
#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include <SOIL.h>
#pragma warning(disable:4996)
using namespace std;
GLfloat p[4][3];
GLfloat xs, ys, zs, xref, yref, zref, Vx, Vy, Vz;		//LookAt����
GLfloat xwMin, xwMax, ywMin, ywMax, dnear, dfar;		//����ͶӰ����
GLfloat	angle, aspect, Pnear, Pfar;						//͸��ͶӰ����
bool  pers;		//͸�����Ĭ�ϲ�͸�Ӷ�������
float scalex, scaley, scalez;
float translatex, translatey, translatez;
float rotateA, rotatex, rotatey, rotatez;

				//�ļ���ȡ������ݽṹ
int Ntexturefile;		//�����ļ�����
int Nmaterial;		//������������
int Npoints;		//��������
int Nchartlet;		//��ͼ��������
int Nnormal;		//��������
int Nmodel;			//ģ������

string* texturefile;			//�����ļ�����

int func;		//����ѡ��,0��ͼ��1��ͼ��2��Ⱦͼ

struct Material				//����
{
	GLfloat ambient[4];		//������
	GLfloat diffuse[4];		//������
	GLfloat specular[4];		//���淴��
	GLfloat emission[4];		//����
	GLfloat shiness;			//���
	GLint index;				//�����ļ�����
};
Material* materials;

struct Point			//����������
{
	float p[3];
};
Point* points;

struct Chatlet		//��ͼ
{
	float c[2];
};
Chatlet* chatlets;

struct Normal		//����
{
	float n[3];
};
Normal* normals;

float scale[3];		//����ϵ��

struct Triangle		//������
{
	int  t[9];
};
struct Model
{
	int Ntriangle;		//����������
	Triangle* T; 		//����������
	int index;	//��������

};
Model* models;
int* texture;	//��������ͼ

void ReadFile()		//���ļ���ȡ
{
	string Buffer;		//������
	string fname = "luweiqi.txt";
	ifstream fin(fname);
	if (!fin)
	{
		printf("error:�޷����ļ�\n");
		exit(0);
	}
	//�����ļ�
	fin >> Buffer;
	Ntexturefile = atoi(Buffer.c_str());		//��ȡ�����ļ�����
	texturefile = new string[Ntexturefile];
	texture = new int[Ntexturefile];
	for (int i = 0; i < Ntexturefile; i++)
	{
		fin >> Buffer;
		texturefile[i] = Buffer;
		texture[i] = 0;
	}
	//���ʣ��Թ�ķ��䣩
	fin >> Buffer;
	Nmaterial = atoi(Buffer.c_str());		//��ȡ��������
	materials = new Material[Nmaterial];
	for (int i = 0; i < Nmaterial; i++)
	{
		for (int j = 0; j < 4; j++) {
			fin >> Buffer;
			materials[i].ambient[j] = atof(Buffer.c_str());
		}
		for (int j = 0; j < 4; j++) {
			fin >> Buffer;
			materials[i].diffuse[j] = atof(Buffer.c_str());
		}
		for (int j = 0; j < 4; j++) {
			fin >> Buffer;
			materials[i].specular[j] = atof(Buffer.c_str());;
		}
		for (int j = 0; j < 4; j++) {
			fin >> Buffer;
			materials[i].emission[j] = atof(Buffer.c_str());
		}
		fin >> Buffer;
		materials[i].shiness = atof(Buffer.c_str());
		fin >> Buffer;
		materials[i].index = atoi(Buffer.c_str()) - 1;
	}
	//ע�⣬������index��ֵ����1��Ϊ�˷���

	//����:
	fin >> Buffer;
	Npoints = atoi(Buffer.c_str());		//��ȡ��������
	points = new Point[Npoints];
	for (int i = 0; i < Npoints; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			fin >> Buffer;
			points[i].p[j] = atof(Buffer.c_str());
		}
	}

	//��ͼ����
	fin >> Buffer;
	Nchartlet = atoi(Buffer.c_str());		//��ȡ��ͼ��������
	chatlets = new Chatlet[Nchartlet];
	for (int i = 0; i < Nchartlet; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			fin >> Buffer;
			chatlets[i].c[j] = atof(Buffer.c_str());
		}
	}

	//���ߣ�
	fin >> Buffer;
	Nnormal = atoi(Buffer.c_str());		//��ȡ��������
	normals = new Normal[Nnormal];
	for (int i = 0; i < Nnormal; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			fin >> Buffer;
			normals[i].n[j] = atof(Buffer.c_str());
		}
	}

	//ģ��:
	fin >> Buffer;
	Nmodel = atoi(Buffer.c_str());		//��ȡ��������
	models = new Model[Nmodel];

	//����ϵ��
	for (int i = 0; i < 3; i++)
	{
		fin >> Buffer;
		scale[i] = atof(Buffer.c_str());
	}
	//����ģ�Ͳ���
	for (int i = 0; i < Nmodel; i++)
	{
		fin >> Buffer;
		models[i].Ntriangle = atoi(Buffer.c_str());
		models[i].T = new Triangle[models[i].Ntriangle];
		fin >> Buffer;
		models[i].index = atoi(Buffer.c_str()) - 1;		//ͬ���ģ�����-1������ʹ��
		for (int j = 0; j < models[i].Ntriangle; j++)		//��ȡÿһ�������εĲ���
		{
			for (int k = 0; k < 9; k++)
			{
				fin >> Buffer;
				models[i].T[j].t[k] = atoi(Buffer.c_str());
			}
		}
	}

	//��ȡ���
}

void Init(void)//��ʼ������ ���ļ�
{
	ReadFile();
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);	//����2D��������������Ƕ�ά�ģ�
	glEnable(GL_DEPTH_TEST);	//���Ʋ�͸����ͼ����Ҫ
								//���ù�Դ�������˸���Դ�е�GL_LIGHT0
	float AmbientColor[] = { 0.2f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);		//���û�������ɫ����ɫ��ĳɷ֣�

	float DiffuseColor[] = { 0.2f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);		//�����������ɫ

	float SpecularColor[] = { 0.2f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);	//���þ������ɫ

	float Position[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, Position);		//��Դλ��

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);		//��������ϵͳ

}

void InitNoLight(void)//��ʼ������ ���ļ�
{
	ReadFile();
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1, 1, 1);

}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(xs, ys, zs, xref, yref, zref, Vx, Vy, Vz);
	glTranslatef(translatex, translatey, translatez);
	glScalef(scalex, scaley, scalez);
	glRotatef(rotateA, rotatex, rotatey, rotatez);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); if (!pers)	//����ͶӰ
		glOrtho(xwMin, xwMax, ywMin, ywMax, dnear, dfar);
	else
		gluPerspective(angle, aspect, Pnear, Pfar);

	int index;		//��ǰʹ�õ������ļ�������
	for (int i = 0; i < Nmodel; i++)		//��ģ�ͻ��ƣ�ÿһ��ģ���в�ͬ�Ĳ���
	{
		//���ò���
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materials[models[i].index].ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materials[models[i].index].diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materials[models[i].index].specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, materials[models[i].index].emission);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materials[models[i].index].shiness);
		//��������
		index = materials[models[i].index].index;
		texture[i] = SOIL_load_OGL_texture(texturefile[index].c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
		if (!texture[i])
		{
			printf("cannot load the image\n");
			exit(0);
		}
		
		glBindTexture(GL_TEXTURE_2D, texture[i]);	//texture2D�󶨵�textre[i]������ѡ�����������
													//�趨����������ӳ�䵽֡����λ�õķ�ʽ���μ�https://blog.csdn.net/wangdingqiaoit/article/details/51457675

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filter����������ӳ�䵽����Ƕ���������Linear��ʹ�ø��������ؼ�Ȩ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // �����near�Ļ�����ѡ�����������ֱ��ʹ��
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	//wrap�����������������곬����Χ�õ������REPEAT,ֱ�Ӻ���
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);//��������,��ν������ֵ��ԭ����ɫ��REPLACEֱ�����

																   //���Ƹ�ģ����ÿһ��������
		glBegin(GL_TRIANGLES);
		for (size_t j = 0; j < models[i].Ntriangle; j++)		//�������ȷ���������������ǣ����򻭳��������

		{
			//��1
			glNormal3fv(normals[models[i].T[j].t[2] - 1].n);			//���ߣ�Ĭ������-1������0��ʼ
			glTexCoord2f(chatlets[models[i].T[j].t[1] - 1].c[0], 1 - chatlets[models[i].T[j].t[1] - 1].c[1]);		//��������
			glVertex3fv(points[models[i].T[j].t[0] - 1].p);
			
			//��2
			glNormal3fv(normals[models[i].T[j].t[5] - 1].n);			//���ߣ�Ĭ������-1������0��ʼ
			glTexCoord2f(chatlets[models[i].T[j].t[4] - 1].c[0], 1 - chatlets[models[i].T[j].t[4] - 1].c[1]);		//��������
			glVertex3fv(points[models[i].T[j].t[3] - 1].p);
																		
			//��3
			glNormal3fv(normals[models[i].T[j].t[8] - 1].n);			//���ߣ�Ĭ������-1������0��ʼ
			glTexCoord2f(chatlets[models[i].T[j].t[7] - 1].c[0], 1 - chatlets[models[i].T[j].t[7] - 1].c[1]);		//��������
			glVertex3fv(points[models[i].T[j].t[6] - 1].p);
		}
		glEnd();
	}
	glFlush();
}

void displayline()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(xs, ys, zs, xref, yref, zref, Vx, Vy, Vz);
	glTranslatef(translatex, translatey, translatez);
	glScalef(scalex, scaley, scalez);
	glRotatef(rotateA, rotatex, rotatey, rotatez);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); 
	if (!pers)	//����ͶӰ
		glOrtho(xwMin, xwMax, ywMin, ywMax, dnear, dfar);
	else
		gluPerspective(angle, aspect, Pnear, Pfar);
	for (int i = 0; i < Nmodel; i++)		//��ģ�ͻ��ƣ�ÿһ��ģ���в�ͬ�Ĳ���
	{															   //���Ƹ�ģ����ÿһ��������
		glBegin(GL_LINES);
		for (size_t j = 0; j < models[i].Ntriangle; j++)		//�������ȷ���������������ǣ����򻭳��������

		{
			//��1
			glVertex3fv(points[models[i].T[j].t[0] - 1].p);

			//��2
			glVertex3fv(points[models[i].T[j].t[3] - 1].p);

			//��3
			glVertex3fv(points[models[i].T[j].t[6] - 1].p);
		}
		glEnd();
	}
	glFlush();
}
void displaypoint()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(xs, ys, zs, xref, yref, zref, Vx, Vy, Vz);
	glTranslatef(translatex, translatey, translatez);
	glScalef(scalex, scaley, scalez);
	glRotatef(rotateA, rotatex, rotatey, rotatez);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); if (!pers)	//����ͶӰ
		glOrtho(xwMin, xwMax, ywMin, ywMax, dnear, dfar);
	else
		gluPerspective(angle, aspect, Pnear, Pfar);
	for (int i = 0; i < Nmodel; i++)		//��ģ�ͻ��ƣ�ÿһ��ģ���в�ͬ�Ĳ���
	{															   //���Ƹ�ģ����ÿһ��������
		glBegin(GL_POINTS);
		for (size_t j = 0; j < models[i].Ntriangle; j++)		//�������ȷ���������������ǣ����򻭳��������

		{
			//��1
			glVertex3fv(points[models[i].T[j].t[0] - 1].p);

			//��2
			glVertex3fv(points[models[i].T[j].t[3] - 1].p);

			//��3
			glVertex3fv(points[models[i].T[j].t[6] - 1].p);
		}
		glEnd();
	}
	glFlush();
}

void mouse(int btn, int state, int x, int y) {
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		pers = !pers;

}


void key(unsigned char value, int x, int y)
{
	if (value == 'w')
	{
		yref -= 0.1;
	}
	if (value == 's')
	{
		yref += 0.1;
	}
	if (value == 'a')
	{
		xref -= 0.1;
	}
	if (value == 'd')
	{
		xref += 0.1;
	}if (value == 'q')
	{
		zref -= 0.1;
	}
	if (value == 'e')
	{
		zref += 0.1;
	}

	if (value == '8')
	{
		angle -= 5;
	}
	if (value == '2')
	{
		angle += 5;
	}
	if (value == 'l')
	{
		printf("��ǰLookAt�����ǣ�xs:%f,ys:%f,zs:%f,xref:%f,yref:%f,zref:%f,Vx:%f,Vy:%f,Vz:%f\n", xs, ys, zs, xref, yref, zref, Vx, Vy, Vz);
		printf("������LookAt������xs ys zs xref  yref  zref, Vx  Vy  Vz\n");
		scanf("%f %f %f%f%f%f%f%f%f", &xs, &ys, &zs, &xref, &yref, &zref, &Vx, &Vy, &Vz);
	}
	if (value == 'p')
	{
		printf("��ǰperspective�����ǣ�fovy:%f,aspect:%f,near:%f,far:%f\n", angle, aspect, Pnear, Pfar);
		printf("������Perspective������fovy aspect near far \n");
		scanf("%f %f %f %f", &angle, &aspect, &Pnear, &Pfar);
	}
	if (value == 'o')
	{
		printf("��ǰglOthor�����ǣ�xwMin:%f,xwMax:%f,ywMin:%f,ywMax:%f,near:%f,far:%f\n", xwMin, xwMax, ywMin, ywMax, dnear, dfar);
		printf("������glOthor����ͶӰ������xwMin xwMax ywMin ywMax dnear dfar\n");
		scanf("%f %f %f%f%f%f%f%f%f", &xwMin, &xwMax, &ywMin, &ywMax, &dnear, &dfar);
	}
	if (value == 'T')	//translate
	{
		printf("Ŀǰ��translate����Ϊ:x:%f,y:%f,z:%f\n", translatex, translatey, translatez);
		printf("���������������µ�translate����(x,y,z)\n");
		scanf("%f %f %f", &translatex, &translatey, &translatez);
	}
	if (value == 'S')	//scale
	{
		printf("Ŀǰ��scale����Ϊ:x:%f,y:%f,z:%f\n", scalex, scaley, scalez);
		printf("���������������µ�scale����(x,y,z)\n");
		scanf("%f %f %f", &scalex, &scaley, &scalez);
	}
	if (value == 'R')
	{
		printf("Ŀǰ��rotate����Ϊ:angle:%f, x:%f, y:%f, z:%f\n", rotateA, rotatex, rotatey, rotatez);
		printf("���������������µ�rotate����(Angle,x,y,z)\n");
		scanf("%f %f %f %f", &rotateA, &rotatex, &rotatey, &rotatez);
	}
	if (func == 2)
		display();
	else if (func == 1)
		displayline();
	else
		displaypoint();
}



int main(int argc, char * argv[])
{
	printf("��ѡ��ͼ���ͣ�\n0:��ͼ\n1���߿�ͼ\n2����Ⱦͼ\n");
	scanf("%d", &func);
	pers = false;
	//�۲����
	xs = 0;
	ys = 0;
	zs = 0;
	xref = 1;
	yref = 1;
	zref = 0;
	Vx = 0;
	Vy = 0;
	Vz = 1;
	//����ͶӰ����
	xwMin = -2;
	xwMax = 2;
	ywMin = -2;
	ywMax = 2;
	dnear = 0;
	dfar = 12;

	//͸��ͶӰ����
	angle = 120;
	aspect = 1;
	Pnear = 0.1;
	Pfar = 5;
	//���α任������ʼ��
	scalex = scaley = scalez = 1;
	translatex = translatey = translatez = 0;
	rotateA = rotatex = rotatey = rotatez = 0;
	//��ʼ��
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(0, 0);//����λ��
	glutInitWindowSize(800, 800);//���ڵĴ�С
	glutCreateWindow("luweiqi");//������
	if (func == 2)
	{
		Init();
		glutDisplayFunc(&display);
	}
	else if (func == 1)
	{
		InitNoLight();
		glutDisplayFunc(&displayline);
	}
	else 
	{
		InitNoLight();
		glutDisplayFunc(&displaypoint);
	}
	glutMouseFunc(mouse);
	glutKeyboardFunc(key);
	glutMainLoop();
	return 0;
}
// ������ʾ: 
//   1. ʹ�ý��������Դ�������������/�����ļ�
//   2. ʹ���Ŷ���Դ�������������ӵ�Դ�������
//   3. ʹ��������ڲ鿴���������������Ϣ
//   4. ʹ�ô����б��ڲ鿴����
//   5. ת������Ŀ��>���������Դ����µĴ����ļ�����ת������Ŀ��>�����������Խ����д����ļ���ӵ���Ŀ
//   6. ��������Ҫ�ٴδ򿪴���Ŀ����ת�����ļ���>���򿪡�>����Ŀ����ѡ�� .sln �ļ�
