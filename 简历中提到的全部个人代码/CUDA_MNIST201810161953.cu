/*
*【当前版本进度】202208051529更新：更新代码。
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
//#include <stdbool.h> 
#include <unistd.h>	//sleep();的库函数
#include "book.cuh"

#define N 						784
#define M						10
#define TestNumber				60000		//全部的图片数

#define CheckNumber				60000		//验证的图片数
#define GPUcalculateNumber			1000		//调用一次核函数计算的图片数
#define pixels					28		//图片像素是28*28

#define OldWeightPercentage			0.9		//旧权重占比
#define NewWeightPercentage			0.1		//新权重占比
#define LearningRate				0.005		//学习速率

int ReadTrainPicData(unsigned char*, unsigned char*);
int ReadTest10kPicData(unsigned char*, unsigned char*); 	

void RandomInitWeight(float*, float*);		

void WriteWeightData(float*, float*);
void ReadWeightInputToHiddenData(float*);			
void ReadWeightHiddenToOutputData(float*);		


void TypeCasting();
void AllocatePointer();
void RunCUDA(int);

void RecognizeResult();
void JudgeRightOrWrong(int, int);

unsigned char *pPicture;								//指向读取的60000张图片数组的一维指针，的主机副本，与，设备副本
float *pNetInput60000,			*d_pNetInput60000;			//指向CPU上开的一个float型的60000张图片大小的数组空间的一维指针（开空间用，指针始终指向空间首地址）
float *pWeightInputToHidden,		*d_pWeightInputToHidden;		
float *pNetInput,				*d_pNetInput;				//指向每次只计算1张图片数组至GPU的指针，的主机副本，与，设备副本（因为是用于定位当前计算的图片的指针，无需开空间+释放空间）
float *pNetHidden,				*d_pNetHidden;			
float *pSigmoidHidden,			*d_pSigmoidHidden;			
float *pWeightHiddenToOutput,		*d_pWeightHiddenToOutput;			
float *pNetOutput,				*d_pNetOutput;			
float *pSigmoidOutput60000,			*d_pSigmoidOutput60000;		//需回传SigmoidOutput到CPU，判断图片是否识别正确。因此，60000张图片的SigmoidOutput数据需全部保留						
float *pSigmoidOutput,			*d_pSigmoidOutput;			
float *pTarget,				*d_pTarget;				 
float *pDelta_NetO,				*d_pDelta_NetO;			
float *pTempPD_NetOToNetH,			*d_pTempPD_NetOToNetH;		
float *pDelta_NetH,				*d_pDelta_NetH;			
unsigned char *pLabel60000,			*d_pLabel60000;			//指向读取的60000张Label数组的一维指针（开空间用，指针始终指向空间首地址），的主机副本，与，设备副本
unsigned char *pLabel,			*d_pLabel;				//指向每次只计算1张图片对应的标签进行计算的GPU的指针，的主机副本，与，设备副本（因为是用于定位当前计算的图片的指针，无需开空间+释放空间）

int iPicNum;		//图片序号
int Recognize;	//图片被识别为数字几
int correct = 0;
int MaxCorrect = 0;
float CorrectRate = 0;




__device__ int d_iNum; //调用第iNum次核函数，每次调用计算1000张图

__device__ __forceinline__ float sigmoid (float a)
{
    return 1.0 / (1.0 + exp (-a));
}

__global__ void ForwardAndBackPropagation(float *pNetInput60000, float *pNetInput, float *pWeightInputToHidden, float *pNetHidden, float *pSigmoidHidden, float *pWeightHiddenToOutput, float *pNetOutput, float *pSigmoidOutput60000, float *pSigmoidOutput, float *pTarget, float *pDelta_NetO, float *pTempPD_NetOToNetH, float *pDelta_NetH, unsigned char *pLabel60000, unsigned char *pLabel)
{
	/******************** GPU上开的整块内存空间中的指针定位分配 ********************/	
	pWeightInputToHidden = 	pNetInput60000 + N*TestNumber;
	pWeightHiddenToOutput = 	pWeightInputToHidden + N*N;
	pNetHidden = 		pWeightHiddenToOutput + N*M;
	pSigmoidHidden = 		pNetHidden + N;					
	pNetOutput = 		pSigmoidHidden + N;					
	pSigmoidOutput60000 = 	pNetOutput + M;					
	pTarget = 			pSigmoidOutput60000 + M*TestNumber; 		   		
	pDelta_NetO = 		pTarget + M; 		
	pTempPD_NetOToNetH = 	pDelta_NetO + M;					
	pDelta_NetH = 		pTempPD_NetOToNetH + N*M;					
	pLabel60000 =		(unsigned char*)(pDelta_NetH + N);

	int X = threadIdx.x;
	
	for(int j = 0; j < GPUcalculateNumber; j++)
	{
		pNetInput  = 	pNetInput60000 +		N*(d_iNum + j);
		pSigmoidOutput = 	pSigmoidOutput60000 +	M*(d_iNum + j);
		pLabel = 		pLabel60000 +		(d_iNum + j);

		float tmpSum = 0;

	  	if (X < N) 
		{
	     		for (int i = 0; i < N; i++) 
			{
		   		tmpSum += pWeightInputToHidden[X * N + i] * pNetInput[i];
			}
	     		pNetHidden[X] = tmpSum;
		    	pSigmoidHidden[X] = sigmoid (pNetHidden[X]);
	     	}
	   	__syncthreads();

		tmpSum = 0;

		if (X < M) 
		{
			for (int i = 0; i < N; i++)
			{
		   		tmpSum += pWeightHiddenToOutput[X * N + i] * pSigmoidHidden[i];
			}
	    		pNetOutput[X] = tmpSum;
 		    	pSigmoidOutput[X] = sigmoid (pNetOutput[X]);
	    	}
		__syncthreads();
	
		if (X < M) 
		{
			for(int i = 0; i < M; i++)
			{
				*(pTarget + i) = 0;	//目标值数组清0
			}
			*(pTarget + *pLabel) = 1;	//标签的值设为1
			pDelta_NetO[X] = -(pTarget[X] - pSigmoidOutput[X]) * pSigmoidOutput[X] * (1 - pSigmoidOutput[X]);
		}
		__syncthreads();

		if (X < N) 
		{
	 	     	for (int i = 0; i < M; i++) 
			{
				pWeightHiddenToOutput[i * N + X] = (pWeightHiddenToOutput[i * N + X])*OldWeightPercentage + 
									    (pWeightHiddenToOutput[i * N + X] - LearningRate * pDelta_NetO[i] * pSigmoidHidden[X])*NewWeightPercentage;
			}
		}
		__syncthreads();

		tmpSum = 0;

		if (X < N) 
		{
	 	     	for (int i = 0; i < M; i++) 
			{
				pTempPD_NetOToNetH[i * N + X] = pWeightHiddenToOutput[i * N + X] * pSigmoidHidden[X] * (1 - pSigmoidHidden[X]);

		   		tmpSum += pTempPD_NetOToNetH[i * N + X] * pDelta_NetO[i];
			}
	    	    	pDelta_NetH[X] = tmpSum;
		}
		__syncthreads();

	    	if (X < N) 
		{
		     	for (int i = 0; i < N; i++) 
			{
				pWeightInputToHidden[X * N + i] = OldWeightPercentage * pWeightInputToHidden[X * N + i] +
									   NewWeightPercentage * (pWeightInputToHidden[X * N + i] - LearningRate * pNetInput[i] * pDelta_NetH[X]);
			}
		}
		__syncthreads();
	}
}
//*****************************************	以上为GPU上运行的代码	*****************************************//






int main(void) 
{
	/******************** 为设备副本分配空间 ********************/
	HANDLE_ERROR( cudaMalloc((void **)&d_pNetInput60000,	sizeof(float)*48287718) );

	/******************** 为主机副本分配空间 ********************/
	pPicture		= (unsigned char*)malloc(N*TestNumber);		//从硬盘读到内存上的60000张图片
	pNetInput60000	= (float*)malloc(sizeof(float)*48287718);

	/******************** CPU上开的整块内存中的分配用于定位的指针 ********************/	
	AllocatePointer();

	/******************** 读图片和标签 ********************/
	ReadTrainPicData(pPicture, pLabel60000);
//	ReadTest10kPicData(pPicture, pLabel60000);

	/******************** 读权重or随机初始化权重 ********************/
//	ReadWeightInputToHiddenData(pWeightInputToHidden);
//	ReadWeightHiddenToOutputData(pWeightHiddenToOutput);
	RandomInitWeight(pWeightInputToHidden, pWeightHiddenToOutput);

	/******************** 强制类型转换 ********************/
	TypeCasting();

	/******************** 将输入复制到设备 ********************/
	HANDLE_ERROR( cudaMemcpy(d_pNetInput60000, pNetInput60000, sizeof(float)*48287718, cudaMemcpyHostToDevice) );

	/******************** 进入网络训练 ********************/
	MaxCorrect = 0;
	for (int traintimes = 0; traintimes < 10; traintimes++)
	{
	 	correct = 0;
		CorrectRate = 0;
		
		printf("第%d次训练：\n", traintimes+1);
		RunCUDA(CheckNumber);

		//******************** (1)判断图片被识别为数字几并判断识别正误(2)分类 ********************
		for (iPicNum = 0; iPicNum < CheckNumber; iPicNum++)
		{
			pSigmoidOutput = 	pSigmoidOutput60000 + 	M*iPicNum;
			pLabel = 		pLabel60000 + 		iPicNum;

			RecognizeResult();
			JudgeRightOrWrong(iPicNum, Recognize);
		}
		CorrectRate = ((float)correct) / CheckNumber;
		printf("正确数：%d 正确率：%.4f%%\n", correct, CorrectRate * 100);
		if(correct > MaxCorrect)
		{
			MaxCorrect = correct;
//			WriteWeightData(pWeightInputToHidden, pWeightHiddenToOutput);
		}
	}

	/******************** 释放主机与设备的内存空间 ********************/
	free(pPicture);
	free(pNetInput60000);
	HANDLE_ERROR( cudaFree(d_pNetInput60000) ); 
}


//*****************************************	以下为待调用的函数	*****************************************//
/******************** CPU上开的整块内存中的指针定位分配 ********************/
void AllocatePointer()
{
	pWeightInputToHidden = 	pNetInput60000 + N*TestNumber;
	pWeightHiddenToOutput = 	pWeightInputToHidden + N*N;
	pNetHidden = 		pWeightHiddenToOutput + N*M;
	pSigmoidHidden = 		pNetHidden + N;					
	pNetOutput = 		pSigmoidHidden + N;					
	pSigmoidOutput60000 = 	pNetOutput + M;					
	pTarget = 			pSigmoidOutput60000 + M*TestNumber; 		   		
	pDelta_NetO = 		pTarget + M; 		
	pTempPD_NetOToNetH = 	pDelta_NetO + M;					
	pDelta_NetH = 		pTempPD_NetOToNetH + N*M;					
	pLabel60000 = 		(unsigned char*)(pDelta_NetH + N);	//从float型的指针pDelta_NetH，后移N个float型内存空间（4*N个字节）后的地址，与unsigned char型的指针pLabel60000指向的是同一地址
}

/******************** 判断图片是否被正确识别为对应的label ********************/
void RecognizeResult()
{
	for(int i = 0; i < 10; i++)
	{
		//通过比较SigmoidOutput的大小，得到的最大值即为认定图片被网络识别成几，再与Label比较，看是否识别正确		
		if(*(pSigmoidOutput + i) > *(pSigmoidOutput + Recognize))
		{
			Recognize = i;
		}
	}
}

/******************** 判断图片识别正确或错误 ********************/
void JudgeRightOrWrong(int iNum, int OCR)
{
	if(OCR == *pLabel)
	{
		correct++;
		printf("第%d张图片被正确识别为%d\n", iNum, *pLabel);
	}
	else
	{
		printf("第%d张图片应为%d被错误识别为%d\n", iNum, *pLabel, OCR);
	}
}


/******************** 启用CUDA ********************/
void RunCUDA(int iCheck)
{
	//防止全局指针pNetInput的值一直在累加，跑出数据的有效范围
	pNetInput = 0;
	pSigmoidOutput = 0;
	pLabel = 0;

	for(int iNum = 0; iNum < iCheck; iNum += GPUcalculateNumber)
	{
		HANDLE_ERROR( cudaMemcpyToSymbol(d_iNum, &iNum, sizeof(int)) );
		/******************** 在带有N个线程的GPU上启动ForwardAndBackPropagation（）内核 ********************/
		ForwardAndBackPropagation<<<1, N>>>(d_pNetInput60000, d_pNetInput, d_pWeightInputToHidden, d_pNetHidden, d_pSigmoidHidden, d_pWeightHiddenToOutput, d_pNetOutput, d_pSigmoidOutput60000, d_pSigmoidOutput, d_pTarget, d_pDelta_NetO, d_pTempPD_NetOToNetH, d_pDelta_NetH, d_pLabel60000, d_pLabel);
	}
	/******************** 将结果复制回主机 ********************/ 
	HANDLE_ERROR( cudaMemcpy(pNetInput60000, d_pNetInput60000, sizeof(float)*48287718, cudaMemcpyDeviceToHost) );
}

/******************** 读【训练集】图片文件 ********************/
int ReadTrainPicData(unsigned char*data, unsigned char*pLabel60000)
{
	int iCheck = 0;

	FILE *fp_image = fopen("MNIST/train-images.idx", "rb");//打开这个二进制(b)文件train-images.idx，以只读(r)的方式打开，
	FILE *fp_label = fopen("MNIST/train-labels.idx", "rb");//读类标
	//FILE *fp_image = fopen("MNIST/t10k_images.idx", "rb");//打开这个二进制(b)文件t10k-images.idx，以只读(r)的方式打开，
	//FILE *fp_label = fopen("MNIST/t10k_labels.idx", "rb");//读类标
	if (!fp_image || !fp_label) 
		return 1;

	fseek(fp_image, 16, SEEK_SET);//从文件开始位置偏移16字节，定位像素数据开始
	iCheck = fread(data, sizeof(char)*pixels*pixels, TestNumber, fp_image);//从文件(fp_image)中读(pixels * pixels)字节数，读TestNumber次，到data指向的内存中
	if (iCheck != TestNumber)
	{
		printf("读图片文件失败！\n");
	}
	else
	{
		printf("读图片文件成功！\n");	
	}

	fseek(fp_label, 8, SEEK_SET);//从文件开始位置偏移8字节，定位标签数据
	iCheck = fread(pLabel60000, sizeof(char), TestNumber, fp_label);//从文件(fp_label)中读sizeof(char)个字节数，读TestNumber次，读到pLabel指向的内存中
	if (iCheck != TestNumber)
	{
		printf("读Label文件失败！\n");
	}
	else
	{
		printf("读Label文件成功！\n");	
	}

	fclose(fp_image);
	fclose(fp_label);

	return 0;
}

/******************** 读【测试集】图片文件 ********************/
int ReadTest10kPicData(unsigned char*data, unsigned char*pLabel60000)
{
	int iCheck = 0;

	FILE *fp_image = fopen("MNIST/t10k_images.idx", "rb");//打开这个二进制(b)文件t10k-images.idx，以只读(r)的方式打开，
	FILE *fp_label = fopen("MNIST/t10k_labels.idx", "rb");//读类标
	if (!fp_image || !fp_label) 
		return 1;

	fseek(fp_image, 16, SEEK_SET);//从文件开始位置偏移16字节，定位像素数据开始
	iCheck = fread(data, sizeof(char)*pixels*pixels, CheckNumber, fp_image);//从文件(fp_image)中读(pixels * pixels)字节数，读CheckNumber次，到data指向的内存中
	if (iCheck != CheckNumber)
	{
		printf("读图片文件失败！\n");
	}
	else
	{
		printf("读图片文件成功！\n");	
	}

	fseek(fp_label, 8, SEEK_SET);//从文件开始位置偏移8字节，定位标签数据
	iCheck = fread(pLabel60000, sizeof(char), CheckNumber, fp_label);//从文件(fp_label)中读sizeof(char)个字节数，读CheckNumber次，读到pLabel指向的内存中
	if (iCheck != CheckNumber)
	{
		printf("读Label文件失败！\n");
	}
	else
	{
		printf("读Label文件成功！\n");	
	}

	fclose(fp_image);
	fclose(fp_label);

	return 0;
}


/******************** 随机初始化权重 ********************/
void RandomInitWeight(float *WeightIToH, float *WeightHToO)
{
	for(int m = 0; m < N*N; m++)
	{
		*(WeightIToH + m) = (rand()/(float)(RAND_MAX))/2000; //【输入层-隐藏层】之间任意2个神经元间的权重W
	}
	printf("所有输入层-隐藏层间的权重数据已经初始化完毕！\n");

	for(int n = 0; n < N*M; n++)
	{
		*(WeightHToO + n) = (rand()/(float)(RAND_MAX))/400; //【隐藏层-输出层】之间任意2个神经元间的权重W
	}
	printf("所有隐藏层-输出层间的权重数据已经初始化完毕！\n");
}

/******************** 写文件【训好的权重】 ********************/
void WriteWeightData(float *WeightIToH, float *WeightHToO)
{
	int iNum = 0;
	FILE *fpWriteInputToHidden = fopen("MNIST02/fWeightItoHData201810161953.wet","wb+");
	FILE *fpWriteHiddenToOutput = fopen("MNIST02/fWeightHtoOData201810161953.wet","wb+");
	if(fpWriteInputToHidden == NULL)
	{
		printf("创建fWeightItoHData201810161953.wet文件失败！\n");
		return;
	}
	if(fpWriteHiddenToOutput == NULL)
	{
		printf("创建fWeightHtoOData201810161953.wet文件失败！\n");
		return;
	}

	//将输入层-隐藏层间的权重写入到WeightItoHData.wet文件中
	iNum = fwrite(WeightIToH, sizeof(float), N*N, fpWriteInputToHidden);
	if (iNum != N*N)
	{
		printf("写fWeightItoHData201810161953.wet文件失败！");
		return;
	}
	printf("所有输入层-隐藏层间的权重数据已经保存\n");


	//将隐藏层-输出层间的权重写入到WeightHtoOData.wet文件中
	iNum = fwrite(WeightHToO, sizeof(float), N*M, fpWriteHiddenToOutput);
	if (iNum != N*M)
	{		
		printf("写fWeightHtoOData201810161953.wet文件失败！");
		return;
	}
	printf("所有隐藏层-输出层间的权重数据已经保存\n");

	fclose(fpWriteInputToHidden);
	fclose(fpWriteHiddenToOutput);
	return;
}

/******************** 读文件【权重WeightInputToHidden】 ********************/
void ReadWeightInputToHiddenData(float *WeightIToH)
{
	int iNum = 0;//输入层-隐藏层间的神经元个数

	FILE *fpReadWeightIToH = fopen("MNIST01/fWeightItoHData201810161953.wet","rb");
	if(fpReadWeightIToH == NULL)
	{
		printf("fWeightItoHData201810161953.wet文件不存在！\n");
		return;
	}

	//下面是读数据，将读到的数据存到数组WeightInputToHidden[NeuronNumHidden]中
	iNum = fread(WeightIToH, sizeof(float), N*N, fpReadWeightIToH);
	if(iNum != N*N)
	{
		printf("读数据存到WeightIToH指向的内存错误！！！\n");
		return;
	}
	printf("读【输入层-隐藏层】间的权重数据成功\n");
	fclose(fpReadWeightIToH);
	return;
}

/******************** 读文件【权重WeightHiddenToOutput】 ********************/
void ReadWeightHiddenToOutputData(float *WeightHToO)
{
	int iNum = 0;//隐藏层-输出层间的神经元个数

	FILE *fpReadWeightHToO = fopen("MNIST01/fWeightHtoOData201810161953.wet","rb");
	if(fpReadWeightHToO == NULL)
	{
		printf("fWeightHtoOData201810161953.wet文件不存在！\n");
		return;
	}

	//下面是读数据，将读到的数据存到数组WeightHiddenToOutput[]中
	iNum = fread(WeightHToO, sizeof(float), N*M, fpReadWeightHToO);
	if(iNum != N*M)
	{
		printf("读数据存到WeightHToO指向的内存错误！！！\n");
	}
	printf("读【隐藏层-输出层】间的权重数据成功\n");
	fclose(fpReadWeightHToO);
	return;
}

/******************** 强制类型转换 ********************/
void TypeCasting()
{
	for(int iNum = 0; iNum < N*TestNumber; iNum++)
	{
		*(pNetInput60000 + iNum) = (float)(*(pPicture + iNum));
	}
}

