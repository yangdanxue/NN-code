#include <windows.h>
#include "resource.h"
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
void bulei();
void fanzhuan(int x, int y,	int cxSource, HDC hdc, HDC hdcMem);

int lei[10][10] = {0};
int tie[10][10] = {0};
int flag[10][10] = {0}; //记录(x,y)这个位置周围的一圈上插了几个旗子

BOOL GameOver = FALSE;

#define PQUESTIONMARK 6
#define LEI 10
#define WRONGFLAG 11
#define HONGLEI 12
#define QUESTIONMARK 13
#define FLAG 14
#define LTIE 21


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("HelloWin") ;
	HWND         hWnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;//一个表示windows程序最主要特征的数据结构

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = szAppName ;

	if (!RegisterClass (&wndclass))//将wndclass注册到windows操作系统。
	{
		MessageBox (NULL, TEXT ("This program requires Windows NT!"), szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hWnd = CreateWindow (szAppName, // window class name
		TEXT ("明明的扫雷"), // window caption
		//		WS_OVERLAPPEDWINDOW,        // window style
		//		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX  | WS_MAXIMIZEBOX, 
		WS_OVERLAPPED |WS_CAPTION |WS_SYSMENU| WS_MINIMIZEBOX,	
		CW_USEDEFAULT,              // initial x position
		CW_USEDEFAULT,              // initial y position
		48 * 10 + 6,                // initial x size
		48 * 10 + 28,               // initial y size
		NULL,                       // parent window handle
		NULL,                       // window menu handle
		hInstance,                  // program instance handle
		NULL) ;                     // creation parameters

	ShowWindow (hWnd, iCmdShow) ;//在屏幕上显示hWnd代表的窗口
	UpdateWindow (hWnd) ;//在屏幕上刷新窗口

	while (GetMessage (&msg, NULL, 0, 0))//消息循环，从消息队列中获取消息
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP	hBitmap ;
	static int		cxClient, cyClient, cxSource, cySource ;
	static int		lastxiaxianx = -1;
	static int		lastxiaxiany = -1;

	BITMAP			bitmap ;
	HDC				hdc, hdcMem ;
	HINSTANCE		hInstance ;
	int				nX, nY; //行列坐标
	int				x, y;
	int v = 0;
	int s = 0;
	PAINTSTRUCT    ps ;


	switch (message)
	{
	case WM_CREATE:
		hInstance = ((LPCREATESTRUCT) lParam)->hInstance ;

		hBitmap = LoadBitmap (hInstance, MAKEINTRESOURCE (IDB_BITMAP1)) ;

		GetObject (hBitmap, sizeof (BITMAP), &bitmap) ;

		cxSource = bitmap.bmWidth ;
		cySource = bitmap.bmHeight ;

		bulei();

		return 0 ;


	case WM_SIZE:
		cxClient = 48 * 9 + 16;
		cyClient = 48 * 9 + 38;

		return 0 ;


	case WM_PAINT:
		hdc = BeginPaint (hWnd, &ps) ;

		hdcMem = CreateCompatibleDC (hdc) ;
		SelectObject (hdcMem, hBitmap) ;

		for (x = 0; x < 10; x++) 
		{
			for (y = 0; y < 10; y++) 
			{
				BitBlt (hdc, x * cxSource, y * cxSource, cxSource, cxSource, hdcMem, 0, 0, SRCCOPY);

				if ((tie[x][y] == WRONGFLAG) || (tie[x][y] == QUESTIONMARK) || (tie[x][y] == FLAG)) //修改“最小化还原后，插错旗的标记消失”bug
				{
					BitBlt (hdc, x * cxSource, y * cxSource, cxSource, cxSource, hdcMem, 0, (15-tie[x][y]) * cxSource, SRCCOPY);
				}
				else if (tie[x][y] == LTIE)
				{
					BitBlt (hdc, x * cxSource, y * cxSource, cxSource, cxSource, hdcMem, 0, (15-lei[x][y]) * cxSource, SRCCOPY);
				}
			}			
		}

		DeleteDC (hdcMem) ;
		EndPaint (hWnd, &ps) ;
		return 0 ;



	case WM_LBUTTONDOWN:
		if(GameOver)
			return 0;

		hdc = GetDC(hWnd);

		nX = ((LOWORD (lParam)) / cxSource);
		nY = ((HIWORD (lParam)) / cxSource);

		hdcMem = CreateCompatibleDC (hdc) ;
		SelectObject (hdcMem, hBitmap) ;

		if (tie[nX][nY] == 0)
		{
			BitBlt (hdc, nX * cxSource, nY * cxSource, cxSource, cxSource, hdcMem, 0, 15 * cxSource, SRCCOPY) ;
			tie[nX][nY] = 0;
		}

		else if (tie[nX][nY] == QUESTIONMARK)
		{			
			BitBlt (hdc, nX * cxSource, nY * cxSource, cxSource, cxSource, hdcMem, 0, 6 * cxSource, SRCCOPY) ;
			tie[nX][nY] = PQUESTIONMARK;
		}

		DeleteDC (hdcMem) ;
		ReleaseDC(hWnd, hdc);

		return 0;


	case WM_LBUTTONUP:
		if(GameOver)
			return 0;

		hdc = GetDC(hWnd);

		nX = ((LOWORD (lParam)) / cxSource);
		nY = ((HIWORD (lParam)) / cxSource);

		hdcMem = CreateCompatibleDC (hdc) ;
		SelectObject (hdcMem, hBitmap) ;


		if ((lei[nX][nY] < LEI) && ((tie[nX][nY] == 0) || (tie[nX][nY] == PQUESTIONMARK)))
		{
			BitBlt (hdc, nX * cxSource, nY * cxSource, cxSource, cxSource, hdcMem, 0, (15 - lei[nX][nY]) * cxSource, SRCCOPY);
			tie[nX][nY] = LTIE;
		}


		if (lei[nX][nY] == 0)
		{
			fanzhuan(nX, nY, cxSource, hdc, hdcMem);
		}


		if ((lei[nX][nY] == LEI) && ((tie[nX][nY] == 0) || (tie[nX][nY] == PQUESTIONMARK)))
		{
			GameOver = TRUE;

			lei[nX][nY] = HONGLEI;

			for ( x = 0; x < 10; x++)
			{
				for ( y = 0; y < 10; y++)
				{
					if((lei[x][y] == LEI) && (tie[x][y] == 0)) //修改“一旦点到雷，以前插过的旗子均消失”bug
					{
						BitBlt (hdc, x * cxSource, y * cxSource, cxSource, cxSource, hdcMem, 0, 5 * cxSource, SRCCOPY) ;
						tie[x][y] = LTIE;
					}
					else if((lei[x][y] != LEI) && ((tie[x][y] == FLAG) || (tie[x][y] == QUESTIONMARK))) //插错旗的地方有标记 
					{
						BitBlt (hdc, x * cxSource, y * cxSource, cxSource, cxSource, hdcMem, 0, 4 * cxSource, SRCCOPY) ;
						tie[x][y] = WRONGFLAG;
					}
				}
			}
			BitBlt (hdc, nX * cxSource, nY * cxSource, cxSource, cxSource, hdcMem, 0, 3 * cxSource, SRCCOPY) ;
			tie[nX][nY] = LTIE;
		}

		DeleteDC (hdcMem) ;
		ReleaseDC(hWnd, hdc);

		return 0;


	case WM_RBUTTONDOWN:
		if(GameOver)
			return 0;

		hdc = GetDC(hWnd);

		nX = ((LOWORD (lParam)) / cxSource);
		nY = ((HIWORD (lParam)) / cxSource);

		hdcMem = CreateCompatibleDC (hdc) ;
		SelectObject (hdcMem, hBitmap) ;

		if (tie[nX][nY] == 0)  //如果这个地方贴着方砖
		{
			BitBlt (hdc, nX * cxSource, nY * cxSource, cxSource, cxSource, hdcMem, 0, 1 * cxSource, SRCCOPY) ; 
			tie[nX][nY] = FLAG; //记录这个地方插旗了

			if(nX!=0 && nY!=0)
			{
				flag[nX-1][nY-1]++;
			}
			if(nY!=0)
			{
				flag[nX][nY-1]++;
			}
			if(nX!=9 && nY!=0)
			{
				flag[nX+1][nY-1]++;
			}
			if(nX!=0)
			{	
				flag[nX-1][nY]++;
			}
			if(nX!=9)
			{
				flag[nX+1][nY]++;
			}
			if(nX!=0 && nY!=9)
			{
				flag[nX-1][nY+1]++;
			}
			if(nY!=9)
			{
				flag[nX][nY+1]++;
			}
			if(nX!=9 && nY!=9)
			{
				flag[nX+1][nY+1]++;
			}
		}

		else if (tie[nX][nY] == FLAG) //如果这个地方插旗了
		{
			BitBlt (hdc, nX * cxSource, nY * cxSource, cxSource, cxSource, hdcMem, 0, 2 * cxSource, SRCCOPY) ;
			tie[nX][nY] = QUESTIONMARK; //记录这个地方打问号了

			if(nX!=0 && nY!=0)
			{
				flag[nX-1][nY-1]--;
			}
			if(nY!=0)
			{
				flag[nX][nY-1]--;
			}
			if(nX!=9 && nY!=0)
			{
				flag[nX+1][nY-1]--;
			}
			if(nX!=0)
			{	
				flag[nX-1][nY]--;
			}
			if(nX!=9)
			{
				flag[nX+1][nY]--;
			}
			if(nX!=0 && nY!=9)
			{
				flag[nX-1][nY+1]--;
			}
			if(nY!=9)
			{
				flag[nX][nY+1]--;
			}
			if(nX!=9 && nY!=9)
			{
				flag[nX+1][nY+1]--;
			}
		}

		else if (tie[nX][nY] == QUESTIONMARK) //如果这个地方打问号了
		{
			BitBlt (hdc, nX * cxSource, nY * cxSource, cxSource, cxSource, hdcMem, 0, 0, SRCCOPY) ;
			tie[nX][nY] = 0; //记录这个地方贴上方砖了
		}	


		DeleteDC (hdcMem) ;
		ReleaseDC(hWnd, hdc);

		return 0;


	case WM_MBUTTONDOWN: //双击探雷
		if(GameOver)
			return 0;

		hdc = GetDC(hWnd);

		nX = ((LOWORD (lParam)) / cxSource);
		nY = ((HIWORD (lParam)) / cxSource);

		hdcMem = CreateCompatibleDC (hdc) ;
		SelectObject (hdcMem, hBitmap) ;

		{
			int i = 0;
			int j = 0;

			for (i = (nX-1); i <= (nX+1); i++)
			{                                                                         
				for (j = (nY-1); j <= (nY+1); j++)
				{
					if (tie[i][j] == 0)
					{
						BitBlt (hdc, i * cxSource, j * cxSource, cxSource, cxSource, hdcMem, 0, 15 * cxSource, SRCCOPY) ;
						tie[i][j] = 0;
					}

					else if (tie[i][j] == QUESTIONMARK)
					{
						BitBlt (hdc, i * cxSource, j * cxSource, cxSource, cxSource, hdcMem, 0, 6 * cxSource, SRCCOPY) ;
						tie[i][j] = PQUESTIONMARK;
					}

				}
			}
		}


		DeleteDC (hdcMem) ;
		ReleaseDC(hWnd, hdc);

		return 0;


	case WM_MBUTTONUP:
		if(GameOver)
			return 0;

		hdc = GetDC(hWnd);

		nX = ((LOWORD (lParam)) / cxSource);
		nY = ((HIWORD (lParam)) / cxSource);

		hdcMem = CreateCompatibleDC (hdc) ;
		SelectObject (hdcMem, hBitmap) ;


		{
			int i = 0;
			int j = 0;

			for (i = (nX-1); i <= (nX+1); i++)
			{                                                                         
				for (j = (nY-1); j <= (nY+1); j++)
				{
					if ((flag[nX][nY] != lei[nX][nY]) && (tie[i][j] == 0)) //抬中键，下陷方砖恢复
					{
						BitBlt (hdc, i * cxSource, j * cxSource, cxSource, cxSource, hdcMem, 0, 0 * cxSource, SRCCOPY) ;
					}

					else if ((flag[nX][nY] != lei[nX][nY]) && (tie[i][j] == PQUESTIONMARK)) //抬中键，下陷"?"恢复
					{
						BitBlt (hdc, i * cxSource, j * cxSource, cxSource, cxSource, hdcMem, 0, 2 * cxSource, SRCCOPY) ;
						tie[i][j] = QUESTIONMARK;
					}

					else if ((flag[nX][nY] == lei[nX][nY]) && ((tie[i][j] == 0) || (tie[i][j] == PQUESTIONMARK)) && (lei[i][j] != LEI)) 
					{
						BitBlt (hdc, i * cxSource, j * cxSource, cxSource, cxSource, hdcMem, 0, (15 - lei[i][j]) * cxSource, SRCCOPY) ;

						if (lei[i][j] == 0)
						{
							fanzhuan(i, j, cxSource, hdc, hdcMem);
						}
						tie[i][j] = LTIE;
					}

					else if ((flag[nX][nY] == lei[nX][nY]) && ((tie[i][j] == 0) || (tie[i][j] == PQUESTIONMARK)) && (lei[i][j] == LEI))
					{
						BitBlt (hdc, i * cxSource, j * cxSource, cxSource, cxSource, hdcMem, 0, (15 - lei[i][j]) * cxSource, SRCCOPY) ;

						if (lei[i][j] == 0)
						{
							fanzhuan(i, j, cxSource, hdc, hdcMem);
						}
						tie[i][j] = LTIE;

						GameOver = TRUE;

						lei[i][j] = HONGLEI;

						for ( x = 0; x < 10; x++)
						{
							for ( y = 0; y < 10; y++)
							{
								if((lei[x][y] == LEI) && ((tie[x][y] == 0) || (tie[x][y] == QUESTIONMARK))) //修改“一旦点到雷，以前插过的旗子均消失”bug
								{
									BitBlt (hdc, x * cxSource, y * cxSource, cxSource, cxSource, hdcMem, 0, 5 * cxSource, SRCCOPY) ;
									tie[x][y] = LTIE;
								}

								else if((lei[x][y] != LEI) && (tie[x][y] == FLAG)) //插错旗的地方有标记 
								{
									BitBlt (hdc, x * cxSource, y * cxSource, cxSource, cxSource, hdcMem, 0, 4 * cxSource, SRCCOPY) ;
									tie[x][y] = WRONGFLAG;
								}
							}
						}
						BitBlt (hdc, i * cxSource, j * cxSource, cxSource, cxSource, hdcMem, 0, 3 * cxSource, SRCCOPY) ;
						tie[i][j] = LTIE;
					}
				}
			}
		}


		DeleteDC (hdcMem) ;
		ReleaseDC(hWnd, hdc);

		return 0;

		// **************鼠标移动********************
	case WM_MOUSEMOVE: 
		if(GameOver)
			return 0;

		hdc = GetDC(hWnd);
		hdcMem = CreateCompatibleDC (hdc) ;
		SelectObject (hdcMem, hBitmap) ;

		nX = ((LOWORD (lParam)) / cxSource);
		nY = ((HIWORD (lParam)) / cxSource);

		if (wParam & 0x1) //如果条件满足说明左键按下，执行这里
		{
			//使前面下陷的方砖抬起
			if ((lastxiaxianx != -1) && (lastxiaxiany != -1))
			{
				if (tie[lastxiaxianx][lastxiaxiany] == 0)
				{
					BitBlt (hdc, lastxiaxianx * cxSource, lastxiaxiany * cxSource, cxSource, cxSource, hdcMem, 0, 0 * cxSource, SRCCOPY) ;
				}
			}

			//使当前方砖下陷
			if (tie[nX][nY] == 0)
			{
				BitBlt (hdc, nX * cxSource, nY * cxSource, cxSource, cxSource, hdcMem, 0, 15 * cxSource, SRCCOPY) ;
				//标记下次需要抬起的方砖
				lastxiaxianx = nX;
				lastxiaxiany = nY;
			}
		}

		DeleteDC (hdcMem) ;
		ReleaseDC(hWnd, hdc);

		return 0;

	case WM_DESTROY:
		PostQuitMessage (0) ;

		return 0 ;
	}
	return DefWindowProc (hWnd, message, wParam, lParam) ;
}


void bulei()
{
	int x = 0;
	int y = 0;
	int i = 0;

	srand((unsigned int)time( NULL)); // 用系统时间做随机种子
	for (i = 0; i < 10; )
	{
		x = rand()%10; //算出来的是雷的横坐标
		y = rand()%10; //算出来的是雷的纵坐标

		if( 0 == lei[x][y])
		{
			lei[x][y] = LEI; // 成功布雷
			i++;                                    
		}
	}

	for(x = 0; x < 10; x++) //方砖上的数字（x,y表示方砖的坐标）是几要挨着个儿过一遍，雷的位置刨除在外
	{
		for (y = 0; y < 10; y++) //方砖上的数字是几要挨着个儿过一遍，雷的位置刨除在外
		{
			if (lei[x][y] == LEI) //这个才是真正的数（表示周围有几颗雷）//如果是雷的话，不进行下面的动作
			{
				continue;
			}
			else
			{       
				int v = 0;
				if( (lei[x-1][y-1] == LEI) && (x!=0 && y!=0 ) )  v++;
				if( (lei[x][y-1] == LEI) && (y!=0) )  v++;
				if( (lei[x+1][y-1] == LEI) && (x!=9 && y!=0) )  v++;
				if( (lei[x-1][y] == LEI) && ( x!=0 ) )  v++;
				if( lei[x][y] == LEI)  v++;
				if( (lei[x+1][y] == LEI) && (x!=9) )  v++;
				if( (lei[x-1][y+1] == LEI) && (x!=0 && y!=9 ) )  v++;
				if( (lei[x][y+1] == LEI) && ( y!=9 ) )  v++;
				if( (lei[x+1][y+1] == LEI) && (x!=9 && y!=9 ) )  v++;
				lei[x][y] = v;
			}
		}
	}
}


void fanzhuan(int x, int y,	int cxSource,HDC hdc, HDC hdcMem)	
{
	int i = 0, n = 0;

	for (i = (x-1); i <= (x+1); i++)
	{                                                                         
		for (n = (y-1); n <= (y+1); n++)
		{
			if((i >= 0) && (n >= 0) && (i < 10) && (n < 10))
			{
				if(lei[i][n] < LEI)
				{
					if (0 == tie[i][n])  //如果这个地方没有翻开
					{
						BitBlt (hdc, i * cxSource, n * cxSource, cxSource, cxSource, hdcMem, 0, (15-lei[i][n] ) * cxSource, SRCCOPY) ;
						tie[i][n] = LTIE;  //记录这个地方翻了

						if (lei[i][n] == 0)  //如果这个地方周围没有雷
						{
							fanzhuan(i,n,cxSource,hdc,hdcMem);  //继续翻这个地方的周围
						}
					}
				}				
			}
		}
	}
}