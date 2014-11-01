/*
@Author:	Akshay Vats
@E-Mail:	akshayvats@outlook.com
@Description
------------
Sample file for 'Graphics.hpp'
While using VS press CTRL+M, O [Edit>Outlining>Collapse]
You'll see two sections, 'OLD' and 'NEW'

#OLD
General functions to draw Line, Ellipse, Circle, Hyperbola

#NEW
Supports Transformation
*/
#include "Graphics.hpp"
#include <bits/stdc++.h>

#define putpixel(x,y) SetPixel(hdc,x,y,COLOR)
#define MATRIX Mat44<double>
#define POINT Vector3<int>
HDC hdc;
int COLOR = 0xffffff;

#pragma region "Old Stuff"
void lineDDA(int xa, int ya, int xb, int yb){
	int dx = xb - xa;
	int dy = yb - ya;
	int steps, k;
	float xInc, yInc, x = xa, y = ya;
	if (abs(dx) > abs(dy)) steps = abs(dx);
	else steps = abs(dy);
	xInc = (float)dx / steps;
	yInc = (float)dy / steps;
	putpixel(round(x), round(y));
	for (int k = 0; k < steps; k++)
	{
		x += xInc;
		y += yInc;
		putpixel(round(x), round(y));
	}
	std::string s;
}
#define drawQuad(x, y) putpixel(xc+x, yc+y);putpixel(xc + x, yc + -y);putpixel(xc + -x, yc + -y);putpixel(xc + -x, yc + y)
#define drawOctant(x,y) drawQuad(x,y);drawQuad(y,x)
void circle(int xc, int yc, int r){
	int x = 0, y = r;// round(r);
	int p = 1 - r;// y + (y*y - r*r);
	while (x <= y){
		drawOctant(x, y);
		x++;
		if (p < 0)p += 2 * (x + 1) + 1;
		else{
			y--;
			p += 2 * (x - y) + 1;
		}

	}
}
void Circle(int xc, int yc, int r){
	float x = 0, y = r;
	while (x <= y){
		drawOctant(x, y);
		x++;
		y = sqrt((r*r - x*x));
	}
}

void ellipse(int xc, int yc, int a, int b){
	int x = 0, y = b;
	int a2 = a*a, b2 = b*b;
	int p = b2 + a2 / 4 - b;
	int xb2 = 0, ya2 = y*a2;
	while (xb2 <= ya2){
		drawQuad(x, y);
		x++;
		xb2 += b2;
		if (p < 0)
			p += xb2 + b2;
		else{
			y--;
			ya2 -= a2;
			p += xb2 + b2 - ya2;
		}
	}
	while (y >= 0){
		drawQuad(x, y);
		y--;
		ya2 -= a2;
		if (p >= 0)
			p += a2 - ya2;
		else{
			x++;
			xb2 += b2;
			p += a2 + xb2 - ya2;
		}
	}
}
void hyperbola(int xc, int yc, int a, int b){
	int x = a, y = 0;
	int b2 = b*b, a2 = a*a;
	int p = b2 / 4 + a*b2 - a2;
	while (x*b2 > y*a2){
		drawQuad(x, y);
		y++;
		if (p > 0)
			p -= a2*(2 * y + 1);
		else{
			x++;
			p += b2 * 2 * x - a2*(2 * y + 1);
		}
	}
	while (x<9 * a){
		drawQuad(x, y);
		x++;
		if (p < 0)
			p += b2*(2 * x + 1);
		else{
			y++;
			p += b2*(2 * x) - a2*(2 * y + 1);
		}
	}
}
void Hyperbola(int xc, int yc, int a, int b){
	float x = a, y = 0;
	while (x < 9 * a){
		drawQuad(x, y);
		y = sqrt(x*x / (a*a) - 1)*b;
		x++;
	}
}
#pragma endregion

#pragma region "NEW STUFF"
template <class T>
struct Mat44{
	T m[4][4];
	Mat44(){
		memset(m, 0, sizeof(m));
		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
	}
	
	Mat44 operator *(Mat44 &b){
		Mat44 r;
		for (int i = 0; i < 4;i++)
		for (int j = 0; j < 4; j++){
			r[i][j] = 0;
			for (int k = 0; k < 4; k++)
				r[i][j] += b[i][k] * m[k][j];
		}

		return r;
	}

	static Mat44 CreateTraslation(T x, T y, T z=0){
		Mat44 r;
		//memset(r.m, 0, sizeof(r.m));
		r.m[0][3] += x;
		r.m[1][3] += y;
		r.m[2][3] += z;
		return r;
	}
	static Mat44 CreateRotation(double ax, double ay, double az){
		Mat44 r;
		Mat44 x, y, z;
		z[0][0] = z[1][1] = cos(az);
		z[0][1] = -sin(az);
		z[1][0] = -z[0][1];
		x[1][1] = x[2][2] = cos(ax);
		x[1][2] = -sin(ax);
		x[2][1] = -x[1][2];
		y[0][0] = y[2][2] = cos(ay);
		y[0][2] = -sin(ay);
		y[2][0] = -y[0][2];
		return x*y*z;
	}
	static Mat44 CreateScaling(double sx, double sy, double sz=1){
		MATRIX r;
		r[0][0] = sx;
		r[1][1] = sy;
		r[2][2] = sz;
		return r;
	}
	T* operator [](int i){
		return m[i];
	}
};

template<class T>
struct Vector3{
	T x, y, z;
	Vector3(){
		x = y = z = 0;
	}
	Vector3(T x, T y, T z=0) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Vector3<T> operator *(MATRIX& t){
		return Vector3<T>(t[0][0] * x + t[0][1] * y + t[0][2] * z + t[0][3], t[1][0] * x + t[1][1] * y + t[1][2] * z + t[1][3], t[2][0] * x + t[2][1] * y + t[2][2] * z + t[3][3]);
	}
};
void putpixel2(Vector3<float> v){
	putpixel(v.x, v.y);
}
void lineDDA(int len, MATRIX &transform){
	POINT b;
	POINT e(0, len);
	b = b*transform;
	e = e*transform;
	lineDDA(b.x, b.y, e.x, e.y);
}
void PutPixel(POINT &p){
	putpixel(p.x, p.y);
}
void DrawQuad(POINT &p, MATRIX &tr){
	PutPixel(p*tr); PutPixel(POINT(p.x, -p.y)*tr); PutPixel(POINT(-p.x, -p.y)*tr); PutPixel(POINT(-p.x, p.y)*tr);
}
void ellipse(Vector3<int> axis, MATRIX &transform){
	
	Vector3<int> pt(0, axis.y);
	int a2 = axis.x*axis.x, b2 = axis.y*axis.y;
	int p = b2 + a2 / 4 - axis.y;
	int xb2 = 0, ya2 = axis.y*a2;

	while (xb2 <= ya2){
		DrawQuad(pt, transform);
		pt.x++;
		xb2 += b2;
		if (p < 0)
			p += xb2 + b2;
		else{
			pt.y--;
			ya2 -= a2;
			p += xb2 + b2 - ya2;
		}
	}
	while (pt.y >= 0){
		DrawQuad(pt, transform);
		pt.y--;
		ya2 -= a2;
		if (p >= 0)
			p += a2 - ya2;
		else{
			pt.x++;
			xb2 += b2;
			p += a2 + xb2 - ya2;
		}
	}
}

#pragma endregion



//-----------ACTUAL IMPLEMENTATION-----------------------

#define PI 3.14
MATRIX View;	//View Matrix
MATRIX Model;
double ell_angle, line_angle;

int ticks;
long st;
void Start(){
	st = time(0);
}

void illustraion_pendulum(){
	Model = MATRIX::CreateRotation(0, 0, ell_angle)*MATRIX::CreateTraslation(300, 30);
	ellipse(POINT(20, 5), Model*View);
	Model = MATRIX::CreateRotation(0, 0, ell_angle + 1)*MATRIX::CreateTraslation(300, 30);
	ellipse(POINT(20, 5), Model*View);
	Model = MATRIX::CreateRotation(0, 0, ell_angle + 2)*MATRIX::CreateTraslation(300, 30);
	ellipse(POINT(20, 5), Model*View);
	ell_angle += 0.01;

	Model = MATRIX::CreateRotation(0, 0, line_angle)* MATRIX::CreateTraslation(300, 30);
	lineDDA(300, Model*View);
	Model = MATRIX::CreateTraslation(0, 300)*MATRIX::CreateRotation(0, 0, line_angle)*MATRIX::CreateTraslation(300, 30);
	ellipse(POINT(20, 20), Model*View);
	line_angle = sin(ticks*0.01);
	ticks++;

	//ITS All About MATRIX !!

	if (GetKeyState(37)) 
		View = View*MATRIX::CreateTraslation(1, 0);
}
void illustration_shear(){
	COLOR = 0x000000ff;
	Model = MATRIX::CreateTraslation(300, 0);
	lineDDA(600, Model);
	Model = MATRIX::CreateRotation(0, 0, -PI / 2)*MATRIX::CreateTraslation(0, 300);
	lineDDA(600, Model);
	Model = MATRIX::CreateTraslation(300, 300);
	COLOR = 0x00000000;
	ellipse(POINT(70, 140), Model);
	Model = MATRIX::CreateTraslation(0, -70)*MATRIX::CreateRotation(0, 0, PI / 4)*MATRIX::CreateScaling(0.5, 1)*MATRIX::CreateRotation(0, 0, -PI / 4)*MATRIX::CreateScaling(2, 1)*MATRIX::CreateTraslation(0, 70)*MATRIX::CreateTraslation(300, 300);
	COLOR = 0x00ff0000;
	ellipse(POINT(70, 140), Model);

}
void illustration_inclinedEllipse(){
	static double a1, a2=PI/3, a3=-PI/3;
	Model = MATRIX::CreateRotation(0,PI/2,0);
	ellipse(POINT(70, 200), Model*View);
	Model = MATRIX::CreateRotation(PI/3,PI/2,0);
	ellipse(POINT(70, 200), Model*View);
	Model = MATRIX::CreateRotation(-PI/3, PI/2, 0);
	ellipse(POINT(70, 200), Model*View);

	Model = MATRIX::CreateTraslation(70 * cos(a1), 200 * sin(a1))*MATRIX::CreateRotation(0, PI / 2, 0);
	ellipse(POINT(10, 10), Model*View);
	Model = MATRIX::CreateTraslation(70 * cos(a2), 200 * sin(a2))*MATRIX::CreateRotation(PI / 3, PI / 2, 0);
	ellipse(POINT(10, 10), Model*View);
	Model = MATRIX::CreateTraslation(70 * cos(a3), 200 * sin(a3))*MATRIX::CreateRotation(-PI / 3, PI / 2, 0);
	ellipse(POINT(10, 10), Model*View);

	a1 += 0.01;
	a2 += 0.01;
	a3 += 0.01;
}

int frames = 0;

int Update(HDC hdc){
	static int y;
	static double ar;
	::hdc = hdc;
	View = MATRIX::CreateRotation(0,ar,0)*MATRIX::CreateTraslation(300, 300);
	illustration_inclinedEllipse();
	ar += 0.01;
	return 0;
}
void Event(UINT message, WPARAM wParam, LPARAM lParam){
	
}