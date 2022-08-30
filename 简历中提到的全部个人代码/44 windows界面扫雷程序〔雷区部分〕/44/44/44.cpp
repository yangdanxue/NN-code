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
int flag[10][10] = {0}; //��¼(x,y)���λ����Χ��һȦ�ϲ��˼�������

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
	WNDCLASS     wndclass ;//һ����ʾwindows��������Ҫ���������ݽṹ

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

	if (!RegisterClass (&wndclass))//��wndclassע�ᵽwindows����ϵͳ��
	{
		MessageBox (NULL, TEXT ("This program requires Windows NT!"), szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hWnd = CreateWindow (szAppName, // window class name
		TEXT ("������ɨ��"), // window caption
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

	ShowWindow (hWnd, iCmdShow) ;//����Ļ����ʾhWnd����Ĵ���
	UpdateWindow (hWnd) ;//����Ļ��ˢ�´���

	while (GetMessage (&msg, NULL, 0, 0))//��Ϣѭ��������Ϣ�����л�ȡ��Ϣ
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
	int				nX, nY; //��������
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

				if ((tie[x][y] == WRONGFLAG) || (tie[x][y] == QUESTIONMARK) || (tie[x][y] == FLAG)) //�޸ġ���С����ԭ�󣬲����ı����ʧ��bug
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
					if((lei[x][y] == LEI) && (tie[x][y] == 0)) //�޸ġ�һ���㵽�ף���ǰ��������Ӿ���ʧ��bug
					{
						BitBlt (hdc, x * cxSource, y * cxSource, cxSource, cxSource, hdcMem, 0, 5 * cxSource, SRCCOPY) ;
						tie[x][y] = LTIE;
					}
					else if((lei[x][y] != LEI) && ((tie[x][y] == FLAG) || (tie[x][y] == QUESTIONMARK))) //�����ĵط��б�� 
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

		if (tie[nX][nY] == 0)  //�������ط����ŷ�ש
		{
			BitBlt (hdc, nX * cxSource, nY * cxSource, cxSource, cxSource, hdcMem, 0, 1 * cxSource, SRCCOPY) ; 
			tie[nX][nY] = FLAG; //��¼����ط�������

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

		else if (tie[nX][nY] == FLAG) //�������ط�������
		{
			BitBlt (hdc, nX * cxSource, nY * cxSource, cxSource, cxSource, hdcMem, 0, 2 * cxSource, SRCCOPY) ;
			tie[nX][nY] = QUESTIONMARK; //��¼����ط����ʺ���

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

		else if (tie[nX][nY] == QUESTIONMARK) //�������ط����ʺ���
		{
			BitBlt (hdc, nX * cxSource, nY * cxSource, cxSource, cxSource, hdcMem, 0, 0, SRCCOPY) ;
			tie[nX][nY] = 0; //��¼����ط����Ϸ�ש��
		}	


		DeleteDC (hdcMem) ;
		ReleaseDC(hWnd, hdc);

		return 0;


	case WM_MBUTTONDOWN: //˫��̽��
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
					if ((flag[nX][nY] != lei[nX][nY]) && (tie[i][j] == 0)) //̧�м������ݷ�ש�ָ�
					{
						BitBlt (hdc, i * cxSource, j * cxSource, cxSource, cxSource, hdcMem, 0, 0 * cxSource, SRCCOPY) ;
					}

					else if ((flag[nX][nY] != lei[nX][nY]) && (tie[i][j] == PQUESTIONMARK)) //̧�м�������"?"�ָ�
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
								if((lei[x][y] == LEI) && ((tie[x][y] == 0) || (tie[x][y] == QUESTIONMARK))) //�޸ġ�һ���㵽�ף���ǰ��������Ӿ���ʧ��bug
								{
									BitBlt (hdc, x * cxSource, y * cxSource, cxSource, cxSource, hdcMem, 0, 5 * cxSource, SRCCOPY) ;
									tie[x][y] = LTIE;
								}

								else if((lei[x][y] != LEI) && (tie[x][y] == FLAG)) //�����ĵط��б�� 
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

		// **************����ƶ�********************
	case WM_MOUSEMOVE: 
		if(GameOver)
			return 0;

		hdc = GetDC(hWnd);
		hdcMem = CreateCompatibleDC (hdc) ;
		SelectObject (hdcMem, hBitmap) ;

		nX = ((LOWORD (lParam)) / cxSource);
		nY = ((HIWORD (lParam)) / cxSource);

		if (wParam & 0x1) //�����������˵��������£�ִ������
		{
			//ʹǰ�����ݵķ�ש̧��
			if ((lastxiaxianx != -1) && (lastxiaxiany != -1))
			{
				if (tie[lastxiaxianx][lastxiaxiany] == 0)
				{
					BitBlt (hdc, lastxiaxianx * cxSource, lastxiaxiany * cxSource, cxSource, cxSource, hdcMem, 0, 0 * cxSource, SRCCOPY) ;
				}
			}

			//ʹ��ǰ��ש����
			if (tie[nX][nY] == 0)
			{
				BitBlt (hdc, nX * cxSource, nY * cxSource, cxSource, cxSource, hdcMem, 0, 15 * cxSource, SRCCOPY) ;
				//����´���Ҫ̧��ķ�ש
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

	srand((unsigned int)time( NULL)); // ��ϵͳʱ�����������
	for (i = 0; i < 10; )
	{
		x = rand()%10; //����������׵ĺ�����
		y = rand()%10; //����������׵�������

		if( 0 == lei[x][y])
		{
			lei[x][y] = LEI; // �ɹ�����
			i++;                                    
		}
	}

	for(x = 0; x < 10; x++) //��ש�ϵ����֣�x,y��ʾ��ש�����꣩�Ǽ�Ҫ���Ÿ�����һ�飬�׵�λ���ٳ�����
	{
		for (y = 0; y < 10; y++) //��ש�ϵ������Ǽ�Ҫ���Ÿ�����һ�飬�׵�λ���ٳ�����
		{
			if (lei[x][y] == LEI) //�������������������ʾ��Χ�м����ף�//������׵Ļ�������������Ķ���
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
					if (0 == tie[i][n])  //�������ط�û�з���
					{
						BitBlt (hdc, i * cxSource, n * cxSource, cxSource, cxSource, hdcMem, 0, (15-lei[i][n] ) * cxSource, SRCCOPY) ;
						tie[i][n] = LTIE;  //��¼����ط�����

						if (lei[i][n] == 0)  //�������ط���Χû����
						{
							fanzhuan(i,n,cxSource,hdc,hdcMem);  //����������ط�����Χ
						}
					}
				}				
			}
		}
	}
}