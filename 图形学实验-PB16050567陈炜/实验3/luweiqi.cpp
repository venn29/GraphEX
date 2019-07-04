// luweiqi.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
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
GLfloat xs, ys, zs, xref, yref, zref, Vx, Vy, Vz;		//LookAt参数
GLfloat xwMin, xwMax, ywMin, ywMax, dnear, dfar;		//正交投影参数
GLfloat	angle, aspect, Pnear, Pfar;						//透视投影参数
bool  pers;		//透视与否，默认不透视而是正交
float scalex, scaley, scalez;
float translatex, translatey, translatez;
float rotateA, rotatex, rotatey, rotatez;

				//文件读取相关数据结构
int Ntexturefile;		//纹理文件数量
int Nmaterial;		//材质数据数量
int Npoints;		//顶点数量
int Nchartlet;		//贴图坐标数量
int Nnormal;		//法线数量
int Nmodel;			//模型数量

string* texturefile;			//纹理文件名字

int func;		//功能选择,0点图，1线图，2渲染图

struct Material				//材质
{
	GLfloat ambient[4];		//环境光
	GLfloat diffuse[4];		//漫反射
	GLfloat specular[4];		//镜面反射
	GLfloat emission[4];		//辐射
	GLfloat shiness;			//光斑
	GLint index;				//纹理文件索引
};
Material* materials;

struct Point			//顶点索引表
{
	float p[3];
};
Point* points;

struct Chatlet		//贴图
{
	float c[2];
};
Chatlet* chatlets;

struct Normal		//法线
{
	float n[3];
};
Normal* normals;

float scale[3];		//缩放系数

struct Triangle		//三角形
{
	int  t[9];
};
struct Model
{
	int Ntriangle;		//三角形数量
	Triangle* T; 		//三角形数组
	int index;	//材质索引

};
Model* models;
int* texture;	//各个纹理图

void ReadFile()		//将文件读取
{
	string Buffer;		//缓冲区
	string fname = "luweiqi.txt";
	ifstream fin(fname);
	if (!fin)
	{
		printf("error:无法打开文件\n");
		exit(0);
	}
	//纹理文件
	fin >> Buffer;
	Ntexturefile = atoi(Buffer.c_str());		//读取纹理文件数量
	texturefile = new string[Ntexturefile];
	texture = new int[Ntexturefile];
	for (int i = 0; i < Ntexturefile; i++)
	{
		fin >> Buffer;
		texturefile[i] = Buffer;
		texture[i] = 0;
	}
	//材质（对光的反射）
	fin >> Buffer;
	Nmaterial = atoi(Buffer.c_str());		//读取材质数量
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
	//注意，将索引index的值减了1，为了方便

	//顶点:
	fin >> Buffer;
	Npoints = atoi(Buffer.c_str());		//读取顶点数量
	points = new Point[Npoints];
	for (int i = 0; i < Npoints; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			fin >> Buffer;
			points[i].p[j] = atof(Buffer.c_str());
		}
	}

	//贴图坐标
	fin >> Buffer;
	Nchartlet = atoi(Buffer.c_str());		//读取贴图坐标数量
	chatlets = new Chatlet[Nchartlet];
	for (int i = 0; i < Nchartlet; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			fin >> Buffer;
			chatlets[i].c[j] = atof(Buffer.c_str());
		}
	}

	//法线：
	fin >> Buffer;
	Nnormal = atoi(Buffer.c_str());		//读取法线数量
	normals = new Normal[Nnormal];
	for (int i = 0; i < Nnormal; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			fin >> Buffer;
			normals[i].n[j] = atof(Buffer.c_str());
		}
	}

	//模型:
	fin >> Buffer;
	Nmodel = atoi(Buffer.c_str());		//读取法线数量
	models = new Model[Nmodel];

	//缩放系数
	for (int i = 0; i < 3; i++)
	{
		fin >> Buffer;
		scale[i] = atof(Buffer.c_str());
	}
	//具体模型参数
	for (int i = 0; i < Nmodel; i++)
	{
		fin >> Buffer;
		models[i].Ntriangle = atoi(Buffer.c_str());
		models[i].T = new Triangle[models[i].Ntriangle];
		fin >> Buffer;
		models[i].index = atoi(Buffer.c_str()) - 1;		//同样的，索引-1，方便使用
		for (int j = 0; j < models[i].Ntriangle; j++)		//读取每一个三角形的参数
		{
			for (int k = 0; k < 9; k++)
			{
				fin >> Buffer;
				models[i].T[j].t[k] = atoi(Buffer.c_str());
			}
		}
	}

	//读取完毕
}

void Init(void)//初始化函数 读文件
{
	ReadFile();
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);	//启用2D纹理（纹理坐标就是二维的）
	glEnable(GL_DEPTH_TEST);	//绘制不透明的图像，需要
								//设置光源，开启八个光源中的GL_LIGHT0
	float AmbientColor[] = { 0.2f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);		//设置环境光颜色（三色光的成分）

	float DiffuseColor[] = { 0.2f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);		//设置漫射光颜色

	float SpecularColor[] = { 0.2f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);	//设置镜面光颜色

	float Position[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, Position);		//光源位置

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);		//开启光照系统

}

void InitNoLight(void)//初始化函数 读文件
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
	glLoadIdentity(); if (!pers)	//正交投影
		glOrtho(xwMin, xwMax, ywMin, ywMax, dnear, dfar);
	else
		gluPerspective(angle, aspect, Pnear, Pfar);

	int index;		//当前使用的纹理文件的索引
	for (int i = 0; i < Nmodel; i++)		//分模型绘制，每一个模型有不同的材质
	{
		//设置材质
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materials[models[i].index].ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materials[models[i].index].diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materials[models[i].index].specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, materials[models[i].index].emission);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materials[models[i].index].shiness);
		//加载纹理
		index = materials[models[i].index].index;
		texture[i] = SOIL_load_OGL_texture(texturefile[index].c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
		if (!texture[i])
		{
			printf("cannot load the image\n");
			exit(0);
		}
		
		glBindTexture(GL_TEXTURE_2D, texture[i]);	//texture2D绑定到textre[i]，即是选择了这个纹理
													//设定从纹理坐标映射到帧缓存位置的方式，参见https://blog.csdn.net/wangdingqiaoit/article/details/51457675

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filter参数，处理映射到坐标非对齐的情况，Linear，使用附近的纹素加权
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 如果是near的话，会选择最近的坐标直接使用
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	//wrap参数，处理当纹理坐标超出范围得到情况，REPEAT,直接忽略
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);//环境参数,如何结合纹理值和原有颜色？REPLACE直接替代

																   //绘制该模型中每一个三角形
		glBegin(GL_TRIANGLES);
		for (size_t j = 0; j < models[i].Ntriangle; j++)		//必须是先法向，再纹理，最后三角，否则画出来很奇怪

		{
			//点1
			glNormal3fv(normals[models[i].T[j].t[2] - 1].n);			//法线（默认索引-1），从0开始
			glTexCoord2f(chatlets[models[i].T[j].t[1] - 1].c[0], 1 - chatlets[models[i].T[j].t[1] - 1].c[1]);		//纹理索引
			glVertex3fv(points[models[i].T[j].t[0] - 1].p);
			
			//点2
			glNormal3fv(normals[models[i].T[j].t[5] - 1].n);			//法线（默认索引-1），从0开始
			glTexCoord2f(chatlets[models[i].T[j].t[4] - 1].c[0], 1 - chatlets[models[i].T[j].t[4] - 1].c[1]);		//纹理索引
			glVertex3fv(points[models[i].T[j].t[3] - 1].p);
																		
			//点3
			glNormal3fv(normals[models[i].T[j].t[8] - 1].n);			//法线（默认索引-1），从0开始
			glTexCoord2f(chatlets[models[i].T[j].t[7] - 1].c[0], 1 - chatlets[models[i].T[j].t[7] - 1].c[1]);		//纹理索引
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
	if (!pers)	//正交投影
		glOrtho(xwMin, xwMax, ywMin, ywMax, dnear, dfar);
	else
		gluPerspective(angle, aspect, Pnear, Pfar);
	for (int i = 0; i < Nmodel; i++)		//分模型绘制，每一个模型有不同的材质
	{															   //绘制该模型中每一个三角形
		glBegin(GL_LINES);
		for (size_t j = 0; j < models[i].Ntriangle; j++)		//必须是先法向，再纹理，最后三角，否则画出来很奇怪

		{
			//点1
			glVertex3fv(points[models[i].T[j].t[0] - 1].p);

			//点2
			glVertex3fv(points[models[i].T[j].t[3] - 1].p);

			//点3
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
	glLoadIdentity(); if (!pers)	//正交投影
		glOrtho(xwMin, xwMax, ywMin, ywMax, dnear, dfar);
	else
		gluPerspective(angle, aspect, Pnear, Pfar);
	for (int i = 0; i < Nmodel; i++)		//分模型绘制，每一个模型有不同的材质
	{															   //绘制该模型中每一个三角形
		glBegin(GL_POINTS);
		for (size_t j = 0; j < models[i].Ntriangle; j++)		//必须是先法向，再纹理，最后三角，否则画出来很奇怪

		{
			//点1
			glVertex3fv(points[models[i].T[j].t[0] - 1].p);

			//点2
			glVertex3fv(points[models[i].T[j].t[3] - 1].p);

			//点3
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
		printf("当前LookAt参数是：xs:%f,ys:%f,zs:%f,xref:%f,yref:%f,zref:%f,Vx:%f,Vy:%f,Vz:%f\n", xs, ys, zs, xref, yref, zref, Vx, Vy, Vz);
		printf("请输入LookAt参数，xs ys zs xref  yref  zref, Vx  Vy  Vz\n");
		scanf("%f %f %f%f%f%f%f%f%f", &xs, &ys, &zs, &xref, &yref, &zref, &Vx, &Vy, &Vz);
	}
	if (value == 'p')
	{
		printf("当前perspective参数是：fovy:%f,aspect:%f,near:%f,far:%f\n", angle, aspect, Pnear, Pfar);
		printf("请输入Perspective参数，fovy aspect near far \n");
		scanf("%f %f %f %f", &angle, &aspect, &Pnear, &Pfar);
	}
	if (value == 'o')
	{
		printf("当前glOthor参数是：xwMin:%f,xwMax:%f,ywMin:%f,ywMax:%f,near:%f,far:%f\n", xwMin, xwMax, ywMin, ywMax, dnear, dfar);
		printf("请输入glOthor正交投影参数，xwMin xwMax ywMin ywMax dnear dfar\n");
		scanf("%f %f %f%f%f%f%f%f%f", &xwMin, &xwMax, &ywMin, &ywMax, &dnear, &dfar);
	}
	if (value == 'T')	//translate
	{
		printf("目前的translate参数为:x:%f,y:%f,z:%f\n", translatex, translatey, translatez);
		printf("请在命令行输入新的translate参数(x,y,z)\n");
		scanf("%f %f %f", &translatex, &translatey, &translatez);
	}
	if (value == 'S')	//scale
	{
		printf("目前的scale参数为:x:%f,y:%f,z:%f\n", scalex, scaley, scalez);
		printf("请在命令行输入新的scale参数(x,y,z)\n");
		scanf("%f %f %f", &scalex, &scaley, &scalez);
	}
	if (value == 'R')
	{
		printf("目前的rotate参数为:angle:%f, x:%f, y:%f, z:%f\n", rotateA, rotatex, rotatey, rotatez);
		printf("请在命令行输入新的rotate参数(Angle,x,y,z)\n");
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
	printf("请选择图类型：\n0:点图\n1：线框图\n2、渲染图\n");
	scanf("%d", &func);
	pers = false;
	//观察参数
	xs = 0;
	ys = 0;
	zs = 0;
	xref = 1;
	yref = 1;
	zref = 0;
	Vx = 0;
	Vy = 0;
	Vz = 1;
	//正交投影参数
	xwMin = -2;
	xwMax = 2;
	ywMin = -2;
	ywMax = 2;
	dnear = 0;
	dfar = 12;

	//透视投影参数
	angle = 120;
	aspect = 1;
	Pnear = 0.1;
	Pfar = 5;
	//几何变换参数初始化
	scalex = scaley = scalez = 1;
	translatex = translatey = translatez = 0;
	rotateA = rotatex = rotatey = rotatez = 0;
	//初始化
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(0, 0);//窗口位置
	glutInitWindowSize(800, 800);//窗口的大小
	glutCreateWindow("luweiqi");//窗口名
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
// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
