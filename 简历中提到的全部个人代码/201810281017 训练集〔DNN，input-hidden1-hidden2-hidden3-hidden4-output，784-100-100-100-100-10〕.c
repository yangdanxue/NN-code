#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define pixels						28		//图片像素是28*28
#define NeuronNumHidden1			100		//隐藏1层神经元个数
#define NeuronNumHidden2			100		//隐藏2层神经元个数
#define NeuronNumHidden3			100		//隐藏3层神经元个数
#define NeuronNumHidden4			100		//隐藏4层神经元个数
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
double WeightHidden2ToHidden3[NeuronNumHidden2 * NeuronNumHidden3] = {0};
double WeightHidden3ToHidden4[NeuronNumHidden3 * NeuronNumHidden4] = {0};
double WeightHidden4ToOutput[NeuronNumHidden4 * NeuronNumOutput] = {0};		

//各层反向传播后旧权重保留的比例
double OldWeightInputToHidden1[pixels * pixels * NeuronNumHidden1] = {0};
double OldWeightHidden1ToHidden2[NeuronNumHidden1 * NeuronNumHidden2] = {0};
double OldWeightHidden2ToHidden3[NeuronNumHidden2 * NeuronNumHidden3] = {0};
double OldWeightHidden3ToHidden4[NeuronNumHidden3 * NeuronNumHidden4] = {0};
double OldWeightHidden4ToOutput[NeuronNumHidden4 * NeuronNumOutput] = {0};	

//各层偏置项
double Bias_Input = 0;
double Bias_Hidden = 0;

//各层神经元值
double NetHidden1[NeuronNumHidden1] = {0};	
double NetHidden2[NeuronNumHidden2] = {0};	
double NetHidden3[NeuronNumHidden3] = {0};	
double NetHidden4[NeuronNumHidden4] = {0};	
double NetOutput[NeuronNumOutput] = {0};	

//各层激活函数值（Sigmoid函数）
double SigmoidHidden1[NeuronNumHidden1] = {0};
double SigmoidHidden2[NeuronNumHidden2] = {0};
double SigmoidHidden3[NeuronNumHidden3] = {0};
double SigmoidHidden4[NeuronNumHidden4] = {0};
double SigmoidOutput[NeuronNumOutput] = {0};

int target[NeuronNumOutput] = {0};				//目标输出值
double ErrorOfOutput[NeuronNumOutput] = {0};	//每个输出层神经元的输出值误差（实际输出值与目标值的差）

//各层误差值
double PD_ErrorToNetOutput[NeuronNumOutput] = {0};
double Delta_Output[NeuronNumOutput] = {0};
double PD_ErrorToNetHidden4[NeuronNumHidden4] = {0};
double Delta_Hidden4[NeuronNumHidden4] = {0};
double PD_ErrorToNetHidden3[NeuronNumHidden3] = {0};
double Delta_Hidden3[NeuronNumHidden3] = {0};
double PD_ErrorToNetHidden2[NeuronNumHidden2] = {0};
double Delta_Hidden2[NeuronNumHidden2] = {0};
double PD_ErrorToNetHidden1[NeuronNumHidden1] = {0};
double Delta_Hidden1[NeuronNumHidden1] = {0};

//隐含4层---->输出层的权值更新
double PD_ErrorToSigmoidOutput[NeuronNumOutput] = {0};
double PD_SigmoidOutputToNetOutput[NeuronNumOutput] = {0};
double PD_NetOutputToWeightHidden4ToOutput[NeuronNumHidden4 * NeuronNumOutput] = {0};
double PD_WeightHidden4ToOutput[NeuronNumHidden4 * NeuronNumOutput] = {0};

//隐含3层---->隐含4层的权值更新
double PD_NetOutputToSigmoidHidden4[NeuronNumHidden4 * NeuronNumOutput] = {0};
double PD_SigmoidHidden4ToNetHidden4[NeuronNumHidden4 * NeuronNumOutput] = {0};
double PD_NetHidden4ToWeightHidden3ToHidden4[NeuronNumHidden3 * NeuronNumHidden4] = {0};
double PD_WeightHidden3ToHidden4[NeuronNumHidden3 * NeuronNumHidden4] = {0};

//隐含2层---->隐含3层的权值更新
double PD_NetHidden3ToWeightHidden2ToHidden3[NeuronNumHidden2 * NeuronNumHidden3] = {0};
double PD_SigmoidHidden3ToNetHidden3[NeuronNumHidden3] = {0};
double PD_NetHidden4ToSigmoidHidden3[NeuronNumHidden3 * NeuronNumHidden4] = {0};
double PD_WeightHidden2ToHidden3[NeuronNumHidden2 * NeuronNumHidden3] = {0};

//隐含1层---->隐含2层的权值更新
double PD_NetHidden3ToSigmoidHidden2[NeuronNumHidden2 * NeuronNumHidden3] = {0};
double PD_SigmoidHidden2ToNetHidden2[NeuronNumHidden2] = {0};
double PD_NetHidden2ToWeightHidden1ToHidden2[NeuronNumHidden1 * NeuronNumHidden2] = {0};
double PD_WeightHidden1ToHidden2[NeuronNumHidden1 * NeuronNumHidden2] = {0};

//输入层---->隐含1层的权值更新
double PD_NetHidden2ToSigmoidHidden1[NeuronNumHidden1 * NeuronNumHidden2] = {0};
double PD_SigmoidHidden1ToNetHidden1[NeuronNumHidden1] = {0};
double PD_NetHidden1ToWeightInputToHidden1[pixels * pixels * NeuronNumHidden1] = {0};
double PD_WeightInputToHidden1[pixels * pixels * NeuronNumHidden1] = {0};

double LearningRate = 0.05; //学习速率

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

	FILE *fpReadItoH1 = fopen("d:\\WeightItoH1Data201810281017.wet","rb");
	FILE *fpReadH1toH2 = fopen("d:\\WeightH1toH2Data201810281017.wet","rb");
	FILE *fpReadH2toH3 = fopen("d:\\WeightH2toH3Data201810281017.wet","rb");
	FILE *fpReadH3toH4 = fopen("d:\\WeightH3toH4Data201810281017.wet","rb");
	FILE *fpReadH4toO = fopen("d:\\WeightH4toOData201810281017.wet","rb");
	if(fpReadItoH1 == NULL)
	{
		printf("WeightItoH1Data201810281017.wet文件不存在！\n");
		return;
	}
	if(fpReadH1toH2 == NULL)
	{
		printf("WeightH1toH2Data201810281017.wet文件不存在！\n");
		return;
	}
	if(fpReadH2toH3 == NULL)
	{
		printf("WeightH2toH3Data201810281017.wet文件不存在！\n");
		return;
	}
	if(fpReadH3toH4 == NULL)
	{
		printf("WeightH3toH4Data201810281017.wet文件不存在！\n");
		return;
	}
	if(fpReadH4toO == NULL)
	{
		printf("WeightH4toOData201810281017.wet文件不存在！\n");
		return;
	}

	//下面是读数据，将读到的数据存到数组WeightInputToHidden1[m]中
	iNum = fread(WeightInputToHidden1, sizeof(double), pixels * pixels * NeuronNumHidden1, fpReadItoH1);
	if(iNum != pixels * pixels * NeuronNumHidden1)
	{
		printf("读数据存到数组WeightInputToHidden1[pixels * pixels * NeuronNumHidden1]错误！！！\n");
	}
	printf("读输入层-隐藏1层间的权重数据成功\n");

	//下面是读数据，将读到的数据存到数组WeightHidden1ToHidden2[m]中
	iNum = fread(WeightHidden1ToHidden2, sizeof(double), NeuronNumHidden1 * NeuronNumHidden2, fpReadH1toH2);
	if(iNum != NeuronNumHidden1 * NeuronNumHidden2)
	{
		printf("读数据存到数组WeightHidden1ToHidden2[NeuronNumHidden1 * NeuronNumHidden2]错误！！！\n");
	}
	printf("读隐藏1层-隐藏2层间的权重数据成功\n");

	//下面是读数据，将读到的数据存到数组WeightHidden2ToHidden3[m]中
	iNum = fread(WeightHidden2ToHidden3, sizeof(double), NeuronNumHidden2 * NeuronNumHidden3, fpReadH2toH3);
	if(iNum != NeuronNumHidden2 * NeuronNumHidden3)
	{
		printf("读数据存到数组WeightHidden2ToHidden3[NeuronNumHidden2 * NeuronNumHidden3]错误！！！\n");
	}
	printf("读隐藏2层-隐藏3层间的权重数据成功\n");

	//下面是读数据，将读到的数据存到数组WeightHidden3ToHidden4[m]中
	iNum = fread(WeightHidden3ToHidden4, sizeof(double), NeuronNumHidden3 * NeuronNumHidden4, fpReadH3toH4);
	if(iNum != NeuronNumHidden3 * NeuronNumHidden4)
	{
		printf("读数据存到数组WeightHidden3ToHidden4[NeuronNumHidden3 * NeuronNumHidden4]错误！！！\n");
	}
	printf("读隐藏3层-隐藏4层间的权重数据成功\n");

	//下面是读数据，将读到的数据存到数组WeightHidden4ToOutput[m]中
	iNum = fread(WeightHidden4ToOutput, sizeof(double), NeuronNumHidden4 * NeuronNumOutput, fpReadH4toO);
	if(iNum != NeuronNumHidden4 * NeuronNumOutput)
	{
		printf("读数据存到数组WeightHidden4ToOutput[NeuronNumHidden4 * NeuronNumOutput]错误！！！\n");
	}
	printf("读隐藏4层-输出层间的权重数据成功\n");

	fclose(fpReadItoH1);
	fclose(fpReadH1toH2);
	fclose(fpReadH2toH3);
	fclose(fpReadH3toH4);
	fclose(fpReadH4toO);

	return;
}


//写文件——保留当前调好的权重
void WriteWeightData()
{
	int iNum = 0;
	FILE *fpWriteInputToHidden1 = fopen("d:\\WeightItoH1Data201810281017.wet","wb+");
	FILE *fpWriteHidden1ToHidden2 = fopen("d:\\WeightH1toH2Data201810281017.wet","wb+");
	FILE *fpWriteHidden2ToHidden3 = fopen("d:\\WeightH2toH3Data201810281017.wet","wb+");
	FILE *fpWriteHidden3ToHidden4 = fopen("d:\\WeightH3toH4Data201810281017.wet","wb+");
	FILE *fpWriteHidden4ToOutput = fopen("d:\\WeightH4toOData201810281017.wet","wb+");
	if(fpWriteInputToHidden1 == NULL)
	{
		printf("创建WeightItoH1Data201810281017.wet文件失败！\n");
		return 0;
	}
	if(fpWriteHidden1ToHidden2 == NULL)
	{
		printf("创建WeightH1toH2Data201810281017.wet文件失败！\n");
		return 0;
	}
	if(fpWriteHidden2ToHidden3 == NULL)
	{
		printf("创建WeightH2toH3Data201810281017.wet文件失败！\n");
		return 0;
	}
	if(fpWriteHidden3ToHidden4 == NULL)
	{
		printf("创建WeightH3toH4Data201810281017.wet文件失败！\n");
		return 0;
	}
	if(fpWriteHidden4ToOutput == NULL)
	{
		printf("创建WeightH4toOData201810281017.wet文件失败！\n");
		return 0;
	}

	//将输入层-隐藏1层间的权重写入到WeightItoH1Data201810281017.wet文件中
	iNum = fwrite(WeightInputToHidden1, sizeof(double), pixels * pixels * NeuronNumHidden1, fpWriteInputToHidden1);
	if (iNum != pixels * pixels * NeuronNumHidden1)
	{
		printf("写WeightItoH1Data201810281017.wet文件失败！");
	}
	printf("所有输入层-隐藏1层间的权重数据已经保存\n");

	//将隐藏1层-隐藏2层间的权重写入到WeightH1toH2Data201810281017.wet文件中
	iNum = fwrite(WeightHidden1ToHidden2, sizeof(double), NeuronNumHidden1 * NeuronNumHidden2, fpWriteHidden1ToHidden2);
	if (iNum != NeuronNumHidden1 * NeuronNumHidden2)
	{
		printf("写WeightH1toH2Data201810281017.wet文件失败！");
	}
	printf("所有隐藏1层-隐藏2层间的权重数据已经保存\n");

	//将隐藏2层-隐藏3层间的权重写入到WeightH2toH3Data201810281017.wet文件中
	iNum = fwrite(WeightHidden2ToHidden3, sizeof(double), NeuronNumHidden2 * NeuronNumHidden3, fpWriteHidden2ToHidden3);
	if (iNum != NeuronNumHidden2 * NeuronNumHidden3)
	{
		printf("写WeightH2toH3Data201810281017.wet文件失败！");
	}
	printf("所有隐藏2层-隐藏3层间的权重数据已经保存\n");

	//将隐藏3层-隐藏4层间的权重写入到WeightH3toH4Data201810281017.wet文件中
	iNum = fwrite(WeightHidden3ToHidden4, sizeof(double), NeuronNumHidden3 * NeuronNumHidden4, fpWriteHidden3ToHidden4);
	if (iNum != NeuronNumHidden3 * NeuronNumHidden4)
	{
		printf("写WeightH3toH4Data201810281017.wet文件失败！");
	}
	printf("所有隐藏2层-隐藏3层间的权重数据已经保存\n");

	//将隐藏4层-输出层间的权重写入到WeightH4toOData201810281017.wet文件中
	iNum = fwrite(WeightHidden4ToOutput, sizeof(double), NeuronNumHidden4 * NeuronNumOutput, fpWriteHidden4ToOutput);
	if (iNum != NeuronNumHidden4 * NeuronNumOutput)
	{		
		printf("写WeightH4toOData201810281017.wet文件失败！");
	}
	printf("所有隐藏4层-输出层间的权重数据已经保存\n");

	fclose(fpWriteInputToHidden1);
	fclose(fpWriteHidden1ToHidden2);
	fclose(fpWriteHidden2ToHidden3);
	fclose(fpWriteHidden3ToHidden4);
	fclose(fpWriteHidden4ToOutput);
}


//随机初始化各值
void RandomInitializationWeight()
{
	int wi1, w12, w23, w34, w4o = 0;

	for(wi1 = 0; wi1 < pixels * pixels * NeuronNumHidden1; wi1++)
	{
		//WeightInputToHidden1[wi1] = (rand()/(double)(RAND_MAX))/100000;
		//WeightInputToHidden1[wi1] = 0.0006160119842780; //【序号189日期201810231517】中位数*10
		//WeightInputToHidden1[wi1] = 0.0000616011984278; //【序号189日期201810231517】中位数
		WeightInputToHidden1[wi1] = 0.0017540410310992; //【序号189日期201810231517】平均数
	}

	for(w12 = 0; w12 < NeuronNumHidden1 * NeuronNumHidden2; w12++)
	{
		//WeightInputToHidden1[m] = (rand()/(double)(RAND_MAX))/500;
		//WeightHidden1ToHidden2[w12] = 0.3941045692040610; //【序号189日期201810231517】中位数*10
		//WeightHidden1ToHidden2[w12] = 0.0394104569204061; //【序号189日期201810231517】中位数
		WeightHidden1ToHidden2[w12] = 0.0243395676808447; //【序号189日期201810231517】平均数
	} 


	for(w23 = 0; w23 < NeuronNumHidden2 * NeuronNumHidden3; w23++)
	{
		//WeightInputToHidden1[m] = (rand()/(double)(RAND_MAX))/500;
		//WeightHidden2ToHidden3[w23] = 0.4152185613955350; //【序号189日期201810231517】中位数*10
		//WeightHidden2ToHidden3[w23] = 0.0415218561395535; //【序号189日期201810231517】中位数
		WeightHidden2ToHidden3[w23] = 0.0184187868549414; //【序号189日期201810231517】平均数
	}

	for(w34 = 0; w34 < NeuronNumHidden3 * NeuronNumHidden4; w34++)
	{
		//WeightInputToHidden1[m] = (rand()/(double)(RAND_MAX))/500;
		//WeightHidden3ToHidden4[w34] = 0.4363325535870090; //根据【序号189日期201810231517】（中位数的h1-h2的差，等差到h2-h3）*10
		//WeightHidden3ToHidden4[w34] = 0.0436332553587009; //根据【序号189日期201810231517】（中位数的h1-h2的差，等差到h2-h3）
		WeightHidden3ToHidden4[w34] = 0.0184187868549414; //根据【序号189日期201810231517】（平均数的h1-h2的差，等差到h2-h3）
	}

	for(w4o = 0; w4o < NeuronNumHidden4 * NeuronNumOutput; w4o++)
	{
		//WeightInputToHidden1[m] = (rand()/(double)(RAND_MAX))/500;
		//WeightHidden4ToOutput[w4o] = -1.4015852321395100; //【序号189日期201810231517】中位数*10
		//WeightHidden4ToOutput[w4o] = -0.1401585232139510; //【序号189日期201810231517】中位数
		WeightHidden4ToOutput[w4o] = -0.1264533357735950; //【序号189日期201810231517】平均数
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
	int h1, h2, h3, h4, o = 0;


	////////////////计算隐藏1层神经元的值////////////////
	for (h1 = 0; h1 < NeuronNumHidden1; h1++)
	{
		NetHidden1[h1] = 0;
	}

	for (h1 = 0; h1 < NeuronNumHidden1; h1++)
	{
		for(j = 0; j < pixels; j++)
		{
			for (i = 0; i < pixels; i++)
			{
				NetHidden1[h1] += iPicture[j][i] * WeightInputToHidden1[h1 * pixels * pixels + (j * pixels + i)];
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
	for(h1 = 0; h1 < NeuronNumHidden1; h1++)
	{
		SigmoidHidden1[h1] = 1 / (1 + exp( - NetHidden1[h1]));
	}










	////////////////计算隐藏2层神经元的值////////////////
	for (h2 = 0; h2 < NeuronNumHidden2; h2++)
	{
		NetHidden2[h2] = 0;
	}

	for (h2 = 0; h2 < NeuronNumHidden2; h2++)
	{
		for (h1 = 0; h1 < NeuronNumHidden1; h1++)
		{
			NetHidden2[h2] += SigmoidHidden1[h1] * WeightHidden1ToHidden2[h2 * NeuronNumHidden1 + h1];
		}
	}

	////////////////计算隐藏2层sigmoid函数值)////////////////
	for(h2 = 0; h2 < NeuronNumHidden2; h2++)
	{
		SigmoidHidden2[h2] = 1 / (1 + exp( - NetHidden2[h2]));
	}










	////////////////计算隐藏3层神经元的值////////////////
	for (h3 = 0; h3 < NeuronNumHidden3; h3++)
	{
		NetHidden3[h3] = 0;
	}

	for (h3 = 0; h3 < NeuronNumHidden3; h3++)
	{
		for (h2 = 0; h2 < NeuronNumHidden2; h2++)
		{
			NetHidden3[h3] += SigmoidHidden2[h2] * WeightHidden2ToHidden3[h3 * NeuronNumHidden2 + h2];
		}
	}

	////////////////计算隐藏3层sigmoid函数值)////////////////
	for(h3 = 0; h3 < NeuronNumHidden3; h3++)
	{
		SigmoidHidden3[h3] = 1 / (1 + exp( - NetHidden3[h3]));
	}










	////////////////计算隐藏4层神经元的值////////////////
	for (h4 = 0; h4 < NeuronNumHidden4; h4++)
	{
		NetHidden4[h4] = 0;
	}

	for (h4 = 0; h4 < NeuronNumHidden4; h4++)
	{
		for (h3 = 0; h3 < NeuronNumHidden3; h3++)
		{
			NetHidden4[h4] += SigmoidHidden3[h3] * WeightHidden3ToHidden4[h4 * NeuronNumHidden3 + h3];
		}
	}

	////////////////计算隐藏4层sigmoid函数值)////////////////
	for(h4 = 0; h4 < NeuronNumHidden4; h4++)
	{
		SigmoidHidden4[h4] = 1 / (1 + exp( - NetHidden4[h4]));
	}










	////////////////计算输出层神经元的值////////////////
	for (o = 0; o < NeuronNumOutput; o++)
	{
		NetOutput[o] = 0;
	}

	for(o = 0; o < NeuronNumOutput; o++) //输出层神经元个数为10
	{
		for(h4 = 0; h4 < NeuronNumHidden4; h4++) //隐藏层神经元个数为NeuronNumHidden3
		{
			NetOutput[o] += SigmoidHidden4[h4] * WeightHidden4ToOutput[o * NeuronNumHidden4 + h4]; //隐藏层和输出层间的神经元要一一连接
		}
	}

	//for(n = 0; n < NeuronNumOutput; n++) //输出层神经元个数为10
	//{
	//	NetOutput[n] = NetOutput[n] + Bias_Hidden * 1;
	//}

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
	int h1, h2, h3, h4, o = 0;

	ClearTarget();
	target[Label] = 1; //目标输出值


	//****************输出层误差Delta_Output****************
	for(o = 0; o < NeuronNumOutput; o++)
	{
		//Error[total]---->SigmoidOutput[o]
		PD_ErrorToSigmoidOutput[o] = - (target[o] - SigmoidOutput[o]); 

		//SigmoidOutput[o]---->NetOutput[o]
		PD_SigmoidOutputToNetOutput[o] = SigmoidOutput[o] * (1 - SigmoidOutput[o]); 

		//Error[o]---->NetOutput[o]
		PD_ErrorToNetOutput[o] = PD_ErrorToSigmoidOutput[o] * PD_SigmoidOutputToNetOutput[o]; 

		//输出层的误差值＝总误差对输出层神经元值求偏导
		Delta_Output[o] = PD_ErrorToNetOutput[o];
	}		









	//****************更新WeightHidden4ToOutput****************
	for(o = 0; o < NeuronNumOutput; o++)
	{
		for(h4 = 0; h4 < NeuronNumHidden4; h4++)
		{
			//NetOutput---->WeightHidden4ToOutput
			PD_NetOutputToWeightHidden4ToOutput[o * NeuronNumHidden4 + h4] = SigmoidHidden4[h4];
		}
	}		

	for(o = 0; o < NeuronNumOutput; o++)
	{
		for(h4 = 0; h4 < NeuronNumHidden4; h4++)
		{
			//链式法则求偏导
			PD_WeightHidden4ToOutput[o * NeuronNumHidden4 + h4] = Delta_Output[o] * PD_NetOutputToWeightHidden4ToOutput[o * NeuronNumHidden4 + h4];

			//旧权重保留0.9
			OldWeightHidden4ToOutput[o * NeuronNumHidden4 + h4] = WeightHidden4ToOutput[o * NeuronNumHidden4 + h4] * OldWeightPercentage; 

			//更新WeightHidden4ToOutput[o]的值
			WeightHidden4ToOutput[o * NeuronNumHidden4 + h4] = WeightHidden4ToOutput[o * NeuronNumHidden4 + h4] - LearningRate * PD_WeightHidden4ToOutput[o * NeuronNumHidden4 + h4];

			//新权重是各保留0.1
			WeightHidden4ToOutput[o * NeuronNumHidden4 + h4] = WeightHidden4ToOutput[o * NeuronNumHidden4 + h4] * NewWeightPercentage + OldWeightHidden4ToOutput[o * NeuronNumHidden4 + h4]; 
		}
	}	










	//****************隐藏4层误差Delta_Hidden4****************
	for (h4 = 0; h4 < NeuronNumHidden4; h4++)
	{
		PD_ErrorToNetHidden4[h4] = 0;
	}

	for(h4 = 0; h4 < NeuronNumHidden4; h4++)
	{
		for(o = 0; o < NeuronNumOutput; o++)
		{
			//NetOutput---->SigmoidHidden4
			PD_NetOutputToSigmoidHidden4[o * NeuronNumHidden4 + h4] = WeightHidden4ToOutput[o * NeuronNumHidden4 + h4];

			//SigmoidHidden4---->NetHidden4
			PD_SigmoidHidden4ToNetHidden4[h4] = SigmoidHidden4[h4] * (1 - SigmoidHidden4[h4]);

			//Error---->NetHidden4
			PD_ErrorToNetHidden4[h4] += Delta_Output[o] * PD_NetOutputToSigmoidHidden4[o * NeuronNumHidden4 + h4] * PD_SigmoidHidden4ToNetHidden4[h4]; 

			//输出层的误差值＝总误差对输出层神经元值求偏导
			Delta_Hidden4[h4] = PD_ErrorToNetHidden4[h4];
		}
	}










	//****************更新WeightHidden3ToHidden4****************
	for(h4 = 0; h4 < NeuronNumHidden4; h4++)
	{
		for(h3 = 0; h3 < NeuronNumHidden3; h3++)
		{
			//NetHidden4---->WeightHidden3ToHidden4
			PD_NetHidden4ToWeightHidden3ToHidden4[h4 * NeuronNumHidden3 + h3] = SigmoidHidden3[h3];
		}
	}		

	for(h4 = 0; h4 < NeuronNumHidden4; h4++)
	{
		for(h3 = 0; h3 < NeuronNumHidden3; h3++)
		{
			//链式法则求偏导
			PD_WeightHidden3ToHidden4[h4 * NeuronNumHidden3 + h3] = Delta_Hidden4[h4] * PD_NetHidden4ToWeightHidden3ToHidden4[h4 * NeuronNumHidden3 + h3];

			//旧权重保留0.9
			OldWeightHidden3ToHidden4[h4 * NeuronNumHidden3 + h3] = WeightHidden3ToHidden4[h4 * NeuronNumHidden3 + h3] * OldWeightPercentage; 

			//更新WeightHidden3ToHidden4[h4]的值
			WeightHidden3ToHidden4[h4 * NeuronNumHidden3 + h3] = WeightHidden3ToHidden4[h4 * NeuronNumHidden3 + h3] - LearningRate * PD_WeightHidden3ToHidden4[h4 * NeuronNumHidden3 + h3];

			//新权重是各保留0.1
			WeightHidden3ToHidden4[h4 * NeuronNumHidden3 + h3] = WeightHidden3ToHidden4[h4 * NeuronNumHidden3 + h3] * NewWeightPercentage + OldWeightHidden3ToHidden4[h4 * NeuronNumHidden3 + h3]; 
		}
	}	










	//****************隐藏3层误差Delta_Hidden3****************
	for (h3 = 0; h3 < NeuronNumHidden3; h3++)
	{
		PD_ErrorToNetHidden3[h3] = 0;
	}

	for(h3 = 0; h3 < NeuronNumHidden3; h3++)
	{
		for(h4 = 0; h4 < NeuronNumHidden4; h4++)
		{
			//NetHidden4---->SigmoidHidden3
			PD_NetHidden4ToSigmoidHidden3[h4 * NeuronNumHidden3 + h3] = WeightHidden3ToHidden4[h4 * NeuronNumHidden3 + h3];

			//SigmoidHidden3---->NetHidden3
			PD_SigmoidHidden3ToNetHidden3[h3] = SigmoidHidden3[h3] * (1 - SigmoidHidden3[h3]);

			//Error---->NetHidden3
			PD_ErrorToNetHidden3[h3] += Delta_Hidden4[h4] * PD_NetHidden4ToSigmoidHidden3[h4 * NeuronNumHidden3 + h3] * PD_SigmoidHidden3ToNetHidden3[h3]; 

			//输出层的误差值＝总误差对输出层神经元值求偏导
			Delta_Hidden3[h3] = PD_ErrorToNetHidden3[h3];
		}
	}










	//****************更新WeightHidden2ToHidden3****************
	for(h3 = 0; h3 < NeuronNumHidden3; h3++)
	{
		for(h2 = 0; h2 < NeuronNumHidden2; h2++)
		{
			//NetHidden3---->WeightHidden2ToHidden3
			PD_NetHidden3ToWeightHidden2ToHidden3[h3 * NeuronNumHidden2 + h2] = SigmoidHidden2[h2];
		}
	}		

	for(h3 = 0; h3 < NeuronNumHidden3; h3++)
	{
		for(h2 = 0; h2 < NeuronNumHidden2; h2++)
		{
			//链式法则求偏导
			PD_WeightHidden2ToHidden3[h3 * NeuronNumHidden2 + h2] = Delta_Hidden3[h3] * PD_NetHidden3ToWeightHidden2ToHidden3[h3 * NeuronNumHidden2 + h2];

			//旧权重保留0.9
			OldWeightHidden2ToHidden3[h3 * NeuronNumHidden2 + h2] = WeightHidden2ToHidden3[h3 * NeuronNumHidden2 + h2] * OldWeightPercentage; 

			//更新WeightHidden2ToHidden3的值
			WeightHidden2ToHidden3[h3 * NeuronNumHidden2 + h2] = WeightHidden2ToHidden3[h3 * NeuronNumHidden2 + h2] - LearningRate * PD_WeightHidden2ToHidden3[h3 * NeuronNumHidden2 + h2];

			//新权重是各保留0.1
			WeightHidden2ToHidden3[h3 * NeuronNumHidden2 + h2] = WeightHidden2ToHidden3[h3 * NeuronNumHidden2 + h2] * NewWeightPercentage + OldWeightHidden2ToHidden3[h3 * NeuronNumHidden2 + h2]; 
		}
	}	










	//****************隐藏2层误差Delta_Hidden2****************
	for (h2 = 0; h2 < NeuronNumHidden2; h2++)
	{
		PD_ErrorToNetHidden2[h2] = 0;
	}

	for(h2 = 0; h2 < NeuronNumHidden2; h2++)
	{
		for(h3 = 0; h3 < NeuronNumHidden3; h3++)
		{
			//NetHidden3---->SigmoidHidden2
			PD_NetHidden3ToSigmoidHidden2[h3 * NeuronNumHidden2 + h2] = WeightHidden2ToHidden3[h3 * NeuronNumHidden2 + h2];

			//SigmoidHidden2---->NetHidden2
			PD_SigmoidHidden2ToNetHidden2[h2] = SigmoidHidden2[h2] * (1 - SigmoidHidden2[h2]);

			//Error---->NetHidden2
			PD_ErrorToNetHidden2[h2] += Delta_Hidden3[h3] * PD_NetHidden3ToSigmoidHidden2[h3 * NeuronNumHidden2 + h2] * PD_SigmoidHidden2ToNetHidden2[h2]; 

			//输出层的误差值＝总误差对输出层神经元值求偏导
			Delta_Hidden2[h2] = PD_ErrorToNetHidden2[h2];
		}
	}










	//****************更新WeightHidden1ToHidden2****************
	for(h2 = 0; h2 < NeuronNumHidden2; h2++)
	{
		for(h1 = 0; h1 < NeuronNumHidden1; h1++)
		{
			//NetHidden2---->WeightHidden1ToHidden2
			PD_NetHidden2ToWeightHidden1ToHidden2[h2 * NeuronNumHidden1 + h1] = SigmoidHidden1[h1];
		}
	}		

	for(h2 = 0; h2 < NeuronNumHidden2; h2++)
	{
		for(h1 = 0; h1 < NeuronNumHidden1; h1++)
		{
			//链式法则求偏导
			PD_WeightHidden1ToHidden2[h2 * NeuronNumHidden1 + h1] = Delta_Hidden2[h2] * PD_NetHidden2ToWeightHidden1ToHidden2[h2 * NeuronNumHidden1 + h1];

			//旧权重保留0.9
			OldWeightHidden1ToHidden2[h2 * NeuronNumHidden1 + h1] = WeightHidden1ToHidden2[h2 * NeuronNumHidden1 + h1] * OldWeightPercentage; 

			//更新WeightHidden1ToHidden2[h2]的值
			WeightHidden1ToHidden2[h2 * NeuronNumHidden1 + h1] = WeightHidden1ToHidden2[h2 * NeuronNumHidden1 + h1] - LearningRate * PD_WeightHidden1ToHidden2[h2 * NeuronNumHidden1 + h1];

			//新权重是各保留0.1
			WeightHidden1ToHidden2[h2 * NeuronNumHidden1 + h1] = WeightHidden1ToHidden2[h2 * NeuronNumHidden1 + h1] * NewWeightPercentage + OldWeightHidden1ToHidden2[h2 * NeuronNumHidden1 + h1]; 
		}
	}	










	//****************隐藏1层误差Delta_Hidden1****************
	for (h1 = 0; h1 < NeuronNumHidden1; h1++)
	{
		PD_ErrorToNetHidden1[h1] = 0;
	}

	for(h1 = 0; h1 < NeuronNumHidden1; h1++)
	{
		for(h2 = 0; h2 < NeuronNumHidden2; h2++)
		{
			//NetHidden2---->SigmoidHidden1
			PD_NetHidden2ToSigmoidHidden1[h2 * NeuronNumHidden1 + h1] = WeightHidden1ToHidden2[h2 * NeuronNumHidden1 + h1];

			//SigmoidHidden1---->NetHidden1
			PD_SigmoidHidden1ToNetHidden1[h1] = SigmoidHidden1[h1] * (1 - SigmoidHidden1[h1]);

			//Error---->NetHidden1
			PD_ErrorToNetHidden1[h1] += Delta_Hidden2[h2] * PD_NetHidden2ToSigmoidHidden1[h2 * NeuronNumHidden1 + h1] * PD_SigmoidHidden1ToNetHidden1[h1]; 

			//输出层的误差值＝总误差对输出层神经元值求偏导
			Delta_Hidden1[h1] = PD_ErrorToNetHidden1[h1];
		}
	}










	//****************更新WeightInputToHidden1****************
	for (h1 = 0; h1 < NeuronNumHidden1; h1++)
	{
		for(j = 0; j < pixels; j++)
		{
			for (i = 0; i < pixels; i++)
			{
				//NetHidden1---->WeightInputToHidden1
				PD_NetHidden1ToWeightInputToHidden1[h1 * pixels * pixels + (j * pixels + i)] = iPicture[j][i];
			}
		}
	}


	for (h1 = 0; h1 < NeuronNumHidden1; h1++)
	{
		for(j = 0; j < pixels; j++)
		{
			for (i = 0; i < pixels; i++)
			{
				PD_WeightInputToHidden1[h1 * pixels * pixels + (j * pixels + i)] = Delta_Hidden1[h1] * PD_NetHidden1ToWeightInputToHidden1[h1 * pixels * pixels + (j * pixels + i)];

				//旧权重保留0.9
				OldWeightInputToHidden1[h1 * pixels * pixels + (j * pixels + i)] = WeightInputToHidden1[h1 * pixels * pixels + (j * pixels + i)] * OldWeightPercentage;

				////更新WeightInputToHidden1的值
				WeightInputToHidden1[h1 * pixels * pixels + (j * pixels + i)] = 
					WeightInputToHidden1[h1 * pixels * pixels + (j * pixels + i)] - LearningRate * PD_WeightInputToHidden1[h1 * pixels * pixels + (j * pixels + i)]; 

				//新权重是各保留0.1
				WeightInputToHidden1[h1 * pixels * pixels + (j * pixels + i)] = 
					WeightInputToHidden1[h1 * pixels * pixels + (j * pixels + i)] * NewWeightPercentage + OldWeightInputToHidden1[h1 * pixels * pixels + (j * pixels + i)];
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