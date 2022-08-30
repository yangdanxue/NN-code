#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define pixels						28		//图片像素是28*28
#define NeuronNumHidden1			100		//隐藏1层神经元个数
#define NeuronNumHidden2			100		//隐藏2层神经元个数
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
double WeightInputToHidden1[pixels * pixels * NeuronNumHidden1] = {0};
double WeightHidden1ToHidden2[NeuronNumHidden1 * NeuronNumHidden2] = {0};
double WeightHidden2ToOutput[NeuronNumHidden2 * NeuronNumOutput] = {0};		

//各层反向传播后旧权重保留的比例
double OldWeightInputToHidden1[pixels * pixels * NeuronNumHidden1] = {0};
double OldWeightHidden1ToHidden2[NeuronNumHidden1 * NeuronNumHidden2] = {0};
double OldWeightHidden2ToOutput[NeuronNumHidden2 * NeuronNumOutput] = {0};	

//各层偏置项
double Bias_Input = 0;
double Bias_Hidden = 0;

//各层神经元值
double NetHidden1[NeuronNumHidden1] = {0};	
double NetHidden2[NeuronNumHidden2] = {0};	
double NetOutput[NeuronNumOutput] = {0};	

//各层激活函数值（Sigmoid函数）
double SigmoidHidden1[NeuronNumHidden1] = {0};
double SigmoidHidden2[NeuronNumHidden2] = {0};
double SigmoidOutput[NeuronNumOutput] = {0};

int target[NeuronNumOutput] = {0};				//目标输出值
double ErrorOfOutput[NeuronNumOutput] = {0};	//每个输出层神经元的输出值误差（实际输出值与目标值的差）

//各层误差值
double PD_ErrorToNetOutput[NeuronNumOutput] = {0};
double Delta_Output[NeuronNumOutput] = {0};
double PD_ErrorToNetHidden2[NeuronNumHidden2] = {0};
double Delta_Hidden2[NeuronNumHidden2] = {0};
double PD_ErrorToNetHidden1[NeuronNumHidden1] = {0};
double Delta_Hidden1[NeuronNumHidden1] = {0};

//隐2含层---->输出层的权值更新
double PD_WeightHidden2ToOutput[NeuronNumHidden2 * NeuronNumOutput] = {0};
double PD_ErrorToSigmoidOutput[NeuronNumOutput] = {0};
double PD_SigmoidOutputToNetOutput[NeuronNumOutput] = {0};
double PD_NetOutputToWeightHidden2ToOutput[NeuronNumHidden2 * NeuronNumOutput] = {0};

//隐含1层---->隐含2层的权值更新
double PD_NetOutputToSigmoidHidden2[NeuronNumHidden2 * NeuronNumOutput] = {0};
double PD_WeightHidden1ToHidden2[NeuronNumHidden1 * NeuronNumHidden2] = {0};
double PD_SigmoidHidden2ToNetHidden2[NeuronNumHidden2] = {0};
double PD_NetHidden2ToWeightHidden1ToHidden2[NeuronNumHidden1 * NeuronNumHidden2] = {0};

//输入层---->隐含1层的权值更新
double PD_NetHidden2ToSigmoidHidden1[NeuronNumHidden1 * NeuronNumHidden2] = {0};
double PD_WeightInputToHidden1[pixels * pixels * NeuronNumHidden1] = {0};
double PD_SigmoidHidden1ToNetHidden1[NeuronNumHidden1] = {0};
double PD_NetHidden1ToWeightInputToHidden1[pixels * pixels * NeuronNumHidden1] = {0};

double LearningRate = 0.03; //学习速率

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
	int iNum = 0; //输入层-隐藏1层间的神经元个数

	FILE *fpReadItoH1 = fopen("d:\\WeightItoH1Data201810191617.wet","rb");
	FILE *fpReadH1toH2 = fopen("d:\\WeightH1toH2Data201810191617.wet","rb");
	FILE *fpReadH2toO = fopen("d:\\WeightH2toOData201810191617.wet","rb");
	if(fpReadItoH1 == NULL)
	{
		printf("WeightItoH1Data201810191617.wet文件不存在！\n");
		return;
	}
	if(fpReadH1toH2 == NULL)
	{
		printf("WeightH1toH2Data201810191617.wet文件不存在！\n");
		return;
	}
	if(fpReadH2toO == NULL)
	{
		printf("WeightH2toOData201810191617.wet文件不存在！\n");
		return;
	}

	//下面是读数据，将读到的数据存到数组WeightInputToHidden11[m]中
	iNum = fread(WeightInputToHidden1, sizeof(double), pixels * pixels * NeuronNumHidden1, fpReadItoH1);
	if(iNum != pixels * pixels * NeuronNumHidden1)
	{
		printf("读数据存到数组WeightInputToHidden1[pixels * pixels * NeuronNumHidden1]错误！！！\n");
	}
	printf("读输入层-隐藏1层间的权重数据成功\n");

	//下面是读数据，将读到的数据存到数组WeightHidden11ToHidden12[m]中
	iNum = fread(WeightHidden1ToHidden2, sizeof(double), NeuronNumHidden1 * NeuronNumHidden2, fpReadH1toH2);
	if(iNum != NeuronNumHidden1 * NeuronNumHidden2)
	{
		printf("读数据存到数组WeightHidden1ToHidden2[NeuronNumHidden1 * NeuronNumHidden2]错误！！！\n");
	}
	printf("读隐藏1层-隐藏2层间的权重数据成功\n");

	//下面是读数据，将读到的数据存到数组WeightHidden2ToOutput[NeuronNumHidden * 10]中
	iNum = fread(WeightHidden2ToOutput, sizeof(double), NeuronNumHidden2 * NeuronNumOutput, fpReadH2toO);
	if(iNum != NeuronNumHidden2 * NeuronNumOutput)
	{
		printf("读数据存到数组WeightHidden2ToOutput[NeuronNumHidden2 * NeuronNumOutput]错误！！！\n");
	}
	printf("读隐藏2层-输出层间的权重数据成功\n");

	fclose(fpReadItoH1);
	fclose(fpReadH1toH2);
	fclose(fpReadH2toO);

	return;
}


//写文件——保留当前调好的权重
void WriteWeightData()
{
	int iNum = 0;
	FILE *fpWriteInputToHidden1 = fopen("d:\\WeightItoH1Data201810191617.wet","wb+");
	FILE *fpWriteHidden1ToHidden2 = fopen("d:\\WeightH1toH2Data201810191617.wet","wb+");
	FILE *fpWriteHidden2ToOutput = fopen("d:\\WeightH2toOData201810191617.wet","wb+");
	if(fpWriteInputToHidden1 == NULL)
	{
		printf("创建WeightItoH1Data201810191617.wet文件失败！\n");
		return 0;
	}
	if(fpWriteHidden1ToHidden2 == NULL)
	{
		printf("创建WeightH1toH2Data201810191617.wet文件失败！\n");
		return 0;
	}
	if(fpWriteHidden2ToOutput == NULL)
	{
		printf("创建WeightH2toOData201810191617.wet文件失败！\n");
		return 0;
	}

	//将输入层-隐藏1层间的权重写入到WeightItoH1Data201810191617.wet文件中
	iNum = fwrite(WeightInputToHidden1, sizeof(double), pixels * pixels * NeuronNumHidden1, fpWriteInputToHidden1);
	if (iNum != pixels * pixels * NeuronNumHidden1)
	{
		printf("写WeightItoH1Data201810191617.wet文件失败！");
	}
	printf("所有输入层-隐藏1层间的权重数据已经保存\n");

	//隐藏1层-隐藏2层间的权重写入到WeightH1toH2Data201810191617.wet文件中
	iNum = fwrite(WeightHidden1ToHidden2, sizeof(double), NeuronNumHidden1 * NeuronNumHidden2, fpWriteHidden1ToHidden2);
	if (iNum != NeuronNumHidden1 * NeuronNumHidden2)
	{
		printf("写WeightH1toH2Data201810191617.wet文件失败！");
	}
	printf("所有隐藏1层-隐藏2层间的权重数据已经保存\n");


	//将隐藏2层-输出层间的权重写入到WeightH2toOData201810191617.wet文件中
	iNum = fwrite(WeightHidden2ToOutput, sizeof(double), NeuronNumHidden2 * NeuronNumOutput, fpWriteHidden2ToOutput);
	if (iNum != NeuronNumHidden2 * NeuronNumOutput)
	{		
		printf("写WeightH2toOData201810191617.wet文件失败！");
	}
	printf("所有隐藏2层-输出层间的权重数据已经保存\n");

	fclose(fpWriteInputToHidden1);
	fclose(fpWriteHidden1ToHidden2);
	fclose(fpWriteHidden2ToOutput);
}


//随机初始化各值
void RandomInitializationWeight()
{
	int m, n, k = 0;

	for(m = 0; m < pixels * pixels * NeuronNumHidden1; m++)
	{
		//WeightInputToHidden1[m] = 0;
		WeightInputToHidden1[m] = (rand()/(double)(RAND_MAX))/2000;
	}

	for(k = 0; k < NeuronNumHidden1 * NeuronNumHidden2; k++)
	{
		//WeightHidden1ToHidden2[k] = 0;
		WeightHidden1ToHidden2[k] = (rand()/(double)(RAND_MAX))/400;
	}

	for(n = 0; n < NeuronNumHidden2 * NeuronNumOutput; n++)
	{
		//WeightHidden2ToOutput[n] = 0;
		WeightHidden2ToOutput[n] = (rand()/(double)(RAND_MAX))/400;
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


	////////////////计算隐藏1层神经元的值////////////////
	for (m = 0; m < NeuronNumHidden1; m++)
	{
		NetHidden1[m] = 0;
	}

	for (m = 0; m < NeuronNumHidden1; m++)
	{
		for(j = 0; j < pixels; j++)
		{
			for (i = 0; i < pixels; i++)
			{
				NetHidden1[m] += iPicture[j][i] * WeightInputToHidden1[m * pixels * pixels + (j * pixels + i)];
			}
		}
	}

	//for (m = 0; m < NeuronNumHidden1; m++)
	//{
	//	NetHidden1[m] = NetHidden1[m] / 32;
	//}


	//for (m = 0; m < NeuronNumHidden1; m++)
	//{
	//	NetHidden1[m] = NetHidden1[m] + Bias_Input * 1;
	//}


	////////////////计算隐藏1层sigmoid函数值)////////////////
	for(m = 0; m < NeuronNumHidden1; m++)
	{
		SigmoidHidden1[m] = 1 / (1 + exp( - NetHidden1[m]));
	}










	////////////////计算隐藏2层神经元的值////////////////
	for (n = 0; n < NeuronNumHidden2; n++)
	{
		NetHidden2[n] = 0;
	}

	for (n = 0; n < NeuronNumHidden2; n++)
	{
		for (m = 0; m < NeuronNumHidden1; m++)
		{
			NetHidden2[n] += SigmoidHidden1[m] * WeightHidden1ToHidden2[n * NeuronNumHidden1 + m];
		}
	}

	////////////////计算隐藏2层sigmoid函数值)////////////////
	for(n = 0; n < NeuronNumHidden2; n++)
	{
		SigmoidHidden2[n] = 1 / (1 + exp( - NetHidden2[n]));
	}










	////////////////计算输出层神经元的值////////////////
	for (n = 0; n < NeuronNumOutput; n++)
	{
		NetOutput[n] = 0;
	}

	for(n = 0; n < NeuronNumOutput; n++) //输出层神经元个数为10
	{
		for(m = 0; m < NeuronNumHidden2; m++) //隐藏层神经元个数为NeuronNumHidden2
		{
			NetOutput[n] += SigmoidHidden2[m] * WeightHidden2ToOutput[n * NeuronNumHidden2 + m]; //隐藏层和输出层间的神经元要一一连接
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









	//****************更新WeightHidden2ToOutput****************
	for(n = 0; n < NeuronNumOutput; n++)
	{
		for(m = 0; m < NeuronNumHidden2; m++)
		{
			//NetOutput[m]---->WeightHidden2ToOutput[m]
			PD_NetOutputToWeightHidden2ToOutput[n * NeuronNumHidden2 + m] = SigmoidHidden2[m];
		}
	}		

	for(n = 0; n < NeuronNumOutput; n++)
	{
		for(m = 0; m < NeuronNumHidden2; m++)
		{
			//链式法则求偏导
			PD_WeightHidden2ToOutput[n * NeuronNumHidden2 + m] = Delta_Output[n] * PD_NetOutputToWeightHidden2ToOutput[n * NeuronNumHidden2 + m];

			//旧权重保留0.9
			OldWeightHidden2ToOutput[n * NeuronNumHidden2 + m] = WeightHidden2ToOutput[n * NeuronNumHidden2 + m] * OldWeightPercentage; 

			//更新WeightHidden2ToOutput[n]的值
			WeightHidden2ToOutput[n * NeuronNumHidden2 + m] = WeightHidden2ToOutput[n * NeuronNumHidden2 + m] - LearningRate * PD_WeightHidden2ToOutput[n * NeuronNumHidden2 + m];

			//新权重是各保留0.1
			WeightHidden2ToOutput[n * NeuronNumHidden2 + m] = WeightHidden2ToOutput[n * NeuronNumHidden2 + m] * NewWeightPercentage + OldWeightHidden2ToOutput[n * NeuronNumHidden2 + m]; 
		}
	}	










	//****************隐藏2层误差Delta_Hidden2****************
	for (temp = 0; temp < NeuronNumHidden2; temp++)
	{
		PD_ErrorToNetHidden2[temp] = 0;
	}

	for(m = 0; m < NeuronNumHidden2; m++)
	{
		for(n = 0; n < NeuronNumOutput; n++)
		{
			//NetOutput[n]---->SigmoidHidden2[n]
			PD_NetOutputToSigmoidHidden2[n * NeuronNumHidden2 + m] = WeightHidden2ToOutput[n * NeuronNumHidden2 + m];

			//SigmoidHidden2[m]---->NetHidden2[m]
			PD_SigmoidHidden2ToNetHidden2[m] = SigmoidHidden2[m] * (1 - SigmoidHidden2[m]);

			//Error[m]---->NetHidden2[m]
			PD_ErrorToNetHidden2[m] += Delta_Output[n] * PD_NetOutputToSigmoidHidden2[n * NeuronNumHidden2 + m] * PD_SigmoidHidden2ToNetHidden2[m]; 

			//输出层的误差值＝总误差对输出层神经元值求偏导
			Delta_Hidden2[m] = PD_ErrorToNetHidden2[m];
		}
	}










	//****************更新WeightHidden1ToHidden2****************
	for(n = 0; n < NeuronNumHidden2; n++)
	{
		for(m = 0; m < NeuronNumHidden1; m++)
		{
			//NetHidden2[m]---->WeightHidden1ToHidden2[m]
			PD_NetHidden2ToWeightHidden1ToHidden2[n * NeuronNumHidden1 + m] = SigmoidHidden1[m];
		}
	}		

	for(n = 0; n < NeuronNumHidden2; n++)
	{
		for(m = 0; m < NeuronNumHidden1; m++)
		{
			//链式法则求偏导
			PD_WeightHidden1ToHidden2[n * NeuronNumHidden1 + m] = Delta_Hidden2[n] * PD_NetHidden2ToWeightHidden1ToHidden2[n * NeuronNumHidden1 + m];

			//旧权重保留0.9
			OldWeightHidden1ToHidden2[n * NeuronNumHidden1 + m] = WeightHidden1ToHidden2[n * NeuronNumHidden1 + m] * OldWeightPercentage; 

			//更新WeightHidden1ToHidden2[n]的值
			WeightHidden1ToHidden2[n * NeuronNumHidden1 + m] = WeightHidden1ToHidden2[n * NeuronNumHidden1 + m] - LearningRate * PD_WeightHidden1ToHidden2[n * NeuronNumHidden1 + m];

			//新权重是各保留0.1
			WeightHidden1ToHidden2[n * NeuronNumHidden1 + m] = WeightHidden1ToHidden2[n * NeuronNumHidden1 + m] * NewWeightPercentage + OldWeightHidden1ToHidden2[n * NeuronNumHidden1 + m]; 
		}
	}	










	//****************隐藏1层误差Delta_Hidden1****************
	for (temp = 0; temp < NeuronNumHidden1; temp++)
	{
		PD_ErrorToNetHidden1[temp] = 0;
	}

	for(m = 0; m < NeuronNumHidden1; m++)
	{
		for(n = 0; n < NeuronNumHidden2; n++)
		{
			//NetHidden2[n]---->SigmoidHidden1[n]
			PD_NetHidden2ToSigmoidHidden1[n * NeuronNumHidden1 + m] = WeightHidden1ToHidden2[n * NeuronNumHidden1 + m];

			//SigmoidHidden1[m]---->NetHidden1[m]
			PD_SigmoidHidden1ToNetHidden1[m] = SigmoidHidden1[m] * (1 - SigmoidHidden1[m]);

			//Error[m]---->NetHidden1[m]
			PD_ErrorToNetHidden1[m] += Delta_Hidden2[n] * PD_NetHidden2ToSigmoidHidden1[n * NeuronNumHidden1 + m] * PD_SigmoidHidden1ToNetHidden1[m]; 

			//输出层的误差值＝总误差对输出层神经元值求偏导
			Delta_Hidden1[m] = PD_ErrorToNetHidden1[m];
		}
	}










	//****************更新WeightInputToHidden1****************
	for (m = 0; m < NeuronNumHidden1; m++)
	{
		for(j = 0; j < pixels; j++)
		{
			for (i = 0; i < pixels; i++)
			{
				//NetHidden1[n]---->WeightInputToHidden1[n]
				PD_NetHidden1ToWeightInputToHidden1[m * pixels * pixels + (j * pixels + i)] = iPicture[j][i];
			}
		}
	}


	for (m = 0; m < NeuronNumHidden1; m++)
	{
		for(j = 0; j < pixels; j++)
		{
			for (i = 0; i < pixels; i++)
			{
				PD_WeightInputToHidden1[m * pixels * pixels + (j * pixels + i)] = Delta_Hidden1[m] * PD_NetHidden1ToWeightInputToHidden1[m * pixels * pixels + (j * pixels + i)];

				//旧权重保留0.9
				OldWeightInputToHidden1[m * pixels * pixels + (j * pixels + i)] = WeightInputToHidden1[m * pixels * pixels + (j * pixels + i)] * OldWeightPercentage;

				////更新WeightInputToHidden1[m]的值
				WeightInputToHidden1[m * pixels * pixels + (j * pixels + i)] = 
					WeightInputToHidden1[m * pixels * pixels + (j * pixels + i)] - LearningRate * PD_WeightInputToHidden1[m * pixels * pixels + (j * pixels + i)]; 

				//新权重是各保留0.1
				WeightInputToHidden1[m * pixels * pixels + (j * pixels + i)] = 
					WeightInputToHidden1[m * pixels * pixels + (j * pixels + i)] * NewWeightPercentage + OldWeightInputToHidden1[m * pixels * pixels + (j * pixels + i)];
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