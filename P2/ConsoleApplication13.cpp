﻿// ConsoleApplication13.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
// 图形学HW3
//分辨率800*600


#include <stdio.h>
#include<gl/GLUT.h>
#include<math.h>

float x, y, z;
float xs, ys,zs, xt, yt,zt;
float ang;
int func;
GLfloat v[4][3] = { {0.0, 0.0, 1.0},
{0.0, 0.942809, -0.33333},
{-0.816497, -0.471405, -0.333333},
{0.816497, -0.471405, -0.333333} };
GLfloat colors[4][3] = { {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0},
{0.0, 0.0, 1.0}, {0.0, 0.0, 0.0} };

void triangle(GLfloat *va, GLfloat *vb, GLfloat *vc)
{ //画三角形
	glVertex3fv(va);
	glVertex3fv(vb);
	glVertex3fv(vc);
}
void tetra(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d)
{ //画四面体
	glColor3fv(colors[0]);
	triangle(a, b, c);
	glColor3fv(colors[1]);
	triangle(a, c, d);
	glColor3fv(colors[2]);
	triangle(a, d, b);
	glColor3fv(colors[3]);
	triangle(b, d, c);
}

void divide_tetra(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d, int m)
{
	GLfloat mid[6][3];
	int j;
	if (m > 0)
	{ /* compute six midpoints */
		for (j = 0; j < 3; j++) mid[0][j] = (a[j] + b[j]) / 2;
		for (j = 0; j < 3; j++) mid[1][j] = (a[j] + c[j]) / 2;
		for (j = 0; j < 3; j++) mid[2][j] = (a[j] + d[j]) / 2;
		for (j = 0; j < 3; j++) mid[3][j] = (b[j] + c[j]) / 2;
		for (j = 0; j < 3; j++) mid[4][j] = (c[j] + d[j]) / 2;
		for (j = 0; j < 3; j++) mid[5][j] = (b[j] + d[j]) / 2;
		/* create 4 tetrahedrons by subdivision */
		divide_tetra(a, mid[0], mid[1], mid[2], m - 1);
		divide_tetra(mid[0], b, mid[3], mid[5], m - 1);
		divide_tetra(mid[1], mid[3], c, mid[4], m - 1);
		divide_tetra(mid[2], mid[4], d, mid[5], m - 1);
	}
	else(tetra(a, b, c, d)); /* draw tetrahedron at end of recursion */
}

void Init(void)//初始化函数 
{
	glClearColor(2.0, 2.0, 2.0, 1.0);
	glMatrixMode(GL_PROJECTION);//OpenGL按照三维方式来处理图像，所以需要一个投影变换将三维图形投影到显示器的二维空间中
	glOrtho(-4, 4, -4, 4, -4, 4);//指定使用正投影将一个x坐标在-400~400，y坐标-300~300范围内的矩形坐标区域投影到显示器窗口
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0 ,2,0, 0, 0, 0, 0,0,1);
	if (func == 1)
	{
		glTranslatef(x, y, z);
	}
	else if (func == 2)
	{
		glScalef(x, y, z);
	}
	else if (func == 3)
	{
		glTranslatef(-xs, -ys,-zs);
		glRotatef(ang, xt - xs, yt - ys, zt - zs);

	}
//	gluPerspective(1, 0, 1, 3);
	return;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT
		| GL_DEPTH_BUFFER_BIT);
	
	glBegin(GL_TRIANGLES);
	divide_tetra(v[0], v[1], v[2], v[3],1);
	glEnd();
	glFlush();
}

int main(int argc, char * argv[])
{
	printf("观察者坐标为(0,2,0)\n请选择功能：1、平移\n2、缩放\n3、旋转\n0、不做处理\n");
	scanf_s("%d", &func);
	if (func == 1)
	{
		printf("请输入平移向量 x y z \n");
		scanf_s("%f %f %f", &x, &y, &z);
	}
	else if (func == 2)
	{
		printf("请输入三个维度的缩放因子 x y z \n");
		scanf_s("%f %f %f", &x, &y, &z);
	}
	else if (func == 3)
	{
		printf("请输入旋转轴的两个点x1 y1 z1 x2 y2 z2\n");
		scanf_s("%f %f %f %f %f %f", &xs, &ys,&zs, &xt, &yt,&zt);
		printf("请输入旋转的角度(0~360)：\n");
		scanf_s("%f", &ang);

	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(0, 0);//窗口位置
	glutInitWindowSize(800, 600);//窗口的大小
	glutCreateWindow("Sierpinski");//窗口名
	Init();
	glutDisplayFunc(&display);
	glutMainLoop();
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
