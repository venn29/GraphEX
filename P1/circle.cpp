#include <stdlib.h>
#include <GL/glut.h>
#include<stdio.h>

GLint xs, ys;
GLint r;

void Paint(int x,int y)
{
	glBegin(GL_POINTS);
	glVertex2d(x, y);
	glVertex2d(-x, y);
	glVertex2d(x, -y);
	glVertex2d(-x, -y);
	glVertex2d(y, x);
	glVertex2d(-y, x);
	glVertex2d(y, -x);
	glVertex2d(-y, -x);
	glEnd();
	glFlush();
}

void init()
{

	glClearColor(1.0, 1.0, 1.0, 0.0);
	

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(-400, 400, -300, 300);//指定使用正投影将一个x坐标在-400~400，y坐标-300~300范围内的矩形坐标区域投影到显示器窗口
	glTranslatef(xs, ys, 0);
}
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 1.0, 0.0);
	int x, y;
	x = 0;
	y = r;
	int p;
	p = 1 - r;
	Paint(x, y);
	while (x < y)
	{
		x++;
		if (p >= 0)
		{
			y--;
			p += 2 * x - 2 * y + 1;
		}
		else
		{
			p += 2 * x + 1;
		}
		Paint(x, y);
	}
	


}



void main(int argc, char **argv)
{
	printf("请输入圆心坐标:\n");
	scanf_s("%d %d", &xs, &ys);
	printf("请输入半径\n");
	scanf_s("%d", &r);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(0, 0);//窗口位置
	glutInitWindowSize(800, 600);
	glutCreateWindow("circle");
	init();
	glutDisplayFunc(&display);
	//glutKeyboardFunc(keys);
	glutMainLoop();
}