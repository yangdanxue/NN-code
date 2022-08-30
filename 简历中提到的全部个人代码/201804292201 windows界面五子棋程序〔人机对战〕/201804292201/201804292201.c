/*--------------------------------------------------
LINEDEMO.C -- Line-Drawing Demonstration Program
(c) Charles Petzold, 1998
--------------------------------------------------*/

#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);


#define Chess_Board 48 //棋盘每个格子的像素数
#define BLACK_COLOR 10 //黑棋颜色
#define WHITE_COLOR 20 //白棋颜色
#define BOARD_SIZE 14 //棋盘行列数


BOOL GameOver = FALSE;



static HBITMAP hBitmap;
static int  cxClient, cyClient, cxSource, cySource;
BITMAP         bitmap;
HINSTANCE      hInstance;
HDC            hdc, hdcMem;
PAINTSTRUCT ps;



int x = 0, y = 0; //鼠标点击的横纵坐标（按像素统计）
int i, j;
int Board[15][15] = {0};
int xBlack = 0, yBlack = 0; //黑棋输入的棋子坐标：xBlack横坐标；yBlack纵坐标
int xWhite = 0, yWhite = 0; //白棋输入的棋子坐标：xWhite横坐标；yWhite纵坐标
int xBlackLast = -1, yBlackLast = -1; //黑棋上次落子的位置坐标，行：xBlackLast；列：yBlackLast
int xWhiteLast = -1, yWhiteLast = -1; //白棋上次落子的位置坐标，行：xWhiteLast；列：yWhiteLast

int count = 0; //连子数
int BeginH, EndH;//横向连子串的起始和结尾标志
int BeginV, EndV;//纵向连子串的起始和结尾标志
int BeginOTx, BeginOTy, EndOTx, EndOTy;//一三象限连子串的起始和结尾标志
int BeginTFx, BeginTFy, EndTFx, EndTFy;//二四象限连子串的起始和结尾标志


void ChooseGameMode(); //选择游戏模式
void PeopleVSComputer(); //人机对战
void PeopleVSPeople(); //人人对战
void PeopleVSPeople_BlackPlay(); //人人对战_黑棋下
void PeopleVSPeople_WhitePlay(); //人人对战_白棋下
void PeopleVSComputer_PeopleBlackPlay(); //人机对战_人持黑棋
void PeopleVSComputer_PeopleWhitePlay(); //人机对战_人持白棋
void PeopleVSComputer_ComputerBlackPlay(); //人机对战_机持黑棋
void PeopleVSComputer_ComputerWhitePlay(); //人机对战_机持白棋

void DisplayChess(); //显示棋子
void ChessShape(); //设定棋子形状——当前步的棋子形状是三角▲/△，上一步的棋子形状是圆形●/◎

void WhoIsWinning();
void WhoIsWin_Horizon(); //水平方向判断输赢
void WhoIsWin_Vertical(); //垂直方向判断输赢
void WhoIsWin_OneThreeQuadrant(); //一三象限方向判断输赢
void WhoIsWin_TwoFourQuadrant(); //二四象限方向判断输赢
void BanHand_3_Horizon(int); //水平方向（x++，y++）判断黑棋禁手
void BanHand_3_Vertical(int); //垂直方向（x++，y++）判断黑棋禁手
void BanHand_3_OneThreeQuadrant(int, int); //一三象限方向（x++，y++）判断黑棋禁手
void BanHand_3_TwoFourQuadrant(int, int); //二四象限方向（x++，y--）判断黑棋禁手
void BanHand_4_Horizon(int); //水平方向（x++，y++）判断黑棋禁手
void BanHand_4_Vertical(int); //垂直方向（x++，y++）判断黑棋禁手
void BanHand_4_OneThreeQuadrant(int, int); //一三象限方向（x++，y++）判断黑棋禁手
void BanHand_4_TwoFourQuadrant(int, int); //二四象限方向（x++，y--）判断黑棋禁手

void Score_H(int, int, int); //横向打分
void Score_V(int, int, int); //纵向打分
void Score_OT(int, int, int, int); //一三象限打分
void Score_TF(int, int, int, int); //二四象限打分
void getScope_H_Black(int Line, int BeginH, int EndH, int scope); //横向堵黑棋打分倍数
void getScope_H_White(int Line, int BeginH, int EndH, int scope); //横向发展白棋打分倍数
void getScope_V_Black(int Column, int BeginV, int EndV, int scope); //纵向堵黑棋打分倍数
void getScope_V_White(int Column, int BeginV, int EndV, int scope); //纵向发展白棋打分倍数
void getScope_OT_Black(int, int, int, int, int, int, int); //一三象限堵黑棋打分倍数
void getScope_OT_White(int, int, int, int, int, int, int); //一三象限发展白棋打分倍数
void getScope_TF_Black(int, int, int, int, int, int, int); //二四象限堵黑棋打分倍数
void getScope_TF_White(int, int, int, int, int, int, int); //二四象限发展白棋打分倍数
void CleanScore(); //清积分
void MaxScore(); //求积分最大坐标


int ChessCount_H(int Line, int Color);	//横向计连子数
int ChessCount_V(int Column, int Color);	//纵向计连子数
int ChessCount_OT(int Line, int Column, int Color); //一三象限计连子数
int ChessCount_TF(int Line, int Column, int Color); //二四象限计连子数



struct Color SetColor(struct Color); //设定棋子颜色及玩家颜色的结构体函数

struct Color //设定棋子颜色及玩家颜色的结构体
{
	int iChangeColor; //设定棋子颜色
	int iChessColor; //设定玩家颜色
};
struct Color sColor = {1, 0}; //sColor结构体中每个成员的初值，其中iChangeColor = 1; iChessColor = 0



struct MAX //设定最大值坐标结构体
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
		(Chess_Board / 2) + (14 * Chess_Board) + (65 * Chess_Board / 100), //客户区的长(棋盘左侧空1/2的棋子宽，棋盘右侧空1/2棋子宽)
		(Chess_Board / 2) + (14 * Chess_Board) + (12 * Chess_Board / 11), //客户区的宽(棋盘上方空1/2的棋子宽，棋盘下方空1/2棋子宽)
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


	//////int x = 0, y = 0; //鼠标点击的横纵坐标（按像素统计）
	//////int xMemory, yMemory; //记录已落子坐标的x，y

	static aaa[15][15]= {0};



	switch (message)
	{
	case WM_CREATE:
		hInstance = ((LPCREATESTRUCT) lParam)->hInstance ;

		hBitmap = LoadBitmap (hInstance, MAKEINTRESOURCE (IDB_BITMAP1)) ;

		GetObject (hBitmap, sizeof (BITMAP), &bitmap) ;

		cxSource = bitmap.bmWidth;
		cySource = bitmap.bmHeight / 6; //cySource是〔▲〕图片

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
		for (j = BOARD_SIZE ; 0 <= j ; j--) //(0，0)坐标在左下角
		{
			for (i = 0 ; i <= BOARD_SIZE ; i++)
			{
				if(aaa[j][i] == 1)
				{
					switch(Board[yBlack][xBlack] % 100)	//考虑有分值后的显示棋盘
					{
					case BLACK_COLOR + 1:
						BitBlt (hdc, xBlack * Chess_Board, yBlack * Chess_Board, cxSource, cySource, hdcMem, 0, 0, SRCCOPY) ; //贴▲图片,11
						break;

					case BLACK_COLOR + 2:
						BitBlt (hdc, xBlackLast * Chess_Board, yBlackLast * Chess_Board, cxSource, cySource, hdcMem, 0, 4 * cySource, SRCCOPY) ; //贴▲WIN图片,12
						break;
					}


					switch(Board[yWhite][xWhite] % 100)	//考虑有分值后的显示棋盘
					{
					case WHITE_COLOR + 1:
						BitBlt (hdc, xWhite * Chess_Board, yWhite * Chess_Board, cxSource, cySource, hdcMem, 0, cySource, SRCCOPY) ; //贴△图片,21
						break;

					case WHITE_COLOR + 2:
						BitBlt (hdc, xWhiteLast * Chess_Board, yWhiteLast * Chess_Board, cxSource, cySource, hdcMem, 0, 5 * cySource, SRCCOPY) ; //贴△WIN图片,22
						break;
					}


					switch(Board[yBlackLast][xBlackLast] % 100)	//考虑有分值后的显示棋盘
					{
					case BLACK_COLOR:
						BitBlt (hdc, xBlackLast * Chess_Board, yBlackLast * Chess_Board, cxSource, cySource, hdcMem, 0, 2 * cySource, SRCCOPY) ; //贴●图片,10
						break;
					}


					switch(Board[yWhiteLast][xWhiteLast] % 100)	//考虑有分值后的显示棋盘
					{
					case WHITE_COLOR:
						BitBlt (hdc, xWhiteLast * Chess_Board, yWhiteLast * Chess_Board, cxSource, cySource, hdcMem, 0, 3 * cySource, SRCCOPY) ; //贴◎图片,20
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

		if (sColor.iChangeColor == 1) //如果轮到黑棋落子
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

		else if (sColor.iChangeColor == 2) //如果轮到白棋落子
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



		if (sColor.iChangeColor == 1) //如果是黑棋最新一步，贴▲图片
		{
			Board[yBlack][xBlack] = BLACK_COLOR + 1;
			DisplayChess();

			if (xBlackLast != -1 && yBlackLast != -1)
			{
				Board[yBlackLast][xBlackLast] = BLACK_COLOR;
				DisplayChess();
			}
		}


		else if (sColor.iChangeColor == 2) //如果是白棋最新一步，贴△图片
		{
			Board[yWhite][xWhite] = WHITE_COLOR + 1;
			DisplayChess();

			if (xWhiteLast != -1 && yWhiteLast != -1)
			{
				Board[yWhiteLast][xWhiteLast] = WHITE_COLOR;
				DisplayChess();
			}
		}

		sColor = SetColor(sColor); //设定棋子颜色及玩家颜色

		ChessShape();

		WhoIsWinning();

		PeopleVSComputer_ComputerWhitePlay(); //机持白棋后下


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



//人机对战之机持◎
void PeopleVSComputer_ComputerWhitePlay()
{
	//增加游戏结束的判断，是为了防止黑棋禁手判白棋获胜后，白棋仍旧会落子。之后游戏结束了，白棋不会显示【△WIN】图片
	if(GameOver)
		return 0;

	if (Board[yBlack][xBlack] = BLACK_COLOR + 1)
	{
		MaxScore(); //求分值最高的位置

		xWhite = max.x;
		yWhite = max.y;
	}


	if (sColor.iChangeColor == 2) //如果是白棋最新一步，贴△图片
	{
		Board[yWhite][xWhite] = WHITE_COLOR + 1;
		DisplayChess();

		if (xWhiteLast != -1 && yWhiteLast != -1)
		{
			Board[yWhiteLast][xWhiteLast] = WHITE_COLOR;
			DisplayChess();
		}
	}

	sColor = SetColor(sColor); //设定棋子颜色及玩家颜色

	ChessShape();

	WhoIsWinning();
}




//设定棋子颜色及玩家颜色
struct Color SetColor(struct Color sColor)  
{
	if (sColor.iChangeColor == 1)
	{
		sColor.iChessColor = BLACK_COLOR + 1;
		if (xBlackLast != -1 && yBlackLast != -1) //判断黑棋玩家是否有上一步落子，有则进入if语句
		{
			Board[yBlackLast][xBlackLast] = BLACK_COLOR; //上一步棋子由▲变为●
		}
		sColor.iChangeColor = 2; //******玩家交换
	}
	else
	{
		sColor.iChessColor = WHITE_COLOR + 1;
		if (xWhiteLast != -1 && yWhiteLast != -1) //判断白棋玩家是否有上一步落子，有则进入if语句
		{
			Board[yWhiteLast][xWhiteLast] = WHITE_COLOR; //上一步棋子由△变为◎
		}
		sColor.iChangeColor = 1; //******玩家交换
	}
	//*****结束设定玩家棋子颜色
	return sColor;
}


//设定棋子形状——当前步的棋子形状是三角▲/△，上一步的棋子形状是圆形●/◎
void ChessShape()
{
	if (sColor.iChessColor == BLACK_COLOR + 1) //把当前棋子位置坐标赋给前一步位置
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


//显示棋子
void DisplayChess() 
{
	for (j = BOARD_SIZE ; 0 <= j ; j--) //(0，0)坐标在左下角
	{
		for (i = 0 ; i <= BOARD_SIZE ; i++)
		{
			switch(Board[yBlack][xBlack] % 100)	//考虑有分值后的显示棋盘
			{
			case BLACK_COLOR + 1:
				BitBlt (hdc, xBlack * Chess_Board, yBlack * Chess_Board, cxSource, cySource, hdcMem, 0, 0, SRCCOPY) ; //贴▲图片,11
				break;

			case BLACK_COLOR + 2:
				BitBlt (hdc, xBlackLast * Chess_Board, yBlackLast * Chess_Board, cxSource, cySource, hdcMem, 0, 4 * cySource, SRCCOPY) ; //贴▲WIN图片,12
				break;
			}


			switch(Board[yWhite][xWhite] % 100)	//考虑有分值后的显示棋盘
			{
			case WHITE_COLOR + 1:
				BitBlt (hdc, xWhite * Chess_Board, yWhite * Chess_Board, cxSource, cySource, hdcMem, 0, cySource, SRCCOPY) ; //贴△图片,21
				break;

			case WHITE_COLOR + 2:
				BitBlt (hdc, xWhiteLast * Chess_Board, yWhiteLast * Chess_Board, cxSource, cySource, hdcMem, 0, 5 * cySource, SRCCOPY) ; //贴△WIN图片,22
				break;
			}


			switch(Board[yBlackLast][xBlackLast] % 100)	//考虑有分值后的显示棋盘
			{
			case BLACK_COLOR:
				BitBlt (hdc, xBlackLast * Chess_Board, yBlackLast * Chess_Board, cxSource, cySource, hdcMem, 0, 2 * cySource, SRCCOPY) ; //贴●图片,10
				break;
			}


			switch(Board[yWhiteLast][xWhiteLast] % 100)	//考虑有分值后的显示棋盘
			{
			case WHITE_COLOR:
				BitBlt (hdc, xWhiteLast * Chess_Board, yWhiteLast * Chess_Board, cxSource, cySource, hdcMem, 0, 3 * cySource, SRCCOPY) ; //贴◎图片,20
				break;
			}
		}
	}
}



//横向计连子数
int ChessCount_H(int Line, int Color)
{
	while(((Board[Line][BeginH] % 100) != Color) && ((Board[Line][BeginH] % 100) != Color + 1)) //跳过非该颜色棋子的点
	{
		if (BeginH > BOARD_SIZE) //超边界了
		{
			EndH = BeginH;

			return 0;
		}
		BeginH ++;
	}

	EndH = BeginH + 1; //尾标移到始标的下一位

	//开始计连子数
	while((Board[Line][EndH] % 100 == Color) || (Board[Line][EndH] % 100 == Color + 1))//+1就是三角形的棋子颜色
	{
		EndH ++;
	}


	//*******分情况考虑连子串中间有1个空格的情形*******

	//考虑[●　●]的情况（用黑棋举例，白棋同）
	if (
		(EndH - BeginH == 1) 
		&& (Board[Line][EndH] % 100 <= 9) 
		&& ((Board[Line][EndH + 1] % 100 == Color) || (Board[Line][EndH + 1] % 100 == Color + 1)) 
		&& ((Board[Line][EndH + 2] % 100 != Color) && (Board[Line][EndH + 2] % 100 != Color + 1)) 
		)
	{
		EndH = EndH + 2;

		return EndH - BeginH - 1; //返回连子串长度
	}

	//考虑[●●　●]的情况
	else if (
		(EndH - BeginH == 2) 
		&& (Board[Line][EndH] % 100 <= 9) 
		&& ((Board[Line][EndH + 1] % 100 == Color) || (Board[Line][EndH + 1] % 100 == Color + 1))
		&& ((Board[Line][EndH + 2] % 100 != Color) && (Board[Line][EndH + 2] % 100 != Color + 1)) 
		)
	{
		EndH = EndH + 2;

		return EndH - BeginH - 1; //返回连子串长度
	}

	//考虑[●●●　●]的情况
	else if (
		(EndH - BeginH == 3) 
		&& (Board[Line][EndH] % 100 <= 9) 
		&& ((Board[Line][EndH + 1] % 100 == Color) || (Board[Line][EndH + 1] % 100 == Color + 1))
		&& ((Board[Line][EndH + 2] % 100 != Color) && (Board[Line][EndH + 2] % 100 != Color + 1)) 
		)
	{
		EndH = EndH + 2;

		return EndH - BeginH - 1; //返回连子串长度
	}

	//考虑[●　●●]的情况
	else if (
		(EndH - BeginH == 1) 
		&& (Board[Line][EndH] % 100 <= 9)
		&& ((Board[Line][EndH + 1] % 100 == Color) || (Board[Line][EndH + 1] % 100 == Color + 1))
		&& ((Board[Line][EndH + 2] % 100 == Color) || (Board[Line][EndH + 2] % 100 == Color + 1))
		&& ((Board[Line][EndH + 3] % 100 != Color) && (Board[Line][EndH + 3] % 100 != Color + 1)) 
		)
	{
		EndH = EndH + 3;

		return EndH - BeginH - 1; //返回连子串长度
	}

	//考虑[●●　●●]的情况
	else if (
		(EndH - BeginH == 2)
		&& (Board[Line][EndH] % 100 <= 9)
		&& ((Board[Line][EndH + 1] % 100 == Color) || (Board[Line][EndH + 1] % 100 == Color + 1))
		&& ((Board[Line][EndH + 2] % 100 == Color) || (Board[Line][EndH + 2] % 100 == Color + 1))
		&& ((Board[Line][EndH + 3] % 100 != Color) && (Board[Line][EndH + 3] % 100 != Color + 1)) 
		)
	{
		EndH = EndH + 3;

		return EndH - BeginH - 1; //返回连子串长度
	}

	//考虑[●　●●●]的情况
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

		return EndH - BeginH - 1; //返回连子串长度
	}

	return EndH - BeginH; //返回连子串长度
}


//纵向计连子数
int ChessCount_V(int Column, int Color)
{
	while(((Board[BeginV][Column] % 100) != Color) && ((Board[BeginV][Column] % 100) != Color + 1)) //跳过非该颜色棋子的点
	{
		if (BeginV > BOARD_SIZE) //超边界了
		{
			EndV = BeginV;

			return 0;
		}
		BeginV++;
	}

	EndV = BeginV + 1; //尾标移到始标的下一位

	//开始计连子数
	while((Board[EndV][Column] % 100 == Color) || (Board[EndV][Column] % 100 == Color + 1))//+1就是三角形的棋子颜色
	{
		EndV ++;
	}


	//*******分情况考虑连子串中间有1个空格的情形*******

	//考虑[●　●]的情况（用黑棋举例，白棋同）
	if (
		(EndV - BeginV == 1) 
		&& (Board[EndV][Column] % 100 <= 9) 
		&& ((Board[EndV + 1][Column] % 100 == Color) || (Board[EndV + 1][Column] % 100 == Color + 1)) 
		&& ((Board[EndV + 2][Column] % 100 != Color) && (Board[EndV + 2][Column] % 100 != Color + 1)) 
		)
	{
		EndV = EndV + 2;

		return EndV - BeginV - 1; //返回连子串长度
	}

	//考虑[●●　●]的情况
	else if (
		(EndV - BeginV == 2) 
		&& (Board[EndV][Column] % 100 <= 9) 
		&& ((Board[EndV + 1][Column] % 100 == Color) || (Board[EndV + 1][Column] % 100 == Color + 1))
		&& ((Board[EndV + 2][Column] % 100 != Color) && (Board[EndV + 2][Column] % 100 != Color + 1)) 
		)
	{
		EndV = EndV + 2;

		return EndV - BeginV - 1; //返回连子串长度
	}

	//考虑[●●●　●]的情况
	else if (
		(EndV - BeginV == 3) 
		&& (Board[EndV][Column] % 100 <= 9) 
		&& ((Board[EndV + 1][Column] % 100 == Color) || (Board[EndV + 1][Column] % 100 == Color + 1))
		&& ((Board[EndV + 2][Column] % 100 != Color) && (Board[EndV + 2][Column] % 100 != Color + 1)) 
		)
	{
		EndV = EndV + 2;

		return EndV - BeginV - 1; //返回连子串长度
	}

	//考虑[●　●●]的情况
	else if (
		(EndV - BeginV == 1) 
		&& (Board[EndV][Column] % 100 <= 9) 
		&& ((Board[EndV + 1][Column] % 100 == Color) || (Board[EndV + 1][Column] % 100 == Color + 1))
		&& ((Board[EndV + 2][Column] % 100 == Color) || (Board[EndV + 2][Column] % 100 == Color + 1))
		&& ((Board[EndV + 3][Column] % 100 != Color) && (Board[EndV + 3][Column] % 100 != Color + 1)) 
		)
	{
		EndV = EndV + 3;

		return EndV - BeginV - 1; //返回连子串长度
	}

	//考虑[●●　●●]的情况
	else if (
		(EndV - BeginV == 2)
		&& (Board[EndV][Column] % 100 <= 9)
		&& ((Board[EndV + 1][Column] % 100 == Color) || (Board[EndV + 1][Column] % 100 == Color + 1))
		&& ((Board[EndV + 2][Column] % 100 == Color) || (Board[EndV + 2][Column] % 100 == Color + 1))
		&& ((Board[EndV + 3][Column] % 100 != Color) && (Board[EndV + 3][Column] % 100 != Color + 1)) 
		)
	{
		EndV = EndV + 3;

		return EndV - BeginV - 1; //返回连子串长度
	}

	//考虑[●　●●●]的情况
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

		return EndV - BeginV - 1; //返回连子串长度
	}

	return EndV - BeginV; //返回连子串长度
}


//一三象限计连子数（爸写）
int ChessCount_OT(int Line, int Column, int Color)
{
	while ((Column > BOARD_SIZE) || (Column < 0))//跳过超出棋盘的部分
	{
		Column++;
		Line++;
	}
	BeginOTx = Column;
	BeginOTy = Line;

	while(((Board[BeginOTy][BeginOTx] % 100) != Color) 
		&&((Board[BeginOTy][BeginOTx] % 100) != Color + 1)) //跳过非该颜色棋子的点
	{
		if ((BeginOTx > BOARD_SIZE) || (BeginOTy > BOARD_SIZE) || (BeginOTx < 0) || (BeginOTy < 0)) //超边界了
		{
			EndOTx = BeginOTx;
			EndOTy = BeginOTy;

			return 0;
		}
		BeginOTx ++;
		BeginOTy ++;
	}
	//考虑有限如果超出边界怎么办？
	EndOTx = BeginOTx + 1; //尾标移到始标的下一位
	EndOTy = BeginOTy + 1; //尾标移到始标的下一位

	//开始计连子数
	while((Board[EndOTy][EndOTx] % 100 == Color) || (Board[EndOTy][EndOTx] % 100 == Color + 1))//+1就是三角形的棋子颜色
	{
		EndOTx ++;
		EndOTy ++;
	}


	//*******分情况考虑连子串中间有1个空格的情形*******

	//考虑[●　●]的情况（用黑棋举例，白棋同）
	if (
		(EndOTx - BeginOTx == 1) 
		&& (Board[EndOTy][EndOTx] % 100 <= 9) 
		&& ((Board[EndOTy + 1][EndOTx + 1] % 100 == Color) || (Board[EndOTy + 1][EndOTx + 1] % 100 == Color + 1)) 
		&& ((Board[EndOTy + 2][EndOTx + 2] % 100 != Color) && (Board[EndOTy + 2][EndOTx + 2] % 100 != Color + 1)) 
		)
	{
		EndOTx = EndOTx + 2;
		EndOTy = EndOTy + 2;

		return EndOTx - BeginOTx - 1; //返回连子串长度
	}

	//考虑[●●　●]的情况
	else if (
		(EndOTx - BeginOTx == 2) 
		&& (Board[EndOTy][EndOTx] % 100 <= 9) 
		&& ((Board[EndOTy + 1][EndOTx + 1] % 100 == Color) || (Board[EndOTy + 1][EndOTx + 1] % 100 == Color + 1))
		&& ((Board[EndOTy + 2][EndOTx + 2] % 100 != Color) && (Board[EndOTy + 2][EndOTx + 2] % 100 != Color + 1)) 
		)
	{
		EndOTx = EndOTx + 2;
		EndOTy = EndOTy + 2;

		return EndOTx - BeginOTx - 1; //返回连子串长度
	}

	//考虑[●●●　●]的情况
	else if (
		(EndOTx - BeginOTx == 3) 
		&& (Board[EndOTy][EndOTx] % 100 <= 9) 
		&& ((Board[EndOTy + 1][EndOTx + 1] % 100 == Color) || (Board[EndOTy + 1][EndOTx + 1] % 100 == Color + 1))
		&& ((Board[EndOTy + 2][EndOTx + 2] % 100 != Color) && (Board[EndOTy + 2][EndOTx + 2] % 100 != Color + 1)) 
		)
	{
		EndOTx = EndOTx + 2;
		EndOTy = EndOTy + 2;

		return EndOTx - BeginOTx - 1; //返回连子串长度
	}

	//考虑[●　●●]的情况
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

		return EndOTx - BeginOTx - 1; //返回连子串长度
	}

	//考虑[●●　●●]的情况
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

		return EndOTx - BeginOTx - 1; //返回连子串长度
	}

	//考虑[●　●●●]的情况
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

		return EndOTx - BeginOTx - 1; //返回连子串长度
	}

	return EndOTx - BeginOTx; //返回连子串长度
}

//二四象限计连子数（爸写）
int ChessCount_TF(int Line, int Column, int Color)
{
	while (Column < 0) //跳过超出棋盘的部分（棋盘左侧）
	{
		Column++;
		Line--;
	}
	BeginTFx = Column;
	BeginTFy = Line;

	while (Column > BOARD_SIZE) //跳过超出棋盘的部分（棋盘右侧）
	{
		EndTFx = BeginTFx;
		EndTFy = BeginTFy;

		return 0;
	}


	while(((Board[BeginTFy][BeginTFx] % 100) != Color) 
		&& ((Board[BeginTFy][BeginTFx] % 100) != Color + 1)) //跳过非该颜色棋子的点
	{
		if ((BeginTFx > BOARD_SIZE) || (BeginTFy > BOARD_SIZE) || (BeginTFx < 0) || (BeginTFy < 0)) //超边界了
		{
			EndTFx = BeginTFx;
			EndTFy = BeginTFy;

			return 0;
		}
		BeginTFx ++;
		BeginTFy --;
	}
	//考虑有限如果超出边界怎么办？
	EndTFx = BeginTFx + 1; //尾标移到始标的下一位
	EndTFy = BeginTFy - 1; //尾标移到始标的下一位

	//开始计连子数
	while((Board[EndTFy][EndTFx] % 100 == Color) || (Board[EndTFy][EndTFx] % 100 == Color + 1))//+1就是三角形的棋子颜色
	{
		EndTFx ++;
		EndTFy --;
	}


	//*******分情况考虑连子串中间有1个空格的情形*******

	//考虑[●　●]的情况（用黑棋举例，白棋同）
	if (
		(EndTFx - BeginTFx == 1) 
		&& (Board[EndTFy][EndTFx] % 100 <= 9) 
		&& ((Board[EndTFy - 1][EndTFx + 1] % 100 == Color) || (Board[EndTFy - 1][EndTFx + 1] % 100 == Color + 1)) 
		&& ((Board[EndTFy - 2][EndTFx + 2] % 100 != Color) && (Board[EndTFy - 2][EndTFx + 2] % 100 != Color + 1)) 
		)
	{
		EndTFx = EndTFx + 2;
		EndTFy = EndTFy - 2;

		return EndTFx - BeginTFx - 1; //返回连子串长度
	}

	//考虑[●●　●]的情况
	else if (
		(EndTFx - BeginTFx == 2) 
		&& (Board[EndTFy][EndTFx] % 100 <= 9) 
		&& ((Board[EndTFy - 1][EndTFx + 1] % 100 == Color) || (Board[EndTFy - 1][EndTFx + 1] % 100 == Color + 1))
		&& ((Board[EndTFy - 2][EndTFx + 2] % 100 != Color) && (Board[EndTFy - 2][EndTFx + 2] % 100 != Color + 1)) 
		)
	{
		EndTFx = EndTFx + 2;
		EndTFy = EndTFy - 2;

		return EndTFx - BeginTFx - 1; //返回连子串长度
	}

	//考虑[●●●　●]的情况
	else if (
		(EndTFx - BeginTFx == 3) 
		&& (Board[EndTFy][EndTFx] % 100 <= 9) 
		&& ((Board[EndTFy - 1][EndTFx + 1] % 100 == Color) || (Board[EndTFy - 1][EndTFx + 1] % 100 == Color + 1))
		&& ((Board[EndTFy - 2][EndTFx + 2] % 100 != Color) && (Board[EndTFy - 2][EndTFx + 2] % 100 != Color + 1)) 
		)
	{
		EndTFx = EndTFx + 2;
		EndTFy = EndTFy - 2;

		return EndTFx - BeginTFx - 1; //返回连子串长度
	}

	//考虑[●　●●]的情况
	//(由于二四象限判断的方向是x++，y--，所以，为了满足二四象限禁手判断的for循环条件，只能一次判断完毕)
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

		return EndTFx - BeginTFx - 1; //返回连子串长度
	}

	//考虑[●●　●●]的情况
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

		return EndTFx - BeginTFx - 1; //返回连子串长度
	}

	//考虑[●　●●●]的情况
	//(由于二四象限判断的方向是x++，y--，所以，为了满足二四象限禁手判断的for循环条件，只能一次判断完毕)
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

		return EndTFx - BeginTFx - 1; //返回连子串长度
	}

	return EndTFx - BeginTFx; //返回连子串长度
}


//判断输赢
void WhoIsWinning()
{
	int x = 0, y = 0; //棋盘的横纵坐标

	CleanScore();

	WhoIsWin_Horizon(); // 一行一行循环判断

	WhoIsWin_Vertical(); // 一列一列循环判断

	WhoIsWin_OneThreeQuadrant(); // 一三象限循环判断（爸写）

	WhoIsWin_TwoFourQuadrant(); // 二四象限循环判断（爸写）
}


//水平方向判断输赢
void WhoIsWin_Horizon()
{
	int x = 0, y = 0; //棋盘的横纵坐标

	for(y = 0; y <= BOARD_SIZE; y++)
	{
		//"BeginH = EndH + 1"：计算后续的连子数
		for (BeginH = 0, EndH = 0; (BeginH <= BOARD_SIZE) && (EndH <= BOARD_SIZE); BeginH = EndH + 1)
		{
			count = ChessCount_H(y, BLACK_COLOR);

			//禁手的规定：黑方五连与禁手同时形成，禁手失效，黑方胜。所以，只要五连，其余禁手都无须考虑
			if (count == 5)
			{
				Board[yBlack][xBlack] = BLACK_COLOR + 2;

				GameOver = TRUE;

				DisplayChess();
			}

			Score_H(y, count, BLACK_COLOR);//上移原因：防止判断禁手后，将真正的count值变为禁手的count值，导致没有按真正的连子数值打分
		}


		//判断三三禁手的思路：
		//******* 1）最简单的三三禁手情形，就是2个方向上有三个连子，且黑棋一子落下同时形成两个活三，此子必须为两个活三共同的构成子。
		//******* 2）所以，只须先判断其中1个方向上是否有三连子，若有，则随即判断(yBlack, xBlack)所在的另三个方向上是否有三连子。
		//******* 3）因为程序的运行顺序是逐行-逐列-逐一三-逐二四方向统计连子数。
		//******* 4）所以只需利用一行一行判断的特点，在判断出1个方向上已有三连子后，马上判断其余3个方向是否有三连子即可。


		//保证(yBlack, xBlack)在三三禁手的第一条中
		if (EndH - 1 <= BOARD_SIZE)
		{
			BeginH = xBlack - 4;
			EndH = xBlack - 4;

			count = ChessCount_H(yBlack, BLACK_COLOR); //横向连子数

			//横向三连子的两端均无白子，且不挨着棋盘边缘
			if ((count == 3)
				&& (Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR) 
				&& (Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR + 1)
				&& (Board[yBlack][EndH] % 100 != WHITE_COLOR) 
				&& (Board[yBlack][EndH] % 100 != WHITE_COLOR + 1)
				&& (BeginH != 0) && (BeginH != BOARD_SIZE) 
				&& (EndH - 1 != 0) && (EndH - 1 != BOARD_SIZE) 
				&& (yBlack != 0) && (yBlack != BOARD_SIZE)) 
			{
				BanHand_3_Vertical(y); //垂直方向（x++，y++）判断黑棋禁手

				BanHand_3_OneThreeQuadrant(x, y); //一三象限方向（x++，y++）判断黑棋禁手

				BanHand_3_TwoFourQuadrant(x, y); //二四象限方向（x++，y--）判断黑棋禁手
			}

			else if ((count == 4)
				&& 
				(((Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR) 
				&& (Board[yBlack][BeginH - 1] % 100 != WHITE_COLOR + 1))
				|| 
				((Board[yBlack][EndH] % 100 != WHITE_COLOR) 
				&& (Board[yBlack][EndH] % 100 != WHITE_COLOR + 1))))
			{
				BanHand_4_Vertical(y); //垂直方向（x++，y++）判断黑棋禁手

				BanHand_4_OneThreeQuadrant(x, y); //一三象限方向（x++，y++）判断黑棋禁手

				BanHand_4_TwoFourQuadrant(x, y); //二四象限方向（x++，y--）判断黑棋禁手
			}

			else if (count > 5)
			{
				Board[yWhite][xWhite] = WHITE_COLOR + 2;

				DisplayChess();

				GameOver = TRUE;


				//////printf("黑棋长连，白棋获胜！\n");
			}

			else
			{
				continue;
			}
		}
	}

	for(y = 0; y <= BOARD_SIZE; y++)
	{
		//"BeginH = EndH + 1"：计算后续的连子数
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


//垂直方向判断输赢
void WhoIsWin_Vertical()
{
	int x = 0, y = 0; //棋盘的横纵坐标

	for(x = 0; x <= BOARD_SIZE; x++)
	{
		//[BeginV = EndV + 1]:计算当前列的余下棋盘里的后续连子数
		for (BeginV = 0, EndV = 0; (BeginV <= BOARD_SIZE) && (EndV <= BOARD_SIZE); BeginV = EndV + 1)
		{
			count = ChessCount_V(x, BLACK_COLOR);


			if (count == 5) //黑方五连与禁手同时形成，禁手失效，黑方胜〔游戏规则〕
			{		
				Board[yBlack][xBlack] = BLACK_COLOR + 2;

				DisplayChess();

				GameOver = TRUE;
			}
			//上移原因：防止判断禁手后，将真正的count值变为禁手的count值，导致没有按真正的连子数值打分
			Score_V(x, count, BLACK_COLOR);
		}

		//保证(yBlack, xBlack)在三三禁手的第一条中
		if (EndV - 1 <= BOARD_SIZE)
		{
			BeginV = yBlack - 4;
			EndV = yBlack - 4;

			count = ChessCount_V(xBlack, BLACK_COLOR);

			//纵向三连子的两端均无白子，且不挨着棋盘边缘（第二条活三）
			//且最后一步黑棋(yBlack, xBlack)在两条活三的交点处
			if ((count == 3)
				&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR) 
				&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR + 1)
				&& (Board[EndV][xBlack] % 100 != WHITE_COLOR) 
				&& (Board[EndV][xBlack] % 100 != WHITE_COLOR + 1)
				&& (BeginV != 0) && (BeginV != BOARD_SIZE) 
				&& (EndV - 1 != 0) && (EndV - 1 != BOARD_SIZE) 
				&& (xBlack != 0) && (xBlack != BOARD_SIZE)) 
			{
				BanHand_3_Horizon(x); //水平方向（x++，y++）判断黑棋禁手

				BanHand_3_OneThreeQuadrant(x, y); //一三象限方向（x++，y++）判断黑棋禁手

				BanHand_3_TwoFourQuadrant(x, y); //二四象限方向（x++，y--）判断黑棋禁手
			}

			else if ((count == 4)
				&& 
				(((Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR) 
				&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR + 1))
				|| 
				((Board[EndV][xBlack] % 100 != WHITE_COLOR) 
				&& (Board[EndV][xBlack] % 100 != WHITE_COLOR + 1))))
			{
				BanHand_4_Horizon(x); //水平方向（x++，y++）判断黑棋禁手

				BanHand_4_OneThreeQuadrant(x, y); //一三象限方向（x++，y++）判断黑棋禁手

				BanHand_4_TwoFourQuadrant(x, y); //二四象限方向（x++，y--）判断黑棋禁手
			}

			else if (count > 5)
			{
				Board[yWhite][xWhite] = WHITE_COLOR + 2;

				DisplayChess();

				GameOver = TRUE;

				////printf("黑棋长连，白棋获胜！\n");
			}

			else
			{
				continue;
			}
		}
	}



	for(x = 0; x <= BOARD_SIZE; x++)
	{
		//"BeginV = EndV + 1"：计算后续的连子数
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


//一三象限方向判断输赢
void WhoIsWin_OneThreeQuadrant() //一三象限方向判断输赢
{
	int x = 0, y = 0; //棋盘的横纵坐标

	for(x = -14, y = 0; (x <= BOARD_SIZE) && (0 <= y); x++) //x++，y++方向逐一判断
	{
		for (BeginOTx = x, BeginOTy = y, EndOTx = x, EndOTy = y; 
			(-14 <= BeginOTx) && (BeginOTx <= BOARD_SIZE) 
			&& 
			(-14 <= EndOTx) && (EndOTx <= BOARD_SIZE) 
			&& 
			(0 <= BeginOTy) && (BeginOTy <= BOARD_SIZE)  
			&& 
			(0 <= EndOTy) && (EndOTy <= BOARD_SIZE);
		(BeginOTx = EndOTx + 1), (BeginOTy = EndOTy + 1)) //尾标移到始标的下一位
		{
			count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

			if (count == 5)
			{					
				Board[yBlack][xBlack] = BLACK_COLOR + 2;

				DisplayChess();

				GameOver = TRUE;
			}
			//上移原因：防止判断禁手后，将真正的count值变为禁手的count值，导致没有按真正的连子数值打分，从而将白棋落在最高分处
			Score_OT(BeginOTy, BeginOTx, count, BLACK_COLOR);
		}


		if ((EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE))
		{
			BeginOTx = xBlack - 4; 
			BeginOTy = yBlack - 4; 
			EndOTx = xBlack - 4; 
			EndOTy = yBlack - 4;

			count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

			//一三象限方向三连子的两端均无白子，且不挨着棋盘边缘
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
				BanHand_3_Horizon(x); //水平方向（x++，y++）判断黑棋禁手

				BanHand_3_Vertical(y); //垂直方向（x++，y++）判断黑棋禁手

				BanHand_3_TwoFourQuadrant(x, y); //二四象限方向（x++，y--）判断黑棋禁手
			}

			else if ((count == 4)
				&& 
				(((Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR) 
				&& (Board[BeginOTy - 1][BeginOTx - 1] % 100 != WHITE_COLOR + 1))
				|| 
				((Board[EndOTy][EndOTx] % 100 != WHITE_COLOR) 
				&& (Board[EndOTy][EndOTx] % 100 != WHITE_COLOR + 1))))
			{
				BanHand_4_Horizon(x); //水平方向（x++，y++）判断黑棋禁手

				BanHand_4_Vertical(y); //垂直方向（x++，y++）判断黑棋禁手

				BanHand_4_TwoFourQuadrant(x, y); //二四象限方向（x++，y--）判断黑棋禁手
			}

			else if (count > 5)
			{
				Board[yWhite][xWhite] = WHITE_COLOR + 2;

				DisplayChess();

				GameOver = TRUE;

				////printf("黑棋长连，白棋获胜！\n");
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
		(BeginOTx = EndOTx + 1), (BeginOTy = EndOTy + 1)) //尾标移到始标的下一位
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


//二四象限方向判断输赢
void WhoIsWin_TwoFourQuadrant()
{
	int x = 0, y = 0; //棋盘的横纵坐标

	for(x = -14, y = BOARD_SIZE; (x <= BOARD_SIZE) && (y >= 0); x++) //x++，y--方向逐一判断
	{
		for (BeginTFx = x, BeginTFy = y, EndTFx = x, EndTFy = y; 
			(-14 <= BeginTFx) && (BeginTFx <= BOARD_SIZE) 
			&& 
			(-14 <= EndTFx) && (EndTFx <= BOARD_SIZE) 
			&& 
			(0 <= BeginTFy) && (BeginTFy <= BOARD_SIZE)
			&& 
			(0 <= EndTFy) && (EndTFy <= BOARD_SIZE);
		(BeginTFx = EndTFx + 1), (BeginTFy = EndTFy - 1)) //尾标移到始标的下一位
		{
			count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

			//二四象限方向三连子的两端均无白子，且不挨着棋盘边缘（第一条活三）
			if (count == 5)			
			{
				Board[yBlack][xBlack] = BLACK_COLOR + 2;

				DisplayChess();

				GameOver = TRUE;
			}
			//上移原因：防止判断禁手后，将真正的count值变为禁手的count值，导致没有按真正的连子数值打分，从而将白棋落在最高分处
			Score_TF(BeginTFy, BeginTFx, count, BLACK_COLOR);
		}


		if ((EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1))
		{
			BeginTFx = xBlack - 4;
			BeginTFy = yBlack + 4;
			EndTFx = xBlack - 4;
			EndTFy = yBlack + 4;

			count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

			//二四象限方向三连子的两端均无白子，且不挨着棋盘边缘
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
				BanHand_3_Horizon(x); //水平方向（x++，y++）判断黑棋禁手

				BanHand_3_Vertical(y); //垂直方向（x++，y++）判断黑棋禁手

				BanHand_3_OneThreeQuadrant(x, y); //一三象限方向（x++，y++）判断黑棋禁手
			}

			else if ((count == 4) 
				&& 
				(((Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR) 
				&& (Board[BeginTFy + 1][BeginTFx - 1] % 100 != WHITE_COLOR + 1))
				|| 
				((Board[EndTFy][EndTFx] % 100 != WHITE_COLOR) 
				&& (Board[EndTFy][EndTFx] % 100 != WHITE_COLOR + 1))))
			{
				BanHand_4_Horizon(x); //水平方向（x++，y++）判断黑棋禁手

				BanHand_4_Vertical(y); //垂直方向（x++，y++）判断黑棋禁手

				BanHand_4_OneThreeQuadrant(x, y); //一三象限方向（x++，y++）判断黑棋禁手
			}

			else if (count > 5)
			{
				Board[yWhite][xWhite] = WHITE_COLOR + 2;

				DisplayChess();

				GameOver = TRUE;

				////printf("黑棋长连，白棋获胜！\n");
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
		(BeginTFx = EndTFx + 1), (BeginTFy = EndTFy - 1)) //尾标移到始标的下一位
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


//判断三三禁手的思路：
//******* 1）最简单的三三禁手情形，就是2个方向上有三个连子，且黑棋一子落下同时形成两个活三，此子必须为两个活三共同的构成子。
//******* 2）所以，只须先判断其中1个方向上是否有三连子，若有，则随即判断(yBlack, xBlack)所在的另三个方向上是否有三连子。
//******* 3）因为程序的运行顺序是逐行-逐列-逐一三-逐二四方向统计连子数。
//******* 4）所以只需利用一行一行判断的特点，在判断出1个方向上已有三连子后，马上判断其余3个方向是否有三连子即可。


//水平方向（x++，y++）判断黑棋禁手
void BanHand_3_Horizon(int Column)
{
	//考虑边界限制的思路：按照棋盘和连子串的遍历方向（从左至右，从下至上，从左下至右上，从右上至左下），只需对尾端边界进行限制即可，起始边即使超边缘了，统计连子数时也会跳过
	if (EndH - 1 <= BOARD_SIZE)
	{
		BeginH = xBlack - 4;
		EndH = xBlack - 4;

		count = ChessCount_H(yBlack, BLACK_COLOR); //横向连子数

		//横向三连子两端均无白子，且不挨着棋盘边缘
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
			//垂直方向判断是否有第三条〔活四〕
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


			//一三象限方向判断是否有第三条〔活四〕
			else if ((EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE))
			{
				BeginOTx = xBlack - 4;
				BeginOTy = yBlack - 4;
				EndOTx = xBlack - 4;
				EndOTy = yBlack - 4;

				count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

				//四连子的两端只少有一端无白子
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


			//二四象限方向判断是否有第三条〔活四〕
			else if ((EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1))
			{
				BeginTFx = xBlack - 4;
				BeginTFy = yBlack + 4;
				EndTFx = xBlack - 4;
				EndTFy = yBlack + 4;

				count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

				//二四象限方向三连子的两端均无白子，且不挨着棋盘边缘
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


			//若无第三条〔活四〕，则判定为三三禁手
			Board[yWhite][xWhite] = WHITE_COLOR + 2;

			DisplayChess();

			GameOver = TRUE;
		}
	}
}



//水平方向（x++，y++）判断黑棋禁手
void BanHand_4_Horizon(int Column)
{
	//考虑边界限制的思路：按照棋盘和连子串的遍历方向（从左至右，从下至上，从左下至右上，从右上至左下），只需对尾端边界进行限制即可，起始边即使超边缘了，统计连子数时也会跳过
	if (EndH - 1 <= BOARD_SIZE)
	{
		BeginH = xBlack - 4;
		EndH = xBlack - 4;

		count = ChessCount_H(yBlack, BLACK_COLOR); //横向连子数

		//四连子的两端只少有一端无白子
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
			//垂直方向判断是否有第三条〔活三〕
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


			//一三象限方向判断是否有第三条〔活三〕
			if ( (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE))
			{
				BeginOTx = xBlack - 4;
				BeginOTy = yBlack - 4;
				EndOTx = xBlack - 4;
				EndOTy = yBlack - 4;

				count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

				//一三象限方向三连子的两端均无白子，且不挨着棋盘边缘
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


			//二四象限方向判断是否有第三条〔活三〕
			if ((EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1))
			{
				BeginTFx = xBlack - 4;
				BeginTFy = yBlack + 4;
				EndTFx = xBlack - 4;
				EndTFy = yBlack + 4;

				count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

				//二四象限方向三连子的两端均无白子，且不挨着棋盘边缘
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


			//若无第三条〔活三〕，则判定为四四禁手
			Board[yWhite][xWhite] = WHITE_COLOR + 2;

			DisplayChess();

			GameOver = TRUE;
		}
	}
}



//垂直方向（x++，y++）判断黑棋禁手
void BanHand_3_Vertical(int Line)
{
	if (EndV - 1 <= BOARD_SIZE)
	{
		BeginV = yBlack - 4;
		EndV = yBlack - 4;



		count = ChessCount_V(xBlack, BLACK_COLOR);

		//纵向三连子的两端均无白子，且不挨着棋盘边缘（第二条活三）
		//且最后一步黑棋(yBlack, xBlack)在两条活三的交点处
		if ((count == 3)
			&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR) 
			&& (Board[BeginV - 1][xBlack] % 100 != WHITE_COLOR + 1)
			&& (Board[EndV][xBlack] % 100 != WHITE_COLOR) 
			&& (Board[EndV][xBlack] % 100 != WHITE_COLOR + 1)
			&& (BeginV != 0) && (BeginV != BOARD_SIZE) 
			&& (EndV - 1 != 0) && (EndV - 1 != BOARD_SIZE) 
			&& (xBlack != 0) && (xBlack != BOARD_SIZE)) 
		{
			//水平方向判断是否有第三条〔活四〕
			for ((BeginH = xBlack - 4, EndH = xBlack - 4) && (EndH - 1 <= BOARD_SIZE); 
				(BeginH <= xBlack + 4) && (BeginH <= BOARD_SIZE) && (EndH - 1 <= xBlack + 4) && (EndH - 1 <= BOARD_SIZE); 
				BeginH = EndH + 1)
			{
				count = ChessCount_H(yBlack, BLACK_COLOR); //横向连子数

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


			//一三象限方向判断是否有第三条〔活四〕
			for (
				(BeginOTx = xBlack - 4, BeginOTy = yBlack - 4, EndOTx = xBlack - 4, EndOTy = yBlack - 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
				(BeginOTx <= xBlack + 4) && (BeginOTy <= yBlack + 4) && (EndOTx - 1 <= xBlack + 4) && (EndOTy - 1 <= yBlack + 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
			BeginOTx = EndOTx + 1, BeginOTy = EndOTy + 1)
			{
				count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

				//四连子的两端只少有一端无白子
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


			//二四象限方向判断是否有第三条〔活四〕
			for (
				(BeginTFx = xBlack - 4, BeginTFy = yBlack + 4, EndTFx = xBlack - 4, EndTFy = yBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
				(BeginTFx <= xBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
			BeginTFx = EndTFx + 1, BeginTFy = EndTFy - 1)
			{
				count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

				//二四象限方向三连子的两端均无白子，且不挨着棋盘边缘
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

			//若无第三条〔活四〕，则判定为三三禁手
			Board[yWhite][xWhite] = WHITE_COLOR + 2;

			DisplayChess();

			GameOver = TRUE;
		}
	}
}



//垂直方向（x++，y++）判断黑棋禁手
void BanHand_4_Vertical(int Line)
{
	if (EndV - 1 <= BOARD_SIZE)
	{
		BeginV = yBlack - 4;
		EndV = yBlack - 4;


		count = ChessCount_V(xBlack, BLACK_COLOR);

		//四连子的两端只少有一端无白子
		//且最后一步黑棋(yBlack, xBlack)在两条活三的交点处
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
			//水平方向判断是否有第三条〔活三〕
			for (
				(BeginH = xBlack - 4, EndH = xBlack - 4) && (EndH - 1 <= BOARD_SIZE); 
				(BeginH <= xBlack + 4) && (BeginH <= BOARD_SIZE) && (EndH - 1 <= xBlack + 4) && (EndH - 1 <= BOARD_SIZE); 
			BeginH = EndH + 1)
			{
				count = ChessCount_H(yBlack, BLACK_COLOR); //横向连子数

				//横向三连子两端均无白子，且不挨着棋盘边缘
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


			//一三象限方向判断是否有第三条〔活三〕
			for (
				(BeginOTx = xBlack - 4, BeginOTy = yBlack - 4, EndOTx = xBlack - 4, EndOTy = yBlack - 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
				(BeginOTx <= xBlack + 4) && (BeginOTy <= yBlack + 4) && (EndOTx - 1 <= xBlack + 4) && (EndOTy - 1 <= yBlack + 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
			BeginOTx = EndOTx + 1, BeginOTy = EndOTy + 1)
			{
				count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

				//一三象限方向三连子的两端均无白子，且不挨着棋盘边缘
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


			//二四象限方向判断是否有第三条〔活三〕
			for (
				(BeginTFx = xBlack - 4, BeginTFy = yBlack + 4, EndTFx = xBlack - 4, EndTFy = yBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
				(BeginTFx <= xBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
			BeginTFx = EndTFx + 1, BeginTFy = EndTFy - 1)
			{
				count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

				//二四象限方向三连子的两端均无白子，且不挨着棋盘边缘
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


			//若无第三条〔活三〕，则判定为四四禁手
			Board[yWhite][xWhite] = WHITE_COLOR + 2;

			DisplayChess();

			GameOver = TRUE;
		}
	}
}



//一三象限方向（x++，y++）判断黑棋禁手
void BanHand_3_OneThreeQuadrant(int Column, int Line)
{
	if ((EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE))
	{
		BeginOTx = xBlack - 4;
		BeginOTy = yBlack - 4;
		EndOTx = xBlack - 4;
		EndOTy = yBlack - 4; 


		count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

		//一三象限方向三连子的两端均无白子，且不挨着棋盘边缘
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
			//水平方向判断是否有第三条〔活四〕
			for ((BeginH = xBlack - 4, EndH = xBlack - 4) && (EndH - 1 <= BOARD_SIZE); 
				(BeginH <= xBlack + 4) && (BeginH <= BOARD_SIZE) && (EndH - 1 <= xBlack + 4) && (EndH - 1 <= BOARD_SIZE); 
				BeginH = EndH + 1)
			{
				count = ChessCount_H(yBlack, BLACK_COLOR); //横向连子数

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


			//垂直方向判断是否有第三条〔活四〕
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


			//二四象限方向判断是否有第三条〔活四〕
			for (
				(BeginTFx = xBlack - 4, BeginTFy = yBlack + 4, EndTFx = xBlack - 4, EndTFy = yBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
				(BeginTFx <= xBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
			BeginTFx = EndTFx + 1, BeginTFy = EndTFy - 1)
			{
				count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

				//二四象限方向三连子的两端均无白子，且不挨着棋盘边缘
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

			//若无第三条〔活四〕，则判定为三三禁手
			Board[yWhite][xWhite] = WHITE_COLOR + 2;

			DisplayChess();

			GameOver = TRUE;

			
		}
	}
}


//一三象限方向（x++，y++）判断黑棋禁手
void BanHand_4_OneThreeQuadrant(int Column, int Line)
{
	if ((EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE))
	{
		BeginOTx = xBlack - 4;
		BeginOTy = yBlack - 4;
		EndOTx = xBlack - 4;
		EndOTy = yBlack - 4; 

		count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

		//四连子的两端只少有一端无白子
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
			//水平方向判断是否有第三条〔活三〕
			for (
				(BeginH = xBlack - 4, EndH = xBlack - 4) && (EndH - 1 <= BOARD_SIZE); 
				(BeginH <= xBlack + 4) && (BeginH <= BOARD_SIZE) && (EndH - 1 <= xBlack + 4) && (EndH - 1 <= BOARD_SIZE); 
			BeginH = EndH + 1)
			{
				count = ChessCount_H(yBlack, BLACK_COLOR); //横向连子数

				//横向三连子两端均无白子，且不挨着棋盘边缘
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



			//垂直方向判断是否有第三条〔活三〕
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


			//二四象限方向判断是否有第三条〔活三〕
			for (
				(BeginTFx = xBlack - 4, BeginTFy = yBlack + 4, EndTFx = xBlack - 4, EndTFy = yBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
				(BeginTFx <= xBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
			BeginTFx = EndTFx + 1, BeginTFy = EndTFy - 1)
			{
				count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

				//二四象限方向三连子的两端均无白子，且不挨着棋盘边缘
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


			//若无第三条〔活三〕，则判定为四四禁手
			Board[yWhite][xWhite] = WHITE_COLOR + 2;

			DisplayChess();

			GameOver = TRUE;
		}
	}
}


//二四象限方向（x++，y--）判断黑棋禁手
void BanHand_3_TwoFourQuadrant(int Column, int Line)
{
	/////考虑边界限制的思路：按照棋盘和连子串的遍历方向（从左至右，从下至上，从左下至右上，从右上至左下），只需对尾端边界进行限制即可，起始边即时超边缘了，统计连子数时也会跳过

	//for循环的表达式2只限制了(BeginTFx <= xBlack + 4)，而未限制(BeginTFy = yBlack - 4)，是因为y和x的遍历方向不一致，拧着，怕矛盾；
	//且y随x变化而变化，无需再作为限制条件写进表达式2里


	if ((EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1))
	{
		BeginTFx = xBlack - 4;
		BeginTFy = yBlack + 4;
		EndTFx = xBlack - 4;
		EndTFy = yBlack + 4;


		count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

		//二四象限方向三连子的两端均无白子，且不挨着棋盘边缘
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
			//水平方向判断是否有第三条〔活四〕
			for ((BeginH = xBlack - 4, EndH = xBlack - 4) && (EndH - 1 <= BOARD_SIZE); 
				(BeginH <= xBlack + 4) && (BeginH <= BOARD_SIZE) && (EndH - 1 <= xBlack + 4) && (EndH - 1 <= BOARD_SIZE); 
				BeginH = EndH + 1)
			{
				count = ChessCount_H(yBlack, BLACK_COLOR); //横向连子数

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


			//垂直方向判断是否有第三条〔活四〕
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


			//一三象限方向判断是否有第三条〔活四〕
			for (
				(BeginOTx = xBlack - 4, BeginOTy = yBlack - 4, EndOTx = xBlack - 4, EndOTy = yBlack - 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
				(BeginOTx <= xBlack + 4) && (BeginOTy <= yBlack + 4) && (EndOTx - 1 <= xBlack + 4) && (EndOTy - 1 <= yBlack + 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
			BeginOTx = EndOTx + 1, BeginOTy = EndOTy + 1)
			{
				count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

				//四连子的两端只少有一端无白子
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

			//若无第三条〔活四〕，则判定为三三禁手
			Board[yWhite][xWhite] = WHITE_COLOR + 2;

			DisplayChess();

			GameOver = TRUE;
		}
	}
} 



//二四象限方向（x++，y--）判断黑棋禁手
void BanHand_4_TwoFourQuadrant(int Column, int Line)
{
	/////考虑边界限制的思路：按照棋盘和连子串的遍历方向（从左至右，从下至上，从左下至右上，从右上至左下），只需对尾端边界进行限制即可，起始边即时超边缘了，统计连子数时也会跳过

	//for循环的表达式2只限制了(BeginTFx <= xBlack + 4)，而未限制(BeginTFy = yBlack - 4)，是因为y和x的遍历方向不一致，拧着，怕矛盾；
	//且y随x变化而变化，无需再作为限制条件写进表达式2里


	if ((EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1))
	{
		BeginTFx = xBlack - 4;
		BeginTFy = yBlack + 4;
		EndTFx = xBlack - 4;
		EndTFy = yBlack + 4;


		count = ChessCount_TF(BeginTFy, BeginTFx, BLACK_COLOR);

		//二四象限方向三连子的两端均无白子，且不挨着棋盘边缘
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
			//水平方向判断是否有第三条〔活三〕
			for (
				(BeginH = xBlack - 4, EndH = xBlack - 4) && (EndH - 1 <= BOARD_SIZE); 
				(BeginH <= xBlack + 4) && (BeginH <= BOARD_SIZE) && (EndH - 1 <= xBlack + 4) && (EndH - 1 <= BOARD_SIZE); 
			BeginH = EndH + 1)
			{
				count = ChessCount_H(yBlack, BLACK_COLOR); //横向连子数

				//横向三连子两端均无白子，且不挨着棋盘边缘
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



			//垂直方向判断是否有第三条〔活三〕
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


			//一三象限方向判断是否有第三条〔活三〕
			for (
				(BeginOTx = xBlack - 4, BeginOTy = yBlack - 4, EndOTx = xBlack - 4, EndOTy = yBlack - 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
				(BeginOTx <= xBlack + 4) && (BeginOTy <= yBlack + 4) && (EndOTx - 1 <= xBlack + 4) && (EndOTy - 1 <= yBlack + 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
			BeginOTx = EndOTx + 1, BeginOTy = EndOTy + 1)
			{
				count = ChessCount_OT(BeginOTy, BeginOTx, BLACK_COLOR);

				//一三象限方向三连子的两端均无白子，且不挨着棋盘边缘
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


			//若无第三条〔活三〕，则判定为四四禁手
			Board[yWhite][xWhite] = WHITE_COLOR + 2;

			DisplayChess();

			GameOver = TRUE;
		}
	}
}//横向打分
void Score_H(int Line, int count, int Color) 
{
	if  ((Color == BLACK_COLOR) || (Color == BLACK_COLOR + 1)) //统计黑棋连子，即堵对手
	{
		switch(count)	//打分标准
		{
		case 1:	
			getScope_H_Black(Line, BeginH, EndH, 1000);
			break;

		case 2:	
			//考虑[●　●]的情况
			if (Board[Line][BeginH + 1] % 100 <= 9) 
			{
				getScope_H_Black(Line, BeginH, EndH, 2100);
				break;
			}

			getScope_H_Black(Line, BeginH, EndH, 2000);
			break;

		case 3:	
			//考虑[●●　●]的情况
			if (Board[Line][BeginH + 2] % 100 <= 9) 
			{
				getScope_H_Black(Line, BeginH, EndH, 20000);
				break;
			}

			//考虑[●　●●]的情况
			else if (Board[Line][BeginH + 1] % 100 <= 9) 
			{
				getScope_H_Black(Line, BeginH, EndH, 20000);
				break;
			}

			getScope_H_Black(Line, BeginH, EndH, 19000);
			break;

		case 4:	
			//考虑[●●●　●]的情况
			if (Board[Line][BeginH + 3] % 100 <= 9) 
			{
				getScope_H_Black(Line, BeginH, EndH, 210000);
				break;
			}

			//考虑[●●　●●]的情况
			else if (Board[Line][BeginH + 2] % 100 <= 9) 
			{
				getScope_H_Black(Line, BeginH, EndH, 210000);
				break;
			}

			//考虑[●　●●●]的情况
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

	else //统计白棋连子，即发展自己
	{
		switch(count)	//打分标准
		{
		case 1:	
			getScope_H_White(Line, BeginH, EndH, 1200);
			break;

		case 2:	
			//考虑[◎　◎]的情况
			if (Board[Line][BeginH + 1] % 100 <= 9) 
			{
				getScope_H_White(Line, BeginH, EndH, 5000);
				break;
			}

			getScope_H_White(Line, BeginH, EndH, 5000);
			break;

		case 3:	
			//考虑[◎◎　◎]的情况
			if (Board[Line][BeginH + 2] % 100 <= 9) 
			{
				getScope_H_White(Line, BeginH, EndH, 28000);
				break;
			}

			//考虑[◎　◎◎]的情况
			else if (Board[Line][BeginH + 1] % 100 <= 9) 
			{
				getScope_H_White(Line, BeginH, EndH, 28000);
				break;
			}

			getScope_H_White(Line, BeginH, EndH, 28000);
			break;

		case 4:	
			//考虑[◎◎◎　◎]的情况
			if (Board[Line][BeginH + 3] % 100 <= 9) 
			{
				getScope_H_White(Line, BeginH, EndH, 5000000);
				break;
			}

			//考虑[◎◎　◎◎]的情况
			else if (Board[Line][BeginH + 2] % 100 <= 9) 
			{
				getScope_H_White(Line, BeginH, EndH, 5000000);
				break;
			}

			//考虑[◎　◎◎◎]的情况
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


//纵向打分
void Score_V(int Column, int count, int Color) 
{
	if ((Color == BLACK_COLOR) || (Color == BLACK_COLOR + 1)) //统计黑棋连子，即堵对手
	{
		switch(count)	//打分标准
		{
		case 1:	
			getScope_V_Black(Column, BeginV, EndV, 1000);
			break;

		case 2:	
			//考虑[●　●]的情况
			if (Board[BeginV + 1][Column] % 100 <= 9) 
			{
				getScope_V_Black(Column, BeginV, EndV, 2100);
				break;
			}

			getScope_V_Black(Column, BeginV, EndV, 2000);
			break;
			//往下应该考虑两个子之间有空格的情形

		case 3:	
			//考虑[●●　●]的情况
			if (Board[BeginV + 2][Column] % 100 <= 9) 
			{
				getScope_V_Black(Column, BeginV, EndV, 20000);
				break;
			}

			//考虑[●　●●]的情况
			else if (Board[BeginV + 1][Column] % 100 <= 9) 
			{
				getScope_V_Black(Column, BeginV, EndV, 20000);
				break;
			}

			getScope_V_Black(Column, BeginV, EndV, 19000);
			break;
			//往下应该考虑两个子之间有空格的情形

		case 4:	
			//考虑[●●●　●]的情况
			if (Board[BeginV + 3][Column] % 100 <= 9) 
			{
				getScope_V_Black(Column, BeginV, EndV, 210000);
				break;
			}

			//考虑[●●　●●]的情况
			else if (Board[BeginV + 2][Column] % 100 <= 9) 
			{
				getScope_V_Black(Column, BeginV, EndV, 210000);
				break;
			}

			//考虑[●　●●●]的情况
			else if (Board[BeginV + 1][Column] % 100 <= 9) 
			{
				getScope_V_Black(Column, BeginV, EndV, 210000);
				break;
			}

			getScope_V_Black(Column, BeginV, EndV, 200000);
			break;
			//往下应该考虑两个子之间有空格的情形

		default:
			break;
		}
	}

	else //统计白棋连子，即发展自己
	{
		switch(count)	//打分标准
		{
		case 1:	
			getScope_V_White(Column, BeginV, EndV, 1200);
			break;

		case 2:	
			//考虑[◎　◎]的情况
			if (Board[BeginV + 1][Column] % 100 <= 9) 
			{
				getScope_V_White(Column, BeginV, EndV, 5000);
				break;
			}

			getScope_V_White(Column, BeginV, EndV, 5000);
			break;

		case 3:	
			//考虑[◎◎　◎]的情况
			if (Board[BeginV + 2][Column] % 100 <= 9) 
			{
				getScope_V_White(Column, BeginV, EndV, 28000);
				break;
			}

			//考虑[◎　◎◎]的情况
			else if (Board[BeginV + 1][Column] % 100 <= 9) 
			{
				getScope_V_White(Column, BeginV, EndV, 28000);
				break;
			}

			getScope_V_White(Column, BeginV, EndV, 28000);
			break;

		case 4:	
			//考虑[◎◎◎　◎]的情况
			if (Board[BeginV + 3][Column] % 100 <= 9) 
			{
				getScope_V_White(Column, BeginV, EndV, 5000000);
				break;
			}

			//考虑[◎◎　◎◎]的情况
			else if (Board[BeginV + 2][Column] % 100 <= 9) 
			{
				getScope_V_White(Column, BeginV, EndV, 5000000);
				break;
			}

			//考虑[◎　◎◎◎]的情况
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


//一三象限打分
void Score_OT(int Line, int Column, int count, int Color) 
{
	if ((Color == BLACK_COLOR) || (Color == BLACK_COLOR + 1)) //统计黑棋连子，即堵对手
	{
		switch(count)	//打分标准
		{
		case 1:	
			getScope_OT_Black(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 1000);
			break;

		case 2:	
			//考虑[●　●]的情况
			if (Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9) 
			{
				getScope_OT_Black(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 2100);
				break;
			}

			getScope_OT_Black(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 2000);
			break;

		case 3:	
			//考虑[●●　●]的情况
			if (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			{
				getScope_OT_Black(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 20000);
				break;
			}

			//考虑[●　●●]的情况
			else if (Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9) 
			{
				getScope_OT_Black(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 20000);
				break;
			}

			getScope_OT_Black(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 19000);
			break;

		case 4:	
			//考虑[●●●　●]的情况
			if (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9) 
			{
				getScope_OT_Black(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 210000);
				break;
			}

			//考虑[●●　●●]的情况
			else if (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			{
				getScope_OT_Black(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 210000);
				break;
			}

			//考虑[●　●●●]的情况
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

	else //统计白棋连子，即发展自己
	{
		switch(count)	//打分标准
		{
		case 1:	
			getScope_OT_White(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 1200);			
			break;

		case 2:	
			//考虑[◎　◎]的情况
			if (Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9) 
			{
				getScope_OT_White(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 5000);			
				break;
			}

			getScope_OT_White(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 5000);			
			break;

		case 3:	
			//考虑[◎◎　◎]的情况
			if (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			{
				getScope_OT_White(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 28000);			
				break;
			}

			//考虑[◎　◎◎]的情况
			else if (Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9) 
			{
				getScope_OT_White(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 28000);			
				break;
			}

			getScope_OT_White(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 28000);			
			break;

		case 4:	
			//考虑[◎◎◎　◎]的情况
			if (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9) 
			{
				getScope_OT_White(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 5000000);			
				break;
			}

			//考虑[◎◎　◎◎]的情况
			else if (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			{
				getScope_OT_White(Line, Column, BeginOTx, BeginOTy, EndOTx, EndOTy, 5000000);			
				break;
			}

			//考虑[◎　◎◎◎]的情况
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


//二四象限打分
void Score_TF(int Line, int Column, int count, int Color) 
{
	if ((Color == BLACK_COLOR) || (Color == BLACK_COLOR + 1)) //统计黑棋连子，即堵对手
	{
		switch(count)	//打分标准
		{
		case 1:	
			getScope_TF_Black(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 1000);
			break;

		case 2:	
			//考虑[●　●]的情况
			if (Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9) 
			{
				getScope_TF_Black(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 2100);				
				break;
			}

			getScope_TF_Black(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 2000);				
			break;

		case 3:	
			//考虑[●●　●]的情况
			if (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			{
				getScope_TF_Black(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 20000);				
				break;
			}

			//考虑[●　●●]的情况
			else if (Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9) 
			{
				getScope_TF_Black(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 20000);				
				break;
			}

			getScope_TF_Black(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 19000);				
			break;

		case 4:	
			//考虑[●●●　●]的情况
			if (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9) 
			{
				getScope_TF_Black(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 210000);				
				break;
			}

			//考虑[●●　●●]的情况
			else if (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			{
				getScope_TF_Black(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 210000);				
				break;
			}

			//考虑[●　●●●]的情况
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

	else //统计白棋连子，即发展自己
	{
		switch(count)	//打分标准
		{
		case 1:	
			getScope_TF_White(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 1200);			
			break;

		case 2:	
			//考虑[◎　◎]的情况
			if (Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9) 
			{
				getScope_TF_White(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 5000);			
				break;
			}

			getScope_TF_White(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 5000);			
			break;

		case 3:	
			//考虑[◎◎　◎]的情况
			if (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			{
				getScope_TF_White(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 28000);			
				break;
			}

			//考虑[◎　◎◎]的情况
			else if (Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9) 
			{
				getScope_TF_White(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 28000);			
				break;
			}

			getScope_TF_White(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 28000);			
			break;

		case 4:	
			//考虑[◎◎◎　◎]的情况
			if (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9) 
			{
				getScope_TF_White(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 5000000);			
				break;
			}

			//考虑[◎◎　◎◎]的情况
			else if (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			{
				getScope_TF_White(Line, Column, BeginTFx, BeginTFy, EndTFx, EndTFy, 5000000);			
				break;
			}

			//考虑[◎　◎◎◎]的情况
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


//横向堵黑棋打分倍数
void getScope_H_Black(int Line, int BeginH, int EndH, int scope) 
{
	//两端都被白子堵了
	if (((Board[Line][BeginH - 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH - 1] % 100 == WHITE_COLOR + 1) || (BeginH == 0))
		&&
		((Board[Line][EndH] % 100 == WHITE_COLOR) || (Board[Line][EndH] % 100 == WHITE_COLOR + 1) || (EndH - 1 == BOARD_SIZE)))
	{
		//考虑[●　●]的情况
		if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*1;
		}

		//考虑[●●　●]的情况
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9) 
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*1;
		}

		//考虑[●　●●]的情况
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*1;
		}

		//考虑[●●●　●]的情况
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 <= 9)
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 3] += scope*10;
		}

		//考虑[●●　●●]的情况
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9)
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*10;
		}

		//考虑[●　●●●]的情况
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


	//两端均无白子(==两端没有子，因为只有白子、黑子、空三种状态，若有黑子，则会统计连子串棋子个数)
	else if ((Board[Line][BeginH - 1] % 100 <= 9) && (Board[Line][EndH] % 100 <= 9) && (0 < BeginH) && (EndH - 1 < BOARD_SIZE))
	{
		//考虑[●　●]的情况
		if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*4;
		}

		//考虑[●●　●]的情况
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9)  
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*4;
		}

		//考虑[●　●●]的情况
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*4;
		}

		//考虑[●●●　●]的情况
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 <= 9)
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 3] += scope*10;
		}

		//考虑[●●　●●]的情况
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9)
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*10;
		}

		//考虑[●　●●●]的情况
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*10;
		}


		//[Begin - 1]被堵，[End + 1]未被堵 
		if (((Board[Line][BeginH - 2] == WHITE_COLOR) || (Board[Line][BeginH - 2] == WHITE_COLOR + 1) || (BeginH - 1 == 0))
			&& 
			((Board[Line][EndH + 1] != WHITE_COLOR) && (Board[Line][EndH + 1] != WHITE_COLOR + 1) && (EndH != BOARD_SIZE)))
		{
			Board[Line][BeginH - 1] += scope*1;
			Board[Line][EndH] += scope*3;
		}

		//[Begin - 1]未被堵，[End + 1]被堵 
		else if (((Board[Line][BeginH - 2] != WHITE_COLOR) && (Board[Line][BeginH - 2] != WHITE_COLOR + 1) && (BeginH - 1 != 0))
			&&
			((Board[Line][EndH + 1] == WHITE_COLOR) || (Board[Line][EndH + 1] == WHITE_COLOR + 1) || (EndH == BOARD_SIZE)))
		{
			Board[Line][BeginH - 1] += scope*3;
			Board[Line][EndH] += scope*1;
		}

		//[Begin - 1]，[End + 1]均被堵
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


	//begin端被白子堵，同时，end端无白子 
	else if (((Board[Line][BeginH - 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH - 1] % 100 == WHITE_COLOR + 1) || (BeginH == 0)) 
		&& 
		((Board[Line][EndH] % 100 <= 9) && (EndH - 1 < BOARD_SIZE))) 
	{
		//考虑[●　●]的情况
		if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*2;
		}

		//考虑[●●　●]的情况
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9) 
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*2;
		}

		//考虑[●　●●]的情况
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*2;
		}

		//考虑[●●●　●]的情况
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 <= 9)
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 3] += scope*10;
		}

		//考虑[●●　●●]的情况
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9)
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*10;
		}

		//考虑[●　●●●]的情况
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*10;
		}

		//[End + 1]若被堵 
		if ((Board[Line][EndH + 1] == WHITE_COLOR) || (Board[Line][EndH + 1] == WHITE_COLOR + 1) || (EndH == BOARD_SIZE))
		{
			Board[Line][EndH] += scope*1;
		}

		else
		{
			Board[Line][EndH] += scope*2;
		}
	}


	//begin端无白子，同时，end端被白子堵 
	else if (((Board[Line][BeginH - 1] % 100 <= 9) && (0 < BeginH))
		&& 
		((Board[Line][EndH] % 100 == WHITE_COLOR) || (Board[Line][EndH] % 100 == WHITE_COLOR + 1) || (EndH - 1 == BOARD_SIZE))) 
	{
		//考虑[●　●]的情况
		if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*2;
		}

		//考虑[●●　●]的情况
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9) 
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*2;
		}

		//考虑[●　●●]的情况
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*2;
		}

		//考虑[●●●　●]的情况
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 <= 9)
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 3] += scope*10;
		}

		//考虑[●●　●●]的情况
		else if (((Board[Line][BeginH + 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9)
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*10;
		}

		//考虑[●　●●●]的情况
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == BLACK_COLOR) || (Board[Line][BeginH + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*10;
		}

		//////[Begin - 1]若被堵 
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


//横向发展白棋打分倍数
void getScope_H_White(int Line, int BeginH, int EndH, int scope) 
{
	//两端都被黑子堵了
	if (((Board[Line][BeginH - 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH - 1] % 100 == BLACK_COLOR + 1) || (BeginH == 0))
		&& 
		((Board[Line][EndH] % 100 == BLACK_COLOR) || (Board[Line][EndH] % 100 == BLACK_COLOR + 1) || (EndH - 1 == BOARD_SIZE)))
	{
		//考虑[◎　◎]的情况
		if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*1;
		}

		//考虑[◎◎　◎]的情况
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9) 
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*1;
		}

		//考虑[◎　◎◎]的情况
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*1;
		}

		//考虑[◎◎◎　◎]的情况
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 <= 9)
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 3] += scope*10;
		}

		//考虑[◎◎　◎◎]的情况
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&&(Board[Line][BeginH + 2] % 100 <= 9)
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*10;
		}

		//考虑[◎　◎◎◎]的情况
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


	//两端均无黑子
	else if ((Board[Line][BeginH - 1] % 100 <= 9) && (Board[Line][EndH] % 100 <= 9) && (0 < BeginH) && (EndH - 1 < BOARD_SIZE))
	{
		//考虑[◎　◎]的情况
		if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*4;
		}

		//考虑[◎◎　◎]的情况
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9)  
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*4;
		}

		//考虑[◎　◎◎]的情况
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*4;
		}

		//考虑[◎◎◎　◎]的情况
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 <= 9)
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 3] += scope*10;
		}

		//考虑[◎◎　◎◎]的情况
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9)
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*10;
		}

		//考虑[◎　◎◎◎]的情况
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*10;
		}


		//[Begin - 1]被堵，[End + 1]未被堵 
		if (((Board[Line][BeginH - 2] == BLACK_COLOR) || (Board[Line][BeginH - 2] == BLACK_COLOR + 1) || (BeginH - 1 == 0))
			&& 
			((Board[Line][EndH + 1] != BLACK_COLOR) && (Board[Line][EndH + 1] != BLACK_COLOR + 1) && (EndH != BOARD_SIZE)))
		{
			Board[Line][BeginH - 1] += scope*1;
			Board[Line][EndH] += scope*3;
		}

		//[Begin - 1]未被堵，[End + 1]被堵 
		else if (((Board[Line][BeginH - 2] != BLACK_COLOR) && (Board[Line][BeginH - 2] != BLACK_COLOR + 1) && (BeginH - 1 != 0))
			&&
			((Board[Line][EndH + 1] == BLACK_COLOR) || (Board[Line][EndH + 1] == BLACK_COLOR + 1) || (EndH == BOARD_SIZE)))
		{
			Board[Line][BeginH - 1] += scope*3;
			Board[Line][EndH] += scope*1;
		}

		//[Begin - 1]，[End + 1]均被堵
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


	//begin端被黑子堵，同时，end端无黑子 
	else if (((Board[Line][BeginH - 1] % 100 == BLACK_COLOR) || (Board[Line][BeginH - 1] % 100 == BLACK_COLOR + 1) || (BeginH == 0))
		&& 
		((Board[Line][EndH] % 100 <= 9) && (EndH - 1 < BOARD_SIZE))) 
	{
		//考虑[◎　◎]的情况
		if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*2;
		}

		//考虑[◎◎　◎]的情况
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9) 
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*2;
		}

		//考虑[◎　◎◎]的情况
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*2;
		}

		//考虑[◎◎◎　◎]的情况
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 <= 9)
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 3] += scope*10;
		}

		//考虑[◎◎　◎◎]的情况
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9)
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*10;
		}

		//考虑[◎　◎◎◎]的情况
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*10;
		}

		//[End + 1]若被堵 
		if ((Board[Line][EndH + 1] == BLACK_COLOR) || (Board[Line][EndH + 1] == BLACK_COLOR + 1) || (EndH == BOARD_SIZE))
		{
			Board[Line][EndH] += scope*1;
		}

		else
		{
			Board[Line][EndH] += scope*2;
		}
	}


	//begin端无黑子，同时，end端被黑子堵 
	else if (((Board[Line][BeginH - 1] % 100 <= 9) && (0 < BeginH))
		&& 
		((Board[Line][EndH] % 100 == BLACK_COLOR) || (Board[Line][EndH] % 100 == BLACK_COLOR + 1) || (EndH - 1 == BOARD_SIZE))) 
	{
		//考虑[◎　◎]的情况
		if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 3] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*2;
		}

		//考虑[◎◎　◎]的情况
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9) 
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*2;
		}

		//考虑[◎　◎◎]的情况
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 4] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*2;
		}

		//考虑[◎◎◎　◎]的情况
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1)) 
			&& (Board[Line][BeginH + 3] % 100 <= 9)
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 3] += scope*10;
		}

		//考虑[◎◎　◎◎]的情况
		else if (((Board[Line][BeginH + 1] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 2] % 100 <= 9)
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 2] += scope*10;
		}

		//考虑[◎　◎◎◎]的情况
		else if ((Board[Line][BeginH + 1] % 100 <= 9)
			&& ((Board[Line][BeginH + 2] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 3] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[Line][BeginH + 4] % 100 == WHITE_COLOR) || (Board[Line][BeginH + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[Line][BeginH + 5] % 100 == Board[Line][EndH] % 100))
		{
			Board[Line][BeginH + 1] += scope*10;
		}

		//////[Begin - 1]若被堵 
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


//纵向堵黑棋打分倍数
void getScope_V_Black(int Column, int BeginV, int EndV, int scope) 
{
	//两端都被白子堵了
	if (((Board[BeginV - 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV - 1][Column] % 100 == WHITE_COLOR + 1) || (BeginV == 0))
		&&
		((Board[EndV][Column] % 100 == WHITE_COLOR) || (Board[EndV][Column] % 100 == WHITE_COLOR + 1) || (EndV - 1 == BOARD_SIZE)))
	{
		//考虑[●　●]的情况
		if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*1;
		}

		//考虑[●●　●]的情况
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9) 
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*1;
		}

		//考虑[●　●●]的情况
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*1;
		}

		//考虑[●●●　●]的情况
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 <= 9)
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 3][Column] += scope*10;
		}

		//考虑[●●　●●]的情况
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*10;
		}

		//考虑[●　●●●]的情况
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


	//两端均无白子(==两端没有子，因为只有白子、黑子、空三种状态，若有黑子，则会统计连子串棋子个数)
	else if ((Board[BeginV - 1][Column] % 100 <= 9) && (Board[EndV][Column] % 100 <= 9) && (0 < BeginV) && (EndV - 1 < BOARD_SIZE))
	{
		//考虑[●　●]的情况
		if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*4;
		}

		//考虑[●●　●]的情况
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)  
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*4;
		}

		//考虑[●　●●]的情况
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*4;
		}

		//考虑[●●●　●]的情况
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 <= 9)
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 3][Column] += scope*10;
		}

		//考虑[●●　●●]的情况
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*10;
		}

		//考虑[●　●●●]的情况
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*10;
		}


		//[Begin - 1]被堵，[End + 1]未被堵 
		if (((Board[BeginV - 2][Column] == WHITE_COLOR) || (Board[BeginV - 2][Column] == WHITE_COLOR + 1) || (BeginV - 1 == 0))
			&& 
			((Board[EndV + 1][Column] != WHITE_COLOR) && (Board[EndV + 1][Column] != WHITE_COLOR + 1) && (EndV != BOARD_SIZE)))
		{
			Board[BeginV - 1][Column] += scope*1;
			Board[EndV][Column] += scope*3;
		}


		//[Begin - 1]未被堵，[End + 1]被堵 
		else if (((Board[BeginV - 2][Column] != WHITE_COLOR) && (Board[BeginV - 2][Column] != WHITE_COLOR + 1) && (BeginV - 1 != 0))
			&&
			((Board[EndV + 1][Column] == WHITE_COLOR) || (Board[EndV + 1][Column] == WHITE_COLOR + 1) || (EndV == BOARD_SIZE)))
		{
			Board[BeginV - 1][Column] += scope*3;
			Board[EndV][Column] += scope*1;
		}

		//[Begin - 1]，[End + 1]均被堵
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


	//begin端被白子堵，同时，end端无白子 
	else if (((Board[BeginV - 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV - 1][Column] % 100 == WHITE_COLOR + 1) || (BeginV == 0)) 
		&& 
		((Board[EndV][Column] % 100 <= 9) && (EndV - 1 < BOARD_SIZE))) 
	{
		//考虑[●　●]的情况
		if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*2;
		}

		//考虑[●●　●]的情况
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9) 
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*2;
		}

		//考虑[●　●●]的情况
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*2;
		}

		//考虑[●●●　●]的情况
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 <= 9)
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 3][Column] += scope*10;
		}

		//考虑[●●　●●]的情况
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*10;
		}

		//考虑[●　●●●]的情况
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*10;
		}


		//[End + 1]若被堵 
		if ((Board[EndV + 1][Column] == WHITE_COLOR) || (Board[EndV + 1][Column] == WHITE_COLOR + 1) || (EndV == BOARD_SIZE))
		{
			Board[EndV][Column] += scope*1;
		}

		else
		{
			Board[EndV][Column] += scope*2;
		}
	}


	//begin端无白子，同时，end端被白子堵 
	else if (((Board[BeginV - 1][Column] % 100 <= 9) && (0 < BeginV))
		&& 
		((Board[EndV][Column] % 100 == WHITE_COLOR) || (Board[EndV][Column] % 100 == WHITE_COLOR + 1) || (EndV - 1 == BOARD_SIZE))) 
	{
		//考虑[●　●]的情况
		if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*2;
		}

		//考虑[●●　●]的情况
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9) 
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*2;
		}

		//考虑[●　●●]的情况
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*2;
		}

		//考虑[●●●　●]的情况
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 <= 9)
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 3][Column] += scope*10;
		}

		//考虑[●●　●●]的情况
		else if (((Board[BeginV + 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 1][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*10;
		}

		//考虑[●　●●●]的情况
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 2][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 3][Column] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == BLACK_COLOR) || (Board[BeginV + 4][Column] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*10;
		}

		//////[Begin - 1]若被堵 
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


//纵向发展白棋打分倍数
void getScope_V_White(int Column, int BeginV, int EndV, int scope) 
{
	//两端都被黑子堵了
	if (((Board[BeginV - 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV - 1][Column] % 100 == BLACK_COLOR + 1) || (BeginV == 0))
		&& 
		((Board[EndV][Column] % 100 == BLACK_COLOR) || (Board[EndV][Column] % 100 == BLACK_COLOR + 1) || (EndV - 1 == BOARD_SIZE)))
	{
		//考虑[◎　◎]的情况
		if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*1;
		}

		//考虑[◎◎　◎]的情况
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9) 
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*1;
		}

		//考虑[◎　◎◎]的情况
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*1;
		}

		//考虑[◎◎◎　◎]的情况
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 <= 9)
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 3][Column] += scope*10;
		}

		//考虑[◎◎　◎◎]的情况
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*10;
		}

		//考虑[◎　◎◎◎]的情况
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


	//两端均无黑子
	else if ((Board[BeginV - 1][Column] % 100 <= 9) && (Board[EndV][Column] % 100 <= 9) && (0 < BeginV) && (EndV - 1 < BOARD_SIZE))
	{
		//考虑[◎　◎]的情况
		if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*4;
		}

		//考虑[◎◎　◎]的情况
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)  
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*4;
		}

		//考虑[◎　◎◎]的情况
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*4;
		}

		//考虑[◎◎◎　◎]的情况
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 <= 9)
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 3][Column] += scope*10;
		}

		//考虑[◎◎　◎◎]的情况
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*10;
		}

		//考虑[◎　◎◎◎]的情况
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*10;
		}


		//[Begin - 1]被堵，[End + 1]未被堵 
		if (((Board[BeginV - 2][Column] == BLACK_COLOR) || (Board[BeginV - 2][Column] == BLACK_COLOR + 1) || (BeginV - 1 == 0))
			&& 
			((Board[EndV + 1][Column] != BLACK_COLOR) && (Board[EndV + 1][Column] != BLACK_COLOR + 1) && (EndV != BOARD_SIZE)))
		{
			Board[BeginV - 1][Column] += scope*1;
			Board[EndV][Column] += scope*3;
		}


		//[Begin - 1]未被堵，[End + 1]被堵 
		else if (((Board[BeginV - 2][Column] != BLACK_COLOR) && (Board[BeginV - 2][Column] != BLACK_COLOR + 1) && (BeginV - 1 != 0))
			&&
			((Board[EndV + 1][Column] == BLACK_COLOR) || (Board[EndV + 1][Column] == BLACK_COLOR + 1) || (EndV == BOARD_SIZE)))
		{
			Board[BeginV - 1][Column] += scope*3;
			Board[EndV][Column] += scope*1;
		}

		//[Begin - 1]，[End + 1]均被堵
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


	//begin端被黑子堵，同时，end端无黑子 
	else if (((Board[BeginV - 1][Column] % 100 == BLACK_COLOR) || (Board[BeginV - 1][Column] % 100 == BLACK_COLOR + 1) || (BeginV == 0)) 
		&& 
		((Board[EndV][Column] % 100 <= 9) && (EndV - 1 < BOARD_SIZE))) 
	{
		//考虑[◎　◎]的情况
		if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*2;
		}

		//考虑[◎◎　◎]的情况
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9) 
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*2;
		}

		//考虑[◎　◎◎]的情况
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*2;
		}

		//考虑[◎◎◎　◎]的情况
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 <= 9)
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 3][Column] += scope*10;
		}

		//考虑[◎◎　◎◎]的情况
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*10;
		}

		//考虑[◎　◎◎◎]的情况
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*10;
		}


		//[End + 1]若被堵 
		if ((Board[EndV + 1][Column] == BLACK_COLOR) || (Board[EndV + 1][Column] == BLACK_COLOR + 1) || (EndV == BOARD_SIZE))
		{
			Board[EndV][Column] += scope*1;
		}

		else
		{
			Board[EndV][Column] += scope*2;
		}
	}


	//begin端无黑子，同时，end端被黑子堵 
	else if (((Board[BeginV - 1][Column] % 100 <= 9) && (0 < BeginV)) 
		&& 
		((Board[EndV][Column] % 100 == BLACK_COLOR) || (Board[EndV][Column] % 100 == BLACK_COLOR + 1) || (EndV - 1 == BOARD_SIZE))) 
	{
		//考虑[◎　◎]的情况
		if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*2;
		}

		//考虑[◎◎　◎]的情况
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9) 
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*2;
		}

		//考虑[◎　◎◎]的情况
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 4][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*2;
		}

		//考虑[◎◎◎　◎]的情况
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 3][Column] % 100 <= 9)
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 3][Column] += scope*10;
		}

		//考虑[◎◎　◎◎]的情况
		else if (((Board[BeginV + 1][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 1][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 2][Column] % 100 <= 9)
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 2][Column] += scope*10;
		}

		//考虑[◎　◎◎◎]的情况
		else if ((Board[BeginV + 1][Column] % 100 <= 9)
			&& ((Board[BeginV + 2][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 2][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 3][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 3][Column] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginV + 4][Column] % 100 == WHITE_COLOR) || (Board[BeginV + 4][Column] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginV + 5][Column] % 100 == Board[EndV][Column] % 100))
		{
			Board[BeginV + 1][Column] += scope*10;
		}


		//////[Begin - 1]若被堵 
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


//一三象限堵黑棋打分倍数
void getScope_OT_Black(int Line, int Column, int BeginOTx, int BeginOTy, int EndOTx, int EndOTy, int scope) 
{
	//两端都被白子堵了
	if (((Board[BeginOTy - 1][BeginOTx - 1] % 100 == WHITE_COLOR) || (Board[BeginOTy - 1][BeginOTx - 1] % 100 == WHITE_COLOR + 1)
		|| (BeginOTy == 0) || (BeginOTx == 0))
		&& 
		((Board[EndOTy][EndOTx] % 100 == WHITE_COLOR) || (Board[EndOTy][EndOTx] % 100 == WHITE_COLOR + 1)
		|| (EndOTy - 1 == BOARD_SIZE) || (EndOTx - 1 == BOARD_SIZE)))
	{
		//考虑[●　●]的情况
		if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*1;
		}

		//考虑[●●　●]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*1;
		}

		//考虑[●　●●]的情况
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*1;
		}

		//考虑[●●●　●]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9)
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 3][BeginOTx + 3] += scope*10;
		}

		//考虑[●●　●●]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*10;
		}

		//考虑[●　●●●]的情况
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


	//两端均无白子(==两端没有子，因为只有白子、黑子、空三种状态，若有黑子，则会统计连子串棋子个数)
	else if ((Board[BeginOTy - 1][BeginOTx - 1] % 100 <= 9) && (Board[EndOTy][EndOTx] % 100 <= 9) 
		&& 
		(0 < BeginOTy) && (0 < BeginOTx) && (EndOTy - 1 < BOARD_SIZE) && (EndOTx - 1 < BOARD_SIZE))
	{
		//考虑[●　●]的情况
		if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*4;
		}

		//考虑[●●　●]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)  
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*4;
		}

		//考虑[●　●●]的情况
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*4;
		}

		//考虑[●●●　●]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9)
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 3][BeginOTx + 3] += scope*10;
		}

		//考虑[●●　●●]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*10;
		}

		//考虑[●　●●●]的情况
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*10;
		}


		//////////////////////////////[Begin - 1]被堵，[End + 1]未被堵 
		if (((Board[BeginOTy - 2][BeginOTx - 2] == WHITE_COLOR) || (Board[BeginOTy - 2][BeginOTx - 2] == WHITE_COLOR + 1) 
			|| (BeginOTy - 1 == 0) || (BeginOTx - 1 == 0))
			&& 
			((Board[EndOTy + 1][EndOTx + 1] != WHITE_COLOR) && (Board[EndOTy + 1][EndOTx + 1] != WHITE_COLOR + 1) 
			&& (EndOTy != BOARD_SIZE) && (EndOTx != BOARD_SIZE)))
		{
			Board[BeginOTy - 1][BeginOTx - 1] += scope*1;
			Board[EndOTy][EndOTx] += scope*3;
		}


		//[Begin - 1]未被堵，[End + 1]被堵 
		else if (((Board[BeginOTy - 2][BeginOTx - 2] != WHITE_COLOR) && (Board[BeginOTy - 2][BeginOTx - 2] != WHITE_COLOR + 1) 
			&& (BeginOTy - 1 != 0) && (BeginOTx - 1 != 0))
			&& 
			((Board[EndOTy + 1][EndOTx + 1] == WHITE_COLOR) || (Board[EndOTy + 1][EndOTx + 1] == WHITE_COLOR + 1) 
			|| (EndOTy == BOARD_SIZE) || (EndOTx == BOARD_SIZE)))
		{
			Board[BeginOTy - 1][BeginOTx - 1] += scope*3;
			Board[EndOTy][EndOTx] += scope*1;
		}

		//[Begin - 1]，[End + 1]均被堵
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


	//begin端被白子堵，同时，end端无白子 
	else if (((Board[BeginOTy - 1][BeginOTx - 1] % 100 == WHITE_COLOR) || (Board[BeginOTy - 1][BeginOTx - 1] % 100 == WHITE_COLOR + 1)
		|| (BeginOTy == 0) || (BeginOTx == 0)) 
		&& 
		((Board[EndOTy][EndOTx] % 100 <= 9) && (EndOTy - 1 < BOARD_SIZE) && (EndOTx - 1 < BOARD_SIZE))) 
	{
		//考虑[●　●]的情况
		if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy][BeginOTx + 1] += scope*2;
		}

		//考虑[●●　●]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*2;
		}

		//考虑[●　●●]的情况
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*2;
		}

		//考虑[●●●　●]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9)
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 3][BeginOTx + 3] += scope*10;
		}

		//考虑[●●　●●]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*10;
		}

		//考虑[●　●●●]的情况
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*10;
		}


		//[End + 1]若被堵 
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


	//begin端无白子，同时，end端被白子堵 
	else if (((Board[BeginOTy - 1][BeginOTx - 1] % 100 <= 9) && (0 < BeginOTy) && (0 < BeginOTx)) 
		&& 
		((Board[EndOTy][EndOTx] % 100 == WHITE_COLOR) || (Board[EndOTy][EndOTx] % 100 == WHITE_COLOR + 1) 
		|| (EndOTy - 1 == BOARD_SIZE) || (EndOTx - 1 == BOARD_SIZE))) 
	{
		//考虑[●　●]的情况
		if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*2;
		}

		//考虑[●●　●]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*2;
		}

		//考虑[●　●●]的情况
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*2;
		}

		//考虑[●●●　●]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9)
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 3][BeginOTx + 3] += scope*10;
		}

		//考虑[●●　●●]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*10;
		}

		//考虑[●　●●●]的情况
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*10;
		}


		//////[Begin - 1]若被堵 
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


//一三象限发展白棋打分倍数
void getScope_OT_White(int Line, int Column, int BeginOTx, int BeginOTy, int EndOTx, int EndOTy, int scope) 
{
	//两端都被黑子堵了
	if (((Board[BeginOTy - 1][BeginOTx - 1] % 100 == BLACK_COLOR) || (Board[BeginOTy - 1][BeginOTx - 1] % 100 == BLACK_COLOR + 1)
		|| (BeginOTy == 0) || (BeginOTx == 0))
		&& 
		((Board[EndOTy][EndOTx] % 100 == BLACK_COLOR) || (Board[EndOTy][EndOTx] % 100 == BLACK_COLOR + 1)
		|| (EndOTy - 1 == BOARD_SIZE) || (EndOTx - 1 == BOARD_SIZE)))
	{
		//考虑[◎　◎]的情况
		if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*1;
		}

		//考虑[◎◎　◎]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*1;
		}

		//考虑[◎　◎◎]的情况
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*1;
		}

		//考虑[◎◎◎　◎]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9)
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 3][BeginOTx + 3] += scope*10;
		}

		//考虑[◎◎　◎◎]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*10;
		}

		//考虑[◎　◎◎◎]的情况
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


	//两端均无黑子(==两端没有子，因为只有白子、黑子、空三种状态，若有黑子，则会统计连子串棋子个数)
	else if ((Board[BeginOTy - 1][BeginOTx - 1] % 100 <= 9) && (Board[EndOTy][EndOTx] % 100 <= 9)
		&& (0 < BeginOTy) && (0 < BeginOTx) && (EndOTy - 1 < BOARD_SIZE) && (EndOTx - 1 < BOARD_SIZE))
	{
		//考虑[◎　◎]的情况
		if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*4;
		}

		//考虑[◎◎　◎]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)  
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*4;
		}

		//考虑[◎　◎◎]的情况
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*4;
		}

		//考虑[◎◎◎　◎]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9)
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 3][BeginOTx + 3] += scope*10;
		}

		//考虑[◎◎　◎◎]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*10;
		}

		//考虑[◎　◎◎◎]的情况
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*10;
		}


		//////////////////////////////[Begin - 1]被堵，[End + 1]未被堵 
		if (((Board[BeginOTy - 2][BeginOTx - 2] == BLACK_COLOR) || (Board[BeginOTy - 2][BeginOTx - 2] == BLACK_COLOR + 1) 
			|| (BeginOTy - 1 == 0) || (BeginOTx - 1 == 0))
			&& 
			((Board[EndOTy + 1][EndOTx + 1] != BLACK_COLOR) && (Board[EndOTy + 1][EndOTx + 1] != BLACK_COLOR + 1) 
			&& (EndOTy != BOARD_SIZE) && (EndOTx != BOARD_SIZE)))
		{
			Board[BeginOTy - 1][BeginOTx - 1] += scope*1;
			Board[EndOTy][EndOTx] += scope*3;
		}


		//[Begin - 1]未被堵，[End + 1]被堵 
		else if (((Board[BeginOTy - 2][BeginOTx - 2] != BLACK_COLOR) && (Board[BeginOTy - 2][BeginOTx - 2] != BLACK_COLOR + 1) 
			&& (BeginOTy - 1 != 0) && (BeginOTx - 1 != 0))
			&& 
			((Board[EndOTy + 1][EndOTx + 1] == BLACK_COLOR) || (Board[EndOTy + 1][EndOTx + 1] == BLACK_COLOR + 1) 
			|| (EndOTy == BOARD_SIZE) || (EndOTx == BOARD_SIZE)))
		{
			Board[BeginOTy - 1][BeginOTx - 1] += scope*3;
			Board[EndOTy][EndOTx] += scope*1;
		}

		//[Begin - 1]，[End + 1]均被堵
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


	//begin端被黑子堵，同时，end端无黑子 
	else if (((Board[BeginOTy - 1][BeginOTx - 1] % 100 == BLACK_COLOR) || (Board[BeginOTy - 1][BeginOTx - 1] % 100 == BLACK_COLOR + 1)
		|| (BeginOTy == 0) || (BeginOTx == 0)) 
		&& 
		((Board[EndOTy][EndOTx] % 100 <= 9) && (EndOTy - 1 < BOARD_SIZE) && (EndOTx - 1 < BOARD_SIZE))) 
	{
		//考虑[◎　◎]的情况
		if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy][BeginOTx + 1] += scope*2;
		}

		//考虑[◎◎　◎]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*2;
		}

		//考虑[◎　◎◎]的情况
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*2;
		}

		//考虑[◎◎◎　◎]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9)
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 3][BeginOTx + 3] += scope*10;
		}

		//考虑[◎◎　◎◎]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*10;
		}

		//考虑[◎　◎◎◎]的情况
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*10;
		}


		//[End + 1]若被堵 
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


	//begin端无黑子，同时，end端被黑子堵 
	else if (((Board[BeginOTy - 1][BeginOTx - 1] % 100 <= 9) && (0 < BeginOTy) && (0 < BeginOTx)) 
		&& 
		((Board[EndOTy][EndOTx] % 100 == BLACK_COLOR) || (Board[EndOTy][EndOTx] % 100 == BLACK_COLOR + 1)
		|| (EndOTy - 1 == BOARD_SIZE) || (EndOTx - 1 == BOARD_SIZE))) 
	{
		//考虑[◎　◎]的情况
		if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*2;
		}

		//考虑[◎◎　◎]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9) 
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*2;
		}

		//考虑[◎　◎◎]的情况
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 4][BeginOTx + 4] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*2;
		}

		//考虑[◎◎◎　◎]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 3][BeginOTx + 3] % 100 <= 9)
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 3][BeginOTx + 3] += scope*10;
		}

		//考虑[◎◎　◎◎]的情况
		else if (((Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR) || (Board[BeginOTy + 1][BeginOTx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 2][BeginOTx + 2] % 100 <= 9)
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 2][BeginOTx + 2] += scope*10;
		}

		//考虑[◎　◎◎◎]的情况
		else if ((Board[BeginOTy + 1][BeginOTx + 1] % 100 <= 9)
			&& ((Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR) || (Board[BeginOTy + 2][BeginOTx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR) || (Board[BeginOTy + 3][BeginOTx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR) || (Board[BeginOTy + 4][BeginOTx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginOTy + 5][BeginOTx + 5] % 100 == Board[EndOTy][EndOTx] % 100))
		{
			Board[BeginOTy + 1][BeginOTx + 1] += scope*10;
		}


		//////[Begin - 1]若被堵 
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


//二四象限堵黑棋打分倍数
void getScope_TF_Black(int Line, int Column, int BeginTFx, int BeginTFy, int EndTFx, int EndTFy, int scope) 
{
	//两端都被白子堵了
	if (((Board[BeginTFy + 1][BeginTFx - 1] % 100 == WHITE_COLOR) || (Board[BeginTFy + 1][BeginTFx - 1] % 100 == WHITE_COLOR + 1)
		|| (BeginTFy == BOARD_SIZE) || (BeginTFx == 0))
		&& 
		((Board[EndTFy][EndTFx] % 100 == WHITE_COLOR) || (Board[EndTFy][EndTFx] % 100 == WHITE_COLOR + 1)
		|| (EndTFy + 1 == 0) || (EndTFx - 1 == BOARD_SIZE)))
	{
		//考虑[●　●]的情况
		if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*1;
		}

		//考虑[●●　●]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*1;
		}

		//考虑[●　●●]的情况
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*1;
		}

		//考虑[●●●　●]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9)
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 3][BeginTFx + 3] += scope*10;
		}

		//考虑[●●　●●]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*10;
		}

		//考虑[●　●●●]的情况
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


	//两端均无白子(==两端没有子，因为只有白子、黑子、空三种状态，若有黑子，则会统计连子串棋子个数)
	else if ((Board[BeginTFy + 1][BeginTFx - 1] % 100 <= 9) && (Board[EndTFy][EndTFx] % 100 <= 9)
		&& (BeginTFy < BOARD_SIZE) && (0 < BeginTFx) && (0 < EndTFy + 1) && (EndTFx - 1 < BOARD_SIZE))
	{
		//考虑[●　●]的情况
		if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*4;
		}

		//考虑[●●　●]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)  
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*4;
		}

		//考虑[●　●●]的情况
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*4;
		}

		//考虑[●●●　●]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9)
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 3][BeginTFx + 3] += scope*10;
		}

		//考虑[●●　●●]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*10;
		}

		//考虑[●　●●●]的情况
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*10;
		}


		//////////////////////////////[Begin - 1]被堵，[End + 1]未被堵 
		if (((Board[BeginTFy + 2][BeginTFx - 2] == WHITE_COLOR) || (Board[BeginTFy + 2][BeginTFx - 2] == WHITE_COLOR + 1) 
			|| (BeginTFy + 1 == BOARD_SIZE) || (BeginTFx - 1 == 0))
			&& 
			((Board[EndTFy - 1][EndTFx + 1] != WHITE_COLOR) && (Board[EndTFy - 1][EndTFx + 1] != WHITE_COLOR + 1) 
			&& (EndTFy != 0) && (EndTFx != BOARD_SIZE)))
		{
			Board[BeginTFy + 1][BeginTFx - 1] += scope*1;
			Board[EndTFy][EndTFx] += scope*3;
		}


		//[Begin - 1]未被堵，[End + 1]被堵 
		else if (((Board[BeginTFy + 2][BeginTFx - 2] != WHITE_COLOR) && (Board[BeginTFy + 2][BeginTFx - 2] != WHITE_COLOR + 1) 
			&& (BeginTFy + 1 != BOARD_SIZE) && (BeginTFx - 1 != 0))
			&& 
			((Board[EndTFy - 1][EndTFx + 1] == WHITE_COLOR) || (Board[EndTFy - 1][EndTFx + 1] == WHITE_COLOR + 1) 
			|| (EndTFy == 0) || (EndTFx == BOARD_SIZE)))
		{
			Board[BeginTFy + 1][BeginTFx - 1] += scope*3;
			Board[EndTFy][EndTFx] += scope*1;
		}

		//[Begin - 1]，[End + 1]均被堵
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


	//begin端被白子堵，同时，end端无白子 
	else if (((Board[BeginTFy + 1][BeginTFx - 1] % 100 == WHITE_COLOR) || (Board[BeginTFy + 1][BeginTFx - 1] % 100 == WHITE_COLOR + 1)
		|| (BeginTFy == BOARD_SIZE) || (BeginTFx == 0)) 
		&& 
		((Board[EndTFy][EndTFx] % 100 <= 9) && (0 < EndTFy + 1) && (EndTFx - 1 < BOARD_SIZE))) 
	{
		//考虑[●　●]的情况
		if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy][BeginTFx + 1] += scope*2;
		}

		//考虑[●●　●]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*2;
		}

		//考虑[●　●●]的情况
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*2;
		}

		//考虑[●●●　●]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9)
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 3][BeginTFx + 3] += scope*10;
		}

		//考虑[●●　●●]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*10;
		}

		//考虑[●　●●●]的情况
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*10;
		}


		//[End + 1]若被堵 
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


	//begin端无白子，同时，end端被白子堵 
	else if (((Board[BeginTFy + 1][BeginTFx - 1] % 100 <= 9) && (BeginTFy < BOARD_SIZE) && (0 < BeginTFx)) 
		&& 
		((Board[EndTFy][EndTFx] % 100 == WHITE_COLOR) || (Board[EndTFy][EndTFx] % 100 == WHITE_COLOR + 1)
		|| (EndTFy + 1 == 0) || (EndTFx - 1 == BOARD_SIZE))) 
	{
		//考虑[●　●]的情况
		if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*2;
		}

		//考虑[●●　●]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*2;
		}

		//考虑[●　●●]的情况
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*2;
		}

		//考虑[●●●　●]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9)
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 3][BeginTFx + 3] += scope*10;
		}

		//考虑[●●　●●]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*10;
		}

		//考虑[●　●●●]的情况
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == BLACK_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == BLACK_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*10;
		}


		//////[Begin - 1]若被堵 
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


//二四象限发展白棋打分倍数
void getScope_TF_White(int Line, int Column, int BeginTFx, int BeginTFy, int EndTFx, int EndTFy, int scope) 
{
	//两端都被黑子堵了
	if (((Board[BeginTFy + 1][BeginTFx - 1] % 100 == BLACK_COLOR) || (Board[BeginTFy + 1][BeginTFx - 1] % 100 == BLACK_COLOR + 1)
		|| (BeginTFy == BOARD_SIZE) || (BeginTFx == 0))
		&& 
		((Board[EndTFy][EndTFx] % 100 == BLACK_COLOR) || (Board[EndTFy][EndTFx] % 100 == BLACK_COLOR + 1)
		|| (EndTFy + 1 == 0) || (EndTFx - 1 == BOARD_SIZE)))
	{
		//考虑[◎　◎]的情况
		if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*1;
		}

		//考虑[◎◎　◎]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*1;
		}

		//考虑[◎　◎◎]的情况
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*1;
		}

		//考虑[◎◎◎　◎]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9)
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 3][BeginTFx + 3] += scope*10;
		}

		//考虑[◎◎　◎◎]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*10;
		}

		//考虑[◎　◎◎◎]的情况
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


	//两端均无黑子(==两端没有子，因为只有黑子、白子、空三种状态，若有黑子，则会统计连子串棋子个数)
	else if ((Board[BeginTFy + 1][BeginTFx - 1] % 100 <= 9) && (Board[EndTFy][EndTFx] % 100 <= 9)
		&& (BeginTFy < BOARD_SIZE) && (0 < BeginTFx) && (0 < EndTFy + 1) && (EndTFx - 1 < BOARD_SIZE))
	{
		//考虑[◎　◎]的情况
		if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*4;
		}

		//考虑[◎◎　◎]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)  
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*4;
		}

		//考虑[◎　◎◎]的情况
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*4;
		}

		//考虑[◎◎◎　◎]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9)
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 3][BeginTFx + 3] += scope*10;
		}

		//考虑[◎◎　◎◎]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*10;
		}

		//考虑[◎　◎◎◎]的情况
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*10;
		}


		//////////////////////////////[Begin - 1]被堵，[End + 1]未被堵 
		if (((Board[BeginTFy + 2][BeginTFx - 2] == BLACK_COLOR) || (Board[BeginTFy + 2][BeginTFx - 2] == BLACK_COLOR + 1) 
			|| (BeginTFy + 1 == BOARD_SIZE) || (BeginTFx - 1 == 0))
			&& 
			((Board[EndTFy - 1][EndTFx + 1] != BLACK_COLOR) && (Board[EndTFy - 1][EndTFx + 1] != BLACK_COLOR + 1) 
			&& (EndTFy != 0) && (EndTFx != BOARD_SIZE)))
		{
			Board[BeginTFy + 1][BeginTFx - 1] += scope*1;
			Board[EndTFy][EndTFx] += scope*3;
		}


		//[Begin - 1]未被堵，[End + 1]被堵 
		else if (((Board[BeginTFy + 2][BeginTFx - 2] != BLACK_COLOR) && (Board[BeginTFy + 2][BeginTFx - 2] != BLACK_COLOR + 1) 
			&& (BeginTFy + 1 != BOARD_SIZE) && (BeginTFx - 1 != 0))
			&& 
			((Board[EndTFy - 1][EndTFx + 1] == BLACK_COLOR) || (Board[EndTFy - 1][EndTFx + 1] == BLACK_COLOR + 1) 
			|| (EndTFy == 0) || (EndTFx == BOARD_SIZE)))
		{
			Board[BeginTFy + 1][BeginTFx - 1] += scope*3;
			Board[EndTFy][EndTFx] += scope*1;
		}

		//[Begin - 1]，[End + 1]均被堵
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


	//begin端被黑子堵，同时，end端无黑子 
	else if (((Board[BeginTFy + 1][BeginTFx - 1] % 100 == BLACK_COLOR) || (Board[BeginTFy + 1][BeginTFx - 1] % 100 == BLACK_COLOR + 1)
		|| (BeginTFy == BOARD_SIZE) || (BeginTFx == 0)) 
		&& 
		((Board[EndTFy][EndTFx] % 100 <= 9) && (0 < EndTFy + 1) && (EndTFx - 1 < BOARD_SIZE))) 
	{
		//考虑[◎　◎]的情况
		if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy][BeginTFx + 1] += scope*2;
		}

		//考虑[◎◎　◎]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*2;
		}

		//考虑[◎　◎◎]的情况
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*2;
		}

		//考虑[◎◎◎　◎]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9)
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 3][BeginTFx + 3] += scope*10;
		}

		//考虑[◎◎　◎◎]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*10;
		}

		//考虑[◎　◎◎◎]的情况
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*10;
		}



		//[End + 1]若被堵 
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


	//begin端无黑子，同时，end端被黑子堵 
	else if (((Board[BeginTFy + 1][BeginTFx - 1] % 100 <= 9) && (BeginTFy < BOARD_SIZE) && (0 < BeginTFx)) 
		&& 
		((Board[EndTFy][EndTFx] % 100 == BLACK_COLOR) || (Board[EndTFy][EndTFx] % 100 == BLACK_COLOR + 1)
		|| (EndTFy + 1 == 0) || (EndTFx - 1 == BOARD_SIZE))) 
	{
		//考虑[◎　◎]的情况
		if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*2;
		}

		//考虑[◎◎　◎]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9) 
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*2;
		}

		//考虑[◎　◎◎]的情况
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 4][BeginTFx + 4] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*2;
		}

		//考虑[◎◎◎　◎]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 3][BeginTFx + 3] % 100 <= 9)
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 3][BeginTFx + 3] += scope*10;
		}

		//考虑[◎◎　◎◎]的情况
		else if (((Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR) || (Board[BeginTFy - 1][BeginTFx + 1] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 2][BeginTFx + 2] % 100 <= 9)
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 2][BeginTFx + 2] += scope*10;
		}

		//考虑[◎　◎◎◎]的情况
		else if ((Board[BeginTFy - 1][BeginTFx + 1] % 100 <= 9)
			&& ((Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR) || (Board[BeginTFy - 2][BeginTFx + 2] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR) || (Board[BeginTFy - 3][BeginTFx + 3] % 100 == WHITE_COLOR + 1))
			&& ((Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR) || (Board[BeginTFy - 4][BeginTFx + 4] % 100 == WHITE_COLOR + 1))
			&& (Board[BeginTFy - 5][BeginTFx + 5] % 100 == Board[EndTFy][EndTFx] % 100))
		{
			Board[BeginTFy - 1][BeginTFx + 1] += scope*10;
		}


		//////[Begin - 1]若被堵 
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


//清积分
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


//求最大分值坐标
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

