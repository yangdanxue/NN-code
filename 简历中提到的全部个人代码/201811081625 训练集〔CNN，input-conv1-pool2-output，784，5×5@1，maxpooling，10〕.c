#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


#define pixels						28		//图片像素是28*28
#define Padding_pixels				32		//填充后图片像素是32*32
#define ConvolutionKernel			5		//卷积核，大小是5*5
#define NeuronNumConvolution1       28		//卷积1层神经元个数
#define NeuronNumSubsampling2       14		//池化2层神经元个数
#define Filter						2		//池化层过滤器大小
#define NeuronNumOutput				10   	//输出层神经元个数

#define TrainTimes		1	    //训练次数
#define TestNumber		60000	//总测试图片的数量
#define Memory          1       //记住池化层中每个值，在上一卷积层中的具体坐标


int ReadPicData(unsigned char* data, int iNum); //读图片数据文件

void WriteWeightInputToConvolution1Data();			//保留当前调好的[输入层-卷积1层]权重
void WriteWeightSubsampling2ToOutputData();			//保留当前调好的[池化2层-输出层]权重

void ReadWeightInputToConvolution1Data();			//使用保存的[输入层-卷积1层]权重
void ReadWeightSubsampling2ToOutputData();			//使用保存的[池化2层-输出层]权重

void InitializationWeightInputToConvolution1();		//初始化[输入层-卷积1层]权重
void InitializationWeightSubsampling2ToOutput();	//随机初始化[池化2层-输出层]权重

int CalculateNetResult();		//计算网络输出结果
float TestNet();				//测试网络
void TrainNet();				//选择测试或训练

void CalculateNeuronValue();	//计算各个神经元的值
void BackPropagation();			//反向传播法更新权值
void ClearTarget();				//清理Target[]
void MaxPool();					//最大池化

unsigned char iPicture[pixels][pixels] = {0};							//导入的MNIST数据库图片数组
unsigned char Padding_iPicture[Padding_pixels][Padding_pixels] = {0};	//对于5*5的过滤器，在导入的MNIST数据库图片数组iPicture[y][x]周围，填充宽度为2的0时，就可以保证输出图像和输入图像一样大
unsigned char Label;													//导入的MNIST数据库标签
unsigned char* p = &Label;

double WeightInputToConvolution1[ConvolutionKernel * ConvolutionKernel] = {0};									//【输入层-卷积1层】卷积核（权重）
double WeightRot180InputToConvolution1[ConvolutionKernel * ConvolutionKernel] = {0};							//【输入层-卷积1层】卷积核翻转180°（权重）
double WeightSubsampling2ToOutput[NeuronNumSubsampling2 * NeuronNumSubsampling2 * NeuronNumOutput] = {0};		//【池化2层-输出层】权重

double OldWeightInputToConvolution1[ConvolutionKernel * ConvolutionKernel] = {0};								//【输入层-卷积1层】卷积核（为了反向传播后新旧权重各取一定比例）
double OldWeightSubsampling2ToOutput[NeuronNumSubsampling2 * NeuronNumSubsampling2 * NeuronNumOutput] = {0};	//【池化2层-输出层】权重（为了反向传播后新旧权重各取一定比例）

double NetConvolution1[NeuronNumConvolution1 * NeuronNumConvolution1] = {0};		//卷积1层神经元的值
double SigmoidConvolution1[NeuronNumConvolution1 * NeuronNumConvolution1] = {0};	//卷积1层激活函数值（Sigmoid函数）

double NetSubsampling2[NeuronNumSubsampling2 * NeuronNumSubsampling2] = {0};        //池化2层神经元的值
double SigmoidSubsampling2[NeuronNumSubsampling2 * NeuronNumSubsampling2] = {0};	//池化2层激活函数值（Sigmoid函数）
double NetOutput[NeuronNumOutput] = {0};											//输出层神经元的值
double SigmoidOutput[NeuronNumOutput] = {0};										//输出层激活函数值（Sigmoid函数）

int target[NeuronNumOutput] = {0};					//目标输出值

//输出层---->池化2层的权值更新
double PD_WeightSubsampling2ToOutput[NeuronNumSubsampling2 * NeuronNumSubsampling2 * NeuronNumOutput] = {0};
double PD_ErrorToSigmoidOutput[NeuronNumOutput] = {0};
double PD_SigmoidOutputToNetsumOutput[NeuronNumOutput] = {0};
double PD_ErrorToNetsumOutput[NeuronNumOutput] = {0};
double Delta_NetsumOutput[NeuronNumOutput] = {0};
double PD_NetsumOutputToWeightSubsampling2ToOutput[NeuronNumOutput * NeuronNumSubsampling2 * NeuronNumSubsampling2] = {0};

//池化2层---->卷积1层的权值更新
double PD_NetsumOutputToSigmoidSubsampling2[NeuronNumOutput * NeuronNumSubsampling2 * NeuronNumSubsampling2] = {0};
double PD_SigmoidSubsampling2ToNetSubsampling2[NeuronNumSubsampling2 * NeuronNumSubsampling2] = {0};
double Delta_Subsampling2[NeuronNumSubsampling2 * NeuronNumSubsampling2] = {0};
double PD_ErrorToNetSubsampling2[NeuronNumSubsampling2 * NeuronNumSubsampling2] = {0};

double upsample_Delta_Subsampling2[NeuronNumConvolution1 * NeuronNumConvolution1] = {0};

double PD_SigmoidConvolution1ToNetConvolution1[NeuronNumConvolution1 * NeuronNumConvolution1] = {0};
double PD_ErrorToNetConvolution1[NeuronNumConvolution1 * NeuronNumConvolution1] = {0};
double Delta_Convolution1[NeuronNumConvolution1 * NeuronNumConvolution1] = {0};

//卷积1层---->输入层的权值更新
double PD_WeightInputToConvolution1[ConvolutionKernel * ConvolutionKernel] = {0};


//设定最大池化值的坐标结构体数组
struct MAXPool 
{
	double Value;
	int x;
	int y;
	int i;
	int j;
};
struct MAXPool max[NeuronNumSubsampling2 * NeuronNumSubsampling2] = {0};


float LearningRate = 0.05;	//学习速率

int Correct = 0;//识别正确数


void main()
{
	float CorrectRate;
	int MaxCorrect = 0;
	int i = 0;

	ReadWeightInputToConvolution1Data();
	ReadWeightSubsampling2ToOutputData();

	//InitializationWeightInputToConvolution1();
	//InitializationWeightSubsampling2ToOutput();

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
	//		WriteWeightSubsampling2ToOutputData();
	//	}
	//}
}


//读文件——使用保存的[输入层-卷积1层]权重
void ReadWeightInputToConvolution1Data()
{
	int iNum = 0; //神经元个数

	FILE *fpReadItoC1 = fopen("d:\\WeightItoC1Data201811081625.wet","rb");

	if(fpReadItoC1 == NULL)
	{
		printf("WeightItoC1Data201811081625.wet文件不存在！\n");
		return;
	}

	//下面是读数据，将读到的数据存到数组WeightInputToConvolution1[NeuronNumConvolution1]中
	iNum = fread(WeightInputToConvolution1, sizeof(double), ConvolutionKernel * ConvolutionKernel, fpReadItoC1);
	if(iNum != ConvolutionKernel * ConvolutionKernel)
	{
		printf("读数据存到数组WeightInputToConvolution1[ConvolutionKernel * ConvolutionKernel]错误！！！\n");
	}
	printf("读[输入层-卷积1层]间的权重数据成功\n");

	fclose(fpReadItoC1);

	return;
}


//读文件——使用保存的[池化2层-输出层]权重
void ReadWeightSubsampling2ToOutputData()
{
	int iNum = 0; //输入层-隐藏层间的神经元个数

	FILE *fpReadS2toO = fopen("d:\\WeightS2toOData201811081625.wet","rb");

	if(fpReadS2toO == NULL)
	{
		printf("WeightS2toOData201811081625.wet文件不存在！\n");
		return;
	}

	//下面是读数据，将读到的数据存到数组WeightSubsampling2ToOutput[NeuronNumSubsampling2 * NeuronNumOutput]中
	iNum = fread(WeightSubsampling2ToOutput, sizeof(double), NeuronNumSubsampling2 * NeuronNumSubsampling2 * NeuronNumOutput, fpReadS2toO);
	if(iNum != NeuronNumSubsampling2 * NeuronNumSubsampling2 * NeuronNumOutput)
	{
		printf("读数据存到数组WeightSubsampling2ToOutput[NeuronNumSubsampling2 * NeuronNumSubsampling2 * NeuronNumOutput]错误！！！\n");
	}
	printf("读[池化2层-输出层]间的权重数据成功\n");

	fclose(fpReadS2toO);

	return;
}			


//写文件——保留当前调好的[输入层-卷积1层]权重
void WriteWeightInputToConvolution1Data()
{
	int iNum = 0;
	FILE *fpWriteInputToConvolution1 = fopen("d:\\WeightItoC1Data201811081625.wet","wb+");

	if(fpWriteInputToConvolution1 == NULL)
	{
		printf("创建WeightItoC1Data201811081625.wet文件失败！\n");
		return;
	}

	//将输入层-卷积1层间的权重写入到WeightItoC1Data201811081625.wet文件中
	iNum = fwrite(WeightInputToConvolution1, sizeof(double), ConvolutionKernel * ConvolutionKernel, fpWriteInputToConvolution1);
	if (iNum != ConvolutionKernel * ConvolutionKernel)
	{
		printf("写WeightItoC1Data201811081625.wet文件失败！");
	}
	printf("所有输入层-卷积1层间的权重数据已经保存\n");

	fclose(fpWriteInputToConvolution1);
}


//写文件——保留当前调好的[池化2层-输出层]权重
void WriteWeightSubsampling2ToOutputData()
{
	int iNum = 0;
	FILE *fpWriteSubsampling2ToOutput = fopen("d:\\WeightS2toOData201811081625.wet","wb+");

	if(fpWriteSubsampling2ToOutput == NULL)
	{
		printf("创建WeightS2toOData201811081625.wet文件失败！\n");
		return;
	}

	//将池化2层-输出层间的权重写入到WeightS2toOData201811081625.wet文件中
	iNum = fwrite(WeightSubsampling2ToOutput, sizeof(double), NeuronNumSubsampling2 * NeuronNumSubsampling2 * NeuronNumOutput, fpWriteSubsampling2ToOutput);
	if (iNum != NeuronNumSubsampling2 * NeuronNumSubsampling2 * NeuronNumOutput)
	{		
		printf("写WeightS2toOData201811081625.wet文件失败！");
	}
	printf("所有池化2层-输出层间的权重数据已经保存\n");

	fclose(fpWriteSubsampling2ToOutput);
}			


//随机初始化[输入层-卷积1层]权重
void InitializationWeightInputToConvolution1()
{
	int m = 0;
	int n = 0;

	for(m = 0; m < ConvolutionKernel * ConvolutionKernel; m++)
	{
		//WeightInputToConvolution1[m] = 0.5/2000; //【输入层-卷积1层】之间任意2个神经元间的权重W
		WeightInputToConvolution1[m] = (rand()/(double)(RAND_MAX))/1000; //【输入层-卷积1层】之间任意2个神经元间的权重W
		//WeightInputToConvolution1[m] = 0; //【输入层-卷积1层】之间任意2个神经元间的权重W
		//WeightInputToConvolution1[m] = -0.008001877380; //89.9467%版权重的中位数
		//WeightInputToConvolution1[m] = 0.078278135; //89.9467%版权重的平均数
	}

	//【输入层-卷积1层】权重翻转180度
	//for(m = 0, n = ConvolutionKernel * ConvolutionKernel - 1; m < ConvolutionKernel * ConvolutionKernel, -1 < n ; m++, n--)
	//{
	//	WeightRot180InputToConvolution1[n] = WeightInputToConvolution1[m];
	//}
}


//随机初始化[池化2层-输出层]权重
void InitializationWeightSubsampling2ToOutput()
{
	int m = 0;

	for(m = 0; m < NeuronNumSubsampling2 * NeuronNumSubsampling2 * NeuronNumOutput; m++)
	{
		//WeightSubsampling2ToOutput[m] = 0; //【池化2层-输出层】之间任意2个神经元间的权重W
		WeightSubsampling2ToOutput[m] = (rand()/(double)(RAND_MAX))/200; //【池化2层-输出层】之间任意2个神经元间的权重W
		//WeightSubsampling2ToOutput[m] = -0.065319700309; ///89.9467%版权重的中位数
		//WeightSubsampling2ToOutput[m] = -0.060853877; ///89.9467%版权重的平均数
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

//计算神经元的值
void CalculateNeuronValue()
{
	int i, j = 0;					
	int x, y = 0;
	int n = 0;
	int temp = 0;


	//*********************************************网络架构：输入层 ——> 卷积1层 ——> 池化2层 ——> 输出层*********************************************//

	////////////////计算卷积1层神经元的值////////////////
	for(y = 0; y < NeuronNumConvolution1; y++)
	{
		for (x = 0; x < NeuronNumConvolution1; x++)
		{
			NetConvolution1[y * NeuronNumConvolution1 + x] = 0;
		}
	}


	//在iPicture[y][x]周围，填充2圈0——因为在图像边缘的像素在输出中采用较少，为了不丢掉了很多图像边缘的信息，在图像卷积操作之前，沿着图像边缘用0进行图像填充。
	//对于5*5的过滤器，填充宽度为2时，就可以保证输出图像和输入图像一样大。
	for(y = 0; y < Padding_pixels; y++)
	{
		for (x = 0; x < Padding_pixels; x++)
		{
			Padding_iPicture[y][x] = 0; //20181009改，增加代码
		}
	}

	for(y = 2, j = 0; y < pixels + 2, j < pixels; y++, j++)
	{
		for (x = 2, i = 0; x < pixels + 2, i < pixels; x++, i++)
		{
			Padding_iPicture[y][x] = iPicture[j][i]; //201810092216改
		}
	}


	for(y = 0; y < NeuronNumConvolution1; y++)
	{
		for (x = 0; x < NeuronNumConvolution1; x++)
		{
			for(j = 0; j < ConvolutionKernel; j++)
			{
				for (i = 0; i < ConvolutionKernel; i++)
				{
					//20181009改，将iPicture[y + j][x + i]改为Padding_iPicture[y + j][x + i]
					NetConvolution1[y * NeuronNumConvolution1 + x] += Padding_iPicture[y + j][x + i] * WeightInputToConvolution1[j * ConvolutionKernel + i];
				}
			}
		}
	}


	////////////////计算卷积1层的激活函数值////////////////
	for(y = 0; y < NeuronNumConvolution1; y++)
	{
		for (x = 0; x < NeuronNumConvolution1; x++)
		{
			SigmoidConvolution1[y * NeuronNumConvolution1 + x] = 1 / (1 + exp( - NetConvolution1[y * NeuronNumConvolution1 + x])); 
		}
	}


	////////////////计算池化2层神经元的值////////////////
	for (temp = 0; temp < NeuronNumSubsampling2 * NeuronNumSubsampling2; temp++)
	{
		NetSubsampling2[temp] = 0;
	}

	for(y = 0; y < NeuronNumSubsampling2; y++)
	{
		for (x = 0; x < NeuronNumSubsampling2; x++)
		{
			max[y * NeuronNumSubsampling2 + x].Value = 0;
			max[y * NeuronNumSubsampling2 + x].x = 0;
			max[y * NeuronNumSubsampling2 + x].y = 0;
			max[y * NeuronNumSubsampling2 + x].i = 0;
			max[y * NeuronNumSubsampling2 + x].j = 0;

			for(j = 0; j < Filter; j++)
			{
				for (i = 0; i < Filter; i++)
				{
					//求出每4*4过滤器中的最大值，组成池化层
					if (SigmoidConvolution1[(y * Filter + j) * NeuronNumConvolution1 + (x * Filter + i)] > max[y * NeuronNumSubsampling2 + x].Value)
					{
						max[y * NeuronNumSubsampling2 + x].Value = SigmoidConvolution1[(y * Filter + j) * NeuronNumConvolution1 + (x * Filter + i)];
						max[y * NeuronNumSubsampling2 + x].x = x;
						max[y * NeuronNumSubsampling2 + x].y = y;
						max[y * NeuronNumSubsampling2 + x].i = i;
						max[y * NeuronNumSubsampling2 + x].j = j;
					}
				}
			}
			NetSubsampling2[y * NeuronNumSubsampling2 + x] = SigmoidConvolution1[(max[y * NeuronNumSubsampling2 + x].y * Filter + max[y * NeuronNumSubsampling2 + x].j) * NeuronNumConvolution1 + (max[y * NeuronNumSubsampling2 + x].x * Filter + max[y * NeuronNumSubsampling2 + x].i)]; 
		}
	}


	////////////////计算池化2层的激活函数////////////////
	for(y = 0; y < NeuronNumSubsampling2; y++)
	{
		for (x = 0; x < NeuronNumSubsampling2; x++)
		{
			SigmoidSubsampling2[y * NeuronNumSubsampling2 + x] = NetSubsampling2[y * NeuronNumSubsampling2 + x];
		}
	}


	////////////////计算输出层神经元的值////////////////
	for (n = 0; n < NeuronNumOutput; n++)
	{
		NetOutput[n] = 0;
	}

	for(n = 0; n < NeuronNumOutput; n++) //输出层神经元个数为NeuronNumOutput
	{
		for(y = 0; y < NeuronNumSubsampling2; y++)
		{
			for (x = 0; x < NeuronNumSubsampling2; x++)
			{
				NetOutput[n] += 
					SigmoidSubsampling2[y * NeuronNumSubsampling2 + x] * 
					WeightSubsampling2ToOutput[n * NeuronNumSubsampling2 * NeuronNumSubsampling2 + (y * NeuronNumSubsampling2 + x)]; //池化2层和输出层间的神经元要一一连（全排列）
			}
		}
	}


	////////////////计算输出层sigmoid函数值////////////////
	for(n = 0; n < NeuronNumOutput; n++)
	{
		SigmoidOutput[n] = 1 / (1 + exp( - NetOutput[n]));
	}
}


//反向传播
void BackPropagation()
{
	int i, j = 0;					
	int x, y = 0;					
	int m = 0;
	int temp = 0;
	int c, d, s, t = 0;

	ClearTarget();
	target[Label] = 1; //目标输出值


	//**************************输出层---->池化2层的权值更新（链式法则），全连接**************************
	//输出层误差Delta_NetOutput
	for(m = 0; m < NeuronNumOutput; m++)
	{
		////////////Error[m]---->SigmoidOutput[m]
		PD_ErrorToSigmoidOutput[m] = - (target[m] - SigmoidOutput[m]) / 5; 

		////////////SigmoidOutput[m]---->NetOutput[m]
		PD_SigmoidOutputToNetsumOutput[m] = SigmoidOutput[m] * (1 - SigmoidOutput[m]); 

		////////////Error[m]---->NetOutput[m]
		PD_ErrorToNetsumOutput[m] = PD_ErrorToSigmoidOutput[m] * PD_SigmoidOutputToNetsumOutput[m]; 

		//输出层的误差值＝总误差对输出层神经元值求偏导
		Delta_NetsumOutput[m] = PD_ErrorToNetsumOutput[m];
	}

	////////////NetOutput[n]---->WeightSubsampling2ToOutput[n]
	for(m = 0; m < NeuronNumOutput; m++)
	{
		for(y = 0; y < NeuronNumSubsampling2; y++)
		{
			for(x = 0; x < NeuronNumSubsampling2; x++)
			{
				PD_NetsumOutputToWeightSubsampling2ToOutput[m * NeuronNumSubsampling2 * NeuronNumSubsampling2 + (y * NeuronNumSubsampling2 + x)] = SigmoidSubsampling2[y * NeuronNumSubsampling2 + x];
			}
		}
	}

	for(m = 0; m < NeuronNumOutput; m++)
	{
		for(y = 0; y < NeuronNumSubsampling2; y++)
		{
			for(x = 0; x < NeuronNumSubsampling2; x++)
			{
				////////////链式法则求偏导
				PD_WeightSubsampling2ToOutput[m * NeuronNumSubsampling2 * NeuronNumSubsampling2 + (y * NeuronNumSubsampling2 + x)] = 
					Delta_NetsumOutput[m] * PD_NetsumOutputToWeightSubsampling2ToOutput[m * NeuronNumSubsampling2 * NeuronNumSubsampling2 + (y * NeuronNumSubsampling2 + x)];

				//旧权重保留0.9
				OldWeightSubsampling2ToOutput[m * NeuronNumSubsampling2 * NeuronNumSubsampling2 + (y * NeuronNumSubsampling2 + x)] = 
					WeightSubsampling2ToOutput[m * NeuronNumSubsampling2 * NeuronNumSubsampling2 + (y * NeuronNumSubsampling2 + x)] * 0.9; 

				////////////更新WeightSubsampling2ToOutput[m]的值
				WeightSubsampling2ToOutput[m * NeuronNumSubsampling2 * NeuronNumSubsampling2 + (y * NeuronNumSubsampling2 + x)] = 
					WeightSubsampling2ToOutput[m * NeuronNumSubsampling2 * NeuronNumSubsampling2 + (y * NeuronNumSubsampling2 + x)] - LearningRate * PD_WeightSubsampling2ToOutput[m * NeuronNumSubsampling2 * NeuronNumSubsampling2 + (y * NeuronNumSubsampling2 + x)];

				//新权重是保留0.1*新+0.9*旧
				WeightSubsampling2ToOutput[m * NeuronNumSubsampling2 * NeuronNumSubsampling2 + (y * NeuronNumSubsampling2 + x)] = 
					WeightSubsampling2ToOutput[m * NeuronNumSubsampling2 * NeuronNumSubsampling2 + (y * NeuronNumSubsampling2 + x)] * 0.1 + 
					OldWeightSubsampling2ToOutput[m * NeuronNumSubsampling2 * NeuronNumSubsampling2 + (y * NeuronNumSubsampling2 + x)]; 
			}
		}
	}


	//**************************池化2层---->卷积1层（未涉及权值）从缩小后的误差δ，还原前一次较大区域对应的误差**************************
	for(temp = 0; temp < NeuronNumSubsampling2 * NeuronNumSubsampling2; temp++)
	{
		PD_ErrorToNetSubsampling2[temp] = 0;
	}

	//****************池化2层误差Delta_Subsampling2****************
	for(y = 0; y < NeuronNumSubsampling2; y++)
	{
		for(x = 0; x < NeuronNumSubsampling2; x++)
		{
			for(m = 0; m < NeuronNumOutput; m++)
			{
				//NetOutput[n]---->SigmoidSubsampling2[n]
				PD_NetsumOutputToSigmoidSubsampling2[m * NeuronNumSubsampling2 * NeuronNumSubsampling2 + (y * NeuronNumSubsampling2 + x)] = 
					WeightSubsampling2ToOutput[m * NeuronNumSubsampling2 * NeuronNumSubsampling2 + (y * NeuronNumSubsampling2 + x)];

				//SigmoidSubsampling2[n]---->NetSubsampling2[n]
				PD_SigmoidSubsampling2ToNetSubsampling2[y * NeuronNumSubsampling2 + x] = 1;

				////////////Error[n]---->NetSubsampling2[n]
				PD_ErrorToNetSubsampling2[y * NeuronNumSubsampling2 + x] += 
					Delta_NetsumOutput[m] * 
					PD_NetsumOutputToSigmoidSubsampling2[m * NeuronNumSubsampling2 * NeuronNumSubsampling2 + (y * NeuronNumSubsampling2 + x)] * 
					PD_SigmoidSubsampling2ToNetSubsampling2[y * NeuronNumSubsampling2 + x];

				//池化2层的误差值＝总误差对池化2层神经元值求偏导
				Delta_Subsampling2[y * NeuronNumSubsampling2 + x] = PD_ErrorToNetSubsampling2[y * NeuronNumSubsampling2 + x];
			}
		}
	}

	//****************卷积1层误差Delta_Convolution1****************
	//把池化2层的误差值维度还原成上一层（卷积1层）的误差值维度，即除了填充每4格中的最大误差值外，其余位置补0
	for(y = 0; y < NeuronNumSubsampling2; y++)
	{
		for (x = 0; x < NeuronNumSubsampling2; x++)
		{
			for(j = 0; j < Filter; j++)
			{
				for (i = 0; i < Filter; i++)
				{
					upsample_Delta_Subsampling2[(y * Filter + j) * NeuronNumConvolution1 + (x * Filter + i)] = 0;
					upsample_Delta_Subsampling2[(max[y * NeuronNumSubsampling2 + x].y * Filter + max[y * NeuronNumSubsampling2 + x].j) * NeuronNumConvolution1 + (max[y * NeuronNumSubsampling2 + x].x * Filter + max[y * NeuronNumSubsampling2 + x].i)] = Delta_Subsampling2[y * NeuronNumSubsampling2 + x];
				}
			}
		}
	}

	for(y = 0; y < NeuronNumSubsampling2; y++)
	{
		for (x = 0; x < NeuronNumSubsampling2; x++)
		{
			for(j = 0; j < Filter; j++)
			{
				for (i = 0; i < Filter; i++)
				{
					PD_SigmoidConvolution1ToNetConvolution1[(y * Filter + j) * NeuronNumConvolution1 + (x * Filter + i)] = //201810031800改，将NeuronNumSubsampling2改为NeuronNumConvolution1
						SigmoidConvolution1[(y * Filter + j) * NeuronNumConvolution1 + (x * Filter + i)] * 
						(1 - SigmoidConvolution1[(y * Filter + j) * NeuronNumConvolution1 + (x * Filter + i)]); 

					//卷积1层(池化2层的前一层)的误差值δ＝对池化2层的误差值进行upsample还原 * 卷积1层的激活值对神经元值求偏导
					PD_ErrorToNetConvolution1[(y * Filter + j) * NeuronNumConvolution1 + (x * Filter + i)] = //201810031800改，将NeuronNumSubsampling2改为NeuronNumConvolution1
						//对池化2层的误差值进行upsample还原
						upsample_Delta_Subsampling2[(y * Filter + j) * NeuronNumConvolution1 + (x * Filter + i)] * 
						//卷积1层的激活值对神经元值求偏导
						PD_SigmoidConvolution1ToNetConvolution1[(y * Filter + j) * NeuronNumConvolution1 + (x * Filter + i)];

					//卷积1层误差Delta_Convolution1
					Delta_Convolution1[(y * Filter + j) * NeuronNumConvolution1 + (x * Filter + i)] = PD_ErrorToNetConvolution1[(y * Filter + j) * NeuronNumConvolution1 + (x * Filter + i)];
				}
			}
		}
	}


	//**************************卷积1层---->输入层（卷积核）的权值更新**************************
	for (temp = 0; temp < ConvolutionKernel * ConvolutionKernel; temp++) //201810031800改，将NeuronNumConvolution1改为ConvolutionKernel
	{
		PD_WeightInputToConvolution1[temp] = 0;
	}

	for(t = 0; t < ConvolutionKernel; t++) //201810031800改，将NeuronNumConvolution1改为ConvolutionKernel
	{
		for (s = 0; s < ConvolutionKernel; s++) //201810031800改，将NeuronNumConvolution1改为ConvolutionKernel
		{
			for(d = 0; d < NeuronNumConvolution1; d++)
			{
				for (c = 0; c < NeuronNumConvolution1; c++)
				{
					//卷积操作一般是要把卷积核旋转180度再做协相关（一一位置对应相乘），不过，由于 CNN 中的卷积参数本来就是学出来的，所以旋不旋转，关系其实不大，这里默认不旋转
					PD_WeightInputToConvolution1[t * ConvolutionKernel + s] += Padding_iPicture[t + d][s + c] * Delta_Convolution1[d * NeuronNumConvolution1 + c];  //201810031800改，将NeuronNumConvolution1改为ConvolutionKernel
				}
			}
		}
	}

	for(t = 0; t < ConvolutionKernel; t++) //201810031800改，将NeuronNumConvolution1改为ConvolutionKernel
	{
		for (s = 0; s < ConvolutionKernel; s++) //201810031800改，将NeuronNumConvolution1改为ConvolutionKernel
		{
			//旧权重保留0.9
			OldWeightInputToConvolution1[t * ConvolutionKernel + s] = WeightInputToConvolution1[t * ConvolutionKernel + s] * 0.9; 

			////////////更新WeightInputToConvolution1[m]的值
			WeightInputToConvolution1[t * ConvolutionKernel + s] = WeightInputToConvolution1[t * ConvolutionKernel + s] - LearningRate * PD_WeightInputToConvolution1[t * ConvolutionKernel + s]; //201810031800改，将WeightInputToHidden1[t * ConvolutionKernel + s]改为PD_WeightInputToConvolution1[t * ConvolutionKernel + s]

			//新权重是保留0.1*新+0.9*旧
			WeightInputToConvolution1[t * ConvolutionKernel + s] = WeightInputToConvolution1[t * ConvolutionKernel + s] * 0.1 + OldWeightInputToConvolution1[t * ConvolutionKernel + s]; 
		}
	}
}


//清理Target[]
void ClearTarget()
{
	int i = 0;

	for (i = 0; i < NeuronNumOutput; i++)
	{
		target[i] = 0;
	}
}

//计算网络识别结果
int CalculateNetResult()
{
	int i = 0, result = 0; 
	CalculateNeuronValue(); //计算神经元的值	

	for (i = 1; i < NeuronNumOutput; i++)
	{
		if (SigmoidOutput[result] < SigmoidOutput[i])
		{
			result = i;
		}
	}

	return result;//识别结果
}