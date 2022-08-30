#include <stdio.h>
#include <stdlib.h>

#define BLACK_COLOR 10 //黑棋颜色
#define WHITE_COLOR 20 //白棋颜色
#define BOARD_SIZE 14 //棋盘行列数


void ChooseGameMode(); //选择游戏模式
void PeopleVSComputer(); //人机对战
void PeopleVSPeople(); //人人对战
void PeopleVSPeople_BlackPlay(); //人人对战_黑棋下
void PeopleVSPeople_WhitePlay(); //人人对战_白棋下
void DisplayBoard();
void ResetBoardArray();
void PeopleVSComputer_PeopleBlackPlay(); //人机对战_人持黑棋
void PeopleVSComputer_PeopleWhitePlay(); //人机对战_人持白棋
void PeopleVSComputer_ComputerBlackPlay(); //人机对战_机持黑棋
void PeopleVSComputer_ComputerWhitePlay(); //人机对战_机持白棋
void WhoIsWinning();
void WhoIsWin_Horizon(); //水平方向判断输赢
void WhoIsWin_Vertical(); //垂直方向判断输赢
void WhoIsWin_OneThreeQuadrant(); //一三象限方向判断输赢
void WhoIsWin_TwoFourQuadrant(); //二四象限方向判断输赢
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
void IsOpposite_Black(); //判断黑棋X,Y坐标输入是否规范
void IsOpposite_White(); //判断白棋X,Y坐标输入是否规范
void ChessRepeat_Black(); //判断黑棋是否重复落子
void ChessRepeat_White(); //判断白棋是否重复落子
void ChessBeyondBoard_Black(); //判断黑棋落子是否超出棋盘
void ChessBeyondBoard_White(); //判断白棋落子是否超出棋盘
void ChessShape(); //设定棋子形状——当前步的棋子形状是三角▲/△，上一步的棋子形状是圆形●/◎
void BanHand_3_Horizon(int); //水平方向（x++，y++）判断黑棋禁手
void BanHand_3_Vertical(int); //垂直方向（x++，y++）判断黑棋禁手
void BanHand_3_OneThreeQuadrant(int, int); //一三象限方向（x++，y++）判断黑棋禁手
void BanHand_3_TwoFourQuadrant(int, int); //二四象限方向（x++，y--）判断黑棋禁手
void BanHand_4_Horizon(int); //水平方向（x++，y++）判断黑棋禁手
void BanHand_4_Vertical(int); //垂直方向（x++，y++）判断黑棋禁手
void BanHand_4_OneThreeQuadrant(int, int); //一三象限方向（x++，y++）判断黑棋禁手
void BanHand_4_TwoFourQuadrant(int, int); //二四象限方向（x++，y--）判断黑棋禁手
void GameOver(); //游戏结束


int ChessCount_H(int Line, int Color);	//横向计连子数
int ChessCount_V(int Column, int Color);	//纵向计连子数
int ChessCount_OT(int Line, int Column, int Color); //一三象限计连子数
int ChessCount_TF(int Line, int Column, int Color); //二四象限计连子数
int CharToNum(char c); //字母转数组下标

struct Color SetColor(struct Color); //设定棋子颜色及玩家颜色的结构体函数

int Board[15][15];
//int x = 0, y = 0;
int xBlack = 0, yBlack = 0; //黑棋输入的棋子坐标：xBlack横坐标；yBlack纵坐标
int xWhite = 0, yWhite = 0; //白棋输入的棋子坐标：xWhite横坐标；yWhite纵坐标
int xBlackLast = -1, yBlackLast = -1; //黑棋上次落子的位置坐标，行：xBlackLast；列：yBlackLast
int xWhiteLast = -1, yWhiteLast = -1; //白棋上次落子的位置坐标，行：xWhiteLast；列：yWhiteLast

int count = 0; //连子数
int BeginH, EndH;//横向连子串的起始和结尾标志
int BeginV, EndV;//纵向连子串的起始和结尾标志
int BeginOTx, BeginOTy, EndOTx, EndOTy;//一三象限连子串的起始和结尾标志
int BeginTFx, BeginTFy, EndTFx, EndTFy;//二四象限连子串的起始和结尾标志

char xs[10] = {0};
char ys[10] = {0};


struct MAX //设定最大值坐标结构体
{
	int Value;
	int x;
	int y;
};
struct MAX max;


struct Color //设定棋子颜色及玩家颜色的结构体
{
	int iChangeColor; //设定棋子颜色
	int iChessColor; //设定玩家颜色
};

struct Color sColor = {1, 0}; //sColor结构体中每个成员的初值，其中iChangeColor = 1; iChessColor = 0


void main()
{
	system("color f0"); //f白底、0黑字
	ChooseGameMode(); //选择游戏模式
}


//选择游戏模式
void ChooseGameMode() 
{
	int a;

	printf("*****欢迎进行五子棋对弈*****\n\n");
	printf("【人机对战】 or 【人人对战】\n\n\n");
	printf("选择【人机对战】，请输入1\n");
	printf("选择【人人对战】，请输入2\n");

	scanf("%d", &a);

	for (; ;)
	{
		//重新初始化的目的：重新开始时，不延续上轮交换玩家，交换棋子颜色的记录
		xBlack = 0;
		yBlack = 0;
		xWhite = 0;
		yWhite = 0;
		xBlackLast = -1; 
		yBlackLast = -1;
		xWhiteLast = -1;
		yWhiteLast = -1;
		sColor.iChangeColor = 1;
		sColor.iChessColor = 11;

		if (a == 1)
		{
			PeopleVSComputer(); //人机对战
		}
		else if (a == 2)
		{
			PeopleVSPeople(); //人人对战

		}
		else
		{
			printf("选择无效，请重新输入1或者2！\n");

			scanf("%d", &a);
		}	
	}
}


//人机对战
void PeopleVSComputer() 
{
	int a;

	system("cls");

	printf("*****欢迎进行五子棋对弈之【人机对战】模式*****\n\n【人●】 or 【机●】\n\n\n选择【人●】，请输入1\n选择【机●】，请输入2\n");

	scanf("%d", &a);

	for (; ;)
	{
		if (a == 1)
		{
			PeopleVSComputer_PeopleBlackPlay(); //人持黑棋
		}
		else if (a == 2)
		{
			printf("【机●】功能创建中，未完待续...\n请重新输入1！\n");

			scanf("%d", &a);

			//ComputerBlackPlay(); //？？？？？功能待完善？？？？？
			//PeopleWhitePlay(); //人持白棋
		}
		else
		{
			printf("选择无效，请重新输入1或者2！\n");

			scanf("%d", &a);
		}	
	}
}


//人人对战
void PeopleVSPeople()
{
	char a;
	char as[10] = {0};

	system("cls");

	printf("*****欢迎进行五子棋对弈之【人人对战】模式*****\n\n系统默认：\n【玩家1●】\n【玩家2◎】\n\n输入任一字符开始对弈\n");

	scanf("%s", as); 

	a = atoi(as); 
	//atoi是ASCII码转int型函数。即凡是输入数字（ASCII码中的数字字符，如0对应十进制是48），都能转成int型数字
	//若是输入非0-9的数字字符，则返回0


	for (; ;)
	{
		if (a >= 0 && a <= 10)
		{
			PeopleVSPeople_BlackPlay();
		}
		else
		{
			printf("选择无效，请重新输入任一字符！\n");

			scanf("%s", as); 
		}	
	}
}


//显示棋盘
void DisplayBoard() //****需要改进：应该加上坐标
{	
	int i, j;

	for(j = BOARD_SIZE; j >= 0; j--)
		//for(j = 0; j <= BOARD_SIZE; j++)
	{
		printf("%2d", j+1);
		for(i = 0; i <= BOARD_SIZE; i++)
		{
			switch(Board[j][i] % 100)	//考虑有分值后的显示棋盘
			{
			case 1:
				printf("┗");
				break;

			case 2:
				printf("┛");
				break;

			case 3:
				printf("┓");
				break;

			case 4:
				printf("┏");
				break;

			case 5:
				printf("┠");
				break;

			case 6:
				printf("┷");
				break;

			case 7:
				printf("┨");
				break;

			case 8:
				printf("┯");
				break;

			case 9:
				printf("┼");
				break;

			case 10:
				printf("●");
				break;

			case 11:
				printf("▲");
				break;

			case 20:
				printf("◎");
				break;

			case 21:
				printf("△");
				break;
			}
			if (i == BOARD_SIZE)
			{
				printf("\n");
			}
		}
	}
	printf("   A B C D E F G H I J K L M N O\n");
}


//重置棋盘
void ResetBoardArray()
{
	int i, j; //j＝行，i＝列

	Board[0][0] = 1;
	Board[0][BOARD_SIZE] = 2;
	Board[BOARD_SIZE][BOARD_SIZE] = 3;
	Board[BOARD_SIZE][0] = 4;

	for (j = 1; j <= 13; j++)
	{
		Board[j][0] = 5;
	}

	for (i = 1; i <= 13; i++)
	{
		Board[0][i] = 6;
	}

	for (j = 1; j <= 13; j++)
	{
		Board[j][BOARD_SIZE] = 7;
	}

	for (i = 1; i <= 13; i++)
	{
		Board[BOARD_SIZE][i] = 8;
	}

	for (j = 1; j <= 13; j++)
	{
		for (i = 1; i <= 13; i++)
		{
			Board[j][i] = 9;
		}
	}
}


//人机对战之人持●
void PeopleVSComputer_PeopleBlackPlay() 
{
	int j = 0, i = 0;

	system("cls");	//清掉之前的屏幕显示，使光标恢复最初位置

	ResetBoardArray();
	DisplayBoard();

	while(yBlack >= 0 && yBlack <= BOARD_SIZE)	
	{
		while (0 <= xBlack && xBlack <= BOARD_SIZE)
		{
			//ys,xs是两个字符串，（所有的字符串都）以字符数组形式储存。数组各个元素分别存储字符串的各个字符
			scanf("%s %s", ys, xs); 

			//atoi是ASCII码转int型函数。即凡是输入数字（ASCII码中的数字字符，如0对应十进制是48），都能转成int型数字
			//若是输入非0-9的数字字符，则返回0

			yBlack = atoi(ys); 
			xBlack = xs[0]; //此句意思是，将xs数组的第0个元素的值，赋给xBlack

			IsOpposite_Black(/*yBlack, xBlack, BLACK_COLOR*/); //判断黑棋X,Y坐标输入是否规范

			system("cls");
			DisplayBoard();

			yBlack -= 1; //由数字1～15，转成数字0～14
			xBlack = CharToNum(xBlack);	//输入坐标转数组下标，即由字母转成数字0～14

			ChessRepeat_Black(/*yBlack, xBlack, BLACK_COLOR*/);
			ChessBeyondBoard_Black(/*yBlack, xBlack, BLACK_COLOR*/);

			sColor = SetColor(sColor); //设定棋子颜色及玩家颜色

			Board[yBlack][xBlack] = sColor.iChessColor;

			system("cls");	//清掉之前的屏幕显示，使光标恢复最初位置
			DisplayBoard();

			ChessShape();

			WhoIsWinning();

			PeopleVSComputer_ComputerWhitePlay(); //机持白棋后下
		}
	}
}


//人机对战之机持◎
void PeopleVSComputer_ComputerWhitePlay()
{
	if (Board[yBlack][xBlack] = 11)
	{
		MaxScore(); //求分值最高的位置

		xWhite = max.x;
		yWhite = max.y;
	}

	Board[max.y][max.x] = 21; //先判断计算机欲落子位置是否已有棋子/落出边界，之后再赋值

	sColor = SetColor(sColor);

	Board[yWhite][xWhite] = sColor.iChessColor;
	system("cls");	//清掉之前的屏幕显示，使光标恢复最初位置
	DisplayBoard();

	ChessShape();

	WhoIsWinning();
}


//人机对战之机持●
void PeopleVSComputer_ComputerBlackPlay()
{
	;
}


//人机对战之人持◎
void PeopleVSComputer_PeopleWhitePlay()
{
	;
}


//人人对战之黑棋下
void PeopleVSPeople_BlackPlay()
{
	int j = 0, i = 0;

	system("cls");	//清掉之前的屏幕显示，使光标恢复最初位置

	ResetBoardArray();
	DisplayBoard();

	while(yBlack >= 0 && yBlack <= BOARD_SIZE)	
	{
		while (0 <= xBlack && xBlack <= BOARD_SIZE)
		{
			//ys,xs是两个字符串，（所有的字符串都）以字符数组形式储存。数组各个元素分别存储字符串的各个字符
			scanf("%s %s", ys, xs); 

			//atoi是ASCII码转int型函数。即凡是输入数字（ASCII码中的数字字符，如0对应十进制是48），都能转成int型数字
			//若是输入非0-9的数字字符，则返回0

			yBlack = atoi(ys); 
			xBlack = xs[0]; //此句意思是，将xs数组的第0个元素的值，赋给xBlack

			IsOpposite_Black(/*yBlack, xBlack, BLACK_COLOR*/); //判断黑棋X,Y坐标输入是否规范

			system("cls");
			DisplayBoard();

			yBlack -= 1; //由数字1～15，转成数字0～14
			xBlack = CharToNum(xBlack);	//输入坐标转数组下标，即由字母转成数字0～14

			ChessRepeat_Black(/*yBlack, xBlack, BLACK_COLOR*/);
			ChessBeyondBoard_Black(/*yBlack, xBlack, BLACK_COLOR*/);

			sColor = SetColor(sColor); //设定棋子颜色及玩家颜色

			Board[yBlack][xBlack] = sColor.iChessColor;

			system("cls");	//清掉之前的屏幕显示，使光标恢复最初位置
			DisplayBoard();

			ChessShape();

			WhoIsWinning();

			PeopleVSPeople_WhitePlay();
		}
	}
}


//人人对战之白棋下
void PeopleVSPeople_WhitePlay()
{
	int j = 0, i = 0;

	//ys,xs是两个字符串，（所有的字符串都）以字符数组形式储存。数组各个元素分别存储字符串的各个字符
	scanf("%s %s", ys, xs); 

	//atoi是ASCII码转int型函数。即凡是输入数字（ASCII码中的数字字符，如0对应十进制是48），都能转成int型数字
	//若是输入非0-9的数字字符，则返回0

	yWhite = atoi(ys); 
	xWhite = xs[0]; //此句意思是，将xs数组的第0个元素的值，赋给xWhite

	IsOpposite_White(); //判断白棋X，Y坐标是否输反了

	system("cls");
	DisplayBoard();

	yWhite -= 1; //由数字1～15，转成数字0～14
	xWhite = CharToNum(xWhite);	//输入坐标转数组下标，即由字母转成数字0～14

	ChessRepeat_White(); //判断白棋是否重复落子
	ChessBeyondBoard_White(); //判断白棋落子是否超出棋盘

	sColor = SetColor(sColor); //设定棋子颜色及玩家颜色

	Board[yWhite][xWhite] = sColor.iChessColor;

	system("cls");	//清掉之前的屏幕显示，使光标恢复最初位置
	DisplayBoard();

	ChessShape();

	WhoIsWinning();
}



////判断黑棋X,Y坐标输入是否规范
void IsOpposite_Black()
{
	// (yBlack == 0)：因为atoi函数对输入的非0-9的数字字符，输出结果均为0
	// (xBlack < 58)：因为十进制的57对应的字符是数字9，所以只要小于等于9的，都可认为是输入错了
	while((yBlack == 0) || (xBlack < 58)) 
	{
		system("cls");
		DisplayBoard();
		printf("(X,Y)坐标输入不规范，请重新输入！\n注意：输入的坐标顺序为（数字，字母）\n");

		scanf("%s %s", ys, xs);
		yBlack = atoi(ys);
		xBlack = xs[0];
	}
}


//判断黑棋是否重复落子
void ChessRepeat_Black()
{
	//如果输入位置已有棋子，则重新输入
	while ((Board[yBlack][xBlack] % 100) >=10) //只要是落子错误，就不会往下进行，直到输入正确的坐标为止
	{
		system("cls");
		DisplayBoard();
		printf("该位置已有棋子，请重新输入\n");

		scanf("%d %c", &yBlack, &xBlack);
		xBlack = CharToNum(xBlack);	//输入坐标转数组下标
		yBlack -= 1;
	}
}


//判断黑棋落子是否超出棋盘
void ChessBeyondBoard_Black()
{
	//如果输入位置已超棋盘边界，则重新输入
	while((yBlack > BOARD_SIZE) || (xBlack > BOARD_SIZE)) //只要是落子错误，就不会往下进行，直到输入正确的坐标为止
	{
		system("cls");
		DisplayBoard();
		printf("输入位置已超棋盘边界，请重新输入！\n");

		scanf("%d %c", &yBlack, &xBlack);
		xBlack = CharToNum(xBlack);	//输入坐标转数组下标
		yBlack -= 1;
	}
}


//判断白棋X，Y坐标是否输反了-人持白棋时调用，机持白棋不用
void IsOpposite_White()
{
	// (yWhite == 0)：因为atoi函数对输入的非0-9的数字字符，输出结果均为0
	// (xWhite < 58)：因为十进制的57对应的字符是数字9，所以只要小于等于9的，都可认为是输入错了
	while((yWhite == 0) || (xWhite < 58)) 
	{
		system("cls");
		DisplayBoard();
		printf("(X,Y)坐标输入不规范，请重新输入！\n注意：输入的坐标顺序为（数字，字母）\n");

		scanf("%s %s", ys, xs);
		yWhite = atoi(ys);
		xWhite = xs[0];
	}
}


//判断白棋是否重复落子-人持白棋时调用，机持白棋不用
void ChessRepeat_White()
{
	//如果输入位置已有棋子，则重新输入
	while ((Board[yWhite][xWhite] % 100) >=10) //只要是落子错误，就不会往下进行，直到输入正确的坐标为止
	{
		system("cls");
		DisplayBoard();
		printf("该位置已有棋子，请重新输入\n");

		scanf("%d %c", &yWhite, &xWhite);
		xWhite = CharToNum(xWhite);	//输入坐标转数组下标
		yWhite -= 1;
	}
}


//判断白棋落子是否超出棋盘-人持白棋时调用，机持白棋不用
void ChessBeyondBoard_White()
{
	//如果输入位置已超棋盘边界，则重新输入
	while((yWhite > BOARD_SIZE) || (xWhite > BOARD_SIZE)) //只要是落子错误，就不会往下进行，直到输入正确的坐标为止
	{
		system("cls");
		DisplayBoard();
		printf("输入位置已超棋盘边界，请重新输入！\n");

		scanf("%d %c", &yWhite, &xWhite);
		xWhite = CharToNum(xWhite);	//输入坐标转数组下标
		yWhite -= 1;
	}
} 



//设定棋子颜色及玩家颜色
struct Color SetColor(struct Color sColor)  
{
	if (sColor.iChangeColor == 1)
	{
		sColor.iChessColor = 11;
		if (xBlackLast != -1 && yBlackLast != -1) //判断黑棋玩家是否有上一步落子，有则进入if语句
		{
			Board[yBlackLast][xBlackLast] = 10; //上一步棋子由▲变为●
		}
		sColor.iChangeColor = 2; //******玩家交换
	}
	else
	{
		sColor.iChessColor = 21;
		if (xWhiteLast != -1 && yWhiteLast != -1) //判断白棋玩家是否有上一步落子，有则进入if语句
		{
			Board[yWhiteLast][xWhiteLast] = 20; //上一步棋子由△变为◎
		}
		sColor.iChangeColor = 1; //******玩家交换
	}
	//*****结束设定玩家棋子颜色
	return sColor;
}


//设定棋子形状——当前步的棋子形状是三角▲/△，上一步的棋子形状是圆形●/◎
void ChessShape()
{
	if (sColor.iChessColor == 11) //把当前棋子位置坐标赋给前一步位置
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


//横向打分
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
				printf("黑棋获胜！\n");
				GameOver();
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
				printf("黑棋长连，白棋获胜！\n");
				GameOver();
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
				printf("白棋获胜！\n");
				GameOver();
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
				printf("黑棋获胜！\n");
				GameOver();
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
				printf("黑棋长连，白棋获胜！\n");
				GameOver();
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
				printf("白棋获胜！\n");
				GameOver();
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
				printf("黑棋获胜！\n");
				GameOver();
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
				printf("黑棋长连，白棋获胜！\n");
				GameOver();
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
				printf("白棋获胜！\n");
				GameOver();
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
				printf("黑棋获胜！\n");
				GameOver();
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
				printf("黑棋长连，白棋获胜！\n");
				GameOver();
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
				printf("白棋获胜！\n");
				GameOver();
			}

			Score_TF(BeginTFy, BeginTFx, count, WHITE_COLOR);
		}
	}
}


void CleanScore()
{
	int x, y;
	for (y = 0; y < 15; y++)
	{
		for (x = 0; x < 15; x++)
		{
			Board[y][x] %= 100;
		}
	}

}


void MaxScore()
{
	int x = 0;
	int y = 0;

	max.Value = max.x = max.y = 0;

	for (y = 0; y < 15; y++)
	{
		for (x = 0; x < 15; x++)
		{
			if (Board[y][x] > max.Value)
			{
				max.Value = Board[y][x];
				max.x = x;
				max.y = y;
			}
		}
	}
}


//字母转数组下标
int CharToNum(char c)
{
	if(c >= 'A' && c <= 'O')
	{
		return c - 'A';
	}
	else if(c >= 'a' && c <= 'o')
	{
		return c - 'a';
	}
}


//游戏结束
void GameOver()
{
	int a;

	printf("\n\n是否重新开始一轮五子棋对弈？\n\n是请输入1\n否请输入2\n");

	scanf("%d", &a);

	for (; ;)
	{
		if (a == 1)
		{
			system("cls");	//清掉之前的屏幕显示，使光标恢复最初位置

			ChooseGameMode();
		}
		else if (a == 2)
		{
			system("cls");	//清掉之前的屏幕显示，使光标恢复最初位置
			break;
		}
		else
		{
			printf("选择无效，请重新输入或者！\n");

			scanf("%d", &a);
		}
	}	
	printf("GAME OVER!!!\n");
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
					printf("此步黑棋为四三三禁手，白棋获胜！\n");
					GameOver();
					return;
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
					printf("此步黑棋为四三三禁手，白棋获胜！\n");
					GameOver();
					return;
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
					printf("此步黑棋为四三三禁手，白棋获胜！\n");
					GameOver();
					return;
				}
			}


			//若无第三条〔活四〕，则判定为三三禁手
			printf("此步黑棋为三三禁手，白棋获胜！\n");
			GameOver();
			return;
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
					printf("此步黑棋为四四三禁手，白棋获胜！\n");
					GameOver();
					return;

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
					printf("此步黑棋为四四三禁手，白棋获胜！\n");
					GameOver();
					return;
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
					printf("此步黑棋为四四三禁手，白棋获胜！\n");
					GameOver();
					return;
				}
			}


			//若无第三条〔活三〕，则判定为四四禁手
			printf("此步黑棋为四四禁手，白棋获胜！\n");
			GameOver();
			return;
		}
	}
}



//垂直方向（x++，y++）判断黑棋禁手
void BanHand_3_Vertical(int Line)
{
	//////for (
	//////	(BeginV = yBlack - 4, EndV = yBlack - 4) && (EndV - 1 <= BOARD_SIZE); 
	//////	(BeginV <= yBlack + 4) && (BeginV <= BOARD_SIZE) && (EndV - 1 <= yBlack + 4) && (EndV - 1 <= BOARD_SIZE); 
	//////BeginV = EndV + 1)
	//////{


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
					printf("此步黑棋为四三三禁手，白棋获胜！\n");
					GameOver();
					return;
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
					printf("此步黑棋为四三三禁手，白棋获胜！\n");
					GameOver();
					return;
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
					printf("此步黑棋为四三三禁手，白棋获胜！\n");
					GameOver();
					return;
				}

				else
				{
					continue;
				}
			}

			//若无第三条〔活四〕，则判定为三三禁手
			printf("此步黑棋为三三禁手，白棋获胜！\n");
			GameOver();
			return;
		}
	}
}



//垂直方向（x++，y++）判断黑棋禁手
void BanHand_4_Vertical(int Line)
{
	//////for (
	//////	(BeginV = yBlack - 4, EndV = yBlack - 4) && (EndV - 1 <= BOARD_SIZE); 
	//////	(BeginV <= yBlack + 4) && (BeginV <= BOARD_SIZE) && (EndV - 1 <= yBlack + 4) && (EndV - 1 <= BOARD_SIZE); 
	//////BeginV = EndV + 1)
	//////{

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
					printf("此步黑棋为四四三禁手，白棋获胜！\n");
					GameOver();
					return;
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
					printf("此步黑棋为四四三禁手，白棋获胜！\n");
					GameOver();
					return;
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
					printf("此步黑棋为四四三禁手，白棋获胜！\n");
					GameOver();
					return;
				}

				else
				{
					continue;
				}
			}


			//若无第三条〔活三〕，则判定为四四禁手
			printf("此步黑棋为四四禁手，白棋获胜！\n");
			GameOver();
			return;

		}
	}
}



//一三象限方向（x++，y++）判断黑棋禁手
void BanHand_3_OneThreeQuadrant(int Column, int Line)
{
	//////////for (
	//////////	(BeginOTx = xBlack - 4,BeginOTy = yBlack - 4, EndOTx = xBlack - 4, EndOTy = yBlack - 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
	//////////	(BeginOTx <= xBlack + 4) && (BeginOTy <= yBlack + 4) && (EndOTx - 1 <= xBlack + 4) && (EndOTy - 1 <= yBlack + 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
	//////////BeginOTx = EndOTx + 1, BeginOTy = EndOTy + 1)

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
					printf("此步黑棋为四三三禁手，白棋获胜！\n");
					GameOver();
					return;
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
					printf("此步黑棋为四三三禁手，白棋获胜！\n");
					GameOver();
					return;
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
					printf("此步黑棋为四三三禁手，白棋获胜！\n");
					GameOver();
					return;
				}

				else
				{
					continue;
				}
			}

			//若无第三条〔活四〕，则判定为三三禁手
			printf("此步黑棋为三三禁手，白棋获胜！\n");
			GameOver();
			return;
		}

		//////else
		//////{
		//////	continue;
		//////}
	}
}


//一三象限方向（x++，y++）判断黑棋禁手
void BanHand_4_OneThreeQuadrant(int Column, int Line)
{
	//////////for (
	//////////	(BeginOTx = xBlack - 4, BeginOTy = yBlack - 4, EndOTx = xBlack - 4, EndOTy = yBlack - 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
	//////////	(BeginOTx <= xBlack + 4) && (BeginOTy <= yBlack + 4) && (EndOTx - 1 <= xBlack + 4) && (EndOTy - 1 <= yBlack + 4) && (EndOTx - 1 <= BOARD_SIZE) && (EndOTy - 1 <= BOARD_SIZE); 
	//////////BeginOTx = EndOTx + 1, BeginOTy = EndOTy + 1)
	//////////{


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
					printf("此步黑棋为四四三禁手，白棋获胜！\n");
					GameOver();
					return;
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
					printf("此步黑棋为四四三禁手，白棋获胜！\n");
					GameOver();
					return;
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
					printf("此步黑棋为四四三禁手，白棋获胜！\n");
					GameOver();
					return;
				}

				else
				{
					continue;
				}
			}


			//若无第三条〔活三〕，则判定为四四禁手
			printf("此步黑棋为四四禁手，白棋获胜！\n");
			GameOver();
			return;
		}
	}
}


//二四象限方向（x++，y--）判断黑棋禁手
void BanHand_3_TwoFourQuadrant(int Column, int Line)
{
	/////考虑边界限制的思路：按照棋盘和连子串的遍历方向（从左至右，从下至上，从左下至右上，从右上至左下），只需对尾端边界进行限制即可，起始边即时超边缘了，统计连子数时也会跳过

	//for循环的表达式2只限制了(BeginTFx <= xBlack + 4)，而未限制(BeginTFy = yBlack - 4)，是因为y和x的遍历方向不一致，拧着，怕矛盾；
	//且y随x变化而变化，无需再作为限制条件写进表达式2里


	//////////for (
	//////////	(BeginTFx = xBlack - 4, BeginTFy = yBlack + 4, EndTFx = xBlack - 4, EndTFy = yBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
	//////////	(BeginTFx <= xBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
	//////////BeginTFx = EndTFx + 1, BeginTFy = EndTFy - 1)
	//////////{

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
					printf("此步黑棋为四三三禁手，白棋获胜！\n");
					GameOver();
					return;
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
					printf("此步黑棋为四三三禁手，白棋获胜！\n");
					GameOver();
					return;
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
					printf("此步黑棋为四三三禁手，白棋获胜！\n");
					GameOver();
					return;
				}

				else
				{
					continue;
				}
			}

			//若无第三条〔活四〕，则判定为三三禁手
			printf("此步黑棋为三三禁手，白棋获胜！\n");
			GameOver();
			return;
		}
	}
} 



//二四象限方向（x++，y--）判断黑棋禁手
void BanHand_4_TwoFourQuadrant(int Column, int Line)
{
	/////考虑边界限制的思路：按照棋盘和连子串的遍历方向（从左至右，从下至上，从左下至右上，从右上至左下），只需对尾端边界进行限制即可，起始边即时超边缘了，统计连子数时也会跳过

	//for循环的表达式2只限制了(BeginTFx <= xBlack + 4)，而未限制(BeginTFy = yBlack - 4)，是因为y和x的遍历方向不一致，拧着，怕矛盾；
	//且y随x变化而变化，无需再作为限制条件写进表达式2里


	//////////for (
	//////////	(BeginTFx = xBlack - 4, BeginTFy = yBlack + 4, EndTFx = xBlack - 4, EndTFy = yBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
	//////////	(BeginTFx <= xBlack + 4) && (EndTFx - 1 <= BOARD_SIZE) && (0 <= EndTFy + 1); 
	//////////BeginTFx = EndTFx + 1, BeginTFy = EndTFy - 1)
	//////////{

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
					printf("此步黑棋为四四三禁手，白棋获胜！\n");
					GameOver();
					return;
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
					printf("此步黑棋为四四三禁手，白棋获胜！\n");
					GameOver();
					return;
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
					printf("此步黑棋为四四三禁手，白棋获胜！\n");
					GameOver();
					return;
				}

				else
				{
					continue;
				}
			}


			//若无第三条〔活三〕，则判定为四四禁手
			printf("此步黑棋为四四禁手，白棋获胜！\n");
			GameOver();
			return;
		}
	}
}