#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define pixels				28			//图片像素是28*28
#define unit				10			//每一个单位像素集合共用一个权重，单位集合的像素大小是10*10
#define NumWeight			28*28*10	//隐藏层神经元个数（每一个单位像素集合共用一个权重，组成一个神经元），重复三次
#define TrainTimes			1			//训练次数
#define TestNumber			60000		//总测试图片的数量


int ReadPicData(unsigned char* data, int iNum); //读图片数据文件

void WriteWeightData();				//保留当前调好的权重
void ReadWeightData();				//使用保存的权重
int CalculateNetResult();			//计算网络输出结果
float TestNet();					//测试网络
void TrainNet();					//选择测试或训练
void RandomInitializationWeight();	//随机初始化各值
void CalculateNeuronValue();		//计算各个神经元的值
void BackPropagation();				//反向传播法更新权值
void ClearTarget();					//清理Target[]

unsigned char iPicture[pixels][pixels] = {0};	//导入的MNIST数据库图片数组
unsigned char Label;							//导入的MNIST数据库标签
unsigned char* p = &Label;

double WeightInputToOutput[NumWeight] = {0};		//【输入层-输出层】之间任意2个神经元间的权重W
double OldWeightInputToOutput[NumWeight] =  {0};	//【输入层-输出层】之间任意2个神经元间的旧权重W（为了反向传播后新旧权重各取0.25和0.75）

double NetsumOutput[10] = {0};		//输出层神经元的输入加权和
double SigmoidOutput[10] = {0};		//输出层神经元经激活函数后的输出(此处用到激活函数为sigmoid函数)

int target[10] = {0};				//目标输出值
double EorrorOfOutput[10] = {0};	//每个输出层神经元的输出值误差（实际输出值与目标值的差）

//输入层---->输出层的权值更新
double PD_WeightInputToOutput[NumWeight] = {0}; //求偏导
double PD_EorrorToSigmoidOutput[10] = {0};
double PD_SigmoidOutputToNetsumOutput[10] = {0};
double PD_NetsumOutputToWeightInputToOutput[NumWeight] = {0};

float LearningRate = 0.0005; //学习速率

int Correct = 0; //识别正确数


void main()
{
	int a, i = 0;
	float CorrectRate;
	int MaxCorrect = 0;

	//RandomInitializationWeight();
	ReadWeightData();
	printf("学习速率：%f\n" , LearningRate);

	CorrectRate = TestNet();

	printf("正确数：%d 正确率：%.4f%%\n", Correct, CorrectRate * 100);

	//for ( ;  ; )
	//{
	//	i++;
	//	printf("第%d次训练：\n", i);

	//	TrainNet();//训练网络

	//	CorrectRate = TestNet();
	//	printf("正确数：%d 正确率：%.4f%%\n", Correct, CorrectRate * 100);

	//	if (MaxCorrect < Correct)
	//	{
	//		MaxCorrect = Correct;
	//		WriteWeightData();
	//	}
	//}
}

//读文件——使用保存的权重
void ReadWeightData()
{
	int iNum = 0; //输入层-输出层间的神经元个数

	FILE *fpReadItoO = fopen("d:\\WeightItoOData201812202048.wet","rb");
	if(fpReadItoO == NULL)
	{
		printf("WeightItoOData201812202048.wet文件不存在！\n");
		return;
	}

	//下面是读数据，将读到的数据存到数组WeightInputToOutput[NumWeight]中
	iNum = fread(WeightInputToOutput, sizeof(double), NumWeight, fpReadItoO);
	if(iNum != NumWeight)
	{
		printf("读数据存到数组WeightInputToOutput[NumWeight]错误！！！\n");
	}
	printf("读输入层-输出层间的权重数据成功\n");

	fclose(fpReadItoO);

	return;
}

//写文件——保留当前调好的权重
void WriteWeightData()
{
	int iNum = 0;
	FILE *fpWriteInputToOutput = fopen("d:\\WeightItoOData201812202048.wet","wb+");
	if(fpWriteInputToOutput == NULL)
	{
		printf("创建WeightItoOData201812202048.wet文件失败！\n");
		return 0;
	}

	//将输入层-输出层间的权重写入到WeightItoOData201812202048.wet文件中
	iNum = fwrite(WeightInputToOutput, sizeof(double), NumWeight, fpWriteInputToOutput);
	if (iNum != NumWeight)
	{
		printf("写WeightItoOData201812202048.wet文件失败！");
	}
	printf("所有输入层-输出层间的权重数据已经保存\n");

	fclose(fpWriteInputToOutput);
}

//随机初始化各值
void RandomInitializationWeight()
{
	int m = 0;

	srand((unsigned int)time(NULL)); //用系统时间做随机种子

	for(m = 0; m < NumWeight; m++)
	{
		WeightInputToOutput[m] = (rand()/(double)(RAND_MAX))/20000; //【输入层-输出层】之间任意2个神经元间的权重W

		if (WeightInputToOutput[m] == 0)
		{
			WeightInputToOutput[m] = (rand()/(double)(RAND_MAX))/20000; //【输入层-输出层】之间任意2个神经元间的权重W
		}
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
	int m = 0;

	////////////////计算输出层神经元的值////////////////
	for (m = 0; m < 10; m++)
	{
		NetsumOutput[m] = 0;
	}

	for(m = 0; m < 10; m++) //输出层神经元个数为10
	{
		for(j = 0; j < pixels; j++)
		{
			for (i = 0; i < pixels; i++)
			{
				NetsumOutput[m] += iPicture[j][i] * WeightInputToOutput[m * pixels * pixels + (j * pixels + i)]; //输入层和输出层间的神经元全连接
			}
		}
	}

	////////////////计算输出层sigmoid函数值////////////////
	for(m = 0; m < 10; m++)
	{
		SigmoidOutput[m] = 1 / (1 + exp( - NetsumOutput[m]));
	}
}

//反向传播
void BackPropagation()
{
	int i, j = 0;					
	int m, n = 0;

	ClearTarget();
	target[Label] = 1; //目标输出值


	//*****************反向传播——2.输入层---->输出层的权值更新（链式法则）*****************
	//////////想知道权重参数WeightInputToOutput[m]对整体误差产生了多少影响，可以用整体误差对WeightInputToOutput[m]求偏导求出

	for(m = 0; m < 10; m++)
	{
		//SigmoidOutput[m]对整体误差产生了多少影响
		PD_EorrorToSigmoidOutput[m] = - (target[m] - SigmoidOutput[m]) / 5; 

		//out_o[m]---->NetsumOutput[m]
		PD_SigmoidOutputToNetsumOutput[m] = SigmoidOutput[m] * (1 - SigmoidOutput[m]); 
	}		

	//NetsumOutput[n]---->WeightInputToOutput[n]
	for(m = 0; m < 10; m++)
	{
		for(j = 0; j < pixels; j++)
		{
			for(i = 0; i < pixels; i++)
			{
				PD_NetsumOutputToWeightInputToOutput[m * pixels * pixels + (j * pixels + i)] = iPicture[j][i];
			}
		}
	}		

	for(m = 0; m < 10; m++)
	{
		for(j = 0; j < pixels; j++)
		{
			for(i = 0; i < pixels; i++)
			{
				//链式法则求偏导
				PD_WeightInputToOutput[m * pixels * pixels + (j * pixels + i)] = 
					PD_EorrorToSigmoidOutput[m] * 
					PD_SigmoidOutputToNetsumOutput[m] * 
					PD_NetsumOutputToWeightInputToOutput[m * pixels * pixels + (j * pixels + i)];

				//旧权重保留0.75
				OldWeightInputToOutput[m * pixels * pixels + (j * pixels + i)] = WeightInputToOutput[m * pixels * pixels + (j * pixels + i)] * 0.9; 

				//更新WeightInputToOutput[m]的值
				WeightInputToOutput[m * pixels * pixels + (j * pixels + i)] = 
					WeightInputToOutput[m * pixels * pixels + (j * pixels + i)] - LearningRate * PD_WeightInputToOutput[m * pixels * pixels + (j * pixels + i)];

				//新权重是各保留0.25
				WeightInputToOutput[m * pixels * pixels + (j * pixels + i)] = 
					WeightInputToOutput[m * pixels * pixels + (j * pixels + i)] * 0.1 + OldWeightInputToOutput[m * pixels * pixels + (j * pixels + i)]; 
			}
		}
	}		
}


//清理Target[]
void ClearTarget()
{
	int i = 0;

	for (i = 0; i < 10; i++)
	{
		target[i] = 0;
	}
}


//计算网络识别结果
int CalculateNetResult()
{
	int i = 0, result = 0; 
	CalculateNeuronValue(); //计算神经元的值	

	for (i = 1; i < 10; i++)
	{
		if (SigmoidOutput[result] < SigmoidOutput[i])
		{
			result = i;
		}
	}

	return result;//识别结果
}