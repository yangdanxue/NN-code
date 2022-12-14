#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


#define pixels						28		//MNIST数据库图片像素是28*28
#define Padding_pixels				32		//填充后图片像素是32*32
#define ConvolutionKernel			5		//卷积核，大小是5*5
#define NeuronNumConvolution1       28		//填充图片后，卷积1层矩阵边长
#define NeuronNumConvolution2       24		//填充图片后，卷积2层矩阵边长
#define NeuronNumSubsampling3       12		//填充图片后，池化3层矩阵边长
#define Filter						2		//池化层过滤器大小
#define NeuronNumOutput				10   	//输出层神经元个数

#define OldWeightPercentage			0.9   	//反向传播更新权重时旧权重占比
#define NewWeightPercentage			0.1   	//反向传播更新权重时新权重占比

#define TrainTimes					1		//训练次数
#define TestNumber					10000	//总测试图片的数量
#define Memory						1		//记住池化层中每个值，在上一卷积层中的具体坐标


int ReadPicData(unsigned char* data, int iNum);				//读图片数据文件

//写文件——保存当前各层权重
void WriteWeightInputToConvolution1Data();			
void WriteWeightConvolution1ToConvolution2Data();	
void WriteWeightSubsampling3ToOutputData();		

//读文件——读取已保存的各层权重
void ReadWeightInputToConvolution1Data();		
void ReadWeightConvolution1ToConvolution2Data();		
void ReadWeightSubsampling3ToOutputData();			

//初始化各层权重
void InitializationWeightInputToConvolution1();				
void InitializationWeightConvolution1ToConvolution2();     
void InitializationWeightSubsampling3ToOutput();			

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
double WeightInputToConvolution1[ConvolutionKernel * ConvolutionKernel] = {0};									
double WeightConvolution1ToConvolution2[ConvolutionKernel * ConvolutionKernel] = {0};							
double WeightSubsampling3ToOutput[NeuronNumSubsampling3 * NeuronNumSubsampling3 * NeuronNumOutput] = {0};		

//各层卷积核（权重）翻转180°
double WeightRot180InputToConvolution1[ConvolutionKernel * ConvolutionKernel] = {0};							
double WeightRot180Convolution1ToConvolution2[ConvolutionKernel * ConvolutionKernel] = {0};						

//各层反向传播后旧权重保留的比例
double OldWeightInputToConvolution1[ConvolutionKernel * ConvolutionKernel] = {0};								
double OldWeightConvolution1ToConvolution2[ConvolutionKernel * ConvolutionKernel] = {0};						
double OldWeightSubsampling3ToOutput[NeuronNumSubsampling3 * NeuronNumSubsampling3 * NeuronNumOutput] = {0};	

//各层神经元值
double NetConvolution1[NeuronNumConvolution1 * NeuronNumConvolution1] = {0};									
double NetConvolution2[NeuronNumConvolution2 * NeuronNumConvolution2] = {0};									
double NetSubsampling3[NeuronNumSubsampling3 * NeuronNumSubsampling3] = {0};									
double NetOutput[NeuronNumOutput] = {0};																		

//各层激活函数值（Sigmoid函数）
double SigmoidConvolution1[NeuronNumConvolution1 * NeuronNumConvolution1] = {0};								
double SigmoidConvolution2[NeuronNumConvolution2 * NeuronNumConvolution2] = {0};
double SigmoidSubsampling3[NeuronNumSubsampling3 * NeuronNumSubsampling3] = {0};
double SigmoidOutput[NeuronNumOutput] = {0};																	

//目标输出值
int target[NeuronNumOutput] = {0};																				

//各层误差值
double Delta_Output[NeuronNumOutput] = {0};
double Delta_Subsampling3[NeuronNumSubsampling3 * NeuronNumSubsampling3] = {0};
double Delta_Convolution2[NeuronNumConvolution2 * NeuronNumConvolution2] = {0};
double Delta_Convolution1[NeuronNumConvolution1 * NeuronNumConvolution1] = {0};

//输出层---->池化3层的权值更新
double PD_WeightSubsampling3ToOutput[NeuronNumSubsampling3 * NeuronNumSubsampling3 * NeuronNumOutput] = {0};
double PD_ErrorToSigmoidOutput[NeuronNumOutput] = {0};
double PD_SigmoidOutputToNetOutput[NeuronNumOutput] = {0};
double PD_ErrorToNetOutput[NeuronNumOutput] = {0};
double PD_NetOutputToWeightSubsampling3ToOutput[NeuronNumOutput * NeuronNumSubsampling3 * NeuronNumSubsampling3] = {0};

//池化3层---->卷积2层的权值更新
double PD_ErrorToNetSubsampling3[NeuronNumSubsampling3 * NeuronNumSubsampling3] = {0};
double PD_NetOutputToSigmoidSubsampling3[NeuronNumOutput * NeuronNumSubsampling3 * NeuronNumSubsampling3] = {0};
double PD_SigmoidSubsampling3ToNetSubsampling3[NeuronNumSubsampling3 * NeuronNumSubsampling3] = {0};

double upsample_Delta_Subsampling3[NeuronNumConvolution2 * NeuronNumConvolution2] = {0};
double PD_SigmoidConvolution2ToNetConvolution2[NeuronNumConvolution2 * NeuronNumConvolution2] = {0};
double PD_ErrorToNetConvolution2[NeuronNumConvolution2 * NeuronNumConvolution2] = {0};

//在卷积2层的误差值矩阵周围，填充4圈0，为了符合梯度计算：∇a11=δ11 * w11
double Padding_Delta_Convolution2[(NeuronNumConvolution2+2*(ConvolutionKernel-1)) * (NeuronNumConvolution2+2*(ConvolutionKernel-1))] = {0}; 

//卷积2层---->卷积1层的权值更新
double PD_WeightConvolution1ToConvolution2[ConvolutionKernel * ConvolutionKernel] = {0};
double PD_SigmoidConvolution1ToNetConvolution1[NeuronNumConvolution1 * NeuronNumConvolution1] = {0};
double PD_ErrorToNetConvolution1[NeuronNumConvolution1 * NeuronNumConvolution1] = {0};

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
struct MAXPool maxSubsampling3[NeuronNumSubsampling3 * NeuronNumSubsampling3] = {0};


float LearningRate = 0.05;	//学习速率

int Correct = 0;//识别正确数


void main()
{
	float CorrectRate;
	int MaxCorrect = 0;
	int i = 0;

	ReadWeightInputToConvolution1Data();
	ReadWeightConvolution1ToConvolution2Data();
	ReadWeightSubsampling3ToOutputData();

	//InitializationWeightInputToConvolution1();
	//InitializationWeightConvolution1ToConvolution2();
	//InitializationWeightSubsampling3ToOutput();

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
	//		WriteWeightConvolution1ToConvolution2Data();
	//		WriteWeightSubsampling3ToOutputData();
	//	}
	//}
}


//读文件——使用保存的[输入层-卷积1层]权重
void ReadWeightInputToConvolution1Data()
{
	int iNum = 0; //神经元个数

	FILE *fpReadItoC1 = fopen("d:\\WeightItoC1Data201810141050.wet","rb");

	if(fpReadItoC1 == NULL)
	{
		printf("WeightItoC1Data201810141050.wet文件不存在！\n");
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


//读文件——使用保存的[卷积1层-卷积2层]权重
void ReadWeightConvolution1ToConvolution2Data()
{
	int iNum = 0; //神经元个数

	FILE *fpReadC1toC2 = fopen("d:\\WeightC1toC2Data201810141050.wet","rb");

	if(fpReadC1toC2 == NULL)
	{
		printf("WeightC1toC2Data201810141050.wet文件不存在！\n");
		return;
	}

	//下面是读数据，将读到的数据存到数组WeightConvolution1ToConvolution2[NeuronNumConvolution1]中
	iNum = fread(WeightConvolution1ToConvolution2, sizeof(double), ConvolutionKernel * ConvolutionKernel, fpReadC1toC2);
	if(iNum != ConvolutionKernel * ConvolutionKernel)
	{
		printf("读数据存到数组WeightConvolution1ToConvolution2[ConvolutionKernel * ConvolutionKernel]错误！！！\n");
	}
	printf("读[卷积1层-卷积2层]间的权重数据成功\n");

	fclose(fpReadC1toC2);

	return;
}


//读文件——使用保存的[池化3层-输出层]权重
void ReadWeightSubsampling3ToOutputData()
{
	int iNum = 0; //输入层-隐藏层间的神经元个数

	FILE *fpReadS3toO = fopen("d:\\WeightS3toOData201810141050.wet","rb");

	if(fpReadS3toO == NULL)
	{
		printf("WeightS3toOData201810141050.wet文件不存在！\n");
		return;
	}

	//下面是读数据，将读到的数据存到数组WeightSubsampling3ToOutput[NeuronNumSubsampling3 * NeuronNumSubsampling3 * NeuronNumOutput]中
	iNum = fread(WeightSubsampling3ToOutput, sizeof(double), NeuronNumSubsampling3 * NeuronNumSubsampling3 * NeuronNumOutput, fpReadS3toO);
	if(iNum != NeuronNumSubsampling3 * NeuronNumSubsampling3 * NeuronNumOutput)
	{
		printf("读数据存到数组WeightSubsampling3ToOutput[NeuronNumSubsampling3 * NeuronNumSubsampling3 * NeuronNumOutput]错误！！！\n");
	}
	printf("读[池化3层-输出层]间的权重数据成功\n");

	fclose(fpReadS3toO);

	return;
}			


//写文件——保留当前调好的[输入层-卷积1层]权重
void WriteWeightInputToConvolution1Data()
{
	int iNum = 0;
	FILE *fpWriteInputToConvolution1 = fopen("d:\\WeightItoC1Data201810141050.wet","wb+");

	if(fpWriteInputToConvolution1 == NULL)
	{
		printf("创建WeightItoC1Data201810141050.wet文件失败！\n");
		return;
	}

	//将输入层-卷积1层间的权重写入到WeightItoC1Data201810141050.wet文件中
	iNum = fwrite(WeightInputToConvolution1, sizeof(double), ConvolutionKernel * ConvolutionKernel, fpWriteInputToConvolution1);
	if (iNum != ConvolutionKernel * ConvolutionKernel)
	{
		printf("写WeightItoC1Data201810141050.wet文件失败！");
	}
	printf("所有输入层-卷积1层间的权重数据已经保存\n");

	fclose(fpWriteInputToConvolution1);
}


//写文件——保留当前调好的[卷积1层-卷积2层]权重
void WriteWeightConvolution1ToConvolution2Data()
{
	int iNum = 0;
	FILE *fpWriteConvolution1ToConvolution2 = fopen("d:\\WeightC1toC2Data201810141050.wet","wb+");

	if(fpWriteConvolution1ToConvolution2 == NULL)
	{
		printf("创建WeightC1toC2Data201810141050.wet文件失败！\n");
		return;
	}

	//将卷积1层-卷积2层间的权重写入到WeightC1toC2Data201810141050.wet文件中
	iNum = fwrite(WeightConvolution1ToConvolution2, sizeof(double), ConvolutionKernel * ConvolutionKernel, fpWriteConvolution1ToConvolution2);
	if (iNum != ConvolutionKernel * ConvolutionKernel)
	{
		printf("写WeightC1toC2Data201810141050.wet文件失败！");
	}
	printf("所有卷积1层-卷积2层间的权重数据已经保存\n");

	fclose(fpWriteConvolution1ToConvolution2);
}


//写文件——保留当前调好的[池化3层-输出层]权重
void WriteWeightSubsampling3ToOutputData()
{
	int iNum = 0;
	FILE *fpWriteSubsampling3ToOutput = fopen("d:\\WeightS3toOData201810141050.wet","wb+");

	if(fpWriteSubsampling3ToOutput == NULL)
	{
		printf("创建WeightS3toOData201810141050.wet文件失败！\n");
		return;
	}

	//将池化3层-输出层间的权重写入到WeightS3toOData201810141050.wet文件中
	iNum = fwrite(WeightSubsampling3ToOutput, sizeof(double), NeuronNumSubsampling3 * NeuronNumSubsampling3 * NeuronNumOutput, fpWriteSubsampling3ToOutput);
	if (iNum != NeuronNumSubsampling3 * NeuronNumSubsampling3 * NeuronNumOutput)
	{		
		printf("写WeightS3toOData201810141050.wet文件失败！");
	}
	printf("所有池化3层-输出层间的权重数据已经保存\n");

	fclose(fpWriteSubsampling3ToOutput);
}			


//随机初始化[输入层-卷积1层]权重
void InitializationWeightInputToConvolution1()
{
	int m = 0;
	int n = 0;

	for(m = 0; m < ConvolutionKernel * ConvolutionKernel; m++)
	{
		//WeightInputToConvolution1[m] = (rand()/(double)(RAND_MAX))/500;		
		WeightInputToConvolution1[m] = 0;									
		//WeightInputToConvolution1[m] = 1023.9754277741900000;				//78.0700%版权重的平均数
		//WeightInputToConvolution1[m] = -0.1964562163147680;				//78.0700%版权重的中位数
	}

	//【输入层-卷积1层】权重翻转180度
	//for(m = 0, n = ConvolutionKernel * ConvolutionKernel - 1; m < ConvolutionKernel * ConvolutionKernel, -1 < n ; m++, n--)
	//{
	//	WeightRot180InputToConvolution1[n] = WeightInputToConvolution1[m];
	//}
}


//随机初始化[卷积1层-卷积2层]权重
void InitializationWeightConvolution1ToConvolution2()
{
	int m, n = 0;

	for(m = 0; m < ConvolutionKernel * ConvolutionKernel; m++)
	{
		//WeightConvolution1ToConvolution2[m] = (rand()/(double)(RAND_MAX))/100;	
		WeightConvolution1ToConvolution2[m] = 0;									
		//WeightConvolution1ToConvolution2[m] = -0.1106441036656980;				//78.0700%版权重的平均数
		//WeightConvolution1ToConvolution2[m] = 0.0477659333993450;					//78.0700%版权重的中位数
	}

	//【卷积1层-卷积2层】权重翻转180度
	for(m = 0, n = ConvolutionKernel * ConvolutionKernel - 1; m < ConvolutionKernel * ConvolutionKernel, -1 < n ; m++, n--)
	{
		WeightRot180Convolution1ToConvolution2[n] = WeightConvolution1ToConvolution2[m];
	}
}


//随机初始化[池化3层-输出层]权重
void InitializationWeightSubsampling3ToOutput()
{
	int m = 0;

	for(m = 0; m < NeuronNumSubsampling3 * NeuronNumSubsampling3 * NeuronNumOutput; m++)
	{
		//WeightSubsampling3ToOutput[m] = (rand()/(double)(RAND_MAX))/100;			
		WeightSubsampling3ToOutput[m] = 0;										
		//WeightSubsampling3ToOutput[m] = -0.3302111820947480;						//78.0700%版权重的平均数
		//WeightSubsampling3ToOutput[m] = -0.3983054846310040;						//78.0700%版权重的中位数
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
	int n = 0;
	int temp = 0;

 
	//网络架构：输入层 ——> 卷积1层 ——> 卷积2层 ——> 池化3层 ——> 输出层

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


	////////////////计算卷积2层神经元的值////////////////
	for(y = 0; y < NeuronNumConvolution2; y++)
	{
		for (x = 0; x < NeuronNumConvolution2; x++)
		{
			NetConvolution2[y * NeuronNumConvolution2 + x] = 0;
		}
	}

	for(y = 0; y < NeuronNumConvolution2; y++)
	{
		for (x = 0; x < NeuronNumConvolution2; x++)
		{
			for(j = 0; j < ConvolutionKernel; j++)
			{
				for (i = 0; i < ConvolutionKernel; i++)
				{
					NetConvolution2[y * NeuronNumConvolution2 + x] += SigmoidConvolution1[(y + j) * NeuronNumConvolution1 + (x + i)] * WeightConvolution1ToConvolution2[j * ConvolutionKernel + i];
				}
			}
		}
	}


	////////////////计算卷积2层的激活函数值////////////////
	for(y = 0; y < NeuronNumConvolution2; y++)
	{
		for (x = 0; x < NeuronNumConvolution2; x++)
		{
			SigmoidConvolution2[y * NeuronNumConvolution2 + x] = 1 / (1 + exp( - NetConvolution2[y * NeuronNumConvolution2 + x])); 
		}
	}


	////////////////计算池化3层神经元的值////////////////
	for (temp = 0; temp < NeuronNumSubsampling3 * NeuronNumSubsampling3; temp++)
	{
		NetSubsampling3[temp] = 0;
	}

	for(y = 0; y < NeuronNumSubsampling3; y++)
	{
		for (x = 0; x < NeuronNumSubsampling3; x++)
		{
			maxSubsampling3[y * NeuronNumSubsampling3 + x].Value = 0;
			maxSubsampling3[y * NeuronNumSubsampling3 + x].x = 0;
			maxSubsampling3[y * NeuronNumSubsampling3 + x].y = 0;
			maxSubsampling3[y * NeuronNumSubsampling3 + x].i = 0;
			maxSubsampling3[y * NeuronNumSubsampling3 + x].j = 0;

			for(j = 0; j < Filter; j++)
			{
				for (i = 0; i < Filter; i++)
				{
					//求出每4*4过滤器中的最大值，组成池化层
					if (SigmoidConvolution2[(y * Filter + j) * NeuronNumConvolution2 + (x * Filter + i)] > maxSubsampling3[y * NeuronNumSubsampling3 + x].Value)
					{
						maxSubsampling3[y * NeuronNumSubsampling3 + x].Value = SigmoidConvolution2[(y * Filter + j) * NeuronNumConvolution2 + (x * Filter + i)];
						maxSubsampling3[y * NeuronNumSubsampling3 + x].x = x;
						maxSubsampling3[y * NeuronNumSubsampling3 + x].y = y;
						maxSubsampling3[y * NeuronNumSubsampling3 + x].i = i;
						maxSubsampling3[y * NeuronNumSubsampling3 + x].j = j;
					}
				}
			}
			NetSubsampling3[y * NeuronNumSubsampling3 + x] = SigmoidConvolution2[(maxSubsampling3[y * NeuronNumSubsampling3 + x].y * Filter + maxSubsampling3[y * NeuronNumSubsampling3 + x].j) * NeuronNumConvolution2 + (maxSubsampling3[y * NeuronNumSubsampling3 + x].x * Filter + maxSubsampling3[y * NeuronNumSubsampling3 + x].i)]; 
		}
	}


	////////////////计算池化3层的激活函数////////////////
	for(y = 0; y < NeuronNumSubsampling3; y++)
	{
		for (x = 0; x < NeuronNumSubsampling3; x++)
		{
			SigmoidSubsampling3[y * NeuronNumSubsampling3 + x] = NetSubsampling3[y * NeuronNumSubsampling3 + x];
		}
	}


	////////////////计算输出层神经元的值////////////////
	for (n = 0; n < NeuronNumOutput; n++)
	{
		NetOutput[n] = 0;
	}

	for(n = 0; n < NeuronNumOutput; n++) //输出层神经元个数为NeuronNumOutput
	{
		for(y = 0; y < NeuronNumSubsampling3; y++)
		{
			for (x = 0; x < NeuronNumSubsampling3; x++)
			{
				NetOutput[n] += 
					SigmoidSubsampling3[y * NeuronNumSubsampling3 + x] * 
					WeightSubsampling3ToOutput[n * NeuronNumSubsampling3 * NeuronNumSubsampling3 + (y * NeuronNumSubsampling3 + x)]; //池化3层和输出层间的神经元要一一连（全排列）
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
	int m, n = 0;
	int temp = 0;
	int c, d, s, t = 0;

	ClearTarget();
	target[Label] = 1; //目标输出值


	//**************************输出层---->池化3层的权值更新（全连接）**************************
	
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

	////////////NetOutput[n]---->WeightSubsampling3ToOutput[n]
	for(m = 0; m < NeuronNumOutput; m++)
	{
		for(y = 0; y < NeuronNumSubsampling3; y++)
		{
			for(x = 0; x < NeuronNumSubsampling3; x++)
			{
				PD_NetOutputToWeightSubsampling3ToOutput[m * NeuronNumSubsampling3 * NeuronNumSubsampling3 + (y * NeuronNumSubsampling3 + x)] = SigmoidSubsampling3[y * NeuronNumSubsampling3 + x];
			}
		}
	}

	for(m = 0; m < NeuronNumOutput; m++)
	{
		for(y = 0; y < NeuronNumSubsampling3; y++)
		{
			for(x = 0; x < NeuronNumSubsampling3; x++)
			{
				////////////链式法则求偏导
				PD_WeightSubsampling3ToOutput[m * NeuronNumSubsampling3 * NeuronNumSubsampling3 + (y * NeuronNumSubsampling3 + x)] = 
					Delta_Output[m] * PD_NetOutputToWeightSubsampling3ToOutput[m * NeuronNumSubsampling3 * NeuronNumSubsampling3 + (y * NeuronNumSubsampling3 + x)];

				//旧权重保留0.9
				OldWeightSubsampling3ToOutput[m * NeuronNumSubsampling3 * NeuronNumSubsampling3 + (y * NeuronNumSubsampling3 + x)] = 
					WeightSubsampling3ToOutput[m * NeuronNumSubsampling3 * NeuronNumSubsampling3 + (y * NeuronNumSubsampling3 + x)] * OldWeightPercentage; 

				////////////更新WeightSubsampling3ToOutput[m]的值
				WeightSubsampling3ToOutput[m * NeuronNumSubsampling3 * NeuronNumSubsampling3 + (y * NeuronNumSubsampling3 + x)] = 
					WeightSubsampling3ToOutput[m * NeuronNumSubsampling3 * NeuronNumSubsampling3 + (y * NeuronNumSubsampling3 + x)] - LearningRate * PD_WeightSubsampling3ToOutput[m * NeuronNumSubsampling3 * NeuronNumSubsampling3 + (y * NeuronNumSubsampling3 + x)];

				//新权重是保留0.1*新+0.9*旧
				WeightSubsampling3ToOutput[m * NeuronNumSubsampling3 * NeuronNumSubsampling3 + (y * NeuronNumSubsampling3 + x)] = 
					WeightSubsampling3ToOutput[m * NeuronNumSubsampling3 * NeuronNumSubsampling3 + (y * NeuronNumSubsampling3 + x)] * NewWeightPercentage + 
					OldWeightSubsampling3ToOutput[m * NeuronNumSubsampling3 * NeuronNumSubsampling3 + (y * NeuronNumSubsampling3 + x)]; 
			}
		}
	}







	
	
	//**************************池化3层---->卷积2层（未涉及权值）从缩小后的误差δ，还原前一次较大区域对应的误差**************************
	for(temp = 0; temp < NeuronNumSubsampling3 * NeuronNumSubsampling3; temp++)
	{
		PD_ErrorToNetSubsampling3[temp] = 0;
	}

	//****************池化3层误差Delta_Subsampling3****************
	for(y = 0; y < NeuronNumSubsampling3; y++)
	{
		for(x = 0; x < NeuronNumSubsampling3; x++)
		{
			for(m = 0; m < NeuronNumOutput; m++)
			{
				//NetOutput[n]---->SigmoidSubsampling3[n]
				PD_NetOutputToSigmoidSubsampling3[m * NeuronNumSubsampling3 * NeuronNumSubsampling3 + (y * NeuronNumSubsampling3 + x)] = 
					WeightSubsampling3ToOutput[m * NeuronNumSubsampling3 * NeuronNumSubsampling3 + (y * NeuronNumSubsampling3 + x)];

				//SigmoidSubsampling3[n]---->NetSubsampling3[n]
				PD_SigmoidSubsampling3ToNetSubsampling3[y * NeuronNumSubsampling3 + x] = 1;

				////////////Error[n]---->NetSubsampling3[n]
				PD_ErrorToNetSubsampling3[y * NeuronNumSubsampling3 + x] += 
					Delta_Output[m] * 
					PD_NetOutputToSigmoidSubsampling3[m * NeuronNumSubsampling3 * NeuronNumSubsampling3 + (y * NeuronNumSubsampling3 + x)] * 
					PD_SigmoidSubsampling3ToNetSubsampling3[y * NeuronNumSubsampling3 + x];

				//池化3层的误差值＝总误差对池化3层神经元值求偏导
				Delta_Subsampling3[y * NeuronNumSubsampling3 + x] = PD_ErrorToNetSubsampling3[y * NeuronNumSubsampling3 + x];
			}
		}
	}

	//****************卷积2层误差Delta_Convolution2****************
	//把池化3层的误差值维度还原成上一层（卷积2层）的误差值维度，即除了填充每4格中的最大误差值外，其余位置补0
	for(y = 0; y < NeuronNumSubsampling3; y++)
	{
		for (x = 0; x < NeuronNumSubsampling3; x++)
		{
			for(j = 0; j < Filter; j++)
			{
				for (i = 0; i < Filter; i++)
				{
					upsample_Delta_Subsampling3[(y * Filter + j) * NeuronNumConvolution2 + (x * Filter + i)] = 0;
					upsample_Delta_Subsampling3[(maxSubsampling3[y * NeuronNumSubsampling3 + x].y * Filter + maxSubsampling3[y * NeuronNumSubsampling3 + x].j) * NeuronNumConvolution2 + (maxSubsampling3[y * NeuronNumSubsampling3 + x].x * Filter + maxSubsampling3[y * NeuronNumSubsampling3 + x].i)] = Delta_Subsampling3[y * NeuronNumSubsampling3 + x];
				}
			}
		}
	}

	for(y = 0; y < NeuronNumSubsampling3; y++)
	{
		for (x = 0; x < NeuronNumSubsampling3; x++)
		{
			for(j = 0; j < Filter; j++)
			{
				for (i = 0; i < Filter; i++)
				{
					//SigmoidConvolution2[n]---->NetConvolution2[n]
					PD_SigmoidConvolution2ToNetConvolution2[(y * Filter + j) * NeuronNumConvolution2 + (x * Filter + i)] =
						SigmoidConvolution2[(y * Filter + j) * NeuronNumConvolution2 + (x * Filter + i)] * 
						(1 - SigmoidConvolution2[(y * Filter + j) * NeuronNumConvolution2 + (x * Filter + i)]); 

					//卷积2层(池化3层的前一层)的误差值δ＝对池化3层的误差值进行upsample还原 * 卷积2层的激活值对神经元值求偏导
					PD_ErrorToNetConvolution2[(y * Filter + j) * NeuronNumConvolution2 + (x * Filter + i)] =
						//对池化3层的误差值进行upsample还原
						upsample_Delta_Subsampling3[(y * Filter + j) * NeuronNumConvolution2 + (x * Filter + i)] * 
						//卷积2层的激活值对神经元值求偏导
						PD_SigmoidConvolution2ToNetConvolution2[(y * Filter + j) * NeuronNumConvolution2 + (x * Filter + i)];

					//卷积2层误差Delta_Convolution2
					Delta_Convolution2[(y * Filter + j) * NeuronNumConvolution2 + (x * Filter + i)] = PD_ErrorToNetConvolution2[(y * Filter + j) * NeuronNumConvolution2 + (x * Filter + i)];
				}
			}
		}
	}







	
	
	//**************************卷积2层---->卷积1层的权值更新**************************
	for (temp = 0; temp < ConvolutionKernel * ConvolutionKernel; temp++)
	{
		PD_WeightConvolution1ToConvolution2[temp] = 0;
	}

	for(t = 0; t < ConvolutionKernel; t++)
	{
		for (s = 0; s < ConvolutionKernel; s++)
		{
			for(d = 0; d < NeuronNumConvolution2; d++)
			{
				for (c = 0; c < NeuronNumConvolution2; c++)
				{
					PD_WeightConvolution1ToConvolution2[t * ConvolutionKernel + s] += SigmoidConvolution1[(t + d) * NeuronNumConvolution1 + (s + c)] * Delta_Convolution2[d * NeuronNumConvolution2 + c];
				}
			}
		}
	}

	for(t = 0; t < ConvolutionKernel; t++)
	{
		for (s = 0; s < ConvolutionKernel; s++)
		{
			//旧权重保留0.9
			OldWeightConvolution1ToConvolution2[t * ConvolutionKernel + s] = WeightConvolution1ToConvolution2[t * ConvolutionKernel + s] * OldWeightPercentage; 

			////////////更新WeightConvolution1ToConvolution2[m]的值
			WeightConvolution1ToConvolution2[t * ConvolutionKernel + s] = WeightConvolution1ToConvolution2[t * ConvolutionKernel + s] - LearningRate * PD_WeightConvolution1ToConvolution2[t * ConvolutionKernel + s];

			//新权重是保留0.1*新+0.9*旧
			WeightConvolution1ToConvolution2[t * ConvolutionKernel + s] = WeightConvolution1ToConvolution2[t * ConvolutionKernel + s] * NewWeightPercentage + OldWeightConvolution1ToConvolution2[t * ConvolutionKernel + s]; 
		}
	}

	//【卷积1层-卷积2层】权重翻转180度
	for(m = 0, n = ConvolutionKernel * ConvolutionKernel - 1; m < ConvolutionKernel * ConvolutionKernel, -1 < n ; m++, n--)
	{
		WeightRot180Convolution1ToConvolution2[n] = WeightConvolution1ToConvolution2[m];
	}









	//**************************卷积2层---->卷积1层**************************
	for(temp = 0; temp < NeuronNumConvolution2 * NeuronNumConvolution2; temp++)
	{
		PD_ErrorToNetConvolution2[temp] = 0;
	}


	//在卷积2层的误差值矩阵周围，填充4圈0，为了符合梯度计算：∇a11=δ11 * w11
	for(y = 0; y < NeuronNumConvolution2 + 2 * (ConvolutionKernel - 1); y++)
	{
		for (x = 0; x < NeuronNumConvolution2 + 2 * (ConvolutionKernel - 1); x++)
		{
			Padding_Delta_Convolution2[y * (NeuronNumConvolution2 + 2 * (ConvolutionKernel - 1)) + x] = 0;
		}
	}

	for(y = ConvolutionKernel - 1, j = 0; y < NeuronNumConvolution1, j < NeuronNumConvolution2; y++, j++)
	{
		for (x = ConvolutionKernel - 1, i = 0; x < NeuronNumConvolution1, i < NeuronNumConvolution2; x++, i++)
		{
			Padding_Delta_Convolution2[y * (NeuronNumConvolution2 + 2 * (ConvolutionKernel - 1)) + x] = Delta_Convolution2[j * NeuronNumConvolution2 + i]; 
		}
	}


	//****************卷积1层误差Delta_Convolution1****************
	for(y = 0; y < NeuronNumConvolution1; y++)
	{
		for (x = 0; x < NeuronNumConvolution1; x++)
		{
			for(j = 0; j < ConvolutionKernel; j++)
			{
				for (i = 0; i < ConvolutionKernel; i++)
				{
					//SigmoidConvolution1[n]---->NetConvolution1[n]
					PD_SigmoidConvolution1ToNetConvolution1[y * NeuronNumConvolution1 + x] =
						SigmoidConvolution1[y * NeuronNumConvolution1 + x] * 
						(1 - SigmoidConvolution1[y * NeuronNumConvolution1 + x]); 

					////////////Error[n]---->NetConvolution1[n]
					PD_ErrorToNetConvolution1[y * NeuronNumConvolution1 + x] += 
						Padding_Delta_Convolution2[(y + j) * (NeuronNumConvolution1 + ConvolutionKernel - 1) + (x + i)] *	//在【Delta_Convolution2】矩阵周围填充4圈0，得到【Padding_Delta_Convolution2】
						WeightRot180Convolution1ToConvolution2[j * ConvolutionKernel + i] *									//【卷积1层-卷积2层】卷积核翻转180°
						PD_SigmoidConvolution1ToNetConvolution1[y * NeuronNumConvolution1 + x];								//SigmoidConvolution1[n]---->NetConvolution1[n]求偏导

					//卷积1层的误差值＝总误差对卷积1层神经元值求偏导
					Delta_Convolution1[y * NeuronNumConvolution1 + x] = PD_ErrorToNetConvolution1[y * NeuronNumConvolution1 + x];
				}
			}
		}
	}









	//**************************卷积1层---->输入层（卷积核）的权值更新**************************
	for (temp = 0; temp < ConvolutionKernel * ConvolutionKernel; temp++) 
	{
		PD_WeightInputToConvolution1[temp] = 0;
	}

	for(t = 0; t < ConvolutionKernel; t++) 
	{
		for (s = 0; s < ConvolutionKernel; s++) 
		{
			for(d = 0; d < NeuronNumConvolution1; d++)
			{
				for (c = 0; c < NeuronNumConvolution1; c++)
				{
					PD_WeightInputToConvolution1[t * ConvolutionKernel + s] += iPicture[t + d][s + c] * Delta_Convolution1[d * NeuronNumConvolution1 + c]; 
				}
			}
		}
	}

	for(t = 0; t < ConvolutionKernel; t++)
	{
		for (s = 0; s < ConvolutionKernel; s++) 
		{
			//旧权重保留0.9
			OldWeightInputToConvolution1[t * ConvolutionKernel + s] = WeightInputToConvolution1[t * ConvolutionKernel + s] * OldWeightPercentage; 

			////////////更新WeightInputToConvolution1[m]的值
			WeightInputToConvolution1[t * ConvolutionKernel + s] = WeightInputToConvolution1[t * ConvolutionKernel + s] - LearningRate * PD_WeightInputToConvolution1[t * ConvolutionKernel + s];

			//新权重是保留0.1*新+0.9*旧
			WeightInputToConvolution1[t * ConvolutionKernel + s] = WeightInputToConvolution1[t * ConvolutionKernel + s] * NewWeightPercentage + OldWeightInputToConvolution1[t * ConvolutionKernel + s]; 
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