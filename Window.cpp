#define _CRT_SECURE_NO_DEPRECATE
#include "Window.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "helper.h"

typedef struct point_t
{
	double x;
	double y;
};

float Fn(float x)
{
	return sin(x);
}

/*char* doubleToStr(double a, int precision)
{

}*/

// window info
	Window *CurrWin;
	bool lButtonPressed;
	point_t prevCoord;
//-------------------

/*------Global Variables--------*/
	char *scale_str = NULL;
/*------------------------------*/


LRESULT _stdcall Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			CurrWin->isOpen = false;
			return 0;
		case WM_CLOSE:
			PostQuitMessage(0);
			CurrWin->isOpen = false;
			return 0;
		case WM_QUIT:
			PostQuitMessage(0);
			CurrWin->isOpen = false;
			return 0;
		case WM_SIZE:
			CurrWin->WindowWidth = LOWORD(lParam); //ширина
            CurrWin->WindowHeight = HIWORD(lParam); //высота
			break;
		case WM_MOUSEWHEEL:
			if(CurrWin->scaleX + CurrWin->scrollSensetivity*GET_WHEEL_DELTA_WPARAM(wParam) > 0 &&
			   CurrWin->scaleY + CurrWin->scrollSensetivity*GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			{
				CurrWin->scaleX += CurrWin->scrollSensetivity*GET_WHEEL_DELTA_WPARAM(wParam);
				CurrWin->scaleY += CurrWin->scrollSensetivity*GET_WHEEL_DELTA_WPARAM(wParam);
				CurrWin->Redraw();
			}
			break;
		case WM_LBUTTONDOWN:
			lButtonPressed = true;
			prevCoord.x = LOWORD(lParam);
			prevCoord.y = HIWORD(lParam);
			break;
		case WM_MOUSEMOVE:
			if(lButtonPressed)
			{
				CurrWin->offsetX += LOWORD(lParam)-prevCoord.x;
				CurrWin->offsetY += prevCoord.y - HIWORD(lParam);
				prevCoord.x = LOWORD(lParam);
				prevCoord.y = HIWORD(lParam);
				CurrWin->Redraw();
			}
			break;
		case WM_LBUTTONUP:
			lButtonPressed = false;
			break;
		default: 
			return DefWindowProc(hWnd,message,wParam,lParam);
	};

	return 0;
}


Window::Window(int winX, int winY, int winW, int winH)
{
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);				// yesli vyletet oshibka, to vozmojno tut
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(6);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "Window";

	RegisterClass(&wc);

	hWnd = CreateWindowW(L"Window",
						L"Window",
						WS_OVERLAPPEDWINDOW,
						winX,winY,winW,winH,
						NULL,NULL,NULL,NULL);

	if(hWnd == NULL)
	{
		throw WSASYSCALLFAILURE;
		return ;
	}

	this->WindowX = winX;
	this->WindowY = winY;
	this->WindowWidth = winW;
	this->WindowHeight = winH;
	this->scaleX = 10;
	this->scaleY = 10;
	this->offsetX = 0;
	this->offsetY = 0;
	this->scrollSensetivity = 0.02;
	this->graphsCount = 0;
	CurrWin = this;
	lButtonPressed = false;
	
	mainFont = CreateFont(15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Courier New");
	GridPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
	AxisPen = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));

	scale_str = (char*)malloc(20);	// 7 + for "Scale: "; 1 + for '\0'; 11 + for integer; + 1 for rounding to 20 :-)
	scale_str[0] = 'S';
	scale_str[1] = 'c';
	scale_str[2] = 'a';
	scale_str[3] = 'l';
	scale_str[4] = 'e';
	scale_str[5] = ':';
	scale_str[6] = ' ';

	this->hDC = GetDC(hWnd);
	if(this->hDC == NULL)
		throw WSASYSCALLFAILURE;
	
	return ;
}


Window::~Window(void)
{
}

void Window::Show()
{
	isOpen = true;
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
}

void Window::StartEventSystem()
{
	while( (bRet = GetMessage( &msg, hWnd, 0, 0 )) != 0)
	{
		if (bRet == -1)
		{
			//printf("%ld\n", GetLastError());
			throw GetLastError();
			return ;
		}
		else
		{
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}

		if(!this->isOpen)
		{
			break;
		}
	}
	return ;
}

void Window::DrawAxis()
{
	point_t O;
	/*double deltaX, deltaY;		//	array in view

	deltaX = this->WindowWidth / this->scaleX;
	deltaY = this->WindowHeight / this->scaleY;
	*/
	O.x = this->WindowWidth / 2 + this->offsetX;
	O.y = this->WindowHeight / 2 - this->offsetY;
	
	SelectObject( this->hDC, AxisPen);

    MoveToEx( this->hDC, 0, O.y, NULL );
	LineTo( this->hDC, WindowWidth, O.y );
    MoveToEx( this->hDC, O.x, 0, NULL );
    LineTo( this->hDC, O.x, WindowHeight );

	return ;
}


void Window::ClearWindow()
{
	RECT rect;
	GetClientRect (this->hWnd, &rect);
	FillRect (this->hDC, &rect, (HBRUSH)(6));
	return ;
}

void Window::writeString(int x, int y, char* str)
{
	int strLen = StrLength(str);
	TextOutA(hDC, x, y, str, strLen);
	return;
}

void Window::writeString(int x, int y, double num)
{
	char* str = dblToStr(num, 2);
	int strLen = StrLength(str);
	TextOutA(hDC, x, y, str, strLen);
	free((void*)str);
	return;
}

void Window::writeScale()
{
	char *scaleX_str;
	TCHAR *Tscale_str;
	RECT rc;

	scaleX_str = dblToStr(this->scaleX, 1);
	
	strcpy(&(scale_str[7]), scaleX_str);
	Tscale_str = TEXT(scale_str);
	GetClientRect(this->hWnd, &rc);
	SetTextColor(this->hDC, RGB(0, 0, 0));
	DrawText(this->hDC, Tscale_str, lstrlen(Tscale_str), &rc, DT_SINGLELINE + DT_BOTTOM);

	free((void*)scaleX_str);
	return;
}

void Window::showGraphColors()
{
	char *str = NULL;
	RECT rc;
	int i, t;

	GetClientRect(this->hWnd, &rc);
	i = 0;
	t = this->graphsCount;
	for (std::list<Graph>::iterator it = this->graph.begin(); it != this->graph.end(); it++)
	{
		str = intToStr(i);
		SetTextColor(this->hDC, it->color);
		SelectObject(this->hDC, it->coloredPen);
		TextOut(this->hDC, rc.right - 50, rc.bottom - t * 20 - 8, (LPCSTR)str, StrLength(str));
		MoveToEx(this->hDC, rc.right - 40, rc.bottom - t * 20, NULL);
		LineTo(this->hDC, rc.right - 20, rc.bottom - t * 20);
		
		free((void*)str);
		i++;
		t--;
	}

	return;
}

void Window::DrawGrid()
{
	//double 
	double gridSize, gridSize_coeff;
	int i;
	point_t O;
	double rightBorder, leftBorder;
	point_t p0, p1;				//	right-bottom and left-hight points of view
	
	gridSize_coeff = 1.0;
	if (this->scaleX < 10.0)
		gridSize_coeff = 10.0;
	if (this->scaleX >= 10.0 && this->scaleX < 20.0)
		gridSize_coeff = 5.0;
	if (this->scaleX >= 20.0 && this->scaleX < 50.0)
		gridSize_coeff = 2.0;
	if (this->scaleX >= 50.0 && this->scaleX < 100.0)
		gridSize_coeff = 1.0;
	if (this->scaleX >= 100.0)
		gridSize_coeff = 0.5;


	gridSize = gridSize_coeff * this->scaleX;
	SelectObject(this->hDC, GridPen);
	SetTextColor(this->hDC, RGB(100, 100, 100));
	
	O.x = this->WindowWidth / 2 + this->offsetX;
	O.y = this->WindowHeight / 2 - this->offsetY;
	
	leftBorder = floor((- O.y) / gridSize);
	rightBorder = floor((this->WindowHeight - O.y) / gridSize);
	
	for (i = (int)leftBorder; i <= (int)rightBorder; i++)
	{
		MoveToEx(this->hDC, 0, O.y + i * gridSize, NULL);
		LineTo(this->hDC, this->WindowWidth, O.y + i * gridSize);

		writeString(O.x, O.y + i * gridSize, -i * gridSize_coeff);
	}

	leftBorder = floor((-O.x) / gridSize);
	rightBorder = floor((this->WindowWidth - O.x) / gridSize);
	for (i = (int)leftBorder; i <= (int)rightBorder; i++)
	{
		MoveToEx(this->hDC, O.x + i * gridSize, 0, NULL);
		LineTo(this->hDC, O.x + i * gridSize, this->WindowHeight);

		writeString(O.x + i * gridSize, O.y, i * gridSize_coeff);
	}
	return;
}

void Window::DrawGraph(Window::Graph g)
{
	int i;
	char *Max_Str;
	//double deltaX, deltaY;		//	array in view
	point_t O;
	SelectObject(this->hDC, g.coloredPen);
	SetTextColor(this->hDC, g.color);
	//deltaX = this->WindowWidth / this->scaleX;
	//deltaY = this->WindowHeight / this->scaleY;
	O.x = this->WindowWidth / 2 + this->offsetX;
	O.y = this->WindowHeight / 2 - this->offsetY;
	for (i = 0; i < g.n - 1; i++)
	{
		MoveToEx(this->hDC, this->scaleX * g.X[i] + O.x, -this->scaleY*g.F[i] + O.y, NULL);
		LineTo(this->hDC, this->scaleX * g.X[i + 1] + O.x, -this->scaleY*g.F[i + 1] + O.y);
	}
	//MaxPoint
	Max_Str = dblToStr(g.F_max, 3);
	//printf(Max_Str);
	TextOut(this->hDC, this->scaleX * g.x_max + O.x, -this->scaleY*g.F_max + O.y, Max_Str, StrLength(Max_Str));
	free((void*)Max_Str);
	return;
}


int Window::Redraw()
{
	SelectObject(this->hDC, this->mainFont);
	SetBkMode (this->hDC, TRANSPARENT);
	ClearWindow();
	DrawGrid();
	DrawAxis();
	writeScale();
	showGraphColors();
	for(std::list<Graph>::iterator it = this->graph.begin(); it != this->graph.end(); it++)
	{
		DrawGraph(*it);
	}

	return 0;
}

int Window::Redraw(float offsetX, float offsetY, float scaleX, float scaleY)
{
	this->offsetX = offsetX;
	this->offsetY = offsetY;
	this->scaleX = scaleX;
	this->scaleY = scaleY;
	return this->Redraw();
}

int Window::AddGraph(double* X, double* Fx, int n, COLORREF color)
{
	double max_F;
	int max_i, i;
	Graph *gr;
	if(n <= 0) return -1;
	gr = new Graph();
	gr->n = n;
	gr->X = X;
	gr->F = Fx;
	gr->coloredPen = CreatePen( PS_SOLID, 2, color);
	gr->color = color;
	max_F = Fx[0];

	for(i = 0; i < n; i++)
	{
		if(fabs(Fx[i]) > fabs(max_F))
		{
			max_i = i;
			max_F = Fx[i];
		}
	}

	gr->F_max = max_F;
	gr->x_max = X[max_i];

	this->graph.push_back(*gr);
	this->graphsCount++;
	return 0;
}

int Window::AddGraph(double* F, int n, COLORREF color)
{
	double max_F;
	int max_i, i;
	Graph *gr;
	if(n <= 0) return -1;
	gr = new Graph();
	gr->n = n;
	gr->F = F;
	gr->coloredPen = CreatePen( PS_SOLID, 2, color);
	gr->color = color;

	gr->X = (double*)malloc(n * sizeof(double));
	for (i = 0; i < n; i++)
		gr->X[i] = (double)i;

	max_F = F[0];
	max_i = 0;
	for(i = 0; i < n; i++)
	{
		if(fabs(F[i]) > fabs(max_F))
		{
			max_i = i;
			max_F = F[i];
		}
	}

	gr->F_max = max_F;
	gr->x_max = gr->X[max_i];

	this->graph.push_back(*gr);
	this->graphsCount++;
	return 0;
}