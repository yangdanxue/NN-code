#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define pixels						28		//图片像素是28*28
#define NeuronNumHidden				784		//隐藏层神经元个数
#define NeuronNumOutput				10		//输出层神经元个数
#define TrainTimes					1	    //训练次数
#define TestNumber					60000	//总测试图片的数量
#define OldWeightPercentage			0.9   	//反向传播更新权重时旧权重占比
#define NewWeightPercentage			0.1   	//反向传播更新权重时新权重占比

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

//各层权重
double WeightInputToHidden[NeuronNumHidden * pixels * pixels] = {0};		
double WeightHiddenToOutput[NeuronNumHidden * NeuronNumOutput] = {0};		

//各层反向传播后旧权重保留的比例
double OldWeightInputToHidden[NeuronNumHidden * pixels * pixels] = {0};		
double OldWeightHiddenToOutput[NeuronNumHidden * NeuronNumOutput] = {0};	

//各层偏置项
double Bias_Input = 0;
double Bias_Hidden = 0;

//各层神经元值
double NetHidden[NeuronNumHidden] = {0};	
double NetOutput[NeuronNumOutput] = {0};	

//各层激活函数值（Sigmoid函数）
double SigmoidHidden[NeuronNumHidden] = {0};
double SigmoidOutput[NeuronNumOutput] = {0};

int target[NeuronNumOutput] = {0};				//目标输出值
double ErrorOfOutput[NeuronNumOutput] = {0};	//每个输出层神经元的输出值误差（实际输出值与目标值的差）

//各层误差值
double PD_ErrorToNetOutput[NeuronNumOutput] = {0};
double Delta_Output[NeuronNumOutput] = {0};
double PD_ErrorToNetHidden[NeuronNumHidden] = {0};
double Delta_Hidden[NeuronNumHidden] = {0};

//隐含层---->输出层的权值更新
double PD_WeightHiddenToOutput[NeuronNumHidden * NeuronNumOutput] = {0}; //求偏导
double PD_ErrorToSigmoidOutput[NeuronNumOutput] = {0};
double PD_SigmoidOutputToNetOutput[NeuronNumOutput] = {0};
double PD_NetOutputToWeightHiddenToOutput[NeuronNumHidden * NeuronNumOutput] = {0};

//输入层---->隐含层的权值更新
double PD_WeightInputToHidden[NeuronNumHidden * pixels * pixels] = {0};
double PD_SigmoidHiddenToNetHidden[NeuronNumHidden] = {0};
double PD_NetHiddenToWeightInputToHidden[NeuronNumHidden * pixels * pixels] = {0};
double PD_NetOutputToSigmoidHidden[NeuronNumHidden * NeuronNumOutput] = {0};

double LearningRate = 0.019; //学习速率

int Correct = 0; //识别正确数


void main()
{
	int i = 0;
	float CorrectRate = 0;
	int MaxCorrect = 0;
	int times = 0;

	//RandomInitializationWeight();
	ReadWeightData();

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
	//		WriteWeightData();
	//	}
	//}
}

//读文件——使用保存的权重
void ReadWeightData()
{
	int iNum = 0; //输入层-隐藏层间的神经元个数

	FILE *fpReadItoH = fopen("d:\\WeightItoHData201810161953.wet","rb");
	FILE *fpReadHtoO = fopen("d:\\WeightHtoOData201810161953.wet","rb");
	if(fpReadItoH == NULL)
	{
		printf("WeightItoHData201810161953.wet文件不存在！\n");
		return;
	}
	if(fpReadHtoO == NULL)
	{
		printf("WeightHtoOData201810161953.wet文件不存在！\n");
		return;
	}

	//下面是读数据，将读到的数据存到数组WeightInputToHidden[NeuronNumHidden]中
	iNum = fread(WeightInputToHidden, sizeof(double), NeuronNumHidden * pixels * pixels, fpReadItoH);
	if(iNum != NeuronNumHidden * pixels * pixels)
	{
		printf("读数据存到数组WeightInputToHidden[NeuronNumHidden * pixels * pixels]错误！！！\n");
	}
	printf("读输入层-隐藏层间的权重数据成功\n");

	//下面是读数据，将读到的数据存到数组WeightHiddenToOutput[NeuronNumHidden * 10]中
	iNum = fread(WeightHiddenToOutput, sizeof(double), NeuronNumHidden * NeuronNumOutput, fpReadHtoO);
	if(iNum != NeuronNumHidden * NeuronNumOutput)
	{
		printf("读数据存到数组WeightHiddenToOutput[NeuronNumHidden * NeuronNumOutput]错误！！！\n");
	}
	printf("读隐藏层-输出层间的权重数据成功\n");


	fclose(fpReadItoH);
	fclose(fpReadHtoO);

	return;
}


//写文件——保留当前调好的权重
void WriteWeightData()
{
	int iNum = 0;
	FILE *fpWriteInputToHidden = fopen("d:\\WeightItoHData201810161953.wet","wb+");
	FILE *fpWriteHiddenToOutput = fopen("d:\\WeightHtoOData201810161953.wet","wb+");
	if(fpWriteInputToHidden == NULL)
	{
		printf("创建WeightItoHData201810161953.wet文件失败！\n");
		return 0;
	}
	if(fpWriteHiddenToOutput == NULL)
	{
		printf("创建WeightHtoOData201810161953.wet文件失败！\n");
		return 0;
	}

	//将输入层-隐藏层间的权重写入到WeightItoHData.wet文件中
	iNum = fwrite(WeightInputToHidden, sizeof(double), NeuronNumHidden * pixels * pixels, fpWriteInputToHidden);
	if (iNum != NeuronNumHidden * pixels * pixels)
	{
		printf("写WeightItoHData201810161953.wet文件失败！");
	}
	printf("所有输入层-隐藏层间的权重数据已经保存\n");


	//将隐藏层-输出层间的权重写入到WeightHtoOData.wet文件中
	iNum = fwrite(WeightHiddenToOutput, sizeof(double), NeuronNumHidden * NeuronNumOutput, fpWriteHiddenToOutput);
	if (iNum != NeuronNumHidden * NeuronNumOutput)
	{		
		printf("写WeightHtoOData201810161953.wet文件失败！");
	}
	printf("所有隐藏层-输出层间的权重数据已经保存\n");

	fclose(fpWriteInputToHidden);
	fclose(fpWriteHiddenToOutput);
}


//随机初始化各值(91.4967％版权重，取0.5)
void RandomInitializationWeight()
{
	int m, n = 0;

	for(m = 0; m < NeuronNumHidden * pixels * pixels; m++)
	{
		//WeightInputToHidden[m] = 0; //【输入层-隐藏层】之间任意2个神经元间的权重W
		WeightInputToHidden[m] = (rand()/(double)(RAND_MAX))/2000; //【输入层-输出层】之间任意2个神经元间的权重W
	}

	for(n = 0; n < NeuronNumHidden * NeuronNumOutput; n++)
	{
		//WeightHiddenToOutput[n] = 0; //【隐藏层-输出层】之间任意2个神经元间的权重W
		WeightHiddenToOutput[n] = (rand()/(double)(RAND_MAX))/400; //【输入层-输出层】之间任意2个神经元间的权重W
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
	int m, n = 0;


	////////////////计算隐藏层神经元的值////////////////
	for (m = 0; m < NeuronNumHidden; m++)
	{
		NetHidden[m] = 0;
	}

	for (m = 0; m < NeuronNumHidden; m++)
	{
		for(j = 0; j < pixels; j++)
		{
			for (i = 0; i < pixels; i++)
			{
				NetHidden[m] += iPicture[j][i] * WeightInputToHidden[m * pixels * pixels + (j * pixels + i)];
			}
		}
	}

	//for (m = 0; m < NeuronNumHidden; m++)
	//{
	//	NetHidden[m] = NetHidden[m] / 32;
	//}


	//for (m = 0; m < NeuronNumHidden; m++)
	//{
	//	NetHidden[m] = NetHidden[m] + Bias_Input * 1;
	//}


	////////////////计算隐藏层sigmoid函数值)////////////////
	for(m = 0; m < NeuronNumHidden; m++)
	{
		SigmoidHidden[m] = 1 / (1 + exp( - NetHidden[m]));
	}


	////////////////计算输出层神经元的值////////////////
	for (n = 0; n < NeuronNumOutput; n++)
	{
		NetOutput[n] = 0;
	}

	for(n = 0; n < NeuronNumOutput; n++) //输出层神经元个数为10
	{
		for(m = 0; m < NeuronNumHidden; m++) //隐藏层神经元个数为NeuronNumHidden
		{
			NetOutput[n] += SigmoidHidden[m] * WeightHiddenToOutput[n * NeuronNumHidden + m]; //隐藏层和输出层间的神经元要一一连接
		}
	}

	//for(n = 0; n < NeuronNumOutput; n++) //输出层神经元个数为10
	//{
	//	NetOutput[n] = NetOutput[n] + Bias_Hidden * 1;
	//}

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
	int m, n = 0;
	int temp = 0;

	ClearTarget();
	target[Label] = 1; //目标输出值


	//*****************反向传播——2.隐含层---->输出层的权值更新（链式法则）*****************

	//****************输出层误差Delta_Output****************
	for(n = 0; n < NeuronNumOutput; n++)
	{
		//Error[total]---->SigmoidOutput[n]
		PD_ErrorToSigmoidOutput[n] = - (target[n] - SigmoidOutput[n]); 

		//SigmoidOutput[n]---->NetOutput[n]
		PD_SigmoidOutputToNetOutput[n] = SigmoidOutput[n] * (1 - SigmoidOutput[n]); 

		//Error[n]---->NetOutput[n]
		PD_ErrorToNetOutput[n] = PD_ErrorToSigmoidOutput[n] * PD_SigmoidOutputToNetOutput[n]; 

		//输出层的误差值＝总误差对输出层神经元值求偏导
		Delta_Output[n] = PD_ErrorToNetOutput[n];
	}		










	//NetOutput[m]---->WeightHiddenToOutput[m]
	for(n = 0; n < NeuronNumOutput; n++)
	{
		for(m = 0; m < NeuronNumHidden; m++)
		{
			PD_NetOutputToWeightHiddenToOutput[n * NeuronNumHidden + m] = SigmoidHidden[m]; //[m]改为[n * NeuronNumHidden + m]
		}
	}		

	for(n = 0; n < NeuronNumOutput; n++)
	{
		for(m = 0; m < NeuronNumHidden; m++)
		{
			//链式法则求偏导
			PD_WeightHiddenToOutput[n * NeuronNumHidden + m] = Delta_Output[n] * PD_NetOutputToWeightHiddenToOutput[n * NeuronNumHidden + m];

			//旧权重保留0.9
			OldWeightHiddenToOutput[n * NeuronNumHidden + m] = WeightHiddenToOutput[n * NeuronNumHidden + m] * OldWeightPercentage; 

			//更新WeightHiddenToOutput[n]的值
			WeightHiddenToOutput[n * NeuronNumHidden + m] = WeightHiddenToOutput[n * NeuronNumHidden + m] - LearningRate * PD_WeightHiddenToOutput[n * NeuronNumHidden + m];

			//新权重是各保留0.1
			WeightHiddenToOutput[n * NeuronNumHidden + m] = WeightHiddenToOutput[n * NeuronNumHidden + m] * NewWeightPercentage + OldWeightHiddenToOutput[n * NeuronNumHidden + m]; 
		}
	}	










	//****************隐藏层误差Delta_Hidden****************
	for (temp = 0; temp < NeuronNumHidden; temp++)
	{
		PD_ErrorToNetHidden[temp] = 0;
	}

	for(m = 0; m < NeuronNumHidden; m++)
	{
		for(n = 0; n < NeuronNumOutput; n++)
		{
			//NetOutput[n]---->SigmoidHidden[n]
			PD_NetOutputToSigmoidHidden[n * NeuronNumHidden + m] = WeightHiddenToOutput[n * NeuronNumHidden + m];

			//SigmoidHidden[m]---->NetHidden[m]
			PD_SigmoidHiddenToNetHidden[m] = SigmoidHidden[m] * (1 - SigmoidHidden[m]);

			//Error[m]---->NetHidden[m]
			PD_ErrorToNetHidden[m] += Delta_Output[n] * PD_NetOutputToSigmoidHidden[n * NeuronNumHidden + m] * PD_SigmoidHiddenToNetHidden[m]; 

			//输出层的误差值＝总误差对输出层神经元值求偏导
			Delta_Hidden[m] = PD_ErrorToNetHidden[m];
		}
	}










	//*****************反向传播——3.输入层---->隐含层的权值更新*****************

	//NetHidden[n]---->WeightInputToHidden[n]
	for (m = 0; m < NeuronNumHidden; m++)
	{
		for(j = 0; j < pixels; j++)
		{
			for (i = 0; i < pixels; i++)
			{
				PD_NetHiddenToWeightInputToHidden[m * pixels * pixels + (j * pixels + i)] = iPicture[j][i];
			}
		}
	}


	for (m = 0; m < NeuronNumHidden; m++)
	{
		for(j = 0; j < pixels; j++)
		{
			for (i = 0; i < pixels; i++)
			{
				PD_WeightInputToHidden[m * pixels * pixels + (j * pixels + i)] = Delta_Hidden[m] * PD_NetHiddenToWeightInputToHidden[m * pixels * pixels + (j * pixels + i)];

				//旧权重保留0.9
				OldWeightInputToHidden[m * pixels * pixels + (j * pixels + i)] = WeightInputToHidden[m * pixels * pixels + (j * pixels + i)] * OldWeightPercentage;

				////更新WeightInputToHidden[m]的值
				WeightInputToHidden[m * pixels * pixels + (j * pixels + i)] = 
					WeightInputToHidden[m * pixels * pixels + (j * pixels + i)] - LearningRate * PD_WeightInputToHidden[m * pixels * pixels + (j * pixels + i)]; 

				//新权重是各保留0.1
				WeightInputToHidden[m * pixels * pixels + (j * pixels + i)] = 
					WeightInputToHidden[m * pixels * pixels + (j * pixels + i)] * NewWeightPercentage + OldWeightInputToHidden[m * pixels * pixels + (j * pixels + i)];
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