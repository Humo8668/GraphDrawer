#pragma once
#include <Windows.h>
#include <list>


class Window
{
public:
	typedef struct Graph
	{
		int n; // number of nodes
		double *X;  // array of arrguments, amount = n
		double *F;	// array of values of function, amount = n
		HPEN coloredPen;
		COLORREF color;
		char withArgs;
		double x_max;
		double F_max;
	};

protected:
	std::list<Graph> graph;
	int graphsCount;
	MSG msg;
	BOOL bRet;
	static LRESULT _stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	WNDCLASS wc;
	bool isOpen;
	float offsetX, offsetY, scaleX, scaleY;
	float scrollSensetivity;

	HFONT mainFont;
	HPEN GridPen;
	HPEN AxisPen;
	

public:
	int WindowWidth;
	int WindowHeight;
	int WindowX;
	int WindowY;
	HWND hWnd;
	HDC hDC;

	Window(int winX, int winY, int winW, int winH);
	~Window(void);

	//int DrawGraph(float (F)(float), float ScaleX, float ScaleY, COLORREF);

	void writeString(int x, int y, char* str);
	void writeString(int x, int y, double num);
	void writeScale();
	void showGraphColors();

	int AddGraph(double* X, double* Fx, int n, COLORREF color);
	int AddGraph(double* F, int n, COLORREF color);
	int Redraw(float offsetX, float offsetY, float scaleX, float scaleY);
	int Redraw();
	void ClearWindow();
	void StartEventSystem();
	void Show(); 
	void DrawGraph(Graph g);
	void DrawAxis();
	void DrawGrid();
};

