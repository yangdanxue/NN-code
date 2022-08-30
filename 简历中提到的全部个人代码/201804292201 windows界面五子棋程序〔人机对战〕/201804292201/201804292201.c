/*--------------------------------------------------
LINEDEMO.C -- Line-Drawing Demonstration Program
(c) Charles Petzold, 1998
--------------------------------------------------*/

#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);


#define Chess_Board 48 //����ÿ�����ӵ�������
#define BLACK_COLOR 10 //������ɫ
#define WHITE_COLOR 20 //������ɫ
#define BOARD_SIZE 14 //����������


BOOL GameOver = FALSE;



static HBITMAP hBitmap;
static int  cxClient, cyClient, cxSource, cySource;
BITMAP         bitmap;
HINSTANCE      hInstance;
HDC            hdc, hdcMem;
PAINTSTRUCT ps;



int x = 0, y = 0; //������ĺ������꣨������ͳ�ƣ�
int i, j;
int Board[15][15] = {0};
int xBlack = 0, yBlack = 0; //����������������꣺xBlack�����ꣻyBlack������
int xWhite = 0, yWhite = 0; //����������������꣺xWhite�����ꣻyWhite������
int xBlackLast = -1, yBlackLast = -1; //�����ϴ����ӵ�λ�����꣬�У�xBlackLast���У�yBlackLast
int xWhiteLast = -1, yWhiteLast = -1; //�����ϴ����ӵ�λ�����꣬�У�xWhiteLast���У�yWhiteLast

int count = 0; //������
int BeginH, EndH;//�������Ӵ�����ʼ�ͽ�β��־
int BeginV, EndV;//�������Ӵ�����ʼ�ͽ�β��־
int BeginOTx, BeginOTy, EndOTx, EndOTy;//һ���������Ӵ�����ʼ�ͽ�β��־
int BeginTFx, BeginTFy, EndTFx, EndTFy;//�����������Ӵ�����ʼ�ͽ�β��־


void ChooseGameMode(); //ѡ����Ϸģʽ
void PeopleVSComputer(); //�˻���ս
void PeopleVSPeople(); //���˶�ս
void PeopleVSPeople_BlackPlay(); //���˶�ս_������
void PeopleVSPeople_WhitePlay(); //���˶�ս_������
void PeopleVSComputer_PeopleBlackPlay(); //�˻���ս_�˳ֺ���
void PeopleVSComputer_PeopleWhitePlay(); //�˻���ս_�˳ְ���
void PeopleVSComputer_ComputerBlackPlay(); //�˻���ս_���ֺ���
void PeopleVSComputer_ComputerWhitePlay(); //�˻���ս_���ְ���

void DisplayChess(); //��ʾ����
void ChessShape(); //�趨������״������ǰ����������״�����ǡ�/������һ����������״��Բ�Ρ�/��

void WhoIsWinning();
void WhoIsWin_Horizon(); //ˮƽ�����ж���Ӯ
void WhoIsWin_Vertical(); //��ֱ�����ж���Ӯ
void WhoIsWin_OneThreeQuadrant(); //һ�����޷����ж���Ӯ
void WhoIsWin_TwoFourQuadrant(); //�������޷����ж���Ӯ
void BanHand_3_Horizon(int); //ˮƽ����x++��y++���жϺ������
void BanHand_3_Vertical(int); //��ֱ����x++��y++���жϺ������
void BanHand_3_OneThreeQuadrant(int, int); //һ�����޷���x++��y++���жϺ������
void BanHand_3_TwoFourQuadrant(int, int); //�������޷���x++��y--���жϺ������
void BanHand_4_Horizon(int); //ˮƽ����x++��y++���жϺ������
void BanHand_4_Vertical(int); //��ֱ����x++��y++���жϺ������
void BanHand_4_OneThreeQuadrant(int, int); //һ�����޷���x++��y++���жϺ������
void BanHand_4_TwoFourQuadrant(int, int); //�������޷���x++��y--���жϺ������

void Score_H(int, int, int); //������
void Score_V(int, int, int); //������
void Score_OT(int, int, int, int); //һ�����޴��
void Score_TF(int, int, int, int); //�������޴��
void getScope_H_Black(int Line, int BeginH, int EndH, int scope); //����º����ֱ���
void getScope_H_White(int Line, int BeginH, int EndH, int scope); //����չ�����ֱ���
void getScope_V_Black(int Column, int BeginV, int EndV, int scope); //����º����ֱ���
void getScope_V_White(int Column, int BeginV, int EndV, int scope); //����չ�����ֱ���
void getScope_OT_Black(int, int, int, int, int, int, int); //һ�����޶º����ֱ���
void getScope_OT_White(int, int, int, int, int, int, int); //һ�����޷�չ�����ֱ���
void getScope_TF_Black(int, int, int, int, int, int, int); //�������޶º����ֱ���
void getScope_TF_White(int, int, int, int, int, int, int); //�������޷�չ�����ֱ���
void CleanScore(); //�����
void MaxScore(); //������������


int ChessCount_H(int Line, int Color);	//�����������
int ChessCount_V(int Column, int Color);	//�����������
int ChessCount_OT(int Line, int Column, int Color); //һ�����޼�������
int ChessCount_TF(int Line, int Column, int Color); //�������޼�������



struct Color SetColor(struct Color); //�趨������ɫ�������ɫ�Ľṹ�庯��

struct Color //�趨������ɫ�������ɫ�Ľṹ��
{
	int iChangeColor; //�趨������ɫ
	int iChessColor; //�趨�����ɫ
};
struct Color sColor = {1, 0}; //sColor�ṹ����ÿ����Ա�ĳ�ֵ������iChangeColor = 1; iChessColor = 0



struct MAX //�趨���ֵ����ṹ��
{
	int Value;
	int x;
	int y;
};
struct MAX max;



int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("LineDemo") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;


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

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("Program requires Windows NT!"), 
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hwnd = CreateWindow (szAppName, TEXT ("Line Demonstration"),
		WS_OVERLAPPED |WS_CAPTION |WS_SYSMENU| WS_MINIMIZEBOX,	
		CW_USEDEFAULT, 
		CW_USEDEFAULT,
		(Chess_Board / 2) + (14 * Chess_Board) + (65 * Chess_Board / 100), //�ͻ����ĳ�(��������1/2�����ӿ������Ҳ��1/2���ӿ�)
		(Chess_Board / 2) + (14 * Chess_Board) + (12 * Chess_Board / 11), //�ͻ����Ŀ�(�����Ϸ���1/2�����ӿ������·���1/2���ӿ�)
		NULL, 
		NULL,
		hInstance, 
		NULL) ;

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//////static HBITMAP hBitmap;
	//////static int  cxClient, cyClient, cxSource, cySource;
	//////BITMAP         bitmap;
	//////HINSTANCE      hInstance;
	//////HDC            hdc, hdcMem;
	//////PAINTSTRUCT ps;


	//////int x = 0, y = 0; //������ĺ������꣨������ͳ�ƣ�
	//////int xMemory, yMemory; //��¼�����������x��y

	static aaa[15][15]= {0};



	switch (message)
	{
	case WM_CREATE:
		hInstance = ((LPCREATESTRUCT) lParam)->hInstance ;

		hBitmap = LoadBitmap (hInstance, MAKEINTRESOURCE (IDB_BITMAP1)) ;

		GetObject (hBitmap, sizeof (BITMAP), &bitmap) ;

		cxSource = bitmap.bmWidth;
		cySource = bitmap.bmHeight / 6; //cySource�ǡ�����ͼƬ

		return 0 ;


	case WM_SIZE:
		cxClient = LOWORD (lParam) ;
		cyClient = HIWORD (lParam) ;
		return 0 ;


	case WM_PAINT:
		hdc = BeginPaint (hwnd, &ps) ;

		hdcMem = CreateCompatibleDC (hdc) ;
		SelectObject (hdcMem, hBitmap) ;

		for (j = Chess_Board / 2; j <= 15 * Chess_Board; j += Chess_Board)
			//////for (j = 15 * Chess_Board; j >= Chess_Board / 2; j -= Chess_Board)
		{
			for (i = Chess_Board / 2; i <= 15 * Chess_Board; i += Chess_Board)
			{
				MoveToEx  (hdc, i, j, NULL) ;
				LineTo    (hdc, 14 * Chess_Board, j) ;
			}
		}

		for (i = Chess_Board / 2; i <= 15 * Chess_Board; i += Chess_Board)
		{
			//////for (j = 15 * Chess_Board; j >= Chess_Board / 2; j -= Chess_Board)
			for (j = Chess_Board / 2; j <= 15 * Chess_Board; j += Chess_Board)
			{
				MoveToEx  (hdc, i, j, NULL) ;
				LineTo    (hdc, i, 14 * Chess_Board) ;
			}
		}



		//////for (y = 0 ; y <= BOARD_SIZE ; y++)
		for (j = BOARD_SIZE ; 0 <= j ; j--) //(0��0)���������½�
		{
			for (i = 0 ; i <= BOARD_SIZE ; i++)
			{
				if(aaa[j][i] == 1)
				{
					switch(Board[yBlack][xBlack] % 100)	//�����з�ֵ�����ʾ����
					{
					case BLACK_COLOR + 1:
						BitBlt (hdc, xBlack * Chess_Board, yBlack * Chess_Board, cxSource, cySource, hdcMem, 0, 0, SRCCOPY) ; //����ͼƬ,11
						break;

					case BLACK_COLOR + 2:
						BitBlt (hdc, xBlackLast * Chess_Board, yBlackLast * Chess_Board, cxSource, cySource, hdcMem, 0, 4 * cySource, SRCCOPY) ; //����WINͼƬ,12
						break;
					}


					switch(Board[yWhite][xWhite] % 100)	//�����з�ֵ�����ʾ����
					{
					case WHITE_COLOR + 1:
						BitBlt (hdc, xWhite * Chess_Board, yWhite * Chess_Board, cxSource, cySource, hdcMem, 0, cySource, SRCCOPY) ; //����ͼƬ,21
						break;

					case WHITE_COLOR + 2:
						BitBlt (hdc, xWhiteLast * Chess_Board, yWhiteLast * Chess_Board, cxSource, cySource, hdcMem, 0, 5 * cySource, SRCCOPY) ; //����WINͼƬ,22
						break;
					}


					switch(Board[yBlackLast][xBlackLast] % 100)	//�����з�ֵ�����ʾ����
					{
					case BLACK_COLOR:
						BitBlt (hdc, xBlackLast * Chess_Board, yBlackLast * Chess_Board, cxSource, cySource, hdcMem, 0, 2 * cySource, SRCCOPY) ; //����ͼƬ,10
						break;
					}


					switch(Board[yWhiteLast][xWhiteLast] % 100)	//�����з�ֵ�����ʾ����
					{
					case WHITE_COLOR:
						BitBlt (hdc, xWhiteLast * Chess_Board, yWhiteLast * Chess_Board, cxSource, cySource, hdcMem, 0, 3 * cySource, SRCCOPY) ; //����ͼƬ,20
						break;
					}
				}
			}
		}

		DeleteDC (hdcMem) ;
		EndPaint (hwnd, &ps) ;
		return 0 ;



	case WM_LBUTTONDOWN:
		if(GameOver)
			return 0;

		hdc = GetDC(hwnd);

		if (sColor.iChangeColor == 1) //����ֵ���������
		{
			xBlack = (LOWORD (lParam)) / cxSource ;
			yBlack = (HIWORD (lParam)) / cySource ;

			aaa[yBlack][xBlack] = 1;
			aaa[yBlackLast][xBlackLast] = 1;


			if (Board[yBlack][xBlack] % 100 == 0)
			{
				hdcMem = CreateCompatibleDC (hdc) ;
				SelectObject (hdcMem, hBitmap) ;
			}
			else
			{
				break;
			}
		}

		else if (sColor.iChangeColor == 2) //����ֵ���������
		{
			xWhite = (LOWORD (lParam)) / cxSource ;
			yWhite = (HIWORD (lParam)) / cySource ;

			aaa[yWhite][xWhite] = 1;
			aaa[yWhiteLast][xWhiteLast] = 1;


			if (Board[yWhite][xWhite] % 100 == 0)
			{
				hdcMem = CreateCompatibleDC (hdc) ;
				SelectObject (hdcMem, hBitmap) ;
			}
			else
			{
				break;
			}
		}



		if (sColor.iChangeColor == 1) //����Ǻ�������һ��������ͼƬ
		{
			Board[yBlack][xBlack] = BLACK_COLOR + 1;
			DisplayChess();

			if (xBlackLast != -1 && yBlackLast != -1)
			{
				Board[yBlackLast][xBlackLast] = BLACK_COLOR;
				DisplayChess();
			}
		}


		else if (sColor.iChangeColor == 2) //����ǰ�������һ��������ͼƬ
		{
			Board[yWhite][xWhite] = WHITE_COLOR + 1;
			DisplayChess();

			if (xWhiteLast != -1 && yWhiteLast != -1)
			{
				Board[yWhiteLast][xWhiteLast] = WHITE_COLOR;
				DisplayChess();
			}
		}

		sColor = SetColor(sColor); //�趨������ɫ�������ɫ

		ChessShape();

		WhoIsWinning();

		PeopleVSComputer_ComputerWhitePlay(); //���ְ������


		DeleteDC (hdcMem) ;
		ReleaseDC(hwnd, hdc);
		return 0;


	case WM_DESTROY:
		DeleteObject (hBitmap) ;
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}



//�˻���ս֮���֡�
void PeopleVSComputer_ComputerWhitePlay()
{
	//������Ϸ�������жϣ���Ϊ�˷�ֹ��������а����ʤ�󣬰����Ծɻ����ӡ�֮����Ϸ�����ˣ����岻����ʾ����WIN��ͼƬ
	if(GameOver)
		return 0;

	if (Board[yBlack][xBlack] = BLACK_COLOR + 1)
	{
		MaxScore(); //���ֵ��ߵ�λ��

		xWhite = max.x;
		yWhite = max.y;
	}


	if (sColor.iChangeColor == 2) //����ǰ�������һ��������ͼƬ
	{
		Board[yWhite][xWhite] = WHITE_COLOR + 1;
		DisplayChess();

		if (xWhiteLast != -1 && yWhiteLast != -1)
		{
			Board[yWhiteLast][xWhiteLast] = WHITE_COLOR;
			DisplayChess();
		}
	}

	sColor = SetColor(sColor); //�趨������ɫ�������ɫ

	ChessShape();

	WhoIsWinning();
}




//�趨������ɫ�������ɫ
struct Color SetColor(struct Color sColor)  
{
	if (sColor.iChangeColor == 1)
	{
		sColor.iChessColor = BLACK_COLOR + 1;
		if (xBlackLast != -1 && yBlackLast != -1) //�жϺ�������Ƿ�����һ�����ӣ��������if���
		{
			Board[yBlackLast][xBlackLast] = BLACK_COLOR; //��һ�������ɡ���Ϊ��
		}
		sColor.iChangeColor = 2; //******��ҽ���
	}
	else
	{
		sColor.iChessColor = WHITE_COLOR + 1;
		if (xWhiteLast != -1 && yWhiteLast != -1) //�жϰ�������Ƿ�����һ�����ӣ��������if���
		{
			Board[yWhiteLast][xWhiteLast] = WHITE_COLOR; //��һ�������ɡ���Ϊ��
		}
		sColor.iChangeColor = 1; //******��ҽ���
	}
	//*****�����趨���������ɫ
	return sColor;
}


//�趨������״������ǰ����������״�����ǡ�/������һ����������״��Բ�Ρ�/��
void ChessShape()
{
	if (sColor.iChessColor == BLACK_COLOR + 1) //�ѵ�ǰ����λ�����긳��ǰһ��λ��
	{
		xBlackLast = xBlack;
		yBlackLast = yBlack;
	} 
	else
	{
		xWhiteLast = xWhite;
		yWhiteLast = yWhite;
	}
}


//��ʾ����
void DisplayChess() 
{
	for (j = BOARD_SIZE ; 0 <= j ; j--) //(0��0)���������½�
	{
		for (i = 0 ; i <= BOARD_SIZE ; i++)
		{
			switch(Board[yBlack][xBlack] % 100)	//�����з�ֵ�����ʾ����
			{
			case BLACK_COLOR + 1:
				BitBlt (hdc, xBlack * Chess_Board, yBlack * Chess_Board, cxSource, cySource, hdcMem, 0, 0, SRCCOPY) ; //����ͼƬ,11
				break;

			case BLACK_COLOR + 2:
				BitBlt (hdc, xBlackLast * Chess_Board, yBlackLast * Chess_Board, cxSource, cySource, hdcMem, 0, 4 * cySource, SRCCOPY) ; //����WINͼƬ,12
				break;
			}


			switch(Board[yWhite][xWhite] % 100)	//�����з�ֵ�����ʾ����
			{
			case WHITE_COLOR + 1:
				BitBlt (hdc, xWhite * Chess_Board, yWhite * Chess_Board, cxSource, cySource, hdcMem, 0, cySource, SRCCOPY) ; //����ͼƬ,21
				break;

			case WHITE_COLOR + 2:
				BitBlt (hdc, xWhiteLast * Chess_Board, yWhiteLast * Chess_Board, cxSource, cySource, hdcMem, 0, 5 * cySource, SRCCOPY) ; //����WINͼƬ,22
				break;
			}


			switch(Board[yBlackLast][xBlackLast] % 100)	//�����з�ֵ�����ʾ����
			{
			case BLACK_COLOR:
				BitBlt (hdc, xBlackLast * Chess_Board, yBlackLast * Chess_Board, cxSource, cySource, hdcMem, 0, 2 * cySource, SRCCOPY) ; //����ͼƬ,10
				break;
			}


			switch(Board[yWhiteLast][xWhiteLast] % 100)	//�����з�ֵ�����ʾ����
			{
			case WHITE_COLOR:
				BitBlt (hdc, xWhiteLast * Chess_Board, yWhiteLast * Chess_Board, cxSource, cySource, hdcMem, 0, 3 * cySource, SRCCOPY) ; //����ͼƬ,20
				break;
			}
		}
	}
}



//�����������
int ChessCount_H(int Line, int Color)
{
	while(((Board[Line][BeginH] % 100) != Color) && ((Board[Line][BeginH] % 100) != Color + 1)) //�����Ǹ���ɫ���ӵĵ�
	{
		if (BeginH > BOARD_SIZE) //���߽���
		{
			EndH = BeginH;

			return 0;
		}
		BeginH ++;
	}

	EndH = BeginH + 1; //β���Ƶ�ʼ�����һλ

	//��ʼ��������
	while((Board[Line][EndH] % 100 == Color) || (Board[Line][EndH] % 100 == Color + 1))//+1���������ε�������ɫ
	{
		EndH ++;
	}


	//*******������������Ӵ��м���1���ո������*******

	//����[�񡡡�]��������ú������������ͬ��
	if (
		(EndH - BeginH == 1) 
		&& (Board[Line][EndH] % 100 <= 9) 
		&& ((Board[Line][EndH + 1] % 100 == Color) || (Board[Line][EndH + 1] % 100 == Color + 1)) 
		&& ((Board[Line][EndH + 2] % 100 != Color) && (Board[Line][EndH + 2] % 100 != Color + 1)) 
		)
	{
		EndH = EndH + 2;

		return EndH - BeginH - 1; //�������Ӵ�����
	}

	//����[��񡡡�]�����
	else if (
		(EndH - BeginH == 2) 
		&& (Board[Line][EndH] % 100 <= 9) 
		&& ((Board[Line][EndH + 1] % 100 == Color) || (Board[Line][EndH + 1] % 100 == Color + 1))
		&& ((Board[Line][EndH + 2] % 100 != Color) && (Board[Line][EndH + 2] % 100 != Color + 1)) 
		)
	{
		EndH = EndH + 2;

		return EndH - BeginH - 1; //�������Ӵ�����
	}

	//����[���񡡡�]�����
	else if (
		(EndH - BeginH == 3) 
		&& (Board[Line][EndH] % 100 <= 9) 
		&& ((Board[Line][EndH + 1] % 100 == Color) || (Board[Line][EndH + 1] % 100 == Color + 1))
		&& ((Board[Line][EndH + 2] % 100 != Color) && (Board[Line][EndH + 2] % 100 != Color + 1)) 
		)
	{
		EndH = EndH + 2;

		return EndH - BeginH - 1; //�������Ӵ�����
	}

	//����[�񡡡��]�����
	else if (
		(EndH - BeginH == 1) 
		&& (Board[Line][EndH] % 100 <= 9)
		&& ((Board[Line][EndH + 1] % 100 == Color) || (Board[Line][EndH + 1] % 100 == Color + 1))
		&& ((Board[Line][EndH + 2] % 100 == Color) || (Board[Line][EndH + 2] % 100 == Color + 1))
		&& ((Board[Line][EndH + 3] % 100 != Color) && (Board[Line][EndH + 3] % 100 != Color + 1)) 
		)
	{
		EndH = EndH + 3;

		return EndH - BeginH - 1; //�������Ӵ�����
	}

	//����[��񡡡��]�����
	else if (
		(EndH - BeginH == 2)
		&& (Board[Line][EndH] % 100 <= 9)
		&& ((Board[Line][EndH + 1] % 100 == Color) || (Board[Line][EndH + 1] % 100 == Color + 1))
		&& ((Board[Line][EndH + 2] % 100 == Color) || (Board[Line][EndH + 2] % 100 == Color + 1))
		&& ((Board[Line][EndH + 3] % 100 != Color) && (Board[Line][EndH + 3] % 100 != Color + 1)) 
		)
	{
		EndH = EndH + 3;

		return EndH - BeginH - 1; //�������Ӵ�����
	}

	//����[�񡡡���]�����
	else if (
		(EndH - BeginH == 1) 
		&& (Board[Line][EndH] % 100 <= 9)
		&& ((Board[Line][EndH + 1] % 100 == Color) || (Board[Line][EndH + 1] % 100 == Color + 1))
		&& ((Board[Line][EndH + 2] % 100 == Color) || (Board[Line][EndH + 2] % 100 == Color + 1))
		&& ((Board[Line][EndH + 3] % 100 == Color) || (Board[Line][EndH + 3] % 100 == Color + 1))
		&& ((Board[Line][EndH + 4] % 100 != Color) && (Board[Line][EndH + 4] % 100 != Color + 1)) 
		)
	{
		EndH = EndH + 4;

		return EndH - BeginH - 1; //�������Ӵ�����
	}

	return EndH - BeginH; //�������Ӵ�����
}


//�����������
int ChessCount_V(int Column, int Color)
{
	while(((Board[BeginV][Column] % 100) != Color) && ((Board[BeginV][Column] % 100) != Color + 1)) //�����Ǹ���ɫ���ӵĵ�
	{
		if (BeginV > BOARD_SIZE) //���߽���
		{
			EndV = BeginV;

			return 0;
		}
		BeginV++;
	}

	EndV = BeginV + 1; //β���Ƶ�ʼ�����һλ

	//��ʼ��������
	while((Board[EndV][Column] % 100 == Color) || (Board[EndV][Column] % 100 == Color + 1))//+1���������ε�������ɫ
	{
		EndV ++;
	}


	//*******������������Ӵ��м���1���ո������*******

	//����[�񡡡�]��������ú������������ͬ��
	if (
		(EndV - BeginV == 1) 
		&& (Board[EndV][Column] % 100 <= 9) 
		&& ((Board[EndV + 1][Column] % 100 == Color) || (Board[EndV + 1][Column] % 100 == Color + 1)) 
		&& ((Board[EndV + 2][Column] % 100 != Color) && (Board[EndV + 2][Column] % 100 != Color + 1)) 
		)
	{
		EndV = EndV + 2;

		return EndV - BeginV - 1; //�������Ӵ�����
	}

	//����[��񡡡�]�����
	else if (
		(EndV - BeginV == 2) 
		&& (Board[EndV][Column] % 100 <= 9) 
		&& ((Board[EndV + 1][Column] % 100 == Color) || (Board[EndV + 1][Column] % 100 == Color + 1))
		&& ((Board[EndV + 2][Column] % 100 != Color) && (Board[EndV + 2][Column] % 100 != Color + 1)) 
		)
	{
		EndV = EndV + 2;

		return EndV - BeginV - 1; //�������Ӵ�����
	}

	//����[���񡡡�]�����
	else if (
		(EndV - BeginV == 3) 
		&& (Board[EndV][Column] % 100 <= 9) 
		&& ((Board[EndV + 1][Column] % 100 == Color) || (Board[EndV + 1][Column] % 100 == Color + 1))
		&& ((Board[EndV + 2][Column] % 100 != Color) && (Board[EndV + 2][Column] % 100 != Color + 1)) 
		)
	{
		EndV = EndV + 2;

		return EndV - BeginV - 1; //�������Ӵ�����
	}

	//����[�񡡡��]�����
	else if (
		(EndV - BeginV == 1) 
		&& (Board[EndV][Column] % 100 <= 9) 
		&& ((Board[EndV + 1][Column] % 100 == Color) || (Board[EndV + 1][Column] % 100 == Color + 1))
		&& ((Board[EndV + 2][Column] % 100 == Color) || (Board[EndV + 2][Column] % 100 == Color + 1))
		&& ((Board[EndV + 3][Column] % 100 != Color) && (Board[EndV + 3][Column] % 100 != Color + 1)) 
		)
	{
		EndV = EndV + 3;

		return EndV - BeginV - 1; //�������Ӵ�����
	}

	//����[��񡡡��]�����
	else if (
		(EndV - BeginV == 2)
		&& (Board[EndV][Column] % 100 <= 9)
		&& ((Board[EndV + 1][Column] % 100 == Color) || (Board[EndV + 1][Column] % 100 == Color + 1))
		&& ((Board[EndV + 2][Column] % 100 == Color) || (Board[EndV + 2][Column] % 100 == Color + 1))
		&& ((Board[EndV + 3][Column] % 100 != Color) && (Board[EndV + 3][Column] % 100 != Color + 1)) 
		)
	{
		EndV = EndV + 3;

		return EndV - BeginV - 1; //�������Ӵ�����
	}

	//����[�񡡡���]�����
	else if (
		(EndV - BeginV == 1) 
		&& (Board[EndV][Column] % 100 <= 9)
		&& ((Board[EndV + 1][Column] % 100 == Color) || (Board[EndV + 1][Column] % 100 == Color + 1))
		&& ((Board[EndV + 2][Column] % 100 == Color) || (Board[EndV + 2][Column] % 100 == Color + 1))
		&& ((Board[EndV + 3][Column] % 100 == Color) || (Board[EndV + 3][Column] % 100 == Color + 1))
		&& ((Board[EndV + 4][Column] % 100 != Color) && (Board[EndV + 4][Column] % 100 != Color + 1)) 
		)
	{
		EndV = EndV + 4;

		return EndV - BeginV - 1; //�������Ӵ�����
	}

	return EndV - BeginV; //�������Ӵ�����
}


//һ�����޼�����������д��
int ChessCount_OT(int Line, int Column, int Color)
{
	while ((Column > BOARD_SIZE) || (Column < 0))//�����������̵Ĳ���
	{
		Column++;
		Line++;
	}
	BeginOTx = Column;
	BeginOTy = Line;

	while(((Board[BeginOTy][BeginOTx] % 100) != Color) 
		&&((Board[BeginOTy][BeginOTx] % 100) != Color + 1)) //�����Ǹ���ɫ���ӵĵ�
	{
		if ((BeginOTx > BOARD_SIZE) || (BeginOTy > BOARD_SIZE) || (BeginOTx < 0) || (BeginOTy < 0)) //���߽���
		{
			EndOTx = BeginOTx;
			EndOTy = BeginOTy;

			return 0;
		}
		BeginOTx ++;
		BeginOTy ++;
	}
	//����������������߽���ô�죿
	EndOTx = BeginOTx + 1; //β���Ƶ�ʼ�����һλ
	EndOTy = BeginOTy + 1; //β���Ƶ�ʼ�����һλ

	//��ʼ��������
	while((Board[EndOTy][EndOTx] % 100 == Color) || (Board[EndOTy][EndOTx] % 100 == Color + 1))//+1���������ε�������ɫ
	{
		EndOTx ++;
		EndOTy ++;
	}


	//*******������������Ӵ��м���1���ո������*******

	//����[�񡡡�]��������ú������������ͬ��
	if (
		(EndOTx - BeginOTx == 1) 
		&& (Board[EndOTy][EndOTx] % 100 <= 9) 
		&& ((Board[EndOTy + 1][EndOTx + 1] % 100 == Color) || (Board[EndOTy + 1][EndOTx + 1] % 100 == Color + 1)) 
		&& ((Board[EndOTy + 2][EndOTx + 2] % 100 != Color) && (Board[EndOTy + 2][EndOTx + 2] % 100 != Color + 1)) 
		)
	{
		EndOTx = EndOTx + 2;
		EndOTy = EndOTy + 2;

		return EndOTx - BeginOTx - 1; //�������Ӵ�����
	}

	//����[��񡡡�]�����
	else if (
		(EndOTx - BeginOTx == 2) 
		&& (Board[EndOTy][EndOTx] % 100 <= 9) 
		&& ((Board[EndOTy + 1][EndOTx + 1] % 100 == Color) || (Board[EndOTy + 1][EndOTx + 1] % 100 == Color + 1))
		&& ((Board[EndOTy + 2][EndOTx + 2] % 100 != Color) && (Board[EndOTy + 2][EndOTx + 2] % 100 != Color + 1)) 
		)
	{
		EndOTx = EndOTx + 2;
		EndOTy = EndOTy + 2;

		return EndOTx - BeginOTx - 1; //�������Ӵ�����
	}

	//����[���񡡡�]�����
	else if (
		(EndOTx - BeginOTx == 3) 
		&& (Board[EndOTy][EndOTx] % 100 <= 9) 
		&& ((Board[EndOTy + 1][EndOTx + 1] % 100 == Color) || (Board[EndOTy + 1][EndOTx + 1] % 100 == Color + 1))
		&& ((Board[EndOTy + 2][EndOTx + 2] % 100 != Color) && (Board[EndOTy + 2][EndOTx + 2] % 100 != Color + 1)) 
		)
	{
		EndOTx = EndOTx + 2;
		EndOTy = EndOTy + 2;

		return EndOTx - BeginOTx - 1; //�������Ӵ�����
	}

	//����[�񡡡��]�����
	else if (
		(EndOTx - BeginOTx == 1) 
		&& (Board[EndOTy][EndOTx] % 100 <= 9)
		&& ((Board[EndOTy + 1][EndOTx + 1] % 100 == Color) || (Board[EndOTy + 1][EndOTx + 1] % 100 == Color + 1))
		&& ((Board[EndOTy + 2][EndOTx + 2] % 100 == Color) || (Board[EndOTy + 2][EndOTx + 2] % 100 == Color + 1))
		&& ((Board[EndOTy + 3][EndOTx + 3] % 100 != Color) && (Board[EndOTy + 3][EndOTx + 3] % 100 != Color + 1)) 
		)
	{
		EndOTx = EndOTx + 3;
		EndOTy = EndOTy + 3;

		return EndOTx - BeginOTx - 1; //�������Ӵ�����
	}

	//����[��񡡡��]�����
	else if (
		(EndOTx - BeginOTx == 2)
		&& (Board[EndOTy][EndOTx] % 100 <= 9)
		&& ((Board[EndOTy + 1][EndOTx + 1] % 100 == Color) || (Board[EndOTy + 1][EndOTx + 1] % 100 == Color + 1))
		&& ((Board[EndOTy + 2][EndOTx + 2] % 100 == Color) || (Board[EndOTy + 2][EndOTx + 2] % 100 == Color + 1))
		&& ((Board[EndOTy + 3][EndOTx + 3] % 100 != Color) && (Board[EndOTy + 3][EndOTx + 3] % 100 != Color + 1)) 
		)
	{
		EndOTx = EndOTx + 3;
		EndOTy = EndOTy + 3;

		return EndOTx - BeginOTx - 1; //�������Ӵ�����
	}

	//����[�񡡡���]�����
	else if (
		(EndOTx - BeginOTx == 1) 
		&& (Board[EndOTy][EndOTx] % 100 <= 9)
		&& ((Board[EndOTy + 1][EndOTx + 1] % 100 == Color) || (Board[EndOTy + 1][EndOTx + 1] % 100 == Color + 1))
		&& ((Board[EndOTy + 2][EndOTx + 2] % 100 == Color) || (Board[EndOTy + 2][EndOTx + 2] % 100 == Color + 1))
		&& ((Board[EndOTy + 3][EndOTx + 3] % 100 == Color) || (Board[EndOTy + 3][EndOTx + 3] % 100 == Color + 1))
		&& ((Board[EndOTy + 4][EndOTx + 4] % 100 != Color) && (Board[EndOTy + 4][EndOTx + 4] % 100 != Color + 1)) 
		)
	{
		EndOTx = EndOTx + 4;
		EndOTy = EndOTy + 4;

		return EndOTx - BeginOTx - 1; //�������Ӵ�����
	}

	return EndOTx - BeginOTx; //�������Ӵ�����
}

//�������޼�����������д��
int ChessCount_TF(int Line, int Column, int Color)
{
	while (Column < 0) //�����������̵Ĳ��֣�������ࣩ
	{
		Column++;
		Line--;
	}
	BeginTFx = Column;
	BeginTFy = Line;

	while (Column > BOARD_SIZE) //�����������̵Ĳ��֣������Ҳࣩ
	{
		EndTFx = BeginTFx;
		EndTFy = BeginTFy;

		return 0;
	}


	while(((Board[BeginTFy][BeginTFx] % 100) != Color) 
		&& ((Board[BeginTFy][BeginTFx] % 100) != Color + 1)) //�����Ǹ���ɫ���ӵĵ�
	{
		if ((BeginTFx > BOARD_SIZE) || (BeginTFy > BOARD_SIZE) || (BeginTFx < 0) || (BeginTFy < 0)) //���߽���
		{
			EndTFx = BeginTFx;
			EndTFy = BeginTFy;

			return 0;
		}
		BeginTFx ++;
		BeginTFy --;
	}
	//����������������߽���ô�죿
	EndTFx = BeginTFx + 1; //β���Ƶ�ʼ�����һλ
	EndTFy = BeginTFy - 1; //β���Ƶ�ʼ�����һλ

	//��ʼ��������
	while((Board[EndTFy][EndTFx] % 100 == Color) || (Board[EndTFy][EndTFx] % 100 == Color + 1))//+1���������ε�������ɫ
	{
		EndTFx ++;
		EndTFy --;
	}


	//*******������������Ӵ��м���1���ո������*******

	//����[�񡡡�]��������ú������������ͬ��
	if (
		(EndTFx - BeginTFx == 1) 
		&& (Board[EndTFy][EndTFx] % 100 <= 9) 
		&& ((Board[EndTFy - 1][EndTFx + 1] % 100 == Color) || (Board[EndTFy - 1][EndTFx + 1] % 100 == Color + 1)) 
		&& ((Board[EndTFy - 2][EndTFx + 2] % 100 != Color) && (Board[EndTFy - 2][EndTFx + 2] % 100 != Color + 1)) 
		)
	{
		EndTFx = EndTFx + 2;
		EndTFy = EndTFy - 2;

		return EndTFx - BeginTFx - 1; //�������Ӵ�����
	}

	//����[��񡡡�]�����
	else if (
		(EndTFx - BeginTFx == 2) 
		&& (Board[EndTFy][EndTFx] % 100 <= 9) 
		&& ((Board[EndTFy - 1][EndTFx + 1] % 100 == Color) || (Board[EndTFy - 1][EndTFx + 1] % 100 == Color + 1))
		&& ((Board[EndTFy - 2][EndTFx + 2] % 100 != Color) && (Board[EndTFy - 2][EndTFx + 2] % 100 != Color + 1)) 
		)
	{
		EndTFx = EndTFx + 2;
		EndTFy = EndTFy - 2;

		return EndTFx - BeginTFx - 1; //�������Ӵ�����
	}

	//����[���񡡡�]�����
	else if (
		(EndTFx - BeginTFx == 3) 
		&& (Board[EndTFy][EndTFx] % 100 <= 9) 
		&& ((Board[EndTFy - 1][EndTFx + 1] % 100 == Color) || (Board[EndTFy - 1][EndTFx + 1] % 100 == Color + 1))
		&& ((Board[EndTFy - 2][EndTFx + 2] % 100 != Color) && (Board[EndTFy - 2][EndTFx + 2] % 100 != Color + 1)) 
		)
	{
		EndTFx = EndTFx + 2;
		EndTFy = EndTFy - 2;

		return EndTFx - BeginTFx - 1; //�������Ӵ�����
	}

	//����[�񡡡��]�����
	//(���ڶ��������жϵķ�����x++��y--�����ԣ�Ϊ������������޽����жϵ�forѭ��������ֻ��һ���ж����)
	else if (
		(EndTFx - BeginTFx == 1)
		&& (Board[EndTFy][EndTFx] % 100 <= 9)
		&& ((Board[EndTFy - 1][EndTFx + 1] % 100 == Color) || (Board[EndTFy - 1][EndTFx + 1] % 100 == Color + 1))
		&& ((Board[EndTFy - 2][EndTFx + 2] % 100 == Color) || (Board[EndTFy - 2][EndTFx + 2] % 100 == Color + 1))
		&& ((Board[EndTFy - 3][EndTFx + 3] % 100 != Color) && (Board[EndTFy - 3][EndTFx + 3] % 100 != Color + 1)) 
		)
	{
		EndTFx = EndTFx + 3;
		EndTFy = EndTFy - 3;

		return EndTFx - BeginTFx - 1; //�������Ӵ�����
	}

	//����[��񡡡��]�����
	else if (
		(EndTFx - BeginTFx == 2)
		&& (Board[EndTFy][EndTFx] % 100 <= 9)
		&& ((Board[EndTFy - 1][EndTFx + 1] % 100 == Color) || (Board[EndTFy - 1][EndTFx + 1] % 100 == Color + 1))
		&& ((Board[EndTFy - 2][EndTFx + 2] % 100 == Color) || (Board[EndTFy - 2][EndTFx + 2] % 100 == Color + 1))
		&& ((Board[EndTFy - 3][EndTFx + 3] % 100 != Color) && (Board[EndTFy - 3][EndTFx + 3] % 100 != Color + 1)) 
		)
	{
		EndTFx = EndTFx + 3;
		EndTFy = EndTFy - 3;

		return EndTFx - BeginTFx - 1; //�������Ӵ�����
	}

	//����[�񡡡���]�����
	//(���ڶ��������жϵķ�����x++��y--�����ԣ�Ϊ������������޽����жϵ�forѭ��������ֻ��һ���ж����)
	else if (
		(EndTFx - BeginTFx == 1)
		&& (Board[EndTFy][EndTFx] % 100 <= 9)
		&& ((Board[EndTFy - 1][EndTFx + 1] % 100 == Color) || (Board[EndTFy - 1][EndTFx + 1] % 100 == Color + 1))
		&& ((Board[EndTFy - 2][EndTFx + 2] % 100 == Color) || (Board[EndTFy - 2][EndTFx + 2] % 100 == Color + 1))
		&& ((Board[EndTFy - 3][EndTFx + 3] % 100 == Color) || (Board[EndTFy - 3][EndTFx + 3] % 100 == Color + 1)) 
		&& ((Board[EndTFy - 4][EndTFx + 4] % 100 != Color) && (Board[EndTFy - 4][EndTFx + 4] % 100 != Color + 1)) 
		)
	{
		EndTFx = EndTFx + 4;
		EndTFy = EndTFy - 4;

		return EndTFx - BeginTFx - 1; //�������Ӵ�����
	}

	return EndTFx - BeginTFx; //�������Ӵ�����
}


//�ж���Ӯ
void WhoIsWinning()
{
	int x = 0, y = 0; //���̵ĺ�������

	CleanScore();

	WhoIsWin_Horizon(); // һ��һ��ѭ���ж�

	WhoIsWin_Vertical(); // һ��һ��ѭ���ж�

	WhoIsWin_OneThreeQuadrant(); // һ������ѭ���жϣ���д��

	WhoIsWin_TwoFourQuadrant(); // ��������ѭ���жϣ���д��
}


//ˮƽ�����ж���Ӯ
void WhoIsWin_Horizon()
{
	int x = 0, y = 0; //���̵ĺ�������

	for(y = 0; y <= BOARD_SIZE; y++)
	{
		//"BeginH = EndH + 1"�����������������
		for (BeginH = 0, EndH = 0; (BeginH <= BOARD_SIZE) && (EndH <= BOARD_SIZE); BeginH = EndH + 1)
		{
			count = ChessCount_H(y, BLACK_COLOR);

			//���ֵĹ涨���ڷ����������ͬʱ�γɣ�����ʧЧ���ڷ�ʤ�����ԣ�ֻҪ������������ֶ����뿼��
			if (count == 5)
			{
				Board[yBlack][xBlack] = BLACK_COLOR + 2;

				GameOver = TRUE;

				DisplayChess();
			}

			Score_H(y, count, BLACK_COLOR);//����ԭ�򣺷�ֹ�жϽ��ֺ󣬽�������countֵ��Ϊ���ֵ�countֵ������û�а�������������ֵ���
		}


		//�ж��������ֵ�˼·��
		//******* 1����򵥵������������Σ�����2�����������������ӣ��Һ���һ������ͬʱ�γ��������������ӱ���Ϊ����������ͬ�Ĺ����ӡ�
		//******* 2�����ԣ�ֻ�����ж�����1���������Ƿ��������ӣ����У����漴�ж�(yBlack, xBlack)���ڵ��������������Ƿ��������ӡ�
		//******* 3����Ϊ���������˳��������-����-��һ��-����ķ���ͳ����������
		//******* 4������ֻ������һ��һ���жϵ��ص㣬���жϳ�1�����������������Ӻ������ж�����3�������Ƿ��������Ӽ��ɡ�


		//��֤(yBlack, xBlack)���������ֵĵ�һ����
		if (EndH - 1 <= BOARD_SIZE)
		{
			BeginH = xBlack - 4;
			EndH = xBlack - 4;

			count = ChessCount_H(yBlack, BLACK_COLOR); //����������

			//���������ӵ����˾��ް��ӣ��Ҳ��������̱�Ե
			if ((count == 3)
				&& (Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR) 
				&& (Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR + 1)
				&& (Board[yBlack][EndH] % 100 != WHITE_COLOR) 
				&& (Board[yBlack][EndH] % 100 != WHITE_COLOR + 1)
				&& (BeginH != 0) && (BeginH != BOARD_SIZE) 
				&& (EndH - 1 != 0) && (EndH - 1 != BOARD_SIZE) 
				&& (yBlack != 0) && (yBlack != BOARD_SIZE)) 
			{
				BanHand_3_Vertical(y); //��ֱ����x++��y++���жϺ������

				BanHand_3_OneThreeQuadrant(x, y); //һ�����޷���x++��y++���жϺ������

				BanHand_3_TwoFourQuadrant(x, y); //�������޷���x++��y--���жϺ������
			}

			else if ((count == 4)
				&& 
				(((Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR) 
				&& (Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR + 1))
				|| 
				((Board[yBlack][EndH] % 100 != WHITE_COLOR) 
				&& (Board[yBlack][EndH] % 100 != WHITE_COLOR + 1))))
			{
				BanHand_4_Vertical(y); //��ֱ����x++��y++���жϺ������

				BanHand_4_OneThreeQuadrant(x, y); //һ�����޷���x++��y++���жϺ������

				BanHand_4_TwoFourQuadrant(x, y); //�������޷���x++��y--���жϺ������
			}

			else if (count > 5)
			{
				Board[yWhite][xWhite] = WHITE_COLOR + 2;

				DisplayChess();

				GameOver = TRUE;


				//////printf("���峤���������ʤ��\n");
			}

			else
			{
				continue;
			}
		}
	}

	for(y = 0; y <= BOARD_SIZE; y++)
	{
		//"BeginH = EndH + 1"�����������������
		for (BeginH = 0, EndH = 0; (BeginH <= BOARD_SIZE) && (EndH <= BOARD_SIZE); BeginH = EndH + 1)
		{
			count = ChessCount_H(y, WHITE_COLOR);
			if (count == 5)
			{	
				Board[yWhite][xWhite] = WHITE_COLOR + 2;

				DisplayChess();

				GameOver = TRUE;
			}
			Score_H(y, count, WHITE_COLOR);
		}
	}
}


//��ֱ�����ж���Ӯ
void WhoIsWin_Vertical()
{
	int x = 0, y = 0; //���̵ĺ�������

	for(x = 0; x <= BOARD_SIZE; x++)
	{
		//[BeginV = EndV + 1]:���㵱ǰ�е�����������ĺ���������
		for (BeginV = 0, EndV = 0; (BeginV <= BOARD_SIZE) && (EndV <= BOARD_SIZE); BeginV = EndV + 1)
		{
			count = ChessCount_V(x, BLACK_COLOR);


			if (count == 5) //�ڷ����������ͬʱ�γɣ�����ʧЧ���ڷ�ʤ����Ϸ����
			{		
				Board[yBlack][xBlack] = BLACK_COLOR + 2;

				DisplayChess();

				GameOver = TRUE;
			}
			//����ԭ�򣺷�ֹ�жϽ��ֺ󣬽�������countֵ��Ϊ���ֵ�countֵ������û�а�������������ֵ���
			Score_V(x, count, BLACK_COLOR);
		}

		//��֤(yBlack, xBlack)���������ֵĵ�һ����
		if (EndV - 1 <= BOARD_SIZE)
		{
			BeginV = yBlack - 4;
			EndV = yBlack - 4;

			count = ChessCount_V(xBlack, BLACK_COLOR);

			//���������ӵ����˾��ް��ӣ��Ҳ��������̱�Ե���ڶ���������
			//�����һ������(yBlack, xBlack)�����������Ľ��㴦
			if ((count == 3)
				&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR) 
				&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR + 1)
				&& (Board[EndV][xBlack] % 100 != WHITE_COLOR) 
				&& (Board[EndV][xBlack] % 100 != WHITE_COLOR + 1)
				&& (BeginV != 0) && (BeginV != BOARD_SIZE) 
				&& (EndV - 1 != 0) && (EndV - 1 != BOARD_SIZE) 
				&& (xBlack != 0) && (xBlack != BOARD_SIZE)) 
			{
				BanHand_3_Horizon(x); //ˮƽ����x++��y++���жϺ������

				BanHand_3_OneThreeQuadrant(x, y); //һ�����޷���x++��y++���жϺ������

				BanHand_3_TwoFourQuadrant(x, y); //�������޷���x++��y--���жϺ������
			}

			else if ((count == 4)
				&& 
				(((Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR) 
				&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR + 1))
				|| 
				((Board[EndV][xBlack] % 100 != WHITE_COLOR) 
				&& (Board[EndV][xBlack] % 100 != WHITE_COLOR + 1))))
			{
				BanHand_4_Horizon(x); //ˮƽ����x++��y++���жϺ������

				BanHand_4_OneThreeQuadrant(x, y); //һ�����޷���x++��y++���жϺ������

				BanHand_4_TwoFourQuadrant(x, y); //�������޷���x++��y--���жϺ������
			}

			else if (count > 5)
			{
				Board[yWhite][xWhite] = WHITE_COLOR + 2;

				DisplayChess();

				GameOver = TRUE;

				////printf("���峤���������ʤ��\n");
			}

			else
			{
				continue;
			}
		}
	}



	for(x = 0; x <= BOARD_SIZE; x++)
	{
		//"BeginV = EndV + 1"�����������������
		for (BeginV = 0, EndV = 0; (BeginV <= BOARD_SIZE) && (EndV <= BOARD_SIZE); BeginV = EndV + 1)
		{
			count = ChessCount_V(x, WHITE_COLOR);
			if (count == 5)
			{		
				Board[yWhite][xWhite] = WHITE_COLOR + 2;

				DisplayChess();

				GameOver = TRUE;
			}
			Score_V(x, count, WHITE_COLOR);
		}
	}
}


//һ�����޷����ж���Ӯ
void WhoIsWin_OneThreeQuadrant() //һ�����޷����ж���Ӯ
{
	int x = 0, y = 0; //���̵ĺ�������

	for(x = -14, y = 0; (x <= BOARD_SIZE) && (0 <= y); x++) //x++��y++������һ�ж�
	{
		for (BeginOTx = x, BeginOTy = y, EndOTx = x, EndOTy = y; 
			(-14 <= BeginOTx) && (BeginOTx <= BOARD_SIZE) 
			&& 
			(-14 <= EndOTx) && (EndOTx <= BOARD_SIZE) 
			&& 
			(0 <= BeginOTy) && (BeginOTy <= BOARD_SIZE)  
			&& 
			(0 <= EndOTy) && (EndOTy <= BOARD_SIZE);
		(BeginOTx = EndOTx + 1), (BeginOTy = EndOTy + 1)) //β���Ƶ�ʼ�����һλ
		{
			count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

			if (count == 5)
			{					
				Board[yBlack][xBlack] = BLACK_COLOR + 2;

				DisplayChess();

				GameOver = TRUE;
			}
			//����ԭ�򣺷�ֹ�жϽ��ֺ󣬽�������countֵ��Ϊ���ֵ�countֵ������û�а�������������ֵ��֣��Ӷ�������������߷ִ�
			Score_OT(BeginOTy, BeginOTx, count, BLACK_COLOR);
		}


		if ((EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE))
		{
			BeginOTx = xBlack - 4; 
			BeginOTy = yBlack - 4; 
			EndOTx = xBlack - 4; 
			EndOTy = yBlack - 4;

			count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

			//һ�����޷��������ӵ����˾��ް��ӣ��Ҳ��������̱�Ե
			if ((count == 3)
				&& (Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR) 
				&& (Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR + 1)
				&& (Board[EndOTy][EndOTx] % 100 != WHITE_COLOR) 
				&& (Board[EndOTy][EndOTx] % 100 != WHITE_COLOR + 1)
				&& (BeginOTy != 0) && (BeginOTy != BOARD_SIZE) 
				&& (BeginOTx != 0) && (BeginOTx != BOARD_SIZE)
				&& (EndOTy - 1 != 0) && (EndOTy - 1 != BOARD_SIZE)  
				&& (EndOTx - 1 != 0) && (EndOTx - 1 != BOARD_SIZE))
			{
				BanHand_3_Horizon(x); //ˮƽ����x++��y++���жϺ������

				BanHand_3_Vertical(y); //��ֱ����x++��y++���жϺ������

				BanHand_3_TwoFourQuadrant(x, y); //�������޷���x++��y--���жϺ������
			}

			else if ((count == 4)
				&& 
				(((Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR) 
				&& (Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR + 1))
				|| 
				((Board[EndOTy][EndOTx] % 100 != WHITE_COLOR) 
				&& (Board[EndOTy][EndOTx] % 100 != WHITE_COLOR + 1))))
			{
				BanHand_4_Horizon(x); //ˮƽ����x++��y++���жϺ������

				BanHand_4_Vertical(y); //��ֱ����x++��y++���жϺ������

				BanHand_4_TwoFourQuadrant(x, y); //�������޷���x++��y--���жϺ������
			}

			else if (count > 5)
			{
				Board[yWhite][xWhite] = WHITE_COLOR + 2;

				DisplayChess();

				GameOver = TRUE;

				////printf("���峤���������ʤ��\n");
			}

			else
			{
				continue;
			}
		}
	}


	for(x = -14, y = 0; (x <= BOARD_SIZE) && (y >= 0); x++)
	{
		for (BeginOTx = x, BeginOTy = y, EndOTx = x, EndOTy = y; 
			(-14 <= BeginOTx) && (BeginOTx <= BOARD_SIZE) 
			&& 
			(-14 <= EndOTx) && (EndOTx <= BOARD_SIZE) 
			&& 
			(0 <= BeginOTy) && (BeginOTy <= BOARD_SIZE)  
			&& 
			(0 <= EndOTy) && (EndOTy <= BOARD_SIZE);
		(BeginOTx = EndOTx + 1), (BeginOTy = EndOTy + 1)) //β���Ƶ�ʼ�����һλ
		{
			count = ChessCount_OT(BeginOTy, BeginOTx, WHITE_COLOR);
			if (count == 5)
			{
				Board[yWhite][xWhite] = WHITE_COLOR + 2;

				DisplayChess();

				GameOver = TRUE;
			}

			Score_OT(BeginOTy, BeginOTx, count, WHITE_COLOR);
		}
	}
}


//�������޷����ж���Ӯ
void WhoIsWin_TwoFourQuadrant()
{
	int x = 0, y = 0; //���̵ĺ�������

	for(x = -14, y = BOARD_SIZE; (x <= BOARD_SIZE) && (y >= 0); x++) //x++��y--������һ�ж�
	{
		for (BeginTFx = x, BeginTFy = y, EndTFx = x, EndTFy = y; 
			(-14 <= BeginTFx) && (BeginTFx <= BOARD_SIZE) 
			&& 
			(-14 <= EndTFx) && (EndTFx <= BOARD_SIZE) 
			&& 
			(0 <= BeginTFy) && (BeginTFy <= BOARD_SIZE)
			&& 
			(0 <= EndTFy) && (EndTFy <= BOARD_SIZE);
		(BeginTFx = EndTFx + 1), (BeginTFy = EndTFy - 1)) //β���Ƶ�ʼ�����һλ
		{
			count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

			//�������޷��������ӵ����˾��ް��ӣ��Ҳ��������̱�Ե����һ��������
			if (count == 5)			
			{
				Board[yBlack][xBlack] = BLACK_COLOR + 2;

				DisplayChess();

				GameOver = TRUE;
			}
			//����ԭ�򣺷�ֹ�жϽ��ֺ󣬽�������countֵ��Ϊ���ֵ�countֵ������û�а�������������ֵ��֣��Ӷ�������������߷ִ�
			Score_TF(BeginTFy, BeginTFx, count, BLACK_COLOR);
		}


		if ((EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1))
		{
			BeginTFx = xBlack - 4;
			BeginTFy = yBlack + 4;
			EndTFx = xBlack - 4;
			EndTFy = yBlack + 4;

			count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

			//�������޷��������ӵ����˾��ް��ӣ��Ҳ��������̱�Ե
			if ((count == 3) 
				&& (Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR) 
				&& (Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR + 1)
				&& (Board[EndTFy][EndTFx] % 100 != WHITE_COLOR) 
				&& (Board[EndTFy][EndTFx] % 100 != WHITE_COLOR + 1)
				&& (BeginTFy != 0) && (BeginTFy != BOARD_SIZE) 
				&& (BeginTFx != 0) && (BeginTFx != BOARD_SIZE)
				&& (EndTFy + 1 != 0) && (EndTFy + 1 != BOARD_SIZE) 
				&& (EndTFx - 1 != 0) && (EndTFx - 1 != BOARD_SIZE))
			{
				BanHand_3_Horizon(x); //ˮƽ����x++��y++���жϺ������

				BanHand_3_Vertical(y); //��ֱ����x++��y++���жϺ������

				BanHand_3_OneThreeQuadrant(x, y); //һ�����޷���x++��y++���жϺ������
			}

			else if ((count == 4) 
				&& 
				(((Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR) 
				&& (Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR + 1))
				|| 
				((Board[EndTFy][EndTFx] % 100 != WHITE_COLOR) 
				&& (Board[EndTFy][EndTFx] % 100 != WHITE_COLOR + 1))))
			{
				BanHand_4_Horizon(x); //ˮƽ����x++��y++���жϺ������

				BanHand_4_Vertical(y); //��ֱ����x++��y++���жϺ������

				BanHand_4_OneThreeQuadrant(x, y); //һ�����޷���x++��y++���жϺ������
			}

			else if (count > 5)
			{
				Board[yWhite][xWhite] = WHITE_COLOR + 2;

				DisplayChess();

				GameOver = TRUE;

				////printf("���峤���������ʤ��\n");
			}

			else
			{
				continue;
			}
		}
	}



	for(x = -14, y = BOARD_SIZE; (x <= BOARD_SIZE) && (y >= 0); x++)
	{
		for (BeginTFx = x, BeginTFy = y, EndTFx = x, EndTFy = y; 
			(-14 <= BeginTFx) && (BeginTFx <= BOARD_SIZE) 
			&& 
			(-14 <= EndTFx) && (EndTFx <= BOARD_SIZE) 
			&& 
			(0 <= BeginTFy) && (BeginTFy <= BOARD_SIZE)
			&& 
			(0 <= EndTFy) && (EndTFy <= BOARD_SIZE);
		(BeginTFx = EndTFx + 1), (BeginTFy = EndTFy - 1)) //β���Ƶ�ʼ�����һλ
		{
			count = ChessCount_TF(BeginTFy, BeginTFx, WHITE_COLOR);
			if (count == 5)
			{	
				Board[yWhite][xWhite] = WHITE_COLOR + 2;

				DisplayChess();

				GameOver = TRUE;
			}

			Score_TF(BeginTFy, BeginTFx, count, WHITE_COLOR);
		}
	}
}


//�ж��������ֵ�˼·��
//******* 1����򵥵������������Σ�����2�����������������ӣ��Һ���һ������ͬʱ�γ��������������ӱ���Ϊ����������ͬ�Ĺ����ӡ�
//******* 2�����ԣ�ֻ�����ж�����1���������Ƿ��������ӣ����У����漴�ж�(yBlack, xBlack)���ڵ��������������Ƿ��������ӡ�
//******* 3����Ϊ���������˳��������-����-��һ��-����ķ���ͳ����������
//******* 4������ֻ������һ��һ���жϵ��ص㣬���жϳ�1�����������������Ӻ������ж�����3�������Ƿ��������Ӽ��ɡ�


//ˮƽ����x++��y++���жϺ������
void BanHand_3_Horizon(int Column)
{
	//���Ǳ߽����Ƶ�˼·���������̺����Ӵ��ı������򣨴������ң��������ϣ������������ϣ������������£���ֻ���β�˱߽�������Ƽ��ɣ���ʼ�߼�ʹ����Ե�ˣ�ͳ��������ʱҲ������
	if (EndH - 1 <= BOARD_SIZE)
	{
		BeginH = xBlack - 4;
		EndH = xBlack - 4;

		count = ChessCount_H(yBlack, BLACK_COLOR); //����������

		//�������������˾��ް��ӣ��Ҳ��������̱�Ե
		if (
			(count == 3)
			&& (Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR) 
			&& (Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR + 1)
			&& (Board[yBlack][EndH] % 100 != WHITE_COLOR) 
			&& (Board[yBlack][EndH] % 100 != WHITE_COLOR + 1)
			&& (BeginH != 0) && (BeginH != BOARD_SIZE) 
			&& (EndH - 1 != 0) && (EndH - 1 != BOARD_SIZE) 
			&& (yBlack != 0) && (yBlack != BOARD_SIZE)) 
		{
			//��ֱ�����ж��Ƿ��е����������ġ�
			if (EndV - 1 <= BOARD_SIZE)
			{
				BeginV = yBlack - 4;
				EndV = yBlack - 4;

				count = ChessCount_V(xBlack, BLACK_COLOR);

				if ((count == 4)
					&& 
					(((Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR) 
					&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR + 1))
					|| 
					((Board[EndV][xBlack] % 100 != WHITE_COLOR) 
					&& (Board[EndV][xBlack] % 100 != WHITE_COLOR + 1)))) 
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}
			}


			//һ�����޷����ж��Ƿ��е����������ġ�
			else if ((EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE))
			{
				BeginOTx = xBlack - 4;
				BeginOTy = yBlack - 4;
				EndOTx = xBlack - 4;
				EndOTy = yBlack - 4;

				count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

				//�����ӵ�����ֻ����һ���ް���
				if ((count == 4) 
					&& 
					(((Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR) 
					&& (Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR + 1))
					|| 
					((Board[EndOTy][EndOTx] % 100 != WHITE_COLOR) 
					&& (Board[EndOTy][EndOTx] % 100 != WHITE_COLOR + 1))))
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}
			}


			//�������޷����ж��Ƿ��е����������ġ�
			else if ((EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1))
			{
				BeginTFx = xBlack - 4;
				BeginTFy = yBlack + 4;
				EndTFx = xBlack - 4;
				EndTFy = yBlack + 4;

				count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

				//�������޷��������ӵ����˾��ް��ӣ��Ҳ��������̱�Ե
				if ((count == 4) 
					&& 
					(((Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR) 
					&& (Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR + 1))
					||
					((Board[EndTFy][EndTFx] % 100 != WHITE_COLOR) 
					&& (Board[EndTFy][EndTFx] % 100 != WHITE_COLOR + 1))))
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}
			}


			//���޵����������ġ������ж�Ϊ��������
			Board[yWhite][xWhite] = WHITE_COLOR + 2;

			DisplayChess();

			GameOver = TRUE;
		}
	}
}



//ˮƽ����x++��y++���жϺ������
void BanHand_4_Horizon(int Column)
{
	//���Ǳ߽����Ƶ�˼·���������̺����Ӵ��ı������򣨴������ң��������ϣ������������ϣ������������£���ֻ���β�˱߽�������Ƽ��ɣ���ʼ�߼�ʹ����Ե�ˣ�ͳ��������ʱҲ������
	if (EndH - 1 <= BOARD_SIZE)
	{
		BeginH = xBlack - 4;
		EndH = xBlack - 4;

		count = ChessCount_H(yBlack, BLACK_COLOR); //����������

		//�����ӵ�����ֻ����һ���ް���
		if (
			(count == 4)
			&& 
			(((Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR) 
			&& (Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR + 1))
			|| 
			((Board[yBlack][EndH] % 100 != WHITE_COLOR) 
			&& (Board[yBlack][EndH] % 100 != WHITE_COLOR + 1)))
			)
		{
			//��ֱ�����ж��Ƿ��е�������������
			if (EndV - 1 <= BOARD_SIZE)
			{
				BeginV = yBlack - 4;
				EndV = yBlack - 4;

				count = ChessCount_V(xBlack, BLACK_COLOR);

				if ((count == 3)
					&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR) 
					&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR + 1)
					&& (Board[EndV][xBlack] % 100 != WHITE_COLOR) 
					&& (Board[EndV][xBlack] % 100 != WHITE_COLOR + 1)
					&& (BeginV != 0) && (BeginV != BOARD_SIZE) 
					&& (EndV - 1 != 0) && (EndV - 1 != BOARD_SIZE) 
					&& (xBlack != 0) && (xBlack != BOARD_SIZE)
					) 
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}
			}


			//һ�����޷����ж��Ƿ��е�������������
			if ( (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE))
			{
				BeginOTx = xBlack - 4;
				BeginOTy = yBlack - 4;
				EndOTx = xBlack - 4;
				EndOTy = yBlack - 4;

				count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

				//һ�����޷��������ӵ����˾��ް��ӣ��Ҳ��������̱�Ե
				if ((count == 3) 
					&& (Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR) 
					&& (Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR + 1)
					&& (Board[EndOTy][EndOTx] % 100 != WHITE_COLOR) 
					&& (Board[EndOTy][EndOTx] % 100 != WHITE_COLOR + 1)
					&& (BeginOTy != 0) && (BeginOTy != BOARD_SIZE) 
					&& (BeginOTx != 0) && (BeginOTx != BOARD_SIZE)
					&& (EndOTy - 1 != 0) && (EndOTy - 1 != BOARD_SIZE)  
					&& (EndOTx - 1 != 0) && (EndOTx - 1 != BOARD_SIZE))
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}
			}


			//�������޷����ж��Ƿ��е�������������
			if ((EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1))
			{
				BeginTFx = xBlack - 4;
				BeginTFy = yBlack + 4;
				EndTFx = xBlack - 4;
				EndTFy = yBlack + 4;

				count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

				//�������޷��������ӵ����˾��ް��ӣ��Ҳ��������̱�Ե
				if ((count == 3) 
					&& (Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR) 
					&& (Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR + 1)
					&& (Board[EndTFy][EndTFx] % 100 != WHITE_COLOR) 
					&& (Board[EndTFy][EndTFx] % 100 != WHITE_COLOR + 1)
					&& (BeginTFy != 0) && (BeginTFy != BOARD_SIZE) 
					&& (BeginTFx != 0) && (BeginTFx != BOARD_SIZE)
					&& (EndTFy + 1 != 0) && (EndTFy + 1 != BOARD_SIZE) 
					&& (EndTFx - 1 != 0) && (EndTFx - 1 != BOARD_SIZE))
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}
			}


			//���޵������������������ж�Ϊ���Ľ���
			Board[yWhite][xWhite] = WHITE_COLOR + 2;

			DisplayChess();

			GameOver = TRUE;
		}
	}
}



//��ֱ����x++��y++���жϺ������
void BanHand_3_Vertical(int Line)
{
	if (EndV - 1 <= BOARD_SIZE)
	{
		BeginV = yBlack - 4;
		EndV = yBlack - 4;



		count = ChessCount_V(xBlack, BLACK_COLOR);

		//���������ӵ����˾��ް��ӣ��Ҳ��������̱�Ե���ڶ���������
		//�����һ������(yBlack, xBlack)�����������Ľ��㴦
		if ((count == 3)
			&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR) 
			&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR + 1)
			&& (Board[EndV][xBlack] % 100 != WHITE_COLOR) 
			&& (Board[EndV][xBlack] % 100 != WHITE_COLOR + 1)
			&& (BeginV != 0) && (BeginV != BOARD_SIZE) 
			&& (EndV - 1 != 0) && (EndV - 1 != BOARD_SIZE) 
			&& (xBlack != 0) && (xBlack != BOARD_SIZE)) 
		{
			//ˮƽ�����ж��Ƿ��е����������ġ�
			for ((BeginH = xBlack - 4, EndH = xBlack - 4) && (EndH - 1 <= BOARD_SIZE); 
				(BeginH <= xBlack + 4) && (BeginH <= BOARD_SIZE) && (EndH - 1 <= xBlack + 4) && (EndH - 1 <= BOARD_SIZE); 
				BeginH = EndH + 1)
			{
				count = ChessCount_H(yBlack, BLACK_COLOR); //����������

				if ((count == 4)
					&& 
					(((Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR) 
					&& (Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR + 1))
					|| 
					((Board[yBlack][EndH] % 100 != WHITE_COLOR) 
					&& (Board[yBlack][EndH] % 100 != WHITE_COLOR + 1))))
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}

				else
				{
					continue;
				}
			}


			//һ�����޷����ж��Ƿ��е����������ġ�
			for (
				(BeginOTx = xBlack - 4, BeginOTy = yBlack - 4, EndOTx = xBlack - 4, EndOTy = yBlack - 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
				(BeginOTx <= xBlack + 4) && (BeginOTy <= yBlack + 4) && (EndOTx - 1 <= xBlack + 4) && (EndOTy - 1 <= yBlack + 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
			BeginOTx = EndOTx + 1, BeginOTy = EndOTy + 1)
			{
				count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

				//�����ӵ�����ֻ����һ���ް���
				if ((count == 4) 
					&& 
					(((Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR) 
					&& (Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR + 1))
					|| 
					((Board[EndOTy][EndOTx] % 100 != WHITE_COLOR) 
					&& (Board[EndOTy][EndOTx] % 100 != WHITE_COLOR + 1))))
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}

				else
				{
					continue;
				}
			}


			//�������޷����ж��Ƿ��е����������ġ�
			for (
				(BeginTFx = xBlack - 4, BeginTFy = yBlack + 4, EndTFx = xBlack - 4, EndTFy = yBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
				(BeginTFx <= xBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
			BeginTFx = EndTFx + 1, BeginTFy = EndTFy - 1)
			{
				count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

				//�������޷��������ӵ����˾��ް��ӣ��Ҳ��������̱�Ե
				if ((count == 4) 
					&& 
					(((Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR) 
					&& (Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR + 1))
					||
					((Board[EndTFy][EndTFx] % 100 != WHITE_COLOR) 
					&& (Board[EndTFy][EndTFx] % 100 != WHITE_COLOR + 1))))
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}

				else
				{
					continue;
				}
			}

			//���޵����������ġ������ж�Ϊ��������
			Board[yWhite][xWhite] = WHITE_COLOR + 2;

			DisplayChess();

			GameOver = TRUE;
		}
	}
}



//��ֱ����x++��y++���жϺ������
void BanHand_4_Vertical(int Line)
{
	if (EndV - 1 <= BOARD_SIZE)
	{
		BeginV = yBlack - 4;
		EndV = yBlack - 4;


		count = ChessCount_V(xBlack, BLACK_COLOR);

		//�����ӵ�����ֻ����һ���ް���
		//�����һ������(yBlack, xBlack)�����������Ľ��㴦
		if (
			(count == 4)
			&& 
			(((Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR) 
			&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR + 1))
			|| 
			((Board[EndV][xBlack] % 100 != WHITE_COLOR) 
			&& (Board[EndV][xBlack] % 100 != WHITE_COLOR + 1)))
			) 
		{
			//ˮƽ�����ж��Ƿ��е�������������
			for (
				(BeginH = xBlack - 4, EndH = xBlack - 4) && (EndH - 1 <= BOARD_SIZE); 
				(BeginH <= xBlack + 4) && (BeginH <= BOARD_SIZE) && (EndH - 1 <= xBlack + 4) && (EndH - 1 <= BOARD_SIZE); 
			BeginH = EndH + 1)
			{
				count = ChessCount_H(yBlack, BLACK_COLOR); //����������

				//�������������˾��ް��ӣ��Ҳ��������̱�Ե
				if (
					(count == 3)
					&& (Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR) 
					&& (Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR + 1)
					&& (Board[yBlack][EndH] % 100 != WHITE_COLOR) 
					&& (Board[yBlack][EndH] % 100 != WHITE_COLOR + 1)
					&& (BeginH != 0) && (BeginH != BOARD_SIZE) 
					&& (EndH - 1 != 0) && (EndH - 1 != BOARD_SIZE) 
					&& (yBlack != 0) && (yBlack != BOARD_SIZE))
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}

				else
				{
					continue;
				}
			}


			//һ�����޷����ж��Ƿ��е�������������
			for (
				(BeginOTx = xBlack - 4, BeginOTy = yBlack - 4, EndOTx = xBlack - 4, EndOTy = yBlack - 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
				(BeginOTx <= xBlack + 4) && (BeginOTy <= yBlack + 4) && (EndOTx - 1 <= xBlack + 4) && (EndOTy - 1 <= yBlack + 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
			BeginOTx = EndOTx + 1, BeginOTy = EndOTy + 1)
			{
				count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

				//һ�����޷��������ӵ����˾��ް��ӣ��Ҳ��������̱�Ե
				if ((count == 3) 
					&& (Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR) 
					&& (Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR + 1)
					&& (Board[EndOTy][EndOTx] % 100 != WHITE_COLOR) 
					&& (Board[EndOTy][EndOTx] % 100 != WHITE_COLOR + 1)
					&& (BeginOTy != 0) && (BeginOTy != BOARD_SIZE) 
					&& (BeginOTx != 0) && (BeginOTx != BOARD_SIZE)
					&& (EndOTy - 1 != 0) && (EndOTy - 1 != BOARD_SIZE)  
					&& (EndOTx - 1 != 0) && (EndOTx - 1 != BOARD_SIZE))
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}

				else
				{
					continue;
				}
			}


			//�������޷����ж��Ƿ��е�������������
			for (
				(BeginTFx = xBlack - 4, BeginTFy = yBlack + 4, EndTFx = xBlack - 4, EndTFy = yBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
				(BeginTFx <= xBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
			BeginTFx = EndTFx + 1, BeginTFy = EndTFy - 1)
			{
				count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

				//�������޷��������ӵ����˾��ް��ӣ��Ҳ��������̱�Ե
				if ((count == 3) 
					&& (Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR) 
					&& (Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR + 1)
					&& (Board[EndTFy][EndTFx] % 100 != WHITE_COLOR) 
					&& (Board[EndTFy][EndTFx] % 100 != WHITE_COLOR + 1)
					&& (BeginTFy != 0) && (BeginTFy != BOARD_SIZE) 
					&& (BeginTFx != 0) && (BeginTFx != BOARD_SIZE)
					&& (EndTFy + 1 != 0) && (EndTFy + 1 != BOARD_SIZE) 
					&& (EndTFx - 1 != 0) && (EndTFx - 1 != BOARD_SIZE))
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}

				else
				{
					continue;
				}
			}


			//���޵������������������ж�Ϊ���Ľ���
			Board[yWhite][xWhite] = WHITE_COLOR + 2;

			DisplayChess();

			GameOver = TRUE;
		}
	}
}



//һ�����޷���x++��y++���жϺ������
void BanHand_3_OneThreeQuadrant(int Column, int Line)
{
	if ((EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE))
	{
		BeginOTx = xBlack - 4;
		BeginOTy = yBlack - 4;
		EndOTx = xBlack - 4;
		EndOTy = yBlack - 4; 


		count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

		//һ�����޷��������ӵ����˾��ް��ӣ��Ҳ��������̱�Ե
		if ((count == 3) 
			&& (Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR) 
			&& (Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR + 1)
			&& (Board[EndOTy][EndOTx] % 100 != WHITE_COLOR) 
			&& (Board[EndOTy][EndOTx] % 100 != WHITE_COLOR + 1)
			&& (BeginOTy != 0) && (BeginOTy != BOARD_SIZE) 
			&& (BeginOTx != 0) && (BeginOTx != BOARD_SIZE)
			&& (EndOTy - 1 != 0) && (EndOTy - 1 != BOARD_SIZE)  
			&& (EndOTx - 1 != 0) && (EndOTx - 1 != BOARD_SIZE))
		{
			//ˮƽ�����ж��Ƿ��е����������ġ�
			for ((BeginH = xBlack - 4, EndH = xBlack - 4) && (EndH - 1 <= BOARD_SIZE); 
				(BeginH <= xBlack + 4) && (BeginH <= BOARD_SIZE) && (EndH - 1 <= xBlack + 4) && (EndH - 1 <= BOARD_SIZE); 
				BeginH = EndH + 1)
			{
				count = ChessCount_H(yBlack, BLACK_COLOR); //����������

				if ((count == 4)
					&& 
					(((Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR) 
					&& (Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR + 1))
					|| 
					((Board[yBlack][EndH] % 100 != WHITE_COLOR) 
					&& (Board[yBlack][EndH] % 100 != WHITE_COLOR + 1))))
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}

				else
				{
					continue;
				}
			}


			//��ֱ�����ж��Ƿ��е����������ġ�
			for (
				(BeginV = yBlack - 4, EndV = yBlack - 4) && (EndV - 1 <= BOARD_SIZE); 
				(BeginV <= yBlack + 4) && (BeginV <= BOARD_SIZE) && (EndV - 1 <= yBlack + 4) && (EndV - 1 <= BOARD_SIZE); 
			BeginV = EndV + 1)
			{
				count = ChessCount_V(xBlack, BLACK_COLOR);

				if (
					(count == 4)
					&& 
					(((Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR) 
					&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR + 1))
					|| 
					((Board[EndV][xBlack] % 100 != WHITE_COLOR) 
					&& (Board[EndV][xBlack] % 100 != WHITE_COLOR + 1)))
					) 
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}

				else
				{
					continue;
				}
			}


			//�������޷����ж��Ƿ��е����������ġ�
			for (
				(BeginTFx = xBlack - 4, BeginTFy = yBlack + 4, EndTFx = xBlack - 4, EndTFy = yBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
				(BeginTFx <= xBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
			BeginTFx = EndTFx + 1, BeginTFy = EndTFy - 1)
			{
				count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

				//�������޷��������ӵ����˾��ް��ӣ��Ҳ��������̱�Ե
				if (
					(count == 4) 
					&& 
					(((Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR) 
					&& (Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR + 1))
					||
					((Board[EndTFy][EndTFx] % 100 != WHITE_COLOR) 
					&& (Board[EndTFy][EndTFx] % 100 != WHITE_COLOR + 1)))
					)
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;

					
				}

				else
				{
					continue;
				}
			}

			//���޵����������ġ������ж�Ϊ��������
			Board[yWhite][xWhite] = WHITE_COLOR + 2;

			DisplayChess();

			GameOver = TRUE;

			
		}
	}
}


//һ�����޷���x++��y++���жϺ������
void BanHand_4_OneThreeQuadrant(int Column, int Line)
{
	if ((EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE))
	{
		BeginOTx = xBlack - 4;
		BeginOTy = yBlack - 4;
		EndOTx = xBlack - 4;
		EndOTy = yBlack - 4; 

		count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

		//�����ӵ�����ֻ����һ���ް���
		if (
			(count == 4) 
			&& 
			(((Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR) 
			&& (Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR + 1))
			|| 
			((Board[EndOTy][EndOTx] % 100 != WHITE_COLOR) 
			&& (Board[EndOTy][EndOTx] % 100 != WHITE_COLOR + 1)))
			)
		{
			//ˮƽ�����ж��Ƿ��е�������������
			for (
				(BeginH = xBlack - 4, EndH = xBlack - 4) && (EndH - 1 <= BOARD_SIZE); 
				(BeginH <= xBlack + 4) && (BeginH <= BOARD_SIZE) && (EndH - 1 <= xBlack + 4) && (EndH - 1 <= BOARD_SIZE); 
			BeginH = EndH + 1)
			{
				count = ChessCount_H(yBlack, BLACK_COLOR); //����������

				//�������������˾��ް��ӣ��Ҳ��������̱�Ե
				if (
					(count == 3)
					&& (Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR) 
					&& (Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR + 1)
					&& (Board[yBlack][EndH] % 100 != WHITE_COLOR) 
					&& (Board[yBlack][EndH] % 100 != WHITE_COLOR + 1)
					&& (BeginH != 0) && (BeginH != BOARD_SIZE) 
					&& (EndH - 1 != 0) && (EndH - 1 != BOARD_SIZE) 
					&& (yBlack != 0) && (yBlack != BOARD_SIZE))
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}

				else
				{
					continue;
				}
			}



			//��ֱ�����ж��Ƿ��е�������������
			for (
				(BeginV = yBlack - 4, EndV = yBlack - 4) && (EndV - 1 <= BOARD_SIZE); 
				(BeginV <= yBlack + 4) && (BeginV <= BOARD_SIZE) && (EndV - 1 <= yBlack + 4) && (EndV - 1 <= BOARD_SIZE); 
			BeginV = EndV + 1)
			{
				count = ChessCount_V(xBlack, BLACK_COLOR);

				if ((count == 3)
					&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR) 
					&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR + 1)
					&& (Board[EndV][xBlack] % 100 != WHITE_COLOR) 
					&& (Board[EndV][xBlack] % 100 != WHITE_COLOR + 1)
					&& (BeginV != 0) && (BeginV != BOARD_SIZE) 
					&& (EndV - 1 != 0) && (EndV - 1 != BOARD_SIZE) 
					&& (xBlack != 0) && (xBlack != BOARD_SIZE)
					) 
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}

				else
				{
					continue;
				}
			}


			//�������޷����ж��Ƿ��е�������������
			for (
				(BeginTFx = xBlack - 4, BeginTFy = yBlack + 4, EndTFx = xBlack - 4, EndTFy = yBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
				(BeginTFx <= xBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
			BeginTFx = EndTFx + 1, BeginTFy = EndTFy - 1)
			{
				count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

				//�������޷��������ӵ����˾��ް��ӣ��Ҳ��������̱�Ե
				if ((count == 3) 
					&& (Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR) 
					&& (Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR + 1)
					&& (Board[EndTFy][EndTFx] % 100 != WHITE_COLOR) 
					&& (Board[EndTFy][EndTFx] % 100 != WHITE_COLOR + 1)
					&& (BeginTFy != 0) && (BeginTFy != BOARD_SIZE) 
					&& (BeginTFx != 0) && (BeginTFx != BOARD_SIZE)
					&& (EndTFy + 1 != 0) && (EndTFy + 1 != BOARD_SIZE) 
					&& (EndTFx - 1 != 0) && (EndTFx - 1 != BOARD_SIZE))
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}

				else
				{
					continue;
				}
			}


			//���޵������������������ж�Ϊ���Ľ���
			Board[yWhite][xWhite] = WHITE_COLOR + 2;

			DisplayChess();

			GameOver = TRUE;
		}
	}
}


//�������޷���x++��y--���жϺ������
void BanHand_3_TwoFourQuadrant(int Column, int Line)
{
	/////���Ǳ߽����Ƶ�˼·���������̺����Ӵ��ı������򣨴������ң��������ϣ������������ϣ������������£���ֻ���β�˱߽�������Ƽ��ɣ���ʼ�߼�ʱ����Ե�ˣ�ͳ��������ʱҲ������

	//forѭ���ı��ʽ2ֻ������(BeginTFx <= xBlack + 4)����δ����(BeginTFy = yBlack - 4)������Ϊy��x�ı�������һ�£�š�ţ���ì�ܣ�
	//��y��x�仯���仯����������Ϊ��������д�����ʽ2��


	if ((EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1))
	{
		BeginTFx = xBlack - 4;
		BeginTFy = yBlack + 4;
		EndTFx = xBlack - 4;
		EndTFy = yBlack + 4;


		count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

		//�������޷��������ӵ����˾��ް��ӣ��Ҳ��������̱�Ե
		if ((count == 3) 
			&& (Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR) 
			&& (Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR + 1)
			&& (Board[EndTFy][EndTFx] % 100 != WHITE_COLOR) 
			&& (Board[EndTFy][EndTFx] % 100 != WHITE_COLOR + 1)
			&& (BeginTFy != 0) && (BeginTFy != BOARD_SIZE) 
			&& (BeginTFx != 0) && (BeginTFx != BOARD_SIZE)
			&& (EndTFy + 1 != 0) && (EndTFy + 1 != BOARD_SIZE) 
			&& (EndTFx - 1 != 0) && (EndTFx - 1 != BOARD_SIZE))
		{
			//ˮƽ�����ж��Ƿ��е����������ġ�
			for ((BeginH = xBlack - 4, EndH = xBlack - 4) && (EndH - 1 <= BOARD_SIZE); 
				(BeginH <= xBlack + 4) && (BeginH <= BOARD_SIZE) && (EndH - 1 <= xBlack + 4) && (EndH - 1 <= BOARD_SIZE); 
				BeginH = EndH + 1)
			{
				count = ChessCount_H(yBlack, BLACK_COLOR); //����������

				if ((count == 4)
					&& 
					(((Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR) 
					&& (Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR + 1))
					|| 
					((Board[yBlack][EndH] % 100 != WHITE_COLOR) 
					&& (Board[yBlack][EndH] % 100 != WHITE_COLOR + 1))))
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}

				else
				{
					continue;
				}
			}


			//��ֱ�����ж��Ƿ��е����������ġ�
			for (
				(BeginV = yBlack - 4, EndV = yBlack - 4) && (EndV - 1 <= BOARD_SIZE); 
				(BeginV <= yBlack + 4) && (BeginV <= BOARD_SIZE) && (EndV - 1 <= yBlack + 4) && (EndV - 1 <= BOARD_SIZE); 
			BeginV = EndV + 1)
			{
				count = ChessCount_V(xBlack, BLACK_COLOR);

				if (
					(count == 4)
					&& 
					(((Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR) 
					&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR + 1))
					|| 
					((Board[EndV][xBlack] % 100 != WHITE_COLOR) 
					&& (Board[EndV][xBlack] % 100 != WHITE_COLOR + 1)))
					) 
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}

				else
				{
					continue;
				}
			}


			//һ�����޷����ж��Ƿ��е����������ġ�
			for (
				(BeginOTx = xBlack - 4, BeginOTy = yBlack - 4, EndOTx = xBlack - 4, EndOTy = yBlack - 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
				(BeginOTx <= xBlack + 4) && (BeginOTy <= yBlack + 4) && (EndOTx - 1 <= xBlack + 4) && (EndOTy - 1 <= yBlack + 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
			BeginOTx = EndOTx + 1, BeginOTy = EndOTy + 1)
			{
				count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

				//�����ӵ�����ֻ����һ���ް���
				if (
					(count == 4) 
					&& 
					(((Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR) 
					&& (Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR + 1))
					|| 
					((Board[EndOTy][EndOTx] % 100 != WHITE_COLOR) 
					&& (Board[EndOTy][EndOTx] % 100 != WHITE_COLOR + 1)))
					)
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}

				else
				{
					continue;
				}
			}

			//���޵����������ġ������ж�Ϊ��������
			Board[yWhite][xWhite] = WHITE_COLOR + 2;

			DisplayChess();

			GameOver = TRUE;
		}
	}
} 



//�������޷���x++��y--���жϺ������
void BanHand_4_TwoFourQuadrant(int Column, int Line)
{
	/////���Ǳ߽����Ƶ�˼·���������̺����Ӵ��ı������򣨴������ң��������ϣ������������ϣ������������£���ֻ���β�˱߽�������Ƽ��ɣ���ʼ�߼�ʱ����Ե�ˣ�ͳ��������ʱҲ������

	//forѭ���ı��ʽ2ֻ������(BeginTFx <= xBlack + 4)����δ����(BeginTFy = yBlack - 4)������Ϊy��x�ı�������һ�£�š�ţ���ì�ܣ�
	//��y��x�仯���仯����������Ϊ��������д�����ʽ2��


	if ((EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1))
	{
		BeginTFx = xBlack - 4;
		BeginTFy = yBlack + 4;
		EndTFx = xBlack - 4;
		EndTFy = yBlack + 4;


		count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

		//�������޷��������ӵ����˾��ް��ӣ��Ҳ��������̱�Ե
		if (
			(count == 4) 
			&& 
			(((Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR) 
			&& (Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR + 1))
			||
			((Board[EndTFy][EndTFx] % 100 != WHITE_COLOR) 
			&& (Board[EndTFy][EndTFx] % 100 != WHITE_COLOR + 1)))
			)
		{
			//ˮƽ�����ж��Ƿ��е�������������
			for (
				(BeginH = xBlack - 4, EndH = xBlack - 4) && (EndH - 1 <= BOARD_SIZE); 
				(BeginH <= xBlack + 4) && (BeginH <= BOARD_SIZE) && (EndH - 1 <= xBlack + 4) && (EndH - 1 <= BOARD_SIZE); 
			BeginH = EndH + 1)
			{
				count = ChessCount_H(yBlack, BLACK_COLOR); //����������

				//�������������˾��ް��ӣ��Ҳ��������̱�Ե
				if (
					(count == 3)
					&& (Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR) 
					&& (Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR + 1)
					&& (Board[yBlack][EndH] % 100 != WHITE_COLOR) 
					&& (Board[yBlack][EndH] % 100 != WHITE_COLOR + 1)
					&& (BeginH != 0) && (BeginH != BOARD_SIZE) 
					&& (EndH - 1 != 0) && (EndH - 1 != BOARD_SIZE) 
					&& (yBlack != 0) && (yBlack != BOARD_SIZE))
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}

				else
				{
					continue;
				}
			}



			//��ֱ�����ж��Ƿ��е�������������
			for (
				(BeginV = yBlack - 4, EndV = yBlack - 4) && (EndV - 1 <= BOARD_SIZE); 
				(BeginV <= yBlack + 4) && (BeginV <= BOARD_SIZE) && (EndV - 1 <= yBlack + 4) && (EndV - 1 <= BOARD_SIZE); 
			BeginV = EndV + 1)
			{
				count = ChessCount_V(xBlack, BLACK_COLOR);

				if ((count == 3)
					&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR) 
					&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR + 1)
					&& (Board[EndV][xBlack] % 100 != WHITE_COLOR) 
					&& (Board[EndV][xBlack] % 100 != WHITE_COLOR + 1)
					&& (BeginV != 0) && (BeginV != BOARD_SIZE) 
					&& (EndV - 1 != 0) && (EndV - 1 != BOARD_SIZE) 
					&& (xBlack != 0) && (xBlack != BOARD_SIZE)
					) 
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}

				else
				{
					continue;
				}
			}


			//һ�����޷����ж��Ƿ��е�������������
			for (
				(BeginOTx = xBlack - 4, BeginOTy = yBlack - 4, EndOTx = xBlack - 4, EndOTy = yBlack - 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
				(BeginOTx <= xBlack + 4) && (BeginOTy <= yBlack + 4) && (EndOTx - 1 <= xBlack + 4) && (EndOTy - 1 <= yBlack + 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
			BeginOTx = EndOTx + 1, BeginOTy = EndOTy + 1)
			{
				count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

				//һ�����޷��������ӵ����˾��ް��ӣ��Ҳ��������̱�Ե
				if ((count == 3) 
					&& (Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR) 
					&& (Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR + 1)
					&& (Board[EndOTy][EndOTx] % 100 != WHITE_COLOR) 
					&& (Board[EndOTy][EndOTx] % 100 != WHITE_COLOR + 1)
					&& (BeginOTy != 0) && (BeginOTy != BOARD_SIZE) 
					&& (BeginOTx != 0) && (BeginOTx != BOARD_SIZE)
					&& (EndOTy - 1 != 0) && (EndOTy - 1 != BOARD_SIZE)  
					&& (EndOTx - 1 != 0) && (EndOTx - 1 != BOARD_SIZE))
				{
					Board[yWhite][xWhite] = WHITE_COLOR + 2;

					DisplayChess();

					GameOver = TRUE;
				}

				else
				{
					continue;
				}
			}


			//���޵������������������ж�Ϊ���Ľ���
			Board[yWhite][xWhite] = WHITE_COLOR + 2;

			DisplayChess();

			GameOver = TRUE;
		}
	}
}//������
void Score_H(int Line, int count, int Color) 
{
	if  ((Color == BLACK_COLOR) || (Color == BLACK_COLOR + 1)) //ͳ�ƺ������ӣ����¶���
	{
		switch(count)	//��ֱ�׼
		{
		case 1:	
			getScope_H_Black(Line, BeginH, EndH, 1000);
			break;

		case 2:	
			//����[�񡡡�]�����
			if (Board[Line][BeginH + 1] % 100 <= 9) 
			{
				getScope_H_Black(Line, BeginH, EndH, 2100);
				break;
			}

			getScope_H_Black(Line, BeginH, EndH, 2000);
			break;

		case 3:	
			//����[��񡡡�]�����
			if (Board[Line][BeginH + 2] % 100 <= 9) 
			{
				getScope_H_Black(Line, BeginH, EndH, 20000);
				break;
			}

			//����[�񡡡��]�����
			else if (Board[Line][BeginH + 1] % 100 <= 9) 
			{
				getScope_H_Black(Line, BeginH, EndH, 20000);
				break;
			}

			getScope_H_Black(Line, BeginH, EndH, 19000);
			break;

		case 4:	
			//����[���񡡡�]�����
			if (Board[Line][BeginH + 3] % 100 <= 9) 
			{
				getScope_H_Black(Line, BeginH, EndH, 210000);
				break;
			}

			//����[��񡡡��]�����
			else if (Board[Line][BeginH + 2] % 100 <= 9) 
			{
				getScope_H_Black(Line, BeginH, EndH, 210000);
				break;
			}

			//����[�񡡡���]�����
			else if (Board[Line][BeginH + 1] % 100 <= 9) 
			{
				getScope_H_Black(Line, BeginH, EndH, 210000);
				break;
			}

			getScope_H_Black(Line, BeginH, EndH, 200000);
			break;

		default:
			break;
		}
	}

	else //ͳ�ư������ӣ�����չ�Լ�
	{
		switch(count)	//��ֱ�׼
		{
		case 1:	
			getScope_H_White(Line, BeginH, EndH, 1200);
			break;

		case 2:	
			//����[�򡡡�]�����
			if (Board[Line][BeginH + 1] % 100 <= 9) 
			{
				getScope_H_White(Line, BeginH, EndH, 5000);
				break;
			}

			getScope_H_White(Line, BeginH, EndH, 5000);
			break;

		case 3:	
			//����[��򡡡�]�����
			if (Board[Line][BeginH + 2] % 100 <= 9) 
			{
				getScope_H_White(Line, BeginH, EndH, 28000);
				break;
			}

			//����[�򡡡��]�����
			else if (Board[Line][BeginH + 1] % 100 <= 9) 
			{
				getScope_H_White(Line, BeginH, EndH, 28000);
				break;
			}

			getScope_H_White(Line, BeginH, EndH, 28000);
			break;

		case 4:	
			//����[���򡡡�]�����
			if (Board[Line][BeginH + 3] % 100 <= 9) 
			{
				getScope_H_White(Line, BeginH, EndH, 5000000);
				break;
			}

			//����[��򡡡��]�����
			else if (Board[Line][BeginH + 2] % 100 <= 9) 
			{
				getScope_H_White(Line, BeginH, EndH, 5000000);
				break;
			}

			//����[�򡡡���]�����
			else if (Board[Line][BeginH + 1] % 100 <= 9) 
			{
				getScope_H_White(Line, BeginH, EndH, 5000000);
				break;
			}

			getScope_H_White(Line, BeginH, EndH, 5000000);
			break;

		default:
			break;
		}
	}
}


//������
void Score_V(int Column, int count, int Color) 
{
	if ((Color == BLACK_COLOR) || (Color == BLACK_COLOR + 1)) //ͳ�ƺ������ӣ����¶���
	{
		switch(count)	//��ֱ�׼
		{
		case 1:	
			getScope_V_Black(Column, BeginV, EndV, 1000);
			break;

		case 2:	
			//����[�񡡡�]�����
			if (Board[BeginV + 1][Column] % 100 <= 9) 
			{
				getScope_V_Black(Column, BeginV, EndV, 2100);
				break;
			}

			getScope_V_Black(Column, BeginV, EndV, 2000);
			break;
			//����Ӧ�ÿ���������֮���пո������

		case 3:	
			//����[��񡡡�]�����
			if (Board[BeginV + 2][Column] % 100 <= 9) 
			{
				getScope_V_Black(Column, BeginV, EndV, 20000);
				break;
			}

			//����[�񡡡��]�����
			else if (Board[BeginV + 1][Column] % 100 <= 9) 
			{
				getScope_V_Black(Column, BeginV, EndV, 20000);
				break;
			}

			getScope_V_Black(Column, BeginV, EndV, 19000);
			break;
			//����Ӧ�ÿ���������֮���пո������

		case 4:	
			//����[���񡡡�]�����
			if (Board[BeginV + 3][Column] % 100 <= 9) 
			{
				getScope_V_Black(Column, BeginV, EndV, 210000);
				break;
			}

			//����[��񡡡��]�����
			else if (Board[BeginV + 2][Column] % 100 <= 9) 
			{
				getScope_V_Black(Column, BeginV, EndV, 210000);
				break;
			}

			//����[�񡡡���]�����
			else if (Board[BeginV + 1][Column] % 100 <= 9) 
			{
				getScope_V_Black(Column, BeginV, EndV, 210000);
				break;
			}

			getScope_V_Black(Column, BeginV, EndV, 200000);
			break;
			//����Ӧ�ÿ���������֮���пո������

		default:
			break;
		}
	}

	else //ͳ�ư������ӣ�����չ�Լ�
	{
		switch(count)	//��ֱ�׼
		{
		case 1:	
			getScope_V_White(Column, BeginV, EndV, 1200);
			break;

		case 2:	
			//����[�򡡡�]�����
			if (Board[BeginV + 1][Column] % 100 <= 9) 
			{
				getScope_V_White(Column, BeginV, EndV, 5000);
				break;
			}

			getScope_V_White(Column, BeginV, EndV, 5000);
			break;

		case 3:	
			//����[��򡡡�]�����
			if (Board[BeginV + 2][Column] % 100 <= 9) 
			{
				getScope_V_White(Column, BeginV, EndV, 28000);
				break;
			}

			//����[�򡡡��]�����
			else if (Board[BeginV + 1][Column] % 100 <= 9) 
			{
				getScope_V_White(Column, BeginV, EndV, 28000);
				break;
			}

			getScope_V_White(Column, BeginV, EndV, 28000);
			break;

		case 4:	
			//����[���򡡡�]�����
			if (Board[BeginV + 3][Column] % 100 <= 9) 
			{
				getScope_V_White(Column, BeginV, EndV, 5000000);
				break;
			}

			//����[��򡡡��]�����
			else if (Board[BeginV + 2][Column] % 100 <= 9) 
			{
				getScope_V_White(Column, BeginV, EndV, 5000000);
				break;
			}

			//����[�򡡡���]�����
			else if (Board[BeginV + 1][Column] % 100 <= 9) 
			{
				getScope_V_White(Column, BeginV, EndV, 5000000);
				break;
			}

			getScope_V_White(Column, BeginV, EndV, 5000000);
			break;

		default:
			break;
		}
	}
}


//һ�����޴��
void Score_OT(int Line, int Column, int count, int Color) 
{
	if ((Color == BLACK_COLOR) || (Color == BLACK_COLOR + 1)) //ͳ�ƺ������ӣ����¶���
	{
		switch(count)	//��ֱ�׼
		{
		case 1:	
			getScope_OT_Black(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 1000);
			break;

		case 2:	
			//����[�񡡡�]�����
			if (Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9) 
			{
				getScope_OT_Black(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 2100);
				break;
			}

			getScope_OT_Black(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 2000);
			break;

		case 3:	
			//����[��񡡡�]�����
			if (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			{
				getScope_OT_Black(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 20000);
				break;
			}

			//����[�񡡡��]�����
			else if (Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9) 
			{
				getScope_OT_Black(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 20000);
				break;
			}

			getScope_OT_Black(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 19000);
			break;

		case 4:	
			//����[���񡡡�]�����
			if (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9) 
			{
				getScope_OT_Black(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 210000);
				break;
			}

			//����[��񡡡��]�����
			else if (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			{
				getScope_OT_Black(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 210000);
				break;
			}

			//����[�񡡡���]�����
			else if (Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9) 
			{
				getScope_OT_Black(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 210000);
				break;
			}

			getScope_OT_Black(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 200000);
			break;

		default:
			break;
		}
	}

	else //ͳ�ư������ӣ�����չ�Լ�
	{
		switch(count)	//��ֱ�׼
		{
		case 1:	
			getScope_OT_White(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 1200);			
			break;

		case 2:	
			//����[�򡡡�]�����
			if (Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9) 
			{
				getScope_OT_White(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 5000);			
				break;
			}

			getScope_OT_White(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 5000);			
			break;

		case 3:	
			//����[��򡡡�]�����
			if (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			{
				getScope_OT_White(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 28000);			
				break;
			}

			//����[�򡡡��]�����
			else if (Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9) 
			{
				getScope_OT_White(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 28000);			
				break;
			}

			getScope_OT_White(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 28000);			
			break;

		case 4:	
			//����[���򡡡�]�����
			if (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9) 
			{
				getScope_OT_White(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 5000000);			
				break;
			}

			//����[��򡡡��]�����
			else if (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			{
				getScope_OT_White(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 5000000);			
				break;
			}

			//����[�򡡡���]�����
			else if (Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9) 
			{
				getScope_OT_White(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 5000000);			
				break;
			}

			getScope_OT_White(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 5000000);			
			break;

		default:
			break;
		}
	}
}


//�������޴��
void Score_TF(int Line, int Column, int count, int Color) 
{
	if ((Color == BLACK_COLOR) || (Color == BLACK_COLOR + 1)) //ͳ�ƺ������ӣ����¶���
	{
		switch(count)	//��ֱ�׼
		{
		case 1:	
			getScope_TF_Black(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 1000);
			break;

		case 2:	
			//����[�񡡡�]�����
			if (Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9) 
			{
				getScope_TF_Black(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 2100);				
				break;
			}

			getScope_TF_Black(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 2000);				
			break;

		case 3:	
			//����[��񡡡�]�����
			if (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			{
				getScope_TF_Black(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 20000);				
				break;
			}

			//����[�񡡡��]�����
			else if (Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9) 
			{
				getScope_TF_Black(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 20000);				
				break;
			}

			getScope_TF_Black(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 19000);				
			break;

		case 4:	
			//����[���񡡡�]�����
			if (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9) 
			{
				getScope_TF_Black(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 210000);				
				break;
			}

			//����[��񡡡��]�����
			else if (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			{
				getScope_TF_Black(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 210000);				
				break;
			}

			//����[�񡡡���]�����
			else if (Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9) 
			{
				getScope_TF_Black(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 210000);				
				break;
			}

			getScope_TF_Black(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 200000);				
			break;

		default:
			break;
		}
	}

	else //ͳ�ư������ӣ�����չ�Լ�
	{
		switch(count)	//��ֱ�׼
		{
		case 1:	
			getScope_TF_White(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 1200);			
			break;

		case 2:	
			//����[�򡡡�]�����
			if (Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9) 
			{
				getScope_TF_White(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 5000);			
				break;
			}

			getScope_TF_White(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 5000);			
			break;

		case 3:	
			//����[��򡡡�]�����
			if (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			{
				getScope_TF_White(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 28000);			
				break;
			}

			//����[�򡡡��]�����
			else if (Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9) 
			{
				getScope_TF_White(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 28000);			
				break;
			}

			getScope_TF_White(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 28000);			
			break;

		case 4:	
			//����[���򡡡�]�����
			if (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9) 
			{
				getScope_TF_White(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 5000000);			
				break;
			}

			//����[��򡡡��]�����
			else if (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			{
				getScope_TF_White(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 5000000);			
				break;
			}

			//����[�򡡡���]�����
			else if (Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9) 
			{
				getScope_TF_White(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 5000000);			
				break;
			}

			getScope_TF_White(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 5000000);			
			break;

		default:
			break;
		}
	}
}


//����º����ֱ���
void getScope_H_Black(int Line, int BeginH, int EndH, int scope) 
{
	//���˶������Ӷ���
	if (((Board[Line][BeginH - 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH - 1] % 100 == WHITE_COLOR + 1) || (BeginH == 0))
		&&
		((Board[Line][EndH] % 100 == WHITE_COLOR) || (Board[Line][EndH] % 100 == WHITE_COLOR + 1) || (EndH - 1 == BOARD_SIZE)))
	{
		//����[�񡡡�]�����
		if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*1;
		}

		//����[��񡡡�]�����
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9) 
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*1;
		}

		//����[�񡡡��]�����
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*1;
		}

		//����[���񡡡�]�����
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 <= 9)
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 3] += scope*10;
		}

		//����[��񡡡��]�����
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9)
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*10;
		}

		//����[�񡡡���]�����
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*10;
		}

		return;
	}


	//���˾��ް���(==����û���ӣ���Ϊֻ�а��ӡ����ӡ�������״̬�����к��ӣ����ͳ�����Ӵ����Ӹ���)
	else if ((Board[Line][BeginH - 1] % 100 <= 9) && (Board[Line][EndH] % 100 <= 9) && (0 < BeginH) && (EndH - 1 < BOARD_SIZE))
	{
		//����[�񡡡�]�����
		if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*4;
		}

		//����[��񡡡�]�����
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9)  
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*4;
		}

		//����[�񡡡��]�����
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*4;
		}

		//����[���񡡡�]�����
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 <= 9)
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 3] += scope*10;
		}

		//����[��񡡡��]�����
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9)
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*10;
		}

		//����[�񡡡���]�����
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*10;
		}


		//[Begin - 1]���£�[End + 1]δ���� 
		if (((Board[Line][BeginH - 2] == WHITE_COLOR) || (Board[Line][BeginH - 2] == WHITE_COLOR + 1) || (BeginH - 1 == 0))
			&& 
			((Board[Line][EndH + 1] != WHITE_COLOR) && (Board[Line][EndH + 1] != WHITE_COLOR + 1) && (EndH != BOARD_SIZE)))
		{
			Board[Line][BeginH - 1] += scope*1;
			Board[Line][EndH] += scope*3;
		}

		//[Begin - 1]δ���£�[End + 1]���� 
		else if (((Board[Line][BeginH - 2] != WHITE_COLOR) && (Board[Line][BeginH - 2] != WHITE_COLOR + 1) && (BeginH - 1 != 0))
			&&
			((Board[Line][EndH + 1] == WHITE_COLOR) || (Board[Line][EndH + 1] == WHITE_COLOR + 1) || (EndH == BOARD_SIZE)))
		{
			Board[Line][BeginH - 1] += scope*3;
			Board[Line][EndH] += scope*1;
		}

		//[Begin - 1]��[End + 1]������
		else if (((Board[Line][BeginH - 2] == WHITE_COLOR) || (Board[Line][BeginH - 2] == WHITE_COLOR + 1) || (BeginH - 1 == 0))
			&&
			((Board[Line][EndH + 1] == WHITE_COLOR) || (Board[Line][EndH + 1] == WHITE_COLOR + 1) || (EndH == BOARD_SIZE)))
		{
			Board[Line][BeginH - 1] += scope*1;
			Board[Line][EndH] += scope*1;
		}

		else
		{
			Board[Line][BeginH - 1] += scope*4;
			Board[Line][EndH] += scope*4;
		}
	}


	//begin�˱����Ӷ£�ͬʱ��end���ް��� 
	else if (((Board[Line][BeginH - 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH - 1] % 100 == WHITE_COLOR + 1) || (BeginH == 0)) 
		&& 
		((Board[Line][EndH] % 100 <= 9) && (EndH - 1 < BOARD_SIZE))) 
	{
		//����[�񡡡�]�����
		if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*2;
		}

		//����[��񡡡�]�����
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9) 
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*2;
		}

		//����[�񡡡��]�����
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*2;
		}

		//����[���񡡡�]�����
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 <= 9)
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 3] += scope*10;
		}

		//����[��񡡡��]�����
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9)
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*10;
		}

		//����[�񡡡���]�����
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*10;
		}

		//[End + 1]������ 
		if ((Board[Line][EndH + 1] == WHITE_COLOR) || (Board[Line][EndH + 1] == WHITE_COLOR + 1) || (EndH == BOARD_SIZE))
		{
			Board[Line][EndH] += scope*1;
		}

		else
		{
			Board[Line][EndH] += scope*2;
		}
	}


	//begin���ް��ӣ�ͬʱ��end�˱����Ӷ� 
	else if (((Board[Line][BeginH - 1] % 100 <= 9) && (0 < BeginH))
		&& 
		((Board[Line][EndH] % 100 == WHITE_COLOR) || (Board[Line][EndH] % 100 == WHITE_COLOR + 1) || (EndH - 1 == BOARD_SIZE))) 
	{
		//����[�񡡡�]�����
		if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*2;
		}

		//����[��񡡡�]�����
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9) 
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*2;
		}

		//����[�񡡡��]�����
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*2;
		}

		//����[���񡡡�]�����
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 <= 9)
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 3] += scope*10;
		}

		//����[��񡡡��]�����
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9)
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*10;
		}

		//����[�񡡡���]�����
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*10;
		}

		//////[Begin - 1]������ 
		if ((Board[Line][BeginH - 2] == WHITE_COLOR) || (Board[Line][BeginH - 2] == WHITE_COLOR + 1) || (BeginH - 1 == 0))
		{
			Board[Line][BeginH - 1] += scope*1;
		}

		else
		{
			Board[Line][BeginH - 1] += scope*2;
		}
	}
}


//����չ�����ֱ���
void getScope_H_White(int Line, int BeginH, int EndH, int scope) 
{
	//���˶������Ӷ���
	if (((Board[Line][BeginH - 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH - 1] % 100 == BLACK_COLOR + 1) || (BeginH == 0))
		&& 
		((Board[Line][EndH] % 100 == BLACK_COLOR) || (Board[Line][EndH] % 100 == BLACK_COLOR + 1) || (EndH - 1 == BOARD_SIZE)))
	{
		//����[�򡡡�]�����
		if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*1;
		}

		//����[��򡡡�]�����
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9) 
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*1;
		}

		//����[�򡡡��]�����
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*1;
		}

		//����[���򡡡�]�����
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 <= 9)
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 3] += scope*10;
		}

		//����[��򡡡��]�����
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&&(Board[Line][BeginH + 2] % 100 <= 9)
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*10;
		}

		//����[�򡡡���]�����
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*10;
		}

		return;
	}


	//���˾��޺���
	else if ((Board[Line][BeginH - 1] % 100 <= 9) && (Board[Line][EndH] % 100 <= 9) && (0 < BeginH) && (EndH - 1 < BOARD_SIZE))
	{
		//����[�򡡡�]�����
		if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*4;
		}

		//����[��򡡡�]�����
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9)  
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*4;
		}

		//����[�򡡡��]�����
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*4;
		}

		//����[���򡡡�]�����
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 <= 9)
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 3] += scope*10;
		}

		//����[��򡡡��]�����
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9)
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*10;
		}

		//����[�򡡡���]�����
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*10;
		}


		//[Begin - 1]���£�[End + 1]δ���� 
		if (((Board[Line][BeginH - 2] == BLACK_COLOR) || (Board[Line][BeginH - 2] == BLACK_COLOR + 1) || (BeginH - 1 == 0))
			&& 
			((Board[Line][EndH + 1] != BLACK_COLOR) && (Board[Line][EndH + 1] != BLACK_COLOR + 1) && (EndH != BOARD_SIZE)))
		{
			Board[Line][BeginH - 1] += scope*1;
			Board[Line][EndH] += scope*3;
		}

		//[Begin - 1]δ���£�[End + 1]���� 
		else if (((Board[Line][BeginH - 2] != BLACK_COLOR) && (Board[Line][BeginH - 2] != BLACK_COLOR + 1) && (BeginH - 1 != 0))
			&&
			((Board[Line][EndH + 1] == BLACK_COLOR) || (Board[Line][EndH + 1] == BLACK_COLOR + 1) || (EndH == BOARD_SIZE)))
		{
			Board[Line][BeginH - 1] += scope*3;
			Board[Line][EndH] += scope*1;
		}

		//[Begin - 1]��[End + 1]������
		else if (((Board[Line][BeginH - 2] == BLACK_COLOR) || (Board[Line][BeginH - 2] == BLACK_COLOR + 1) || (BeginH - 1 == 0))
			&&
			((Board[Line][EndH + 1] == BLACK_COLOR) || (Board[Line][EndH + 1] == BLACK_COLOR + 1) || (EndH == BOARD_SIZE)))
		{
			Board[Line][BeginH - 1] += scope*1;
			Board[Line][EndH] += scope*1;
		}

		else
		{
			Board[Line][BeginH - 1] += scope*4;
			Board[Line][EndH] += scope*4;
		}
	}


	//begin�˱����Ӷ£�ͬʱ��end���޺��� 
	else if (((Board[Line][BeginH - 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH - 1] % 100 == BLACK_COLOR + 1) || (BeginH == 0))
		&& 
		((Board[Line][EndH] % 100 <= 9) && (EndH - 1 < BOARD_SIZE))) 
	{
		//����[�򡡡�]�����
		if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*2;
		}

		//����[��򡡡�]�����
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9) 
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*2;
		}

		//����[�򡡡��]�����
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*2;
		}

		//����[���򡡡�]�����
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 <= 9)
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 3] += scope*10;
		}

		//����[��򡡡��]�����
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9)
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*10;
		}

		//����[�򡡡���]�����
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*10;
		}

		//[End + 1]������ 
		if ((Board[Line][EndH + 1] == BLACK_COLOR) || (Board[Line][EndH + 1] == BLACK_COLOR + 1) || (EndH == BOARD_SIZE))
		{
			Board[Line][EndH] += scope*1;
		}

		else
		{
			Board[Line][EndH] += scope*2;
		}
	}


	//begin���޺��ӣ�ͬʱ��end�˱����Ӷ� 
	else if (((Board[Line][BeginH - 1] % 100 <= 9) && (0 < BeginH))
		&& 
		((Board[Line][EndH] % 100 == BLACK_COLOR) || (Board[Line][EndH] % 100 == BLACK_COLOR + 1) || (EndH - 1 == BOARD_SIZE))) 
	{
		//����[�򡡡�]�����
		if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*2;
		}

		//����[��򡡡�]�����
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9) 
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*2;
		}

		//����[�򡡡��]�����
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*2;
		}

		//����[���򡡡�]�����
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1)) 
			&& (Board[Line][BeginH + 3] % 100 <= 9)
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 3] += scope*10;
		}

		//����[��򡡡��]�����
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9)
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*10;
		}

		//����[�򡡡���]�����
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*10;
		}

		//////[Begin - 1]������ 
		if ((Board[Line][BeginH - 2] == BLACK_COLOR) || (Board[Line][BeginH - 2] == BLACK_COLOR + 1) || (BeginH - 1 == 0))
		{
			Board[Line][BeginH - 1] += scope*1;
		}

		else
		{
			Board[Line][BeginH - 1] += scope*2;
		}
	}
}


//����º����ֱ���
void getScope_V_Black(int Column, int BeginV, int EndV, int scope) 
{
	//���˶������Ӷ���
	if (((Board[BeginV - 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV - 1][Column] % 100 == WHITE_COLOR + 1) || (BeginV == 0))
		&&
		((Board[EndV][Column] % 100 == WHITE_COLOR) || (Board[EndV][Column] % 100 == WHITE_COLOR + 1) || (EndV - 1 == BOARD_SIZE)))
	{
		//����[�񡡡�]�����
		if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*1;
		}

		//����[��񡡡�]�����
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9) 
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*1;
		}

		//����[�񡡡��]�����
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*1;
		}

		//����[���񡡡�]�����
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 <= 9)
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 3][Column] += scope*10;
		}

		//����[��񡡡��]�����
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*10;
		}

		//����[�񡡡���]�����
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*10;
		}

		return;
	}


	//���˾��ް���(==����û���ӣ���Ϊֻ�а��ӡ����ӡ�������״̬�����к��ӣ����ͳ�����Ӵ����Ӹ���)
	else if ((Board[BeginV - 1][Column] % 100 <= 9) && (Board[EndV][Column] % 100 <= 9) && (0 < BeginV) && (EndV - 1 < BOARD_SIZE))
	{
		//����[�񡡡�]�����
		if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*4;
		}

		//����[��񡡡�]�����
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)  
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*4;
		}

		//����[�񡡡��]�����
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*4;
		}

		//����[���񡡡�]�����
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 <= 9)
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 3][Column] += scope*10;
		}

		//����[��񡡡��]�����
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*10;
		}

		//����[�񡡡���]�����
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*10;
		}


		//[Begin - 1]���£�[End + 1]δ���� 
		if (((Board[BeginV - 2][Column] == WHITE_COLOR) || (Board[BeginV - 2][Column] == WHITE_COLOR + 1) || (BeginV - 1 == 0))
			&& 
			((Board[EndV + 1][Column] != WHITE_COLOR) && (Board[EndV + 1][Column] != WHITE_COLOR + 1) && (EndV != BOARD_SIZE)))
		{
			Board[BeginV - 1][Column] += scope*1;
			Board[EndV][Column] += scope*3;
		}


		//[Begin - 1]δ���£�[End + 1]���� 
		else if (((Board[BeginV - 2][Column] != WHITE_COLOR) && (Board[BeginV - 2][Column] != WHITE_COLOR + 1) && (BeginV - 1 != 0))
			&&
			((Board[EndV + 1][Column] == WHITE_COLOR) || (Board[EndV + 1][Column] == WHITE_COLOR + 1) || (EndV == BOARD_SIZE)))
		{
			Board[BeginV - 1][Column] += scope*3;
			Board[EndV][Column] += scope*1;
		}

		//[Begin - 1]��[End + 1]������
		else if (((Board[BeginV - 2][Column] == WHITE_COLOR) || (Board[BeginV - 2][Column] == WHITE_COLOR + 1) || (BeginV - 1 == 0))
			&&
			((Board[EndV + 1][Column] == WHITE_COLOR) || (Board[EndV + 1][Column] == WHITE_COLOR + 1) || (EndV == BOARD_SIZE)))
		{
			Board[BeginV - 1][Column] += scope*1;
			Board[EndV][Column] += scope*1;
		}

		else
		{
			Board[BeginV - 1][Column] += scope*4;
			Board[EndV][Column] += scope*4;
		}
	}


	//begin�˱����Ӷ£�ͬʱ��end���ް��� 
	else if (((Board[BeginV - 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV - 1][Column] % 100 == WHITE_COLOR + 1) || (BeginV == 0)) 
		&& 
		((Board[EndV][Column] % 100 <= 9) && (EndV - 1 < BOARD_SIZE))) 
	{
		//����[�񡡡�]�����
		if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*2;
		}

		//����[��񡡡�]�����
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9) 
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*2;
		}

		//����[�񡡡��]�����
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*2;
		}

		//����[���񡡡�]�����
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 <= 9)
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 3][Column] += scope*10;
		}

		//����[��񡡡��]�����
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*10;
		}

		//����[�񡡡���]�����
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*10;
		}


		//[End + 1]������ 
		if ((Board[EndV + 1][Column] == WHITE_COLOR) || (Board[EndV + 1][Column] == WHITE_COLOR + 1) || (EndV == BOARD_SIZE))
		{
			Board[EndV][Column] += scope*1;
		}

		else
		{
			Board[EndV][Column] += scope*2;
		}
	}


	//begin���ް��ӣ�ͬʱ��end�˱����Ӷ� 
	else if (((Board[BeginV - 1][Column] % 100 <= 9) && (0 < BeginV))
		&& 
		((Board[EndV][Column] % 100 == WHITE_COLOR) || (Board[EndV][Column] % 100 == WHITE_COLOR + 1) || (EndV - 1 == BOARD_SIZE))) 
	{
		//����[�񡡡�]�����
		if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*2;
		}

		//����[��񡡡�]�����
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9) 
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*2;
		}

		//����[�񡡡��]�����
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*2;
		}

		//����[���񡡡�]�����
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 <= 9)
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 3][Column] += scope*10;
		}

		//����[��񡡡��]�����
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*10;
		}

		//����[�񡡡���]�����
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*10;
		}

		//////[Begin - 1]������ 
		if ((Board[BeginV - 2][Column] == WHITE_COLOR) || (Board[BeginV - 2][Column] == WHITE_COLOR + 1) || (BeginV - 1 == 0))
		{
			Board[BeginV - 1][Column] += scope*1;
		}

		else
		{
			Board[BeginV - 1][Column] += scope*2;
		}
	}
}


//����չ�����ֱ���
void getScope_V_White(int Column, int BeginV, int EndV, int scope) 
{
	//���˶������Ӷ���
	if (((Board[BeginV - 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV - 1][Column] % 100 == BLACK_COLOR + 1) || (BeginV == 0))
		&& 
		((Board[EndV][Column] % 100 == BLACK_COLOR) || (Board[EndV][Column] % 100 == BLACK_COLOR + 1) || (EndV - 1 == BOARD_SIZE)))
	{
		//����[�򡡡�]�����
		if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*1;
		}

		//����[��򡡡�]�����
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9) 
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*1;
		}

		//����[�򡡡��]�����
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*1;
		}

		//����[���򡡡�]�����
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 <= 9)
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 3][Column] += scope*10;
		}

		//����[��򡡡��]�����
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*10;
		}

		//����[�򡡡���]�����
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*10;
		}

		return;
	}


	//���˾��޺���
	else if ((Board[BeginV - 1][Column] % 100 <= 9) && (Board[EndV][Column] % 100 <= 9) && (0 < BeginV) && (EndV - 1 < BOARD_SIZE))
	{
		//����[�򡡡�]�����
		if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*4;
		}

		//����[��򡡡�]�����
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)  
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*4;
		}

		//����[�򡡡��]�����
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*4;
		}

		//����[���򡡡�]�����
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 <= 9)
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 3][Column] += scope*10;
		}

		//����[��򡡡��]�����
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*10;
		}

		//����[�򡡡���]�����
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*10;
		}


		//[Begin - 1]���£�[End + 1]δ���� 
		if (((Board[BeginV - 2][Column] == BLACK_COLOR) || (Board[BeginV - 2][Column] == BLACK_COLOR + 1) || (BeginV - 1 == 0))
			&& 
			((Board[EndV + 1][Column] != BLACK_COLOR) && (Board[EndV + 1][Column] != BLACK_COLOR + 1) && (EndV != BOARD_SIZE)))
		{
			Board[BeginV - 1][Column] += scope*1;
			Board[EndV][Column] += scope*3;
		}


		//[Begin - 1]δ���£�[End + 1]���� 
		else if (((Board[BeginV - 2][Column] != BLACK_COLOR) && (Board[BeginV - 2][Column] != BLACK_COLOR + 1) && (BeginV - 1 != 0))
			&&
			((Board[EndV + 1][Column] == BLACK_COLOR) || (Board[EndV + 1][Column] == BLACK_COLOR + 1) || (EndV == BOARD_SIZE)))
		{
			Board[BeginV - 1][Column] += scope*3;
			Board[EndV][Column] += scope*1;
		}

		//[Begin - 1]��[End + 1]������
		else if (((Board[BeginV - 2][Column] == BLACK_COLOR) || (Board[BeginV - 2][Column] == BLACK_COLOR + 1) || (BeginV - 1 == 0))
			&&
			((Board[EndV + 1][Column] == BLACK_COLOR) || (Board[EndV + 1][Column] == BLACK_COLOR + 1) || (EndV == BOARD_SIZE)))
		{
			Board[BeginV - 1][Column] += scope*1;
			Board[EndV][Column] += scope*1;
		}

		else
		{
			Board[BeginV - 1][Column] += scope*4;
			Board[EndV][Column] += scope*4;
		}
	}


	//begin�˱����Ӷ£�ͬʱ��end���޺��� 
	else if (((Board[BeginV - 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV - 1][Column] % 100 == BLACK_COLOR + 1) || (BeginV == 0)) 
		&& 
		((Board[EndV][Column] % 100 <= 9) && (EndV - 1 < BOARD_SIZE))) 
	{
		//����[�򡡡�]�����
		if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*2;
		}

		//����[��򡡡�]�����
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9) 
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*2;
		}

		//����[�򡡡��]�����
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*2;
		}

		//����[���򡡡�]�����
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 <= 9)
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 3][Column] += scope*10;
		}

		//����[��򡡡��]�����
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*10;
		}

		//����[�򡡡���]�����
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*10;
		}


		//[End + 1]������ 
		if ((Board[EndV + 1][Column] == BLACK_COLOR) || (Board[EndV + 1][Column] == BLACK_COLOR + 1) || (EndV == BOARD_SIZE))
		{
			Board[EndV][Column] += scope*1;
		}

		else
		{
			Board[EndV][Column] += scope*2;
		}
	}


	//begin���޺��ӣ�ͬʱ��end�˱����Ӷ� 
	else if (((Board[BeginV - 1][Column] % 100 <= 9) && (0 < BeginV)) 
		&& 
		((Board[EndV][Column] % 100 == BLACK_COLOR) || (Board[EndV][Column] % 100 == BLACK_COLOR + 1) || (EndV - 1 == BOARD_SIZE))) 
	{
		//����[�򡡡�]�����
		if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*2;
		}

		//����[��򡡡�]�����
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9) 
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*2;
		}

		//����[�򡡡��]�����
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*2;
		}

		//����[���򡡡�]�����
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 <= 9)
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 3][Column] += scope*10;
		}

		//����[��򡡡��]�����
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*10;
		}

		//����[�򡡡���]�����
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*10;
		}


		//////[Begin - 1]������ 
		if ((Board[BeginV - 2][Column] == BLACK_COLOR) || (Board[BeginV - 2][Column] == BLACK_COLOR + 1) || (BeginV - 1 == 0))
		{
			Board[BeginV - 1][Column] += scope*1;
		}

		else
		{
			Board[BeginV - 1][Column] += scope*2;
		}
	}
}


//һ�����޶º����ֱ���
void getScope_OT_Black(int Line, int Column, int BeginOTx, int BeginOTy, int EndOTx, int EndOTy, int scope) 
{
	//���˶������Ӷ���
	if (((Board[BeginOTy - 1][BeginOTx - 1] % 100 == WHITE_COLOR) || (Board[BeginOTy - 1][BeginOTx - 1] % 100 == WHITE_COLOR + 1)
		|| (BeginOTy == 0) || (BeginOTx == 0))
		&& 
		((Board[EndOTy][EndOTx] % 100 == WHITE_COLOR) || (Board[EndOTy][EndOTx] % 100 == WHITE_COLOR + 1)
		|| (EndOTy - 1 == BOARD_SIZE) || (EndOTx - 1 == BOARD_SIZE)))
	{
		//����[�񡡡�]�����
		if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*1;
		}

		//����[��񡡡�]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*1;
		}

		//����[�񡡡��]�����
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*1;
		}

		//����[���񡡡�]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9)
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 3][BeginOTx + 3] += scope*10;
		}

		//����[��񡡡��]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*10;
		}

		//����[�񡡡���]�����
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*10;
		}

		return;
	}


	//���˾��ް���(==����û���ӣ���Ϊֻ�а��ӡ����ӡ�������״̬�����к��ӣ����ͳ�����Ӵ����Ӹ���)
	else if ((Board[BeginOTy - 1][BeginOTx - 1] % 100 <= 9) && (Board[EndOTy][EndOTx] % 100 <= 9) 
		&& 
		(0 < BeginOTy) && (0 < BeginOTx) && (EndOTy - 1 < BOARD_SIZE) && (EndOTx - 1 < BOARD_SIZE))
	{
		//����[�񡡡�]�����
		if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*4;
		}

		//����[��񡡡�]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)  
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*4;
		}

		//����[�񡡡��]�����
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*4;
		}

		//����[���񡡡�]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9)
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 3][BeginOTx + 3] += scope*10;
		}

		//����[��񡡡��]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*10;
		}

		//����[�񡡡���]�����
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*10;
		}


		//////////////////////////////[Begin - 1]���£�[End + 1]δ���� 
		if (((Board[BeginOTy - 2][BeginOTx - 2] == WHITE_COLOR) || (Board[BeginOTy - 2][BeginOTx - 2] == WHITE_COLOR + 1) 
			|| (BeginOTy - 1 == 0) || (BeginOTx - 1 == 0))
			&& 
			((Board[EndOTy + 1][EndOTx + 1] != WHITE_COLOR) && (Board[EndOTy + 1][EndOTx + 1] != WHITE_COLOR + 1) 
			&& (EndOTy != BOARD_SIZE) && (EndOTx != BOARD_SIZE)))
		{
			Board[BeginOTy - 1][BeginOTx - 1] += scope*1;
			Board[EndOTy][EndOTx] += scope*3;
		}


		//[Begin - 1]δ���£�[End + 1]���� 
		else if (((Board[BeginOTy - 2][BeginOTx - 2] != WHITE_COLOR) && (Board[BeginOTy - 2][BeginOTx - 2] != WHITE_COLOR + 1) 
			&& (BeginOTy - 1 != 0) && (BeginOTx - 1 != 0))
			&& 
			((Board[EndOTy + 1][EndOTx + 1] == WHITE_COLOR) || (Board[EndOTy + 1][EndOTx + 1] == WHITE_COLOR + 1) 
			|| (EndOTy == BOARD_SIZE) || (EndOTx == BOARD_SIZE)))
		{
			Board[BeginOTy - 1][BeginOTx - 1] += scope*3;
			Board[EndOTy][EndOTx] += scope*1;
		}

		//[Begin - 1]��[End + 1]������
		else if (((Board[BeginOTy - 2][BeginOTx - 2] == WHITE_COLOR) || (Board[BeginOTy - 2][BeginOTx - 2] == WHITE_COLOR + 1) 
			|| (BeginOTy - 1 == 0) || (BeginOTx - 1 == 0))
			&& 
			((Board[EndOTy + 1][EndOTx + 1] == WHITE_COLOR) || (Board[EndOTy + 1][EndOTx + 1] == WHITE_COLOR + 1) 
			|| (EndOTy == BOARD_SIZE) || (EndOTx == BOARD_SIZE)))
		{
			Board[BeginV - 1][Column] += scope*1;
			Board[EndV][Column] += scope*1;
		}

		else
		{
			Board[BeginOTy - 1][BeginOTx - 1] += scope*4;
			Board[EndOTy][EndOTx] += scope*4;
		}
	}


	//begin�˱����Ӷ£�ͬʱ��end���ް��� 
	else if (((Board[BeginOTy - 1][BeginOTx - 1] % 100 == WHITE_COLOR) || (Board[BeginOTy - 1][BeginOTx - 1] % 100 == WHITE_COLOR + 1)
		|| (BeginOTy == 0) || (BeginOTx == 0)) 
		&& 
		((Board[EndOTy][EndOTx] % 100 <= 9) && (EndOTy - 1 < BOARD_SIZE) && (EndOTx - 1 < BOARD_SIZE))) 
	{
		//����[�񡡡�]�����
		if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy][BeginOTx + 1] += scope*2;
		}

		//����[��񡡡�]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*2;
		}

		//����[�񡡡��]�����
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*2;
		}

		//����[���񡡡�]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9)
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 3][BeginOTx + 3] += scope*10;
		}

		//����[��񡡡��]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*10;
		}

		//����[�񡡡���]�����
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*10;
		}


		//[End + 1]������ 
		if ((Board[EndOTy + 1][EndOTx + 1] == WHITE_COLOR) || (Board[EndOTy + 1][EndOTx + 1] == WHITE_COLOR + 1) 
			|| (EndOTy == BOARD_SIZE) || (EndOTx == BOARD_SIZE))
		{
			Board[EndOTy][EndOTx] += scope*1;
		}

		else
		{
			Board[EndOTy][EndOTx] += scope*2;
		}
	}


	//begin���ް��ӣ�ͬʱ��end�˱����Ӷ� 
	else if (((Board[BeginOTy - 1][BeginOTx - 1] % 100 <= 9) && (0 < BeginOTy) && (0 < BeginOTx)) 
		&& 
		((Board[EndOTy][EndOTx] % 100 == WHITE_COLOR) || (Board[EndOTy][EndOTx] % 100 == WHITE_COLOR + 1) 
		|| (EndOTy - 1 == BOARD_SIZE) || (EndOTx - 1 == BOARD_SIZE))) 
	{
		//����[�񡡡�]�����
		if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*2;
		}

		//����[��񡡡�]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*2;
		}

		//����[�񡡡��]�����
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*2;
		}

		//����[���񡡡�]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9)
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 3][BeginOTx + 3] += scope*10;
		}

		//����[��񡡡��]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*10;
		}

		//����[�񡡡���]�����
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*10;
		}


		//////[Begin - 1]������ 
		if ((Board[BeginOTy - 2][BeginOTx - 2] == WHITE_COLOR) || (Board[BeginOTy - 2][BeginOTx - 2] == WHITE_COLOR + 1) 
			|| (BeginOTy - 1 == 0) || (BeginOTx - 1 == 0))
		{
			Board[BeginOTy - 1][BeginOTx - 1] += scope*1;
		}

		else
		{
			Board[BeginOTy - 1][BeginOTx - 1] += scope*2;
		}
	}
}


//һ�����޷�չ�����ֱ���
void getScope_OT_White(int Line, int Column, int BeginOTx, int BeginOTy, int EndOTx, int EndOTy, int scope) 
{
	//���˶������Ӷ���
	if (((Board[BeginOTy - 1][BeginOTx - 1] % 100 == BLACK_COLOR) || (Board[BeginOTy - 1][BeginOTx - 1] % 100 == BLACK_COLOR + 1)
		|| (BeginOTy == 0) || (BeginOTx == 0))
		&& 
		((Board[EndOTy][EndOTx] % 100 == BLACK_COLOR) || (Board[EndOTy][EndOTx] % 100 == BLACK_COLOR + 1)
		|| (EndOTy - 1 == BOARD_SIZE) || (EndOTx - 1 == BOARD_SIZE)))
	{
		//����[�򡡡�]�����
		if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*1;
		}

		//����[��򡡡�]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*1;
		}

		//����[�򡡡��]�����
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*1;
		}

		//����[���򡡡�]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9)
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 3][BeginOTx + 3] += scope*10;
		}

		//����[��򡡡��]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*10;
		}

		//����[�򡡡���]�����
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*10;
		}

		return;
	}


	//���˾��޺���(==����û���ӣ���Ϊֻ�а��ӡ����ӡ�������״̬�����к��ӣ����ͳ�����Ӵ����Ӹ���)
	else if ((Board[BeginOTy - 1][BeginOTx - 1] % 100 <= 9) && (Board[EndOTy][EndOTx] % 100 <= 9)
		&& (0 < BeginOTy) && (0 < BeginOTx) && (EndOTy - 1 < BOARD_SIZE) && (EndOTx - 1 < BOARD_SIZE))
	{
		//����[�򡡡�]�����
		if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*4;
		}

		//����[��򡡡�]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)  
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*4;
		}

		//����[�򡡡��]�����
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*4;
		}

		//����[���򡡡�]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9)
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 3][BeginOTx + 3] += scope*10;
		}

		//����[��򡡡��]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*10;
		}

		//����[�򡡡���]�����
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*10;
		}


		//////////////////////////////[Begin - 1]���£�[End + 1]δ���� 
		if (((Board[BeginOTy - 2][BeginOTx - 2] == BLACK_COLOR) || (Board[BeginOTy - 2][BeginOTx - 2] == BLACK_COLOR + 1) 
			|| (BeginOTy - 1 == 0) || (BeginOTx - 1 == 0))
			&& 
			((Board[EndOTy + 1][EndOTx + 1] != BLACK_COLOR) && (Board[EndOTy + 1][EndOTx + 1] != BLACK_COLOR + 1) 
			&& (EndOTy != BOARD_SIZE) && (EndOTx != BOARD_SIZE)))
		{
			Board[BeginOTy - 1][BeginOTx - 1] += scope*1;
			Board[EndOTy][EndOTx] += scope*3;
		}


		//[Begin - 1]δ���£�[End + 1]���� 
		else if (((Board[BeginOTy - 2][BeginOTx - 2] != BLACK_COLOR) && (Board[BeginOTy - 2][BeginOTx - 2] != BLACK_COLOR + 1) 
			&& (BeginOTy - 1 != 0) && (BeginOTx - 1 != 0))
			&& 
			((Board[EndOTy + 1][EndOTx + 1] == BLACK_COLOR) || (Board[EndOTy + 1][EndOTx + 1] == BLACK_COLOR + 1) 
			|| (EndOTy == BOARD_SIZE) || (EndOTx == BOARD_SIZE)))
		{
			Board[BeginOTy - 1][BeginOTx - 1] += scope*3;
			Board[EndOTy][EndOTx] += scope*1;
		}

		//[Begin - 1]��[End + 1]������
		else if (((Board[BeginOTy - 2][BeginOTx - 2] == BLACK_COLOR) || (Board[BeginOTy - 2][BeginOTx - 2] == BLACK_COLOR + 1) 
			|| (BeginOTy - 1 == 0) || (BeginOTx - 1 == 0))
			&& 
			((Board[EndOTy + 1][EndOTx + 1] == BLACK_COLOR) || (Board[EndOTy + 1][EndOTx + 1] == BLACK_COLOR + 1) 
			|| (EndOTy == BOARD_SIZE) || (EndOTx == BOARD_SIZE)))
		{
			Board[BeginV - 1][Column] += scope*1;
			Board[EndV][Column] += scope*1;
		}

		else
		{
			Board[BeginOTy - 1][BeginOTx - 1] += scope*4;
			Board[EndOTy][EndOTx] += scope*4;
		}	}


	//begin�˱����Ӷ£�ͬʱ��end���޺��� 
	else if (((Board[BeginOTy - 1][BeginOTx - 1] % 100 == BLACK_COLOR) || (Board[BeginOTy - 1][BeginOTx - 1] % 100 == BLACK_COLOR + 1)
		|| (BeginOTy == 0) || (BeginOTx == 0)) 
		&& 
		((Board[EndOTy][EndOTx] % 100 <= 9) && (EndOTy - 1 < BOARD_SIZE) && (EndOTx - 1 < BOARD_SIZE))) 
	{
		//����[�򡡡�]�����
		if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy][BeginOTx + 1] += scope*2;
		}

		//����[��򡡡�]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*2;
		}

		//����[�򡡡��]�����
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*2;
		}

		//����[���򡡡�]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9)
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 3][BeginOTx + 3] += scope*10;
		}

		//����[��򡡡��]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*10;
		}

		//����[�򡡡���]�����
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*10;
		}


		//[End + 1]������ 
		if ((Board[EndOTy + 1][EndOTx + 1] == BLACK_COLOR) || (Board[EndOTy + 1][EndOTx + 1] == BLACK_COLOR + 1) 
			|| (EndOTy == BOARD_SIZE) || (EndOTx == BOARD_SIZE))
		{
			Board[EndOTy][EndOTx] += scope*1;
		}

		else
		{
			Board[EndOTy][EndOTx] += scope*2;
		}
	}


	//begin���޺��ӣ�ͬʱ��end�˱����Ӷ� 
	else if (((Board[BeginOTy - 1][BeginOTx - 1] % 100 <= 9) && (0 < BeginOTy) && (0 < BeginOTx)) 
		&& 
		((Board[EndOTy][EndOTx] % 100 == BLACK_COLOR) || (Board[EndOTy][EndOTx] % 100 == BLACK_COLOR + 1)
		|| (EndOTy - 1 == BOARD_SIZE) || (EndOTx - 1 == BOARD_SIZE))) 
	{
		//����[�򡡡�]�����
		if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*2;
		}

		//����[��򡡡�]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*2;
		}

		//����[�򡡡��]�����
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*2;
		}

		//����[���򡡡�]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9)
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 3][BeginOTx + 3] += scope*10;
		}

		//����[��򡡡��]�����
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*10;
		}

		//����[�򡡡���]�����
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*10;
		}


		//////[Begin - 1]������ 
		if ((Board[BeginOTy - 2][BeginOTx - 2] == BLACK_COLOR) || (Board[BeginOTy - 2][BeginOTx - 2] == BLACK_COLOR + 1) 
			|| (BeginOTy - 1 == 0) || (BeginOTx - 1 == 0))
		{
			Board[BeginOTy - 1][BeginOTx - 1] += scope*1;
		}

		else
		{
			Board[BeginOTy - 1][BeginOTx - 1] += scope*2;
		}
	}
}


//�������޶º����ֱ���
void getScope_TF_Black(int Line, int Column, int BeginTFx, int BeginTFy, int EndTFx, int EndTFy, int scope) 
{
	//���˶������Ӷ���
	if (((Board[BeginTFy + 1][BeginTFx - 1] % 100 == WHITE_COLOR) || (Board[BeginTFy + 1][BeginTFx - 1] % 100 == WHITE_COLOR + 1)
		|| (BeginTFy == BOARD_SIZE) || (BeginTFx == 0))
		&& 
		((Board[EndTFy][EndTFx] % 100 == WHITE_COLOR) || (Board[EndTFy][EndTFx] % 100 == WHITE_COLOR + 1)
		|| (EndTFy + 1 == 0) || (EndTFx - 1 == BOARD_SIZE)))
	{
		//����[�񡡡�]�����
		if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*1;
		}

		//����[��񡡡�]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*1;
		}

		//����[�񡡡��]�����
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*1;
		}

		//����[���񡡡�]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9)
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 3][BeginTFx + 3] += scope*10;
		}

		//����[��񡡡��]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*10;
		}

		//����[�񡡡���]�����
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*10;
		}

		return;
	}


	//���˾��ް���(==����û���ӣ���Ϊֻ�а��ӡ����ӡ�������״̬�����к��ӣ����ͳ�����Ӵ����Ӹ���)
	else if ((Board[BeginTFy + 1][BeginTFx - 1] % 100 <= 9) && (Board[EndTFy][EndTFx] % 100 <= 9)
		&& (BeginTFy < BOARD_SIZE) && (0 < BeginTFx) && (0 < EndTFy + 1) && (EndTFx - 1 < BOARD_SIZE))
	{
		//����[�񡡡�]�����
		if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*4;
		}

		//����[��񡡡�]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)  
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*4;
		}

		//����[�񡡡��]�����
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*4;
		}

		//����[���񡡡�]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9)
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 3][BeginTFx + 3] += scope*10;
		}

		//����[��񡡡��]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*10;
		}

		//����[�񡡡���]�����
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*10;
		}


		//////////////////////////////[Begin - 1]���£�[End + 1]δ���� 
		if (((Board[BeginTFy + 2][BeginTFx - 2] == WHITE_COLOR) || (Board[BeginTFy + 2][BeginTFx - 2] == WHITE_COLOR + 1) 
			|| (BeginTFy + 1 == BOARD_SIZE) || (BeginTFx - 1 == 0))
			&& 
			((Board[EndTFy - 1][EndTFx + 1] != WHITE_COLOR) && (Board[EndTFy - 1][EndTFx + 1] != WHITE_COLOR + 1) 
			&& (EndTFy != 0) && (EndTFx != BOARD_SIZE)))
		{
			Board[BeginTFy + 1][BeginTFx - 1] += scope*1;
			Board[EndTFy][EndTFx] += scope*3;
		}


		//[Begin - 1]δ���£�[End + 1]���� 
		else if (((Board[BeginTFy + 2][BeginTFx - 2] != WHITE_COLOR) && (Board[BeginTFy + 2][BeginTFx - 2] != WHITE_COLOR + 1) 
			&& (BeginTFy + 1 != BOARD_SIZE) && (BeginTFx - 1 != 0))
			&& 
			((Board[EndTFy - 1][EndTFx + 1] == WHITE_COLOR) || (Board[EndTFy - 1][EndTFx + 1] == WHITE_COLOR + 1) 
			|| (EndTFy == 0) || (EndTFx == BOARD_SIZE)))
		{
			Board[BeginTFy + 1][BeginTFx - 1] += scope*3;
			Board[EndTFy][EndTFx] += scope*1;
		}

		//[Begin - 1]��[End + 1]������
		else if (((Board[BeginTFy + 2][BeginTFx - 2] == WHITE_COLOR) || (Board[BeginTFy + 2][BeginTFx - 2] == WHITE_COLOR + 1) 
			|| (BeginTFy + 1 == BOARD_SIZE) || (BeginTFx - 1 == 0))
			&& 
			((Board[EndTFy - 1][EndTFx + 1] == WHITE_COLOR) || (Board[EndTFy - 1][EndTFx + 1] == WHITE_COLOR + 1) 
			|| (EndTFy == 0) || (EndTFx == BOARD_SIZE)))
		{
			Board[BeginTFy + 1][BeginTFx - 1] += scope*1;
			Board[EndTFy][EndTFx] += scope*1;
		}

		else
		{
			Board[BeginTFy + 1][BeginTFx - 1] += scope*4;
			Board[EndTFy][EndTFx] += scope*4;
		}
	}


	//begin�˱����Ӷ£�ͬʱ��end���ް��� 
	else if (((Board[BeginTFy + 1][BeginTFx - 1] % 100 == WHITE_COLOR) || (Board[BeginTFy + 1][BeginTFx - 1] % 100 == WHITE_COLOR + 1)
		|| (BeginTFy == BOARD_SIZE) || (BeginTFx == 0)) 
		&& 
		((Board[EndTFy][EndTFx] % 100 <= 9) && (0 < EndTFy + 1) && (EndTFx - 1 < BOARD_SIZE))) 
	{
		//����[�񡡡�]�����
		if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy][BeginTFx + 1] += scope*2;
		}

		//����[��񡡡�]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*2;
		}

		//����[�񡡡��]�����
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*2;
		}

		//����[���񡡡�]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9)
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 3][BeginTFx + 3] += scope*10;
		}

		//����[��񡡡��]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*10;
		}

		//����[�񡡡���]�����
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*10;
		}


		//[End + 1]������ 
		if ((Board[EndTFy - 1][EndTFx + 1] == WHITE_COLOR) || (Board[EndTFy - 1][EndTFx + 1] == WHITE_COLOR + 1) 
			|| (EndTFy == 0) || (EndTFx == BOARD_SIZE))
		{
			Board[EndTFy][EndTFx] += scope*1;
		}

		else
		{
			Board[EndTFy][EndTFx] += scope*2;
		}
	}


	//begin���ް��ӣ�ͬʱ��end�˱����Ӷ� 
	else if (((Board[BeginTFy + 1][BeginTFx - 1] % 100 <= 9) && (BeginTFy < BOARD_SIZE) && (0 < BeginTFx)) 
		&& 
		((Board[EndTFy][EndTFx] % 100 == WHITE_COLOR) || (Board[EndTFy][EndTFx] % 100 == WHITE_COLOR + 1)
		|| (EndTFy + 1 == 0) || (EndTFx - 1 == BOARD_SIZE))) 
	{
		//����[�񡡡�]�����
		if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*2;
		}

		//����[��񡡡�]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*2;
		}

		//����[�񡡡��]�����
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*2;
		}

		//����[���񡡡�]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9)
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 3][BeginTFx + 3] += scope*10;
		}

		//����[��񡡡��]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*10;
		}

		//����[�񡡡���]�����
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*10;
		}


		//////[Begin - 1]������ 
		if ((Board[BeginTFy + 2][BeginTFx - 2] == WHITE_COLOR) || (Board[BeginTFy + 2][BeginTFx - 2] == WHITE_COLOR + 1) 
			|| (BeginTFy + 1 == BOARD_SIZE) || (BeginTFx - 1 == 0))
		{
			Board[BeginTFy + 1][BeginTFx - 1] += scope*1;
		}

		else
		{
			Board[BeginTFy + 1][BeginTFx - 1] += scope*2;
		}
	}
}


//�������޷�չ�����ֱ���
void getScope_TF_White(int Line, int Column, int BeginTFx, int BeginTFy, int EndTFx, int EndTFy, int scope) 
{
	//���˶������Ӷ���
	if (((Board[BeginTFy + 1][BeginTFx - 1] % 100 == BLACK_COLOR) || (Board[BeginTFy + 1][BeginTFx - 1] % 100 == BLACK_COLOR + 1)
		|| (BeginTFy == BOARD_SIZE) || (BeginTFx == 0))
		&& 
		((Board[EndTFy][EndTFx] % 100 == BLACK_COLOR) || (Board[EndTFy][EndTFx] % 100 == BLACK_COLOR + 1)
		|| (EndTFy + 1 == 0) || (EndTFx - 1 == BOARD_SIZE)))
	{
		//����[�򡡡�]�����
		if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*1;
		}

		//����[��򡡡�]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*1;
		}

		//����[�򡡡��]�����
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*1;
		}

		//����[���򡡡�]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9)
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 3][BeginTFx + 3] += scope*10;
		}

		//����[��򡡡��]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*10;
		}

		//����[�򡡡���]�����
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*10;
		}

		return;
	}


	//���˾��޺���(==����û���ӣ���Ϊֻ�к��ӡ����ӡ�������״̬�����к��ӣ����ͳ�����Ӵ����Ӹ���)
	else if ((Board[BeginTFy + 1][BeginTFx - 1] % 100 <= 9) && (Board[EndTFy][EndTFx] % 100 <= 9)
		&& (BeginTFy < BOARD_SIZE) && (0 < BeginTFx) && (0 < EndTFy + 1) && (EndTFx - 1 < BOARD_SIZE))
	{
		//����[�򡡡�]�����
		if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*4;
		}

		//����[��򡡡�]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)  
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*4;
		}

		//����[�򡡡��]�����
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*4;
		}

		//����[���򡡡�]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9)
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 3][BeginTFx + 3] += scope*10;
		}

		//����[��򡡡��]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*10;
		}

		//����[�򡡡���]�����
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*10;
		}


		//////////////////////////////[Begin - 1]���£�[End + 1]δ���� 
		if (((Board[BeginTFy + 2][BeginTFx - 2] == BLACK_COLOR) || (Board[BeginTFy + 2][BeginTFx - 2] == BLACK_COLOR + 1) 
			|| (BeginTFy + 1 == BOARD_SIZE) || (BeginTFx - 1 == 0))
			&& 
			((Board[EndTFy - 1][EndTFx + 1] != BLACK_COLOR) && (Board[EndTFy - 1][EndTFx + 1] != BLACK_COLOR + 1) 
			&& (EndTFy != 0) && (EndTFx != BOARD_SIZE)))
		{
			Board[BeginTFy + 1][BeginTFx - 1] += scope*1;
			Board[EndTFy][EndTFx] += scope*3;
		}


		//[Begin - 1]δ���£�[End + 1]���� 
		else if (((Board[BeginTFy + 2][BeginTFx - 2] != BLACK_COLOR) && (Board[BeginTFy + 2][BeginTFx - 2] != BLACK_COLOR + 1) 
			&& (BeginTFy + 1 != BOARD_SIZE) && (BeginTFx - 1 != 0))
			&& 
			((Board[EndTFy - 1][EndTFx + 1] == BLACK_COLOR) || (Board[EndTFy - 1][EndTFx + 1] == BLACK_COLOR + 1) 
			|| (EndTFy == 0) || (EndTFx == BOARD_SIZE)))
		{
			Board[BeginTFy + 1][BeginTFx - 1] += scope*3;
			Board[EndTFy][EndTFx] += scope*1;
		}

		//[Begin - 1]��[End + 1]������
		else if (((Board[BeginTFy + 2][BeginTFx - 2] == BLACK_COLOR) || (Board[BeginTFy + 2][BeginTFx - 2] == BLACK_COLOR + 1) 
			|| (BeginTFy + 1 == BOARD_SIZE) || (BeginTFx - 1 == 0))
			&& 
			((Board[EndTFy - 1][EndTFx + 1] == BLACK_COLOR) || (Board[EndTFy - 1][EndTFx + 1] == BLACK_COLOR + 1) 
			|| (EndTFy == 0) || (EndTFx == BOARD_SIZE)))
		{
			Board[BeginTFy + 1][BeginTFx - 1] += scope*1;
			Board[EndTFy][EndTFx] += scope*1;
		}

		else
		{
			Board[BeginTFy + 1][BeginTFx - 1] += scope*4;
			Board[EndTFy][EndTFx] += scope*4;
		}
	}


	//begin�˱����Ӷ£�ͬʱ��end���޺��� 
	else if (((Board[BeginTFy + 1][BeginTFx - 1] % 100 == BLACK_COLOR) || (Board[BeginTFy + 1][BeginTFx - 1] % 100 == BLACK_COLOR + 1)
		|| (BeginTFy == BOARD_SIZE) || (BeginTFx == 0)) 
		&& 
		((Board[EndTFy][EndTFx] % 100 <= 9) && (0 < EndTFy + 1) && (EndTFx - 1 < BOARD_SIZE))) 
	{
		//����[�򡡡�]�����
		if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy][BeginTFx + 1] += scope*2;
		}

		//����[��򡡡�]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*2;
		}

		//����[�򡡡��]�����
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*2;
		}

		//����[���򡡡�]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9)
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 3][BeginTFx + 3] += scope*10;
		}

		//����[��򡡡��]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*10;
		}

		//����[�򡡡���]�����
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*10;
		}



		//[End + 1]������ 
		if ((Board[EndTFy - 1][EndTFx + 1] == BLACK_COLOR) || (Board[EndTFy - 1][EndTFx + 1] == BLACK_COLOR + 1) 
			|| (EndTFy == 0) || (EndTFx == BOARD_SIZE))
		{
			Board[EndTFy][EndTFx] += scope*1;
		}

		else
		{
			Board[EndTFy][EndTFx] += scope*2;
		}
	}


	//begin���޺��ӣ�ͬʱ��end�˱����Ӷ� 
	else if (((Board[BeginTFy + 1][BeginTFx - 1] % 100 <= 9) && (BeginTFy < BOARD_SIZE) && (0 < BeginTFx)) 
		&& 
		((Board[EndTFy][EndTFx] % 100 == BLACK_COLOR) || (Board[EndTFy][EndTFx] % 100 == BLACK_COLOR + 1)
		|| (EndTFy + 1 == 0) || (EndTFx - 1 == BOARD_SIZE))) 
	{
		//����[�򡡡�]�����
		if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*2;
		}

		//����[��򡡡�]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*2;
		}

		//����[�򡡡��]�����
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*2;
		}

		//����[���򡡡�]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9)
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 3][BeginTFx + 3] += scope*10;
		}

		//����[��򡡡��]�����
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*10;
		}

		//����[�򡡡���]�����
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*10;
		}


		//////[Begin - 1]������ 
		if ((Board[BeginTFy + 2][BeginTFx - 2] == BLACK_COLOR) || (Board[BeginTFy + 2][BeginTFx - 2] == BLACK_COLOR + 1) 
			|| (BeginTFy + 1 == BOARD_SIZE) || (BeginTFx - 1 == 0))
		{
			Board[BeginTFy + 1][BeginTFx - 1] += scope*1;
		}

		else
		{
			Board[BeginTFy + 1][BeginTFx - 1] += scope*2;
		}
	}
}


//�����
void CleanScore()
{
	int x, y;
	for (y = 0; y <= BOARD_SIZE; y++)
	{
		for (x = 0; x <= BOARD_SIZE; x++)
		{
			Board[y][x] %= 100;
		}
	}

}


//������ֵ����
void MaxScore()
{
	int x = 0;
	int y = 0;

	max.Value = max.x = max.y = 0;

	for (y = 0; y <= BOARD_SIZE; y++)
	{
		for (x = 0; x <= BOARD_SIZE; x++)
		{
			if (max.Value < Board[y][x])
			{
				max.Value = Board[y][x];
				max.x = x;
				max.y = y;
			}
		}
	}
}

