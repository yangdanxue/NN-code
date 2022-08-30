#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


#define pixels						28		//MNIST数据库图片像素是28*28
#define Padding_pixels				32		//填充后图片像素是32*32
#define ConvolutionKernel			5		//卷积核，大小是5*5
#define NeuronNumConvolution1       28		//填充图片后，卷积1层矩阵边长
#define NeuronNumOutput				10   	//输出层神经元个数

#define OldWeightPercentage			0.9   	//反向传播更新权重时旧权重占比
#define NewWeightPercentage			0.1   	//反向传播更新权重时新权重占比

#define TrainTimes					1		//训练次数
#define TestNumber					10000	//总测试图片的数量
#define Memory						1		//记住池化层中每个值，在上一卷积层中的具体坐标


int ReadPicData(unsigned char* data, int iNum);				//读图片数据文件

//写文件——保存当前各层权重
void WriteWeightInputToConvolution1Data();			
void WriteWeightConvolution1ToOutputData();		

//读文件——读取已保存的各层权重
void ReadWeightInputToConvolution1Data();		
void ReadWeightConvolution1ToOutputData();			

//初始化各层权重
void InitializationWeightInputToConvolution1();				
void InitializationWeightConvolution1ToOutput();			

int CalculateNetResult();			//计算网络输出结果
float TestNet();					//测试网络
void TrainNet();					//选择测试或训练

void CalculateNeuronValue();		//计算各个神经元的值
void BackPropagation();				//反向传播法更新权值
void ClearTarget();					//清理Target[]

unsigned char iPicture[pixels][pixels] = {0};								//导入的MNIST数据库图片数组
unsigned char Padding_iPicture[Padding_pixels][Padding_pixels] = {0};		//对于5*5的过滤器，在导入的MNIST数据库图片数组iPicture[y][x]周围，填充宽度为2的0时，就可以保证输出图像和输入图像一样大
unsigned char Label;														//导入的MNIST数据库标签
unsigned char* p = &Label;

//各层卷积核（权重）
double WeightInputToConvolution1[(NeuronNumConvolution1 * ConvolutionKernel) * (NeuronNumConvolution1 * ConvolutionKernel)] = {0};									
double WeightConvolution1ToOutput[NeuronNumConvolution1 * NeuronNumConvolution1 * NeuronNumOutput] = {0};		

//各层反向传播后旧权重保留的比例
double OldWeightInputToConvolution1[(NeuronNumConvolution1 * ConvolutionKernel) * (NeuronNumConvolution1 * ConvolutionKernel)] = {0};								
double OldWeightConvolution1ToOutput[NeuronNumConvolution1 * NeuronNumConvolution1 * NeuronNumOutput] = {0};	

//各层神经元值
double NetConvolution1[NeuronNumConvolution1 * NeuronNumConvolution1] = {0};									
double NetOutput[NeuronNumOutput] = {0};																		

//各层激活函数值（Sigmoid函数）
double SigmoidConvolution1[NeuronNumConvolution1 * NeuronNumConvolution1] = {0};								
double SigmoidOutput[NeuronNumOutput] = {0};																	

//目标输出值
int target[NeuronNumOutput] = {0};																				

//各层误差值
double Delta_Output[NeuronNumOutput] = {0};
double Delta_Convolution1[NeuronNumConvolution1 * NeuronNumConvolution1] = {0};

//输出层---->卷积1层的权值更新
double PD_WeightConvolution1ToOutput[NeuronNumConvolution1 * NeuronNumConvolution1 * NeuronNumOutput] = {0};
double PD_ErrorToSigmoidOutput[NeuronNumOutput] = {0};
double PD_SigmoidOutputToNetOutput[NeuronNumOutput] = {0};
double PD_ErrorToNetOutput[NeuronNumOutput] = {0};
double PD_NetOutputToWeightConvolution1ToOutput[NeuronNumOutput * NeuronNumConvolution1 * NeuronNumConvolution1] = {0};

//卷积1层---->输入层的权值更新
double PD_NetOutputToSigmoidConvolution1[NeuronNumConvolution1 * NeuronNumConvolution1 * NeuronNumOutput] = {0};
double PD_ErrorToNetConvolution1[NeuronNumConvolution1 * NeuronNumConvolution1] = {0};
double PD_SigmoidConvolution1ToNetConvolution1[NeuronNumConvolution1 * NeuronNumConvolution1] = {0};
double PD_WeightInputToConvolution1[(NeuronNumConvolution1 * ConvolutionKernel) * (NeuronNumConvolution1 * ConvolutionKernel)] = {0};


float LearningRate = 0.03;	//学习速率

int Correct = 0;//识别正确数


void main()
{
	float CorrectRate;
	int MaxCorrect = 0;
	int i = 0;

	ReadWeightInputToConvolution1Data();
	ReadWeightConvolution1ToOutputData();

	//InitializationWeightInputToConvolution1();
	//InitializationWeightConvolution1ToOutput();

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
	//		WriteWeightConvolution1ToOutputData();
	//	}
	//}
}


//读文件——使用保存的[输入层-卷积1层]权重
void ReadWeightInputToConvolution1Data()
{
	int iNum = 0; //神经元个数

	FILE *fpReadItoC1 = fopen("d:\\WeightItoC1Data201811072258.wet","rb");

	if(fpReadItoC1 == NULL)
	{
		printf("WeightItoC1Data201811072258.wet文件不存在！\n");
		return;
	}

	//下面是读数据，将读到的数据存到数组WeightInputToConvolution1[NeuronNumConvolution1]中
	iNum = fread(WeightInputToConvolution1, sizeof(double), (NeuronNumConvolution1 * ConvolutionKernel) * (NeuronNumConvolution1 * ConvolutionKernel), fpReadItoC1);
	if(iNum != (NeuronNumConvolution1 * ConvolutionKernel) * (NeuronNumConvolution1 * ConvolutionKernel))
	{
		printf("读数据存到数组WeightInputToConvolution1[(NeuronNumConvolution1 * ConvolutionKernel) * (NeuronNumConvolution1 * ConvolutionKernel)]错误！！！\n");
	}
	printf("读[输入层-卷积1层]间的权重数据成功\n");

	fclose(fpReadItoC1);

	return;
}


//读文件——使用保存的[卷积1层-输出层]权重
void ReadWeightConvolution1ToOutputData()
{
	int iNum = 0; //输入层-隐藏层间的神经元个数

	FILE *fpReadC1toO = fopen("d:\\WeightC1toOData201811072258.wet","rb");

	if(fpReadC1toO == NULL)
	{
		printf("WeightC1toOData201811072258.wet文件不存在！\n");
		return;
	}

	//下面是读数据，将读到的数据存到数组WeightConvolution1ToOutput[NeuronNumConvolution1 * NeuronNumConvolution1 * NeuronNumOutput]中
	iNum = fread(WeightConvolution1ToOutput, sizeof(double), NeuronNumConvolution1 * NeuronNumConvolution1 * NeuronNumOutput, fpReadC1toO);
	if(iNum != NeuronNumConvolution1 * NeuronNumConvolution1 * NeuronNumOutput)
	{
		printf("读数据存到数组WeightConvolution1ToOutput[NeuronNumConvolution1 * NeuronNumConvolution1 * NeuronNumOutput]错误！！！\n");
	}
	printf("读[卷积1层-输出层]间的权重数据成功\n");

	fclose(fpReadC1toO);

	return;
}			


//写文件——保留当前调好的[输入层-卷积1层]权重
void WriteWeightInputToConvolution1Data()
{
	int iNum = 0;
	FILE *fpWriteInputToConvolution1 = fopen("d:\\WeightItoC1Data201811072258.wet","wb+");

	if(fpWriteInputToConvolution1 == NULL)
	{
		printf("创建WeightItoC1Data201811072258.wet文件失败！\n");
		return;
	}

	//将输入层-卷积1层间的权重写入到WeightItoC1Data201811072258.wet文件中
	iNum = fwrite(WeightInputToConvolution1, sizeof(double), (NeuronNumConvolution1 * ConvolutionKernel) * (NeuronNumConvolution1 * ConvolutionKernel), fpWriteInputToConvolution1);
	if (iNum != (NeuronNumConvolution1 * ConvolutionKernel) * (NeuronNumConvolution1 * ConvolutionKernel))
	{
		printf("写WeightItoC1Data201811072258.wet文件失败！");
	}
	printf("所有输入层-卷积1层间的权重数据已经保存\n");

	fclose(fpWriteInputToConvolution1);
}


//写文件——保留当前调好的[卷积1层-输出层]权重
void WriteWeightConvolution1ToOutputData()
{
	int iNum = 0;
	FILE *fpWriteConvolution1ToOutput = fopen("d:\\WeightC1toOData201811072258.wet","wb+");

	if(fpWriteConvolution1ToOutput == NULL)
	{
		printf("创建WeightC1toOData201811072258.wet文件失败！\n");
		return;
	}

	//将卷积1层-输出层间的权重写入到WeightC1toOData201811072258.wet文件中
	iNum = fwrite(WeightConvolution1ToOutput, sizeof(double), NeuronNumConvolution1 * NeuronNumConvolution1 * NeuronNumOutput, fpWriteConvolution1ToOutput);
	if (iNum != NeuronNumConvolution1 * NeuronNumConvolution1 * NeuronNumOutput)
	{		
		printf("写WeightC1toOData201811072258.wet文件失败！");
	}
	printf("所有卷积1层-输出层间的权重数据已经保存\n");

	fclose(fpWriteConvolution1ToOutput);
}			


//随机初始化[输入层-卷积1层]权重
void InitializationWeightInputToConvolution1()
{
	int m = 0;

	for(m = 0; m < (NeuronNumConvolution1 * ConvolutionKernel) * (NeuronNumConvolution1 * ConvolutionKernel); m++) //卷积1层的每个神经元，都是由不同的卷积核卷积得到的。共计784个卷积核（每个核有5*5个元素）
	{
		WeightInputToConvolution1[m] = (rand()/(double)(RAND_MAX))/1000;		
		//WeightInputToConvolution1[m] = 0;									
		//WeightInputToConvolution1[m] = 1023.9754277741900000;				//78.0700%版权重的平均数
		//WeightInputToConvolution1[m] = -0.1964562163147680;				//78.0700%版权重的中位数
	}
}


//随机初始化[卷积1层-输出层]权重
void InitializationWeightConvolution1ToOutput()
{
	int m = 0;

	for(m = 0; m < NeuronNumConvolution1 * NeuronNumConvolution1 * NeuronNumOutput; m++)
	{
		WeightConvolution1ToOutput[m] = (rand()/(double)(RAND_MAX))/200;			
		//WeightConvolution1ToOutput[m] = 0;										
		//WeightConvolution1ToOutput[m] = -0.3302111820947480;						//78.0700%版权重的平均数
		//WeightConvolution1ToOutput[m] = -0.3983054846310040;						//78.0700%版权重的中位数
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


//读取MINST数据集
int ReadPicData(unsigned char* data, unsigned char* p, int iNum)
{
	int iCheck = 0;

	//FILE *fp_image = fopen("d:\\train-images.idx", "rb");//打开这个二进制(b)文件train-images.idx，以只读(r)的方式打开，
	//FILE *fp_label = fopen("d:\\train-labels.idx", "rb");//读类标
	FILE *fp_image = fopen("d:\\t10k_images.idx", "rb");//打开这个二进制(b)文件t10k-images.idx，以只读(r)的方式打开，
	FILE *fp_label = fopen("d:\\t10k_labels.idx", "rb");//读类标
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
	int xC1, yC1 = 0;
	int m, n = 0;
	int o = 0;


	//网络架构：输入层 ——> 卷积1层 ——> 输出层

	////////////////计算卷积1层神经元的值////////////////
	for(yC1 = 0; yC1 < NeuronNumConvolution1; yC1++)
	{
		for (xC1 = 0; xC1 < NeuronNumConvolution1; xC1++)
		{
			NetConvolution1[yC1 * NeuronNumConvolution1 + xC1] = 0;
		}
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

	for(y = 2, j = 0; y < pixels + 2, j < pixels; y++, j++)
	{
		for (x = 2, i = 0; x < pixels + 2, i < pixels; x++, i++)
		{
			Padding_iPicture[y][x] = iPicture[j][i]; 
		}
	}


	for(yC1 = 0, n = 0; yC1 < NeuronNumConvolution1, n < NeuronNumConvolution1 * ConvolutionKernel; yC1++, n = n + ConvolutionKernel)
	{
		for (xC1 = 0, m = 0; xC1 < NeuronNumConvolution1, m < NeuronNumConvolution1 * ConvolutionKernel; xC1++, m = m + ConvolutionKernel)
		{
			for(j = 0; j < ConvolutionKernel; j++)
			{
				for (i = 0; i < ConvolutionKernel; i++)
				{
					NetConvolution1[yC1 * NeuronNumConvolution1 + xC1] += Padding_iPicture[yC1 + j][xC1 + i] * WeightInputToConvolution1[(n + j) * NeuronNumConvolution1 * ConvolutionKernel + (m + i)];
				}
			}
		}
	}


	////////////////计算卷积1层的激活函数值////////////////
	for(yC1 = 0; yC1 < NeuronNumConvolution1; yC1++)
	{
		for (xC1 = 0; xC1 < NeuronNumConvolution1; xC1++)
		{
			SigmoidConvolution1[yC1 * NeuronNumConvolution1 + xC1] = 1 / (1 + exp( - NetConvolution1[yC1 * NeuronNumConvolution1 + xC1])); 
		}
	}





	////////////////计算输出层神经元的值////////////////
	for (o = 0; o < NeuronNumOutput; o++)
	{
		NetOutput[o] = 0;
	}

	for(o = 0; o < NeuronNumOutput; o++) //输出层神经元个数为NeuronNumOutput
	{
		for(yC1 = 0; yC1 < NeuronNumConvolution1; yC1++)
		{
			for (xC1 = 0; xC1 < NeuronNumConvolution1; xC1++)
			{
				NetOutput[o] += 
					SigmoidConvolution1[yC1 * NeuronNumConvolution1 + xC1] * 
					WeightConvolution1ToOutput[o * NeuronNumConvolution1 * NeuronNumConvolution1 + (yC1 * NeuronNumConvolution1 + xC1)]; //卷积1层和输出层间的神经元要一一连（全排列）
			}
		}
	}


	////////////////计算输出层sigmoid函数值////////////////
	for(o = 0; o < NeuronNumOutput; o++)
	{
		SigmoidOutput[o] = 1 / (1 + exp( - NetOutput[o]));
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

	ClearTarget();
	target[Label] = 1; //目标输出值


	//**************************输出层---->卷积1层的权值更新（全连接）**************************

	//****************输出层误差Delta_Output****************
	for(m = 0; m < NeuronNumOutput; m++)
	{
		////////////Error[m]---->SigmoidOutput[m]
		PD_ErrorToSigmoidOutput[m] = - (target[m] - SigmoidOutput[m]) / 5; 

		////////////SigmoidOutput[m]---->NetOutput[m]
		PD_SigmoidOutputToNetOutput[m] = SigmoidOutput[m] * (1 - SigmoidOutput[m]); 

		////////////Error[m]---->NetOutput[m]
		PD_ErrorToNetOutput[m] = PD_ErrorToSigmoidOutput[m] * PD_SigmoidOutputToNetOutput[m]; 

		//输出层的误差值＝总误差对输出层神经元值求偏导
		Delta_Output[m] = PD_ErrorToNetOutput[m];
	}

	////////////NetOutput[n]---->WeightConvolution1ToOutput[n]
	for(m = 0; m < NeuronNumOutput; m++)
	{
		for(y = 0; y < NeuronNumConvolution1; y++)
		{
			for(x = 0; x < NeuronNumConvolution1; x++)
			{
				PD_NetOutputToWeightConvolution1ToOutput[m * NeuronNumConvolution1 * NeuronNumConvolution1 + (y * NeuronNumConvolution1 + x)] = SigmoidConvolution1[y * NeuronNumConvolution1 + x];
			}
		}
	}

	for(m = 0; m < NeuronNumOutput; m++)
	{
		for(y = 0; y < NeuronNumConvolution1; y++)
		{
			for(x = 0; x < NeuronNumConvolution1; x++)
			{
				////////////链式法则求偏导
				PD_WeightConvolution1ToOutput[m * NeuronNumConvolution1 * NeuronNumConvolution1 + (y * NeuronNumConvolution1 + x)] = 
					Delta_Output[m] * PD_NetOutputToWeightConvolution1ToOutput[m * NeuronNumConvolution1 * NeuronNumConvolution1 + (y * NeuronNumConvolution1 + x)];

				//旧权重保留0.9
				OldWeightConvolution1ToOutput[m * NeuronNumConvolution1 * NeuronNumConvolution1 + (y * NeuronNumConvolution1 + x)] = 
					WeightConvolution1ToOutput[m * NeuronNumConvolution1 * NeuronNumConvolution1 + (y * NeuronNumConvolution1 + x)] * OldWeightPercentage; 

				////////////更新WeightConvolution1ToOutput[m]的值
				WeightConvolution1ToOutput[m * NeuronNumConvolution1 * NeuronNumConvolution1 + (y * NeuronNumConvolution1 + x)] = 
					WeightConvolution1ToOutput[m * NeuronNumConvolution1 * NeuronNumConvolution1 + (y * NeuronNumConvolution1 + x)] - LearningRate * PD_WeightConvolution1ToOutput[m * NeuronNumConvolution1 * NeuronNumConvolution1 + (y * NeuronNumConvolution1 + x)];

				//新权重是保留0.1*新+0.9*旧
				WeightConvolution1ToOutput[m * NeuronNumConvolution1 * NeuronNumConvolution1 + (y * NeuronNumConvolution1 + x)] = 
					WeightConvolution1ToOutput[m * NeuronNumConvolution1 * NeuronNumConvolution1 + (y * NeuronNumConvolution1 + x)] * NewWeightPercentage + 
					OldWeightConvolution1ToOutput[m * NeuronNumConvolution1 * NeuronNumConvolution1 + (y * NeuronNumConvolution1 + x)]; 
			}
		}
	}










	//**************************卷积1层---->输入层（未涉及权值）**************************
	for(temp = 0; temp < NeuronNumConvolution1 * NeuronNumConvolution1; temp++)
	{
		PD_ErrorToNetConvolution1[temp] = 0;
	}

	//****************卷积1层误差Delta_Convolution1****************
	for(y = 0; y < NeuronNumConvolution1; y++)
	{
		for(x = 0; x < NeuronNumConvolution1; x++)
		{
			for(m = 0; m < NeuronNumOutput; m++)
			{
				//NetOutput[n]---->SigmoidConvolution1[n]
				PD_NetOutputToSigmoidConvolution1[m * NeuronNumConvolution1 * NeuronNumConvolution1 + (y * NeuronNumConvolution1 + x)] = 
					WeightConvolution1ToOutput[m * NeuronNumConvolution1 * NeuronNumConvolution1 + (y * NeuronNumConvolution1 + x)];

				//SigmoidConvolution1[n]---->NetConvolution1[n]
				PD_SigmoidConvolution1ToNetConvolution1[y * NeuronNumConvolution1 + x] = SigmoidConvolution1[y * NeuronNumConvolution1 + x] * (1 - SigmoidConvolution1[y * NeuronNumConvolution1 + x]);

				////////////Error[n]---->NetConvolution1[n]
				PD_ErrorToNetConvolution1[y * NeuronNumConvolution1 + x] += 
					Delta_Output[m] * 
					PD_NetOutputToSigmoidConvolution1[m * NeuronNumConvolution1 * NeuronNumConvolution1 + (y * NeuronNumConvolution1 + x)] * 
					PD_SigmoidConvolution1ToNetConvolution1[y * NeuronNumConvolution1 + x];

				//卷积1层的误差值＝总误差对卷积1层神经元值求偏导
				Delta_Convolution1[y * NeuronNumConvolution1 + x] = PD_ErrorToNetConvolution1[y * NeuronNumConvolution1 + x];
			}
		}
	}










	//**************************卷积1层---->输入层（卷积核）的权值更新**************************
	for (temp = 0; temp < (NeuronNumConvolution1 * ConvolutionKernel) * (NeuronNumConvolution1 * ConvolutionKernel); temp++) 
	{
		PD_WeightInputToConvolution1[temp] = 0;
	}


	for(t = 0, n = 0; t < NeuronNumConvolution1, n < NeuronNumConvolution1 * ConvolutionKernel; t++, n = n + ConvolutionKernel) 
	{
		for (s = 0, m = 0; s < NeuronNumConvolution1, m < NeuronNumConvolution1 * ConvolutionKernel; s++, m = m + ConvolutionKernel) 
		{
			for(d = 0; d < ConvolutionKernel; d++)
			{
				for (c = 0; c < ConvolutionKernel; c++)
				{
					PD_WeightInputToConvolution1[(n + d) * (NeuronNumConvolution1 * ConvolutionKernel) + (m + c)] = Delta_Convolution1[t * NeuronNumConvolution1 + s] * Padding_iPicture[t + d][s + c]; 
				}
			}
		}
	}



	for(n = 0; n < NeuronNumConvolution1 * ConvolutionKernel; n = n + ConvolutionKernel) 
	{
		for (m = 0; m < NeuronNumConvolution1 * ConvolutionKernel; m = m + ConvolutionKernel) 
		{
			for(d = 0; d < ConvolutionKernel; d++)
			{
				for (c = 0; c < ConvolutionKernel; c++)
				{
					//旧权重保留0.9
					OldWeightInputToConvolution1[(n + d) * (NeuronNumConvolution1 * ConvolutionKernel) + (m + c)] = 
						WeightInputToConvolution1[(n + d) * (NeuronNumConvolution1 * ConvolutionKernel) + (m + c)] * OldWeightPercentage; 

					////////////更新WeightInputToConvolution1[m]的值
					WeightInputToConvolution1[(n + d) * (NeuronNumConvolution1 * ConvolutionKernel) + (m + c)] = 
						WeightInputToConvolution1[(n + d) * (NeuronNumConvolution1 * ConvolutionKernel) + (m + c)] - LearningRate * PD_WeightInputToConvolution1[(n + d) * (NeuronNumConvolution1 * ConvolutionKernel) + (m + c)];

					//新权重是保留0.1*新+0.9*旧
					WeightInputToConvolution1[(n + d) * (NeuronNumConvolution1 * ConvolutionKernel) + (m + c)] = 
						WeightInputToConvolution1[(n + d) * (NeuronNumConvolution1 * ConvolutionKernel) + (m + c)] * NewWeightPercentage + OldWeightInputToConvolution1[(n + d) * (NeuronNumConvolution1 * ConvolutionKernel) + (m + c)]; 
				}
			}
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