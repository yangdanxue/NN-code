#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


#define pixels						28		//MNIST数据库图片像素是28*28
#define Padding_pixels				32		//填充0后图片像素是32*32
#define ConvolutionKernel			5		//卷积核，大小是5*5
#define NumConvolution1				28		//卷积1层矩阵边长
#define NumSubsampling2				14		//池化2层矩阵边长
#define Padding_NumSubsampling2		18		//填充0图片后，池化2层矩阵边长
#define NumConvolution3				14		//卷积3层矩阵边长
#define NumSubsampling4				7		//池化4层矩阵边长
#define Filter						2		//池化层过滤器大小
#define NumOutput					10   	//输出层神经元个数

#define NumKernel_IToC1				8		//卷积核数量
#define NumKernel_S2ToC3			16		//卷积核数量

#define OldWeightPercentage			0.9   	//反向传播更新权重时旧权重占比
#define NewWeightPercentage			0.1   	//反向传播更新权重时新权重占比

#define TrainTimes					1	    //训练次数
#define TestNumber					60000	//总测试图片的数量
#define Memory						1       //记住池化层中每个值，在上一卷积层中的具体坐标


int ReadPicData(unsigned char* data, int iNum); //读图片数据文件

//写文件——保存当前各层权重
void WriteWeightInputToConvolution1Data();
void WriteWeightSubsampling2ToConvolution3Data();
void WriteWeightSubsampling4ToOutputData();

//读文件——读取已保存的各层权重
void ReadWeightInputToConvolution1Data();
void ReadWeightSubsampling2ToConvolution3Data();
void ReadWeightSubsampling4ToOutputData();

//初始化各层权重
void InitializationWeightInputToConvolution1();	
void InitializationWeightSubsampling2ToConvolution3();
void InitializationWeightSubsampling4ToOutput();

int CalculateNetResult();		//计算网络输出结果
float TestNet();				//测试网络
void TrainNet();				//选择测试或训练

void CalculateNeuronValue();	//计算各个神经元的值
void BackPropagation();			//反向传播法更新权值
void ClearTarget();				//清理Target[]

unsigned char iPicture[pixels][pixels] = {0};							//导入的MNIST数据库图片数组
unsigned char Padding_iPicture[Padding_pixels][Padding_pixels] = {0};	//对于5*5的过滤器，在导入的MNIST数据库图片数组iPicture[y][x]周围，填充宽度为2的0时，就可以保证输出图像和输入图像一样大
unsigned char Label;													//导入的MNIST数据库标签
unsigned char* p = &Label;

//各层卷积核（权重）
double WeightInputToConvolution1[ConvolutionKernel * ConvolutionKernel * NumKernel_IToC1] = {0};
double WeightSubsampling2ToConvolution3[ConvolutionKernel * ConvolutionKernel * NumKernel_S2ToC3] = {0};
double WeightSubsampling4ToOutput[NumSubsampling4 * NumSubsampling4 * NumOutput * NumKernel_IToC1 * NumKernel_S2ToC3] = {0};

//各层卷积核翻转180°（权重）
double WeightRot180Subsampling2ToConvolution3[ConvolutionKernel * ConvolutionKernel * NumKernel_S2ToC3] = {0};

//各层反向传播后旧权重保留的比例
double OldWeightInputToConvolution1[ConvolutionKernel * ConvolutionKernel * NumKernel_IToC1] = {0};
double OldWeightSubsampling2ToConvolution3[ConvolutionKernel * ConvolutionKernel * NumKernel_S2ToC3] = {0};
double OldWeightSubsampling4ToOutput[NumSubsampling4 * NumSubsampling4 * NumOutput * NumKernel_IToC1 * NumKernel_S2ToC3] = {0};

//各层神经元值
double NetConvolution1[NumConvolution1 * NumConvolution1 * NumKernel_IToC1] = {0};
double NetSubsampling2[NumSubsampling2 * NumSubsampling2 * NumKernel_IToC1] = {0};
double NetConvolution3[NumConvolution3 * NumConvolution3 * NumKernel_IToC1 * NumKernel_S2ToC3] = {0};
double NetSubsampling4[NumSubsampling4 * NumSubsampling4 * NumKernel_IToC1 * NumKernel_S2ToC3] = {0};
double NetOutput[NumOutput] = {0};

//各层激活函数值（Sigmoid函数）
double SigmoidConvolution1[NumConvolution1 * NumConvolution1 * NumKernel_IToC1] = {0};
double SigmoidSubsampling2[NumSubsampling2 * NumSubsampling2 * NumKernel_IToC1] = {0};
double Padding_SigmoidSubsampling2[Padding_NumSubsampling2 * Padding_NumSubsampling2 * NumKernel_IToC1] = {0};	//对于5*5的过滤器，填充宽度为2的0时，就可以保证输出图像和输入图像一样大
double SigmoidConvolution3[NumConvolution3 * NumConvolution3 * NumKernel_IToC1 * NumKernel_S2ToC3] = {0};
double SigmoidSubsampling4[NumSubsampling4 * NumSubsampling4 * NumKernel_IToC1 * NumKernel_S2ToC3] = {0};
double SigmoidOutput[NumOutput] = {0};

//目标输出值
int target[NumOutput] = {0};

//各层误差值
double Delta_Output[NumOutput] = {0};
double Delta_Subsampling4[NumSubsampling4 * NumSubsampling4 * NumKernel_IToC1 * NumKernel_S2ToC3] = {0};
double Delta_Convolution3[NumConvolution3 * NumConvolution3 * NumKernel_IToC1 * NumKernel_S2ToC3] = {0};
double Delta_Subsampling2[NumSubsampling2 * NumSubsampling2 * NumKernel_IToC1] = {0};
double Delta_Convolution1[NumConvolution1 * NumConvolution1 * NumKernel_IToC1] = {0};

double PD_ErrorToNetOutput[NumOutput] = {0};
double PD_ErrorToNetSubsampling4[NumSubsampling4 * NumSubsampling4 * NumKernel_IToC1 * NumKernel_S2ToC3] = {0};
double PD_ErrorToNetConvolution3[NumConvolution3 * NumConvolution3 * NumKernel_IToC1 * NumKernel_S2ToC3] = {0};
double PD_ErrorToNetSubsampling2[NumSubsampling2 * NumSubsampling2 * NumKernel_IToC1] = {0};
double PD_ErrorToNetConvolution1[NumConvolution1 * NumConvolution1 * NumKernel_IToC1] = {0};

//各层权重更新
double PD_WeightSubsampling4ToOutput[NumSubsampling4 * NumSubsampling4 * NumOutput * NumKernel_IToC1 * NumKernel_S2ToC3] = {0};
double PD_WeightSubsampling2ToConvolution3[ConvolutionKernel * ConvolutionKernel * NumKernel_S2ToC3] = {0};
double PD_WeightInputToConvolution1[ConvolutionKernel * ConvolutionKernel * NumKernel_IToC1] = {0};

double PD_SigmoidOutputToNetOutput[NumOutput] = {0};
double PD_SigmoidSubsampling4ToNetSubsampling4[NumSubsampling4 * NumSubsampling4 * NumKernel_IToC1 * NumKernel_S2ToC3] = {0};
double PD_SigmoidConvolution3ToNetConvolution3[NumConvolution3 * NumConvolution3 * NumKernel_IToC1 * NumKernel_S2ToC3] = {0};
double PD_SigmoidSubsampling2ToNetSubsampling2[NumSubsampling2 * NumSubsampling2 * NumKernel_IToC1] = {0};
double PD_SigmoidConvolution1ToNetConvolution1[NumConvolution1 * NumConvolution1 * NumKernel_IToC1] = {0};

//输出层---->池化4层的权值更新
double PD_ErrorToSigmoidOutput[NumOutput] = {0};
double PD_NetOutputToWeightSubsampling4ToOutput[NumSubsampling4 * NumSubsampling4 * NumKernel_IToC1 * NumKernel_S2ToC3] = {0};

//池化4层---->卷积3层的权值更新
double PD_NetOutputToSigmoidSubsampling4[NumOutput * NumSubsampling4 * NumSubsampling4 * NumKernel_IToC1 * NumKernel_S2ToC3] = {0};

//在卷积3层的误差值矩阵周围，填充4圈0，为了符合梯度计算：∇a11=δ11 * w11(∇a:SigS2,δ:NetC3,w:W-S2_C3)
double Padding_Delta_Convolution3[NumKernel_S2ToC3 * NumKernel_IToC1 * (NumConvolution3+2*(ConvolutionKernel - 1)) * (NumConvolution3+2*(ConvolutionKernel - 1))] = {0}; 

//池化2层---->卷积1层的权值更新

double upsample_Delta_Subsampling4[NumConvolution3 * NumConvolution3 * NumKernel_IToC1 * NumKernel_S2ToC3] = {0};
double upsample_Delta_Subsampling2[NumConvolution1 * NumConvolution1 * NumKernel_IToC1] = {0};


//设定最大池化值的坐标结构体数组
struct MAXPool
{
	double Value;
	int NumK_IToC1;
	int NumK_S2ToC3;
	int x;
	int y;
	int i;
	int j;
};
struct MAXPool maxSubsampling2[NumSubsampling2 * NumSubsampling2 * NumKernel_IToC1] = {0};
struct MAXPool maxSubsampling4[NumSubsampling4 * NumSubsampling4 * NumKernel_IToC1 * NumKernel_S2ToC3] = {0};


float LearningRate = 0.005;	//学习速率

int Correct = 0;//识别正确数


void main()
{
	float CorrectRate;
	int MaxCorrect = 0;
	int i = 0;

	ReadWeightInputToConvolution1Data();
	ReadWeightSubsampling2ToConvolution3Data();
	ReadWeightSubsampling4ToOutputData();

	//InitializationWeightInputToConvolution1();
	//InitializationWeightSubsampling2ToConvolution3();
	//InitializationWeightSubsampling4ToOutput();

	printf("学习速率：%f\n" , LearningRate);

	CorrectRate = TestNet();
	printf("正确数：%d 正确率：%.4f%%\n", Correct, CorrectRate * 100);

	//for (;;)
	//{
	//	i++;
	//	printf("第%d次训练：\n", i);

	//	TrainNet();//训练网络

	//	CorrectRate = TestNet();
	//	printf("正确数：%d 正确率：%.4f%%\n", Correct, CorrectRate * 100);

	//	if (MaxCorrect < Correct)
	//	{
	//		MaxCorrect = Correct;
	//		WriteWeightInputToConvolution1Data();
	//		WriteWeightSubsampling2ToConvolution3Data();
	//		WriteWeightSubsampling4ToOutputData();
	//	}
	//}
}


//读文件——使用保存的[输入层-卷积1层]权重
void ReadWeightInputToConvolution1Data()
{
	int iNum = 0; //神经元个数

	FILE *fpReadItoC1 = fopen("d:\\WeightItoC1Data201811182132.wet","rb");

	if(fpReadItoC1 == NULL)
	{
		printf("WeightItoC1Data201811182132.wet文件不存在！\n");
		return;
	}

	//下面是读数据，将读到的数据存到数组WeightInputToConvolution1[NumConvolution1]中
	iNum = fread(WeightInputToConvolution1, sizeof(double), ConvolutionKernel * ConvolutionKernel * NumKernel_IToC1, fpReadItoC1);
	if(iNum != ConvolutionKernel * ConvolutionKernel * NumKernel_IToC1)
	{
		printf("读数据存到数组WeightInputToConvolution1[ConvolutionKernel * ConvolutionKernel * NumKernel_IToC1]错误！！！\n");
	}
	printf("读[输入层-卷积1层]间的权重数据成功\n");

	fclose(fpReadItoC1);

	return;
}


//读文件——使用保存的[池化2层-卷积3层]权重
void ReadWeightSubsampling2ToConvolution3Data()
{
	int iNum = 0; //神经元个数

	FILE *fpReadS2toC3 = fopen("d:\\WeightS2toC3Data201811182132.wet","rb");

	if(fpReadS2toC3 == NULL)
	{
		printf("WeightS2toC3Data201811182132.wet文件不存在！\n");
		return;
	}

	//下面是读数据，将读到的数据存到数组WeightSubsampling2ToConvolution3[NumConvolution1]中
	iNum = fread(WeightSubsampling2ToConvolution3, sizeof(double), ConvolutionKernel * ConvolutionKernel * NumKernel_S2ToC3, fpReadS2toC3);
	if(iNum != ConvolutionKernel * ConvolutionKernel * NumKernel_S2ToC3)
	{
		printf("读数据存到数组WeightSubsampling2ToConvolution3[ConvolutionKernel * ConvolutionKernel * NumKernel_S2ToC3]错误！！！\n");
	}
	printf("读[池化2层-卷积3层]间的权重数据成功\n");

	fclose(fpReadS2toC3);

	return;
}


//读文件——使用保存的[池化4层-输出层]权重
void ReadWeightSubsampling4ToOutputData()
{
	int iNum = 0; //输入层-隐藏层间的神经元个数

	FILE *fpReadS4toO = fopen("d:\\WeightS4toOData201811182132.wet","rb");

	if(fpReadS4toO == NULL)
	{
		printf("WeightS4toOData201811182132.wet文件不存在！\n");
		return;
	}

	//下面是读数据，将读到的数据存到数组WeightSubsampling4ToOutput[NumSubsampling2 * NumOutput]中
	iNum = fread(WeightSubsampling4ToOutput, sizeof(double), NumSubsampling4 * NumSubsampling4 * NumOutput * NumKernel_IToC1 * NumKernel_S2ToC3, fpReadS4toO);
	if(iNum != NumSubsampling4 * NumSubsampling4 * NumOutput * NumKernel_IToC1 * NumKernel_S2ToC3)
	{
		printf("读数据存到数组WeightSubsampling4ToOutput[NumSubsampling4 * NumSubsampling4 * NumOutput * NumKernel_IToC1 * NumKernel_S2ToC3]错误！！！\n");
	}
	printf("读[池化4层-输出层]间的权重数据成功\n");

	fclose(fpReadS4toO);

	return;
}			


//写文件——保留当前调好的[输入层-卷积1层]权重
void WriteWeightInputToConvolution1Data()
{
	int iNum = 0;
	FILE *fpWriteItoC1 = fopen("d:\\WeightItoC1Data201811182132.wet","wb+");

	if(fpWriteItoC1 == NULL)
	{
		printf("创建WeightItoC1Data201811182132.wet文件失败！\n");
		return;
	}

	//将输入层-卷积1层间的权重写入到WeightItoC1Data201811182132.wet文件中
	iNum = fwrite(WeightInputToConvolution1, sizeof(double), ConvolutionKernel * ConvolutionKernel * NumKernel_IToC1, fpWriteItoC1);
	if (iNum != ConvolutionKernel * ConvolutionKernel * NumKernel_IToC1)
	{
		printf("写WeightItoC1Data201811182132.wet文件失败！");
	}
	printf("所有输入层-卷积1层间的权重数据已经保存\n");

	fclose(fpWriteItoC1);
}


//写文件——保留当前调好的[池化2层-卷积3层]权重
void WriteWeightSubsampling2ToConvolution3Data()
{
	int iNum = 0;
	FILE *fpWriteS2toC3 = fopen("d:\\WeightS2toC3Data201811182132.wet","wb+");

	if(fpWriteS2toC3 == NULL)
	{
		printf("创建WeightS2toC3Data201811182132.wet文件失败！\n");
		return;
	}

	//将输入层-卷积1层间的权重写入到WeightS2toC3Data201811182132.wet文件中
	iNum = fwrite(WeightSubsampling2ToConvolution3, sizeof(double), ConvolutionKernel * ConvolutionKernel * NumKernel_S2ToC3, fpWriteS2toC3);
	if (iNum != ConvolutionKernel * ConvolutionKernel * NumKernel_S2ToC3)
	{
		printf("写WeightS2toC3Data201811182132.wet文件失败！");
	}
	printf("所有池化2层-卷积3层间的权重数据已经保存\n");

	fclose(fpWriteS2toC3);
}


//写文件——保留当前调好的[池化4层-输出层]权重
void WriteWeightSubsampling4ToOutputData()
{
	int iNum = 0;
	FILE *fpWriteS4toO = fopen("d:\\WeightS4toOData201811182132.wet","wb+");

	if(fpWriteS4toO == NULL)
	{
		printf("创建WeightS4toOData201811182132.wet文件失败！\n");
		return;
	}

	//将池化4层-输出层间的权重写入到WeightS4toOData201811182132.wet文件中
	iNum = fwrite(WeightSubsampling4ToOutput, sizeof(double), NumSubsampling4 * NumSubsampling4 * NumOutput * NumKernel_IToC1 * NumKernel_S2ToC3, fpWriteS4toO);
	if (iNum != NumSubsampling4 * NumSubsampling4 * NumOutput * NumKernel_IToC1 * NumKernel_S2ToC3)
	{		
		printf("写WeightS4toOData201811182132.wet文件失败！");
	}
	printf("所有池化4层-输出层间的权重数据已经保存\n");

	fclose(fpWriteS4toO);
}			


//随机初始化[输入层-卷积1层]权重
void InitializationWeightInputToConvolution1()
{
	int m = 0;
	int n = 0;

	for(m = 0; m < ConvolutionKernel * ConvolutionKernel * NumKernel_IToC1; m++)
	{
		//WeightInputToConvolution1[m] = (rand()/(double)(RAND_MAX))/8000;
		//WeightInputToConvolution1[m] = 0.5/2000;	
		WeightInputToConvolution1[m] = 0;		
		//WeightInputToConvolution1[m] = 1023.9754277741900000;				//78.0700%版权重的平均数
		//WeightInputToConvolution1[m] = -0.1964562163147680;				//78.0700%版权重的中位数
	}
}


//随机初始化[池化2层-卷积3层]权重
void InitializationWeightSubsampling2ToConvolution3()
{
	int m, n = 0;

	for(m = 0; m < ConvolutionKernel * ConvolutionKernel * NumKernel_S2ToC3; m++)
	{
		//WeightSubsampling2ToConvolution3[m] = (rand()/(double)(RAND_MAX))/3200;		
		WeightSubsampling2ToConvolution3[m] = 0;					
		//WeightSubsampling2ToConvolution3[m] = -0.1106441036656980;				///78.0700%版权重的平均数
		//WeightSubsampling2ToConvolution3[m] = 0.0477659333993450;					///78.0700%版权重的中位数
	}
}


//随机初始化[池化4层-输出层]权重
void InitializationWeightSubsampling4ToOutput()
{
	int m = 0;

	for(m = 0; m < NumSubsampling4 * NumSubsampling4 * NumOutput * NumKernel_IToC1 * NumKernel_S2ToC3; m++)
	{
		WeightSubsampling4ToOutput[m] = (rand()/(double)(RAND_MAX))/6400;			
		//WeightSubsampling4ToOutput[m] = 0;										
		//WeightSubsampling4ToOutput[m] = -0.3302111820947480;						//78.0700%版权重的平均数
		//WeightSubsampling4ToOutput[m] = -0.3983054846310040;						//78.0700%版权重的中位数
	}
}


//训练神经元网络
void TrainNet()
{
	int i = 0;
	int m = 0;

	for (i = 0; i < TestNumber; i++)
	{
		ReadPicData(iPicture, p, i);

		for (m = 0; m < TrainTimes; m++)
		{
			CalculateNeuronValue(); //计算神经元的值	
			BackPropagation();
		}
	}
}

//总测试网络
float TestNet()
{
	int i, result = 0;
	float CorrectRate = 0; 
	Correct = 0;

	for(i = 0; i < TestNumber; i++)
	{
		ReadPicData(iPicture, p, i);

		if ((result = CalculateNetResult()) == Label)
		{	
			Correct++;
		}
	}

	return CorrectRate = (((float)Correct) / TestNumber);//识别结果
}


//读取MNIST数据集
int ReadPicData(unsigned char* data, unsigned char* p, int iNum)
{
	int iCheck = 0;

	FILE *fp_image = fopen("d:\\train-images.idx", "rb");//打开这个二进制(b)文件train-images.idx，以只读(r)的方式打开，
	FILE *fp_label = fopen("d:\\train-labels.idx", "rb");//读类标
	//FILE *fp_image = fopen("d:\\t10k_images.idx", "rb");//打开这个二进制(b)文件t10k-images.idx，以只读(r)的方式打开，
	//FILE *fp_label = fopen("d:\\t10k_labels.idx", "rb");//读类标
	if (!fp_image || !fp_label) 
		return 1;

	fseek(fp_image, 16, SEEK_SET);//从文件开始位置偏移16字节，定位像素数据开始
	fseek(fp_image, iNum * pixels*pixels, SEEK_CUR);//将读文件数据的位置从当前向后移动iNum幅图，与fread和用的效果是读第iNum - 1幅图
	iCheck = fread(data, sizeof(char), pixels * pixels, fp_image);//从文件(fp_image)中读(pixels * pixels)字节数，读一次，到data中
	if (iCheck != pixels * pixels)
	{
		printf("读图片文件失败！");
	}

	fseek(fp_label, 8, SEEK_SET);//从文件开始位置偏移8字节，定位标签数据
	fseek(fp_label, iNum, SEEK_CUR);//从文件当前位置偏移iNum字节，定位标签数据
	iCheck = fread(p, 1, 1, fp_label);
	if (iCheck != 1)
	{
		printf("读Label文件失败！");
	}

	fclose(fp_image);
	fclose(fp_label);

	return 0;
}

////////////////////***********************网络架构：输入层 ——> 卷积1层 ——> 池化2层 ——> 卷积3层 ——> 池化4层 ——> 输出层***********************////////////////////


//计算神经元的值
void CalculateNeuronValue()
{
	int i, j = 0;					
	int x, y = 0;
	int m = 0;
	int NetC1 = 0;
	int SigC1 = 0;
	int NetS2 = 0;
	int SigS2 = 0;
	int Padding_SigS2 = 0;
	int NetC3 = 0;
	int SigC3 = 0;
	int NetS4 = 0;
	int SigS4 = 0;
	int NetO = 0;
	int SigO = 0;
	int temp = 0;
	int NumK_IToC1 = 0;
	int NumK_S2ToC3 = 0;


	////////////////计算卷积1层神经元的值////////////////
	for (NetC1 = 0; NetC1 < NumKernel_IToC1 * NumConvolution1 * NumConvolution1; NetC1++)
	{
		NetConvolution1[NetC1] = 0;
	}


	//在iPicture[y][x]周围，填充2圈0——因为在图像边缘的像素在输出中采用较少，为了不丢掉了很多图像边缘的信息，在图像卷积操作之前，沿着图像边缘用0进行图像填充。
	//对于5*5的过滤器，填充宽度为2时，就可以保证输出图像和输入图像一样大。
	for(y = 0; y < Padding_pixels; y++)
	{
		for (x = 0; x < Padding_pixels; x++)
		{
			Padding_iPicture[y][x] = 0;
		}
	}


	for(y = 2,			j = 0; 
		y < pixels + 2, j < pixels; 
		y++,			j++)
	{
		for (x = 2,			i = 0; 
			x < pixels + 2, i < pixels; 
			x++,			i++)
		{
			Padding_iPicture[y][x] = iPicture[j][i];
		}
	}


	for(NumK_IToC1 = 0; NumK_IToC1 < NumKernel_IToC1; NumK_IToC1++)
	{
		for(y = 0; y < NumConvolution1; y++)
		{
			for (x = 0; x < NumConvolution1; x++)
			{
				for(j = 0; j < ConvolutionKernel; j++)
				{
					for (i = 0; i < ConvolutionKernel; i++)
					{
						NetConvolution1[NumK_IToC1 * NumConvolution1 * NumConvolution1 + (y * NumConvolution1 + x)] += 
							Padding_iPicture[y + j][x + i] * 
							WeightInputToConvolution1[NumK_IToC1 * ConvolutionKernel * ConvolutionKernel + (j * ConvolutionKernel + i)];
					}
				}
			}
		}
	}





	////////////////计算卷积1层的激活函数值////////////////
	for (SigC1 = 0; SigC1 < NumKernel_IToC1 * NumConvolution1 * NumConvolution1; SigC1++)
	{
		SigmoidConvolution1[SigC1] = 1 / (1 + exp( - NetConvolution1[SigC1])); 
	}





	////////////////计算池化2层神经元的值////////////////
	for (NetS2 = 0; NetS2 < NumKernel_IToC1 * NumSubsampling2 * NumSubsampling2; NetS2++)
	{
		NetSubsampling2[NetS2] = 0;
	}

	for(NumK_IToC1 = 0; NumK_IToC1 < NumKernel_IToC1; NumK_IToC1++)
	{
		for(y = 0; y < NumSubsampling2; y++)
		{
			for (x = 0; x < NumSubsampling2; x++)
			{
				maxSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)].Value = 0;
				maxSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)].NumK_IToC1 = 0;
				maxSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)].x = 0;
				maxSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)].y = 0;
				maxSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)].i = 0;
				maxSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)].j = 0;

				for(j = 0; j < Filter; j++)
				{
					for (i = 0; i < Filter; i++)
					{
						//求出每2*2过滤器中的最大值，组成池化层
						if (SigmoidConvolution1[NumK_IToC1 * NumConvolution1 * NumConvolution1 + (y * Filter + j) * NumConvolution1 + (x * Filter + i)] > 
							maxSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)].Value)
						{
							maxSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)].Value = 
								SigmoidConvolution1[NumK_IToC1 * NumConvolution1 * NumConvolution1 + (y * Filter + j) * NumConvolution1 + (x * Filter + i)];

							maxSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)].NumK_IToC1 = NumK_IToC1;
							maxSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)].x = x;
							maxSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)].y = y;
							maxSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)].i = i;
							maxSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)].j = j;
						}
					}
				}

				NetSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)] = 
					SigmoidConvolution1[
						maxSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)].NumK_IToC1 * NumConvolution1 * NumConvolution1 + 
							(maxSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)].y * Filter + maxSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)].j) * NumConvolution1 + 
							(maxSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)].x * Filter + maxSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)].i)
					]; 
			}
		}
	}


	////////////////计算池化2层的激活函数////////////////
	for (SigS2 = 0; SigS2 < NumKernel_IToC1 * NumSubsampling2 * NumSubsampling2; SigS2++)
	{
		SigmoidSubsampling2[SigS2] = NetSubsampling2[SigS2];
	}


	//在SigmoidSubsampling2[x]周围，填充2圈0——因为在图像边缘的像素在输出中采用较少，为了不丢掉了很多图像边缘的信息，在图像卷积操作之前，沿着图像边缘用0进行图像填充。
	//对于5*5的过滤器，填充宽度为2时，就可以保证输出图像和输入图像一样大。
	for (Padding_SigS2 = 0; Padding_SigS2 < NumKernel_IToC1 * Padding_NumSubsampling2 * Padding_NumSubsampling2; Padding_SigS2++)
	{
		Padding_SigmoidSubsampling2[Padding_SigS2] = 0;
	}

	for(NumK_IToC1 = 0; NumK_IToC1 < NumKernel_IToC1; NumK_IToC1++)
	{
		for(y = 2,					 j = 0; 
			y < NumSubsampling2 + 2, j < NumSubsampling2; 
			y++,					 j++)
		{
			for (x = 2,					 i = 0; 
				x < NumSubsampling2 + 2, i < NumSubsampling2; 
				x++,					 i++)
			{
				Padding_SigmoidSubsampling2[NumK_IToC1 * Padding_NumSubsampling2 * Padding_NumSubsampling2 + (y * Padding_NumSubsampling2 + x)] = 
					SigmoidSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (j * NumSubsampling2 + i)];
			}
		}
	}





	////////////////计算卷积3层神经元的值////////////////
	for (NetC3 = 0; NetC3 < NumKernel_S2ToC3 * NumKernel_IToC1 * NumConvolution3 * NumConvolution3; NetC3++)
	{
		NetConvolution3[NetC3] = 0;
	}

	for(NumK_S2ToC3 = 0; NumK_S2ToC3 < NumKernel_S2ToC3; NumK_S2ToC3++)
	{
		for(NumK_IToC1 = 0; NumK_IToC1 < NumKernel_IToC1; NumK_IToC1++)
		{
			for(y = 0; y < NumConvolution3; y++)
			{
				for (x = 0; x < NumConvolution3; x++)
				{
					for(j = 0; j < ConvolutionKernel; j++)
					{
						for (i = 0; i < ConvolutionKernel; i++)
						{
							NetConvolution3[NumK_S2ToC3 * NumKernel_IToC1 * NumConvolution3 * NumConvolution3 + NumK_IToC1 * NumConvolution3 * NumConvolution3 + (y * NumConvolution3 + x)] += 
								Padding_SigmoidSubsampling2[NumK_IToC1 * Padding_NumSubsampling2 * Padding_NumSubsampling2 + (y + j) * Padding_NumSubsampling2 + (x + i)] * 
								WeightSubsampling2ToConvolution3[NumK_S2ToC3 * ConvolutionKernel * ConvolutionKernel + (j * ConvolutionKernel + i)];
						}
					}
				}
			}
		}
	}


	////////////////计算卷积3层的激活函数值////////////////
	for (SigC3 = 0; SigC3 < NumKernel_S2ToC3 * NumKernel_IToC1 * NumConvolution3 * NumConvolution3; SigC3++)
	{
		SigmoidConvolution3[SigC3] = 1 / (1 + exp( - NetConvolution3[SigC3])); 
	}





	////////////////计算池化4层神经元的值////////////////
	for (NetS4 = 0; NetS4 < NumKernel_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4; NetS4++)
	{
		NetSubsampling4[NetS4] = 0;
	}

	for(NumK_S2ToC3 = 0; NumK_S2ToC3 < NumKernel_S2ToC3; NumK_S2ToC3++)
	{
		for(NumK_IToC1 = 0; NumK_IToC1 < NumKernel_IToC1; NumK_IToC1++)
		{
			for(y = 0; y < NumSubsampling4; y++)
			{
				for (x = 0; x < NumSubsampling4; x++)
				{
					maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].Value = 0;
					maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].NumK_S2ToC3 = 0;
					maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].NumK_IToC1 = 0;
					maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].x = 0;
					maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].y = 0;
					maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].i = 0;
					maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].j = 0;

					for(j = 0; j < Filter; j++)
					{
						for (i = 0; i < Filter; i++)
						{
							//求出每2*2过滤器中的最大值，组成池化层
							if (SigmoidConvolution3[NumK_S2ToC3 * NumKernel_IToC1 * NumConvolution3 * NumConvolution3 + NumK_IToC1 * NumConvolution3 * NumConvolution3 + (y * Filter + j) * NumConvolution3 + (x * Filter + i)] > 
								maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].Value)
							{
								maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].Value = 
									SigmoidConvolution3[NumK_S2ToC3 * NumKernel_IToC1 * NumConvolution3 * NumConvolution3 + NumK_IToC1 * NumConvolution3 * NumConvolution3 + ((y * Filter + j) * NumConvolution3 + (x * Filter + i))];

								maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].NumK_S2ToC3 = NumK_S2ToC3;
								maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].NumK_IToC1 = NumK_IToC1;
								maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].x = x;
								maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].y = y;
								maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].i = i;
								maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].j = j;
							}
						}
					}

					NetSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)] = 

						SigmoidConvolution3[
							(maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].NumK_S2ToC3) * NumKernel_IToC1 * NumConvolution3 * NumConvolution3 + 
								(maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].NumK_IToC1) * NumConvolution3 * NumConvolution3 + 
								(maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].y * Filter + maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].j) * NumConvolution3 + 
								(maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].x * Filter + maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].i)
						]; 
				}
			}
		}
	}





	////////////////计算池化4层的激活函数////////////////
	for (SigS4 = 0; SigS4 < NumKernel_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4; SigS4++)
	{
		SigmoidSubsampling4[SigS4] = NetSubsampling4[SigS4];
	}





	////////////////计算输出层神经元的值////////////////
	for (NetO = 0; NetO < NumOutput; NetO++)
	{
		NetOutput[NetO] = 0;
	}

	for(m = 0; m < NumOutput; m++) //输出层神经元个数为NumOutput
	{
		for(NumK_S2ToC3 = 0; NumK_S2ToC3 < NumKernel_S2ToC3; NumK_S2ToC3++)
		{
			for(NumK_IToC1 = 0; NumK_IToC1 < NumKernel_IToC1; NumK_IToC1++)
			{
				for(y = 0; y < NumSubsampling4; y++)
				{
					for (x = 0; x < NumSubsampling4; x++)
					{
						NetOutput[m] += 

							SigmoidSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)] * 

							WeightSubsampling4ToOutput[m * NumKernel_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)];
					}
				}
			}
		}
	}


	////////////////计算输出层sigmoid函数值////////////////
	for(SigO = 0; SigO < NumOutput; SigO++)
	{
		SigmoidOutput[SigO] = 1 / (1 + exp( - NetOutput[SigO]));
	}
}


//反向传播
void BackPropagation()
{
	int i, j = 0;					
	int x, y = 0;					
	int m, n = 0;
	int temp = 0;
	int c, d, s, t = 0;
	int NumK_IToC1 = 0;
	int NumK_S2ToC3 = 0;

	ClearTarget();
	target[Label] = 1; //目标输出值


	//**************************输出层误差Delta_Output**************************
	for(m = 0; m < NumOutput; m++)
	{
		//Error[m]---->SigmoidOutput[m]
		PD_ErrorToSigmoidOutput[m] = - (target[m] - SigmoidOutput[m]) / 5; 

		//SigmoidOutput[m]---->NetOutput[m]
		PD_SigmoidOutputToNetOutput[m] = SigmoidOutput[m] * (1 - SigmoidOutput[m]); 

		//Error[m]---->NetOutput[m]
		PD_ErrorToNetOutput[m] = PD_ErrorToSigmoidOutput[m] * PD_SigmoidOutputToNetOutput[m]; 

		//输出层的误差值＝总误差对输出层神经元值求偏导
		Delta_Output[m] = PD_ErrorToNetOutput[m];
	}





	//**************************输出层---->池化4层的权值更新（链式法则），全连接**************************
	//NetOutput[n]---->WeightSubsampling4ToOutput[n]
	for(m = 0; m < NumOutput; m++)
	{
		for(NumK_S2ToC3 = 0; NumK_S2ToC3 < NumKernel_S2ToC3; NumK_S2ToC3++)
		{
			for(NumK_IToC1 = 0; NumK_IToC1 < NumKernel_IToC1; NumK_IToC1++)
			{
				for(y = 0; y < NumSubsampling4; y++)
				{
					for(x = 0; x < NumSubsampling4; x++)
					{
						PD_NetOutputToWeightSubsampling4ToOutput[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)] = 

							SigmoidSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)];
					}
				}
			}
		}
	}

	for(m = 0; m < NumOutput; m++)
	{
		for(NumK_S2ToC3 = 0; NumK_S2ToC3 < NumKernel_S2ToC3; NumK_S2ToC3++)
		{
			for(NumK_IToC1 = 0; NumK_IToC1 < NumKernel_IToC1; NumK_IToC1++)
			{
				for(y = 0; y < NumSubsampling4; y++)
				{
					for(x = 0; x < NumSubsampling4; x++)
					{
						//链式法则求偏导
						PD_WeightSubsampling4ToOutput[m * NumKernel_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							(y * NumSubsampling4 + x)] = 

							Delta_Output[m] * 

							PD_NetOutputToWeightSubsampling4ToOutput[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							(y * NumSubsampling4 + x)];

						//旧权重保留OldWeightPercentage
						OldWeightSubsampling4ToOutput[m * NumKernel_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							(y * NumSubsampling4 + x)] = 

							WeightSubsampling4ToOutput[m * NumKernel_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							(y * NumSubsampling4 + x)] * 

							OldWeightPercentage; 

						//更新WeightSubsampling4ToOutput[m]的值
						WeightSubsampling4ToOutput[m * NumKernel_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							(y * NumSubsampling4 + x)] = 

							WeightSubsampling4ToOutput[m * NumKernel_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							(y * NumSubsampling4 + x)] - 

							LearningRate * 

							PD_WeightSubsampling4ToOutput[m * NumKernel_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							(y * NumSubsampling4 + x)];

						//新权重是保留NewWeightPercentage*新+OldWeightPercentage*旧
						WeightSubsampling4ToOutput[m * NumKernel_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							(y * NumSubsampling4 + x)] = 

							WeightSubsampling4ToOutput[m * NumKernel_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							(y * NumSubsampling4 + x)] * 

							NewWeightPercentage + 

							OldWeightSubsampling4ToOutput[m * NumKernel_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							(y * NumSubsampling4 + x)]; 
					}
				}
			}
		}
	}



	//**************************池化4层误差Delta_Subsampling4**************************
	for(temp = 0; temp < NumKernel_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4; temp++)
	{
		PD_ErrorToNetSubsampling4[temp] = 0;
	}

	for(NumK_S2ToC3 = 0; NumK_S2ToC3 < NumKernel_S2ToC3; NumK_S2ToC3++)
	{
		for(NumK_IToC1 = 0; NumK_IToC1 < NumKernel_IToC1; NumK_IToC1++)
		{
			for(y = 0; y < NumSubsampling4; y++)
			{
				for(x = 0; x < NumSubsampling4; x++)
				{
					for(m = 0; m < NumOutput; m++)
					{
						//NetOutput[n]---->SigmoidSubsampling4[n]
						PD_NetOutputToSigmoidSubsampling4[m * NumKernel_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)] = 

							WeightSubsampling4ToOutput[m * NumKernel_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)];

						//SigmoidSubsampling4[n]---->NetSubsampling4[n]
						PD_SigmoidSubsampling4ToNetSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)] = 1;

						//Error[n]---->NetSubsampling4[n]
						PD_ErrorToNetSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)] += 

							Delta_Output[m] * 

							PD_NetOutputToSigmoidSubsampling4[m * NumKernel_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)] * 

							PD_SigmoidSubsampling4ToNetSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + 
							NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)];

						//池化4层的误差值＝总误差对池化4层神经元值求偏导
						Delta_Subsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)] = 
							PD_ErrorToNetSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)];
					}
				}
			}
		}
	}





	//**************************把池化4层的误差值维度还原成上一层（卷积3层）的误差值维度——即除了填充每4格中的最大误差值外，其余位置补0**************************
	for(NumK_S2ToC3 = 0; NumK_S2ToC3 < NumKernel_S2ToC3; NumK_S2ToC3++)
	{
		for(NumK_IToC1 = 0; NumK_IToC1 < NumKernel_IToC1; NumK_IToC1++)
		{
			for(y = 0; y < NumSubsampling4; y++)
			{
				for (x = 0; x < NumSubsampling4; x++)
				{
					for(j = 0; j < Filter; j++)
					{
						for (i = 0; i < Filter; i++)
						{
							upsample_Delta_Subsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumConvolution3 * NumConvolution3 + 
								NumK_IToC1 * NumConvolution3 * NumConvolution3 + (y * Filter + j) * NumConvolution3 + (x * Filter + i)] = 0;

							upsample_Delta_Subsampling4[
								(maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].NumK_S2ToC3) * NumKernel_IToC1 * NumConvolution3 * NumConvolution3 + 
									(maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].NumK_IToC1) * NumConvolution3 * NumConvolution3 + 
									(maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].y * Filter + maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].j) * NumConvolution3 + 
									(maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].x * Filter + maxSubsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)].i)] = 

									Delta_Subsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumSubsampling4 * NumSubsampling4 + NumK_IToC1 * NumSubsampling4 * NumSubsampling4 + (y * NumSubsampling4 + x)];
						}
					}
				}
			}
		}
	}





	//**************************卷积3层误差Delta_Convolution3**************************
	for(NumK_S2ToC3 = 0; NumK_S2ToC3 < NumKernel_S2ToC3; NumK_S2ToC3++)
	{
		for(NumK_IToC1 = 0; NumK_IToC1 < NumKernel_IToC1; NumK_IToC1++)
		{
			for(y = 0; y < NumSubsampling4; y++)
			{
				for (x = 0; x < NumSubsampling4; x++)
				{
					for(j = 0; j < Filter; j++)
					{
						for (i = 0; i < Filter; i++)
						{
							//SigmoidConvolution3---->NetConvolution3
							PD_SigmoidConvolution3ToNetConvolution3[NumK_S2ToC3 * NumKernel_IToC1 * NumConvolution3 * NumConvolution3 + 
								NumK_IToC1 * NumConvolution3 * NumConvolution3 + (y * Filter + j) * NumConvolution3 + (x * Filter + i)] =

								SigmoidConvolution3[NumK_S2ToC3 * NumKernel_IToC1 * NumConvolution3 * NumConvolution3 + 
								NumK_IToC1 * NumConvolution3 * NumConvolution3 + (y * Filter + j) * NumConvolution3 + (x * Filter + i)] * 

								(1 - SigmoidConvolution3[NumK_S2ToC3 * NumKernel_IToC1 * NumConvolution3 * NumConvolution3 + 
								NumK_IToC1 * NumConvolution3 * NumConvolution3 + (y * Filter + j) * NumConvolution3 + (x * Filter + i)]); 

							//卷积3层(池化4层的前一层)的误差值δ＝对池化4层的误差值进行upsample还原 * 卷积3层的激活值对神经元值求偏导
							PD_ErrorToNetConvolution3[NumK_S2ToC3  * NumKernel_IToC1* NumConvolution3 * NumConvolution3 + 
								NumK_IToC1 * NumConvolution3 * NumConvolution3 + (y * Filter + j) * NumConvolution3 + (x * Filter + i)] = 

								//对池化4层的误差值进行upsample还原
								upsample_Delta_Subsampling4[NumK_S2ToC3 * NumKernel_IToC1 * NumConvolution3 * NumConvolution3 + 
								NumK_IToC1 * NumConvolution3 * NumConvolution3 + (y * Filter + j) * NumConvolution3 + (x * Filter + i)] * 

								//卷积3层的激活值对神经元值求偏导
								PD_SigmoidConvolution3ToNetConvolution3[NumK_S2ToC3 * NumKernel_IToC1 * NumConvolution3 * NumConvolution3 + 
								NumK_IToC1 * NumConvolution3 * NumConvolution3 + (y * Filter + j) * NumConvolution3 + (x * Filter + i)];

							//卷积3层误差Delta_Convolution3
							Delta_Convolution3[NumK_S2ToC3 * NumKernel_IToC1 * NumConvolution3 * NumConvolution3 + 
								NumK_IToC1 * NumConvolution3 * NumConvolution3 + (y * Filter + j) * NumConvolution3 + (x * Filter + i)] = 

								PD_ErrorToNetConvolution3[NumK_S2ToC3 * NumKernel_IToC1 * NumConvolution3 * NumConvolution3 + 
								NumK_IToC1 * NumConvolution3 * NumConvolution3 + (y * Filter + j) * NumConvolution3 + (x * Filter + i)];
						}
					}
				}
			}
		}
	}




	//**************************卷积3层---->池化2层的权值更新**************************
	for (temp = 0; temp < NumKernel_S2ToC3 * ConvolutionKernel * ConvolutionKernel; temp++)
	{
		PD_WeightSubsampling2ToConvolution3[temp] = 0;
	}


	for(NumK_S2ToC3 = 0; NumK_S2ToC3 < NumKernel_S2ToC3; NumK_S2ToC3++)
	{
		for(NumK_IToC1 = 0; NumK_IToC1 < NumKernel_IToC1; NumK_IToC1++)
		{
			for(t = 0; t < ConvolutionKernel; t++)
			{
				for (s = 0; s < ConvolutionKernel; s++)
				{
					for(d = 0; d < NumConvolution3; d++)
					{
						for (c = 0; c < NumConvolution3; c++)
						{
							PD_WeightSubsampling2ToConvolution3[NumK_S2ToC3 * ConvolutionKernel * ConvolutionKernel + (t * ConvolutionKernel + s)] += 
								(Padding_SigmoidSubsampling2[NumK_IToC1 * Padding_NumSubsampling2 * Padding_NumSubsampling2 + (t + d) * Padding_NumSubsampling2 + (s + c)] * 
								Delta_Convolution3[NumK_S2ToC3 * NumKernel_IToC1 * NumConvolution3 * NumConvolution3 + NumK_IToC1 * NumConvolution3 * NumConvolution3 + (d * NumConvolution3 + c)]) / NumKernel_S2ToC3;
						}
					}
				}
			}
		}
	}


	for(NumK_S2ToC3 = 0; NumK_S2ToC3 < NumKernel_S2ToC3; NumK_S2ToC3++)
	{
		for(NumK_IToC1 = 0; NumK_IToC1 < NumKernel_IToC1; NumK_IToC1++)
		{
			for(t = 0; t < ConvolutionKernel; t++)
			{
				for (s = 0; s < ConvolutionKernel; s++)
				{
					//旧权重保留0.9
					OldWeightSubsampling2ToConvolution3[NumK_S2ToC3 * ConvolutionKernel * ConvolutionKernel + (t * ConvolutionKernel + s)] = 
						WeightSubsampling2ToConvolution3[NumK_S2ToC3 * ConvolutionKernel * ConvolutionKernel + (t * ConvolutionKernel + s)] * OldWeightPercentage; 

					////////////更新WeightSubsampling2ToConvolution3[m]的值
					WeightSubsampling2ToConvolution3[NumK_S2ToC3 * ConvolutionKernel * ConvolutionKernel + (t * ConvolutionKernel + s)] = 
						WeightSubsampling2ToConvolution3[NumK_S2ToC3 * ConvolutionKernel * ConvolutionKernel + (t * ConvolutionKernel + s)] - 
						LearningRate * PD_WeightSubsampling2ToConvolution3[NumK_S2ToC3 * ConvolutionKernel * ConvolutionKernel + (t * ConvolutionKernel + s)];

					//新权重是保留0.1*新+0.9*旧
					WeightSubsampling2ToConvolution3[NumK_S2ToC3 * ConvolutionKernel * ConvolutionKernel + (t * ConvolutionKernel + s)] = 
						WeightSubsampling2ToConvolution3[NumK_S2ToC3 * ConvolutionKernel * ConvolutionKernel + (t * ConvolutionKernel + s)] * NewWeightPercentage + 
						OldWeightSubsampling2ToConvolution3[NumK_S2ToC3 * ConvolutionKernel * ConvolutionKernel + (t * ConvolutionKernel + s)]; 
				}
			}
		}
	}




	////**************************【池化2层-卷积3层】卷积核翻转180度**************************
	for(NumK_S2ToC3 = 0; NumK_S2ToC3 < NumKernel_S2ToC3; NumK_S2ToC3++)
	{
		for(m = 0,											n = ConvolutionKernel * ConvolutionKernel - 1; 
			m < ConvolutionKernel * ConvolutionKernel, -1 < n ; 
			m++,											n--)
		{
			WeightRot180Subsampling2ToConvolution3[NumKernel_S2ToC3 * ConvolutionKernel * ConvolutionKernel + n] = 
				WeightSubsampling2ToConvolution3[NumKernel_S2ToC3 * ConvolutionKernel * ConvolutionKernel + m];
		}
	}




	//在每张图片卷积3层的误差值矩阵周围，都填充4圈0，为了符合梯度计算：∇ Delta_Subsampling2 = Padding_δConvolution3 * rot180W[S2_C3]
	for(NumK_S2ToC3 = 0; NumK_S2ToC3 < NumKernel_S2ToC3; NumK_S2ToC3++)
	{
		for(NumK_IToC1 = 0; NumK_IToC1 < NumKernel_IToC1; NumK_IToC1++)
		{
			for(y = 0; y < NumConvolution3 + 2 * (ConvolutionKernel - 1); y++)
			{
				for (x = 0; x < NumConvolution3 + 2 * (ConvolutionKernel - 1); x++)
				{
					Padding_Delta_Convolution3[NumK_S2ToC3 * NumKernel_IToC1 * (NumConvolution3 + 2 * (ConvolutionKernel - 1)) * (NumConvolution3 + 2 * (ConvolutionKernel - 1)) + 
						NumK_IToC1 * (NumConvolution3 + 2 * (ConvolutionKernel - 1)) * (NumConvolution3 + 2 * (ConvolutionKernel - 1)) + 
						y * (NumConvolution3 + 2 * (ConvolutionKernel - 1)) + x] = 0;
				}
			}
		}
	}


	for(NumK_S2ToC3 = 0; NumK_S2ToC3 < NumKernel_S2ToC3; NumK_S2ToC3++)
	{
		for(NumK_IToC1 = 0; NumK_IToC1 < NumKernel_IToC1; NumK_IToC1++)
		{
			for(y = ConvolutionKernel - 1,					 j = 0; 
				y < NumConvolution3 + ConvolutionKernel - 1, j < NumConvolution3; 
				y++,										 j++)
			{
				for (x = ConvolutionKernel - 1,					  i = 0; 
					x < NumConvolution3 + ConvolutionKernel - 1, i < NumConvolution3; 
					x++,										  i++)
				{
					Padding_Delta_Convolution3[NumK_S2ToC3 * NumKernel_IToC1 * (NumConvolution3 + 2 * (ConvolutionKernel - 1)) * (NumConvolution3 + 2 * (ConvolutionKernel - 1)) + 
						NumK_IToC1 * (NumConvolution3 + 2 * (ConvolutionKernel - 1)) * (NumConvolution3 + 2 * (ConvolutionKernel - 1)) + 
						y * (NumConvolution3 + 2 * (ConvolutionKernel - 1)) + x] = 

						Delta_Convolution3[NumK_S2ToC3 * NumKernel_IToC1 * NumConvolution3 * NumConvolution3 + NumK_IToC1 * NumConvolution3 * NumConvolution3 + (j * NumConvolution3 + i)]; 
				}
			}
		}
	}




	//**************************池化2层---->卷积1层（未涉及权值）从缩小后的误差δ，还原前一次较大区域对应的误差**************************
	for(temp = 0; temp < NumKernel_IToC1 * NumSubsampling2 * NumSubsampling2; temp++)
	{
		PD_ErrorToNetSubsampling2[temp] = 0;
	}


	//****************池化2层误差Delta_Subsampling2****************
	for(NumK_S2ToC3 = 0; NumK_S2ToC3 < NumKernel_S2ToC3; NumK_S2ToC3++)
	{
		for(NumK_IToC1 = 0; NumK_IToC1 < NumKernel_IToC1; NumK_IToC1++)
		{
			for(y = 0; y < NumSubsampling2; y++)
			{
				for (x = 0; x < NumSubsampling2; x++)
				{
					for(j = 0; j < ConvolutionKernel; j++)
					{
						for (i = 0; i < ConvolutionKernel; i++)
						{
							//SigmoidSubsampling2[n]---->NetSubsampling2[n]
							PD_SigmoidSubsampling2ToNetSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)] = 1;

							//Error[n]---->NetSubsampling2[n](=∇ Delta_Subsampling2)
							PD_ErrorToNetSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)] += 

								//在【Delta_Convolution3】矩阵周围填充4圈0，得到【Padding_Delta_Convolution3】
								(Padding_Delta_Convolution3[NumK_S2ToC3 * NumKernel_IToC1 * (NumConvolution3 + 2 * (ConvolutionKernel - 1)) * (NumConvolution3 + 2 * (ConvolutionKernel - 1)) + 
								NumK_IToC1 * (NumConvolution3 + 2 * (ConvolutionKernel - 1)) * (NumConvolution3 + 2 * (ConvolutionKernel - 1)) + 
								(y + j) * (NumConvolution3 + 2 * (ConvolutionKernel - 1)) + (x + i)] *	

								//【池化2层-卷积3层】卷积核翻转180°
								WeightRot180Subsampling2ToConvolution3[NumK_S2ToC3 * ConvolutionKernel * ConvolutionKernel + (j * ConvolutionKernel + i)] *				

								//SigmoidSubsampling2[n]---->NetSubsampling2[n]求偏导
								PD_SigmoidSubsampling2ToNetSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)]) / NumKernel_S2ToC3;			

							//池化2层的误差值＝总误差对池化2层神经元值求偏导
							Delta_Subsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)] = 
								PD_ErrorToNetSubsampling2[NumK_IToC1 * NumSubsampling2 * NumSubsampling2 + (y * NumSubsampling2 + x)];
						}
					}
				}
			}
		}
	}




	//**************************卷积1层误差Delta_Convolution1**************************
	for(NumK_IToC1 = 0; NumK_IToC1 < NumKernel_IToC1; NumK_IToC1++)
	{
		for(y = 0; y < NumSubsampling2; y++)
		{
			for (x = 0; x < NumSubsampling2; x++)
			{
				for(j = 0; j < Filter; j++)
				{
					for (i = 0; i < Filter; i++)
					{
						//SigmoidConvolution1---->NetConvolution1
						PD_SigmoidConvolution1ToNetConvolution1[NumK_IToC1 * NumConvolution1 * NumConvolution1 + (y * Filter + j) * NumConvolution1 + (x * Filter + i)] =
							SigmoidConvolution1[NumK_IToC1 * NumConvolution1 * NumConvolution1 + (y * Filter + j) * NumConvolution1 + (x * Filter + i)] * 
							(1 - SigmoidConvolution1[NumK_IToC1 * NumConvolution1 * NumConvolution1 + (y * Filter + j) * NumConvolution1 + (x * Filter + i)]); 

						//卷积1层(池化2层的前一层)的误差值δ＝对池化2层的误差值进行upsample还原 * 卷积1层的激活值对神经元值求偏导
						PD_ErrorToNetConvolution1[NumK_IToC1 * NumConvolution1 * NumConvolution1 + (y * Filter + j) * NumConvolution1 + (x * Filter + i)] = 

							//对池化2层的误差值进行upsample还原
							upsample_Delta_Subsampling2[NumK_IToC1 * NumConvolution1 * NumConvolution1 + (y * Filter + j) * NumConvolution1 + (x * Filter + i)] * 

							//SigmoidConvolution1---->NetConvolution1
							PD_SigmoidConvolution1ToNetConvolution1[NumK_IToC1 * NumConvolution1 * NumConvolution1 + (y * Filter + j) * NumConvolution1 + (x * Filter + i)];

						//卷积1层误差Delta_Convolution1
						Delta_Convolution1[NumK_IToC1 * NumConvolution1 * NumConvolution1 + (y * Filter + j) * NumConvolution1 + (x * Filter + i)] = 
							PD_ErrorToNetConvolution1[NumK_IToC1 * NumConvolution1 * NumConvolution1 + (y * Filter + j) * NumConvolution1 + (x * Filter + i)];
					}
				}
			}
		}
	}





	//**************************卷积1层---->输入层（卷积核）的权值更新**************************
	for (temp = 0; temp < NumKernel_IToC1 * ConvolutionKernel * ConvolutionKernel; temp++) 
	{
		PD_WeightInputToConvolution1[temp] = 0;
	}

	for(NumK_IToC1 = 0; NumK_IToC1 < NumKernel_IToC1; NumK_IToC1++)
	{
		for(t = 0; t < ConvolutionKernel; t++)
		{
			for (s = 0; s < ConvolutionKernel; s++)
			{
				for(d = 0; d < NumConvolution1; d++)
				{
					for (c = 0; c < NumConvolution1; c++)
					{
						PD_WeightInputToConvolution1[NumK_IToC1 * ConvolutionKernel * ConvolutionKernel + (t * ConvolutionKernel + s)] += 
							Padding_iPicture[t + d][s + c] * 
							Delta_Convolution1[NumK_IToC1 * NumConvolution1 * NumConvolution1 + (d * NumConvolution1 + c)];
					}
				}
			}
		}
	}

	for(NumK_IToC1 = 0; NumK_IToC1 < NumKernel_IToC1; NumK_IToC1++)
	{
		for(t = 0; t < ConvolutionKernel; t++)
		{
			for (s = 0; s < ConvolutionKernel; s++)
			{
				//旧权重保留OldWeightPercentage
				OldWeightInputToConvolution1[NumK_IToC1 * ConvolutionKernel * ConvolutionKernel + (t * ConvolutionKernel + s)] = 
					WeightInputToConvolution1[NumK_IToC1 * ConvolutionKernel * ConvolutionKernel + (t * ConvolutionKernel + s)] * OldWeightPercentage; 

				//更新WeightInputToConvolution1[m]的值
				WeightInputToConvolution1[NumK_IToC1 * ConvolutionKernel * ConvolutionKernel + (t * ConvolutionKernel + s)] = 
					WeightInputToConvolution1[NumK_IToC1 * ConvolutionKernel * ConvolutionKernel + (t * ConvolutionKernel + s)] - 
					LearningRate * PD_WeightInputToConvolution1[NumK_IToC1 * ConvolutionKernel * ConvolutionKernel + (t * ConvolutionKernel + s)];

				//新权重是保留NewWeightPercentage*新+OldWeightPercentage*旧
				WeightInputToConvolution1[NumK_IToC1 * ConvolutionKernel * ConvolutionKernel + (t * ConvolutionKernel + s)] = 
					WeightInputToConvolution1[NumK_IToC1 * ConvolutionKernel * ConvolutionKernel + (t * ConvolutionKernel + s)] * NewWeightPercentage + 
					OldWeightInputToConvolution1[NumK_IToC1 * ConvolutionKernel * ConvolutionKernel + (t * ConvolutionKernel + s)]; 
			}
		}
	}
}


//清理Target[]
void ClearTarget()
{
	int i = 0;

	for (i = 0; i < NumOutput; i++)
	{
		target[i] = 0;
	}
}

//计算网络识别结果
int CalculateNetResult()
{
	int i = 0, result = 0; 
	CalculateNeuronValue(); //计算神经元的值	

	for (i = 1; i < NumOutput; i++)
	{
		if (SigmoidOutput[result] < SigmoidOutput[i])
		{
			result = i;
		}
	}

	return result;//识别结果
}