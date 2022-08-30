/****************************************************************************************************************
父文件：/home/yangdanxue/CUDA_MNIST201811102304（202012132338，【CNN，784-784@100-196-10】多卷积核的反向传播，已全部更新完毕，随机权重，正确率第一次训练就到了78.0667%，之后一直稳步增长）.cu

参考文件：/home/yangdanxue/CUDA_MNIST201811102304（202010302008，【CNN，784-784@100-196-10】一次性改了前向传播部分代码，读取99.5467%版权重，正确率50.3600%）.cu
	  /home/yangdanxue/test_Assignment（202012032255，测试模拟：多个卷积核，pDelta_Subsampling2[196*100] += pPD_NetOutputToSigmoidSubsampling2[1960*100] * pDelta_NetOutput[10]，单核多核分别都跑通啦）.cu
	  /home/yangdanxue/201811081625.c
	  /home/yangdanxue/201811102304.c

当前版本进度：202208162253更新：开一整块内存，1024个线程并行，一次上传60000张图到gpu，每次计算600张。
*****************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "book.cuh"

#define N					784
#define M					10

#define pixels				28
#define TestNumber			60000
#define CheckNumber			60000
#define OldWeightPercentage		0.9
#define NewWeightPercentage		0.1  
#define LearningRate			0.03		
#define GPUcalculateNumber		600

dim3 threadsPerBlock(32, 32);


int ReadTrainPicData(unsigned char*, unsigned char*);
int ReadTest10kPicData(unsigned char*, unsigned char*); 	
void ReadWeightData(float*, float*);
void WriteWeightData(float*, float*);
void RandomInitWeight(float*, float*);

void TypeCasting();
void AllocatePointer();
void RunCUDA(int);

void RecognizeResult();
void JudgeRightOrWrong(int, int);

unsigned char *pPicture;

float *pNetInput60000,			*d_pNetInput60000;
float *pNetInput,				*d_pNetInput;
float *pPadding_NetInput,			*d_pPadding_NetInput;
float *pWeightItoC1,				*d_pWeightItoC1;
float *pWeightS2toO,				*d_pWeightS2toO;
float *pNetConvolution1,			*d_pNetConvolution1;	
float *pSigmoidConvolution1,		*d_pSigmoidConvolution1;
struct MAXPool
{
	float Value;
	int i;
	int j;
};struct MAXPool *pNetSubsampling2,		*d_pNetSubsampling2;
float *pNetOutput,				*d_pNetOutput;			
float *pSigmoidOutput60000,			*d_pSigmoidOutput60000;			
float *pSigmoidOutput,			*d_pSigmoidOutput;			
float *pTarget,				*d_pTarget;	
float *pDelta_NetOutput,			*d_pDelta_NetOutput;
float *pDelta_Subsampling2,			*d_pDelta_Subsampling2;
float *pUpsample_Delta_Subsampling2,	*d_pUpsample_Delta_Subsampling2;
float *pDelta_Convolution1,			*d_pDelta_Convolution1;
float *pPD_WeightItoC1,			*d_pPD_WeightItoC1;
unsigned char *pLabel60000,			*d_pLabel60000;
unsigned char *pLabel,			*d_pLabel;

int iPicNum;		//图片序号
int Recognize;	//图片被识别为数字几
int correct = 0;
int MaxCorrect = 0;
float CorrectRate = 0;

//***********************************************************************************************************
__device__ int d_iNum;

__device__ __forceinline__ float sigmoid (float a)
{
    return 1.0 / (1.0 + exp (-a));
}

__global__ void ForwardAndBackPropagation(float *pNetInput60000, float *pNetInput, float *pPadding_NetInput, float *pWeightItoC1, float *pNetConvolution1, float *pSigmoidConvolution1, struct MAXPool *pNetSubsampling2, float *pWeightS2toO, float *pNetOutput, float *pSigmoidOutput60000, float *pSigmoidOutput, float *pTarget, float *pDelta_NetOutput, float *pDelta_Subsampling2, float *pUpsample_Delta_Subsampling2, float *pDelta_Convolution1, float *pPD_WeightItoC1, unsigned char *pLabel60000, unsigned char *pLabel)
{
	//内存中的指针分配
	pPadding_NetInput =			pNetInput60000 + 						N*TestNumber;
	pWeightItoC1 =				pPadding_NetInput + 					1024;
	pWeightS2toO =				pWeightItoC1 + 						25*100;
	pNetConvolution1 =			pWeightS2toO + 						196*M*100;
	pSigmoidConvolution1 =			pNetConvolution1 + 					N*100;
	pNetSubsampling2 =			(struct MAXPool*)(pSigmoidConvolution1 + 		N*100);
	pNetOutput =				(float*)(pNetSubsampling2 + 				196*100);
	pSigmoidOutput60000 =			pNetOutput + 						M;
	pTarget =					pSigmoidOutput60000 + 					M*TestNumber;			
	pDelta_NetOutput =			pTarget + 							M;	
	pDelta_Subsampling2 =			pDelta_NetOutput + 					M;
	pUpsample_Delta_Subsampling2 =		pDelta_Subsampling2 + 					196*100;	
	pDelta_Convolution1 =			pUpsample_Delta_Subsampling2 +				N*100;				
	pPD_WeightItoC1 =				pDelta_Convolution1 + 					N*100;	
	pLabel60000 =				(unsigned char*)(pPD_WeightItoC1 + 			25*100);

	int Y = threadIdx.y;
 	int X = threadIdx.x;

	for(int j = 0; j < GPUcalculateNumber; j++)
	{
		pNetInput  = 	pNetInput60000 +		N*(d_iNum + j);
		pSigmoidOutput = 	pSigmoidOutput60000 +	M*(d_iNum + j);
		pLabel = 		pLabel60000 +		(d_iNum + j);

		//在iPicture[y][x]周围，填充2圈0——因为在图像边缘的像素在输出中采用较少，为了不丢掉了很多图像边缘的信息，在图像卷积操作之前，沿着图像边缘用0进行图像填充。对于5*5的过滤器，填充宽度为2时，就可以保证输出图像和输入图像一样大。
	    	if (X < 32 && Y < 32) 
		{
			pPadding_NetInput[Y*32+X] = 0;
		}
	     	__syncthreads();

	    	if (X < 28 && Y < 28) 
		{
			pPadding_NetInput[(Y+2)*32+(X+2)] = pNetInput[Y*28+X];
		}
	     	__syncthreads();

	   	///////////////////////////【前向传播】计算卷积1层NetConvolution1神经元的值///////////////////////////
	 	float tmpSum = 0;

		for(int NumK = 0; NumK < 100; NumK++)
		{
		     	tmpSum = 0;

		   	if (X < 28 && Y < 28) 
			{
		     		for (int j = 0; j < 5; j++) 
				{
			     		for (int i = 0; i < 5; i++) 
					{
				   		tmpSum += pPadding_NetInput[(Y+j)*32+(X+i)] * pWeightItoC1[NumK*5*5+(j*5+i)];
					}
				} 
		     		pNetConvolution1[NumK*28*28+(Y*28+X)] = tmpSum;
			    	pSigmoidConvolution1[NumK*28*28+(Y*28+X)] = sigmoid (pNetConvolution1[NumK*28*28+(Y*28+X)]);
		     	}
	 		__syncthreads();
	    	}
		__syncthreads();

	 	///////////////////////////【前向传播】计算池化2层NetSubsampling2神经元的值///////////////////////////
	 	for(int NumK = 0; NumK < 100; NumK++)
		{
		    	if (X < 14 && Y < 14) 
			{
				pNetSubsampling2[NumK*14*14+(Y*14+X)].Value = 0;
				pNetSubsampling2[NumK*14*14+(Y*14+X)].i = 0;
				pNetSubsampling2[NumK*14*14+(Y*14+X)].j = 0;

				for(int j = 0; j < 2; j++)
				{
					for (int i = 0; i < 2; i++)
					{
						//求出每2*2过滤器中的最大值，组成池化层
						if (pSigmoidConvolution1[NumK*28*28+((Y*2+j)*28+(X*2+i))] > pNetSubsampling2[NumK*14*14+(Y*14+X)].Value)	
						{
							pNetSubsampling2[NumK*14*14+(Y*14+X)].Value = pSigmoidConvolution1[NumK*28*28+((Y*2+j)*28+(X*2+i))];
							pNetSubsampling2[NumK*14*14+(Y*14+X)].i = i;
							pNetSubsampling2[NumK*14*14+(Y*14+X)].j = j;
						}
					}
				}
			}
			__syncthreads();
		}
	     	__syncthreads();

		////////////////【前向传播】计算输出层NetOutput神经元的值////////////////
		tmpSum = 0;

		if (X < M && Y == 0) 
		{
			for(int NumK = 0; NumK < 100; NumK++)
			{
				for (int i = 0; i < 196; i++)
				{
			   		tmpSum += pWeightS2toO[X*196*100+NumK*14*14+i] * pNetSubsampling2[NumK*14*14+i].Value;
				}
			}
	    		pNetOutput[X] = tmpSum;
	 	    	pSigmoidOutput[X] = sigmoid (pNetOutput[X]);
	    	}
		__syncthreads();

		if (X < M && Y == 0) 
		{
			pTarget[X] = 0;
			pTarget[*pLabel] = 1;
			pDelta_NetOutput[X] = (-(pTarget[X] - pSigmoidOutput[X])/5) * (pSigmoidOutput[X] * (1 - pSigmoidOutput[X]));
	    	}
		__syncthreads();

		///////////////////////////【反向传播】池化2层---->输出层的权值更新——Error-->pWeightS2toO///////////////////////////	
	     	for (int i = 0; i < M; i++) 
		{
			for(int NumK = 0; NumK < 100; NumK++)   	
			{
				if (X<14 && Y<14) 
				{
					pWeightS2toO[i*196*100 + NumK*196 + Y*14+X] = OldWeightPercentage * (pWeightS2toO[i*196*100 + NumK*196 + Y*14+X]) + NewWeightPercentage * (pWeightS2toO[i*196*100 + NumK*196 + Y*14+X] - LearningRate * (pDelta_NetOutput[i] * pNetSubsampling2[NumK*196 + Y*14+X].Value));			
				}
				__syncthreads();
			}
			__syncthreads();
		}
		__syncthreads();

		///////////////////////////【反向传播】池化2层---->卷积1层的权值更新——Error-->SigmoidSubsampling2(池化2层的误差值＝总误差对池化2层神经元值求偏导)///////////////////////////
		for(int NumK = 0; NumK < 100; NumK++)   	
		{
			tmpSum = 0;

		    	if (X<14 && Y<14) 
			{
			     	for (int i = 0; i < M; i++) 
				{
			   		tmpSum += pWeightS2toO[i*196*100 + NumK*196 + Y*14+X] * pDelta_NetOutput[i];
			   	}
		     	    	pDelta_Subsampling2[NumK*196 + Y*14+X] = tmpSum;
		       	}
			__syncthreads();
		}
		__syncthreads();

		///////////////////////////【反向传播】池化2层---->卷积1层的权值更新——把池化2层的误差值维度还原成上一层（卷积1层）的误差值维度，即除了填充每4格中的最大误差值外，其余位置补0///////////////////////////
		for(int NumK = 0; NumK < 100; NumK++)   	
		{
			if (X < 14 && Y < 14) 
			{
				for(int j = 0; j < 2; j++)
				{
					for (int i = 0; i < 2; i++)
					{
						pUpsample_Delta_Subsampling2[NumK*28*28 + (Y*2+j)*28 + (X*2+i)] = 0;
					}
				}
				pUpsample_Delta_Subsampling2[NumK*28*28 + (Y*2+pNetSubsampling2[NumK*14*14 + Y*14+X].j)*28 + (X*2+pNetSubsampling2[NumK*14*14 + Y*14+X].i)] = pDelta_Subsampling2[NumK*14*14 + Y*14+X];
			}
			__syncthreads();
		}
		__syncthreads();

		///////////////////////////【反向传播】池化2层---->卷积1层的权值更新——卷积1层(池化2层的前一层)的误差值δ＝对池化2层的误差值进行upsample还原 * 卷积1层的激活值对神经元值求偏导///////////////////////////
		for(int NumK = 0; NumK < 100; NumK++)   	
		{
		    	if (X < 28 && Y < 28) 
			{
				pDelta_Convolution1[NumK*28*28 + Y*28+X] = pUpsample_Delta_Subsampling2[NumK*28*28 + Y*28+X] * pSigmoidConvolution1[NumK*28*28 + Y*28+X] * (1 - pSigmoidConvolution1[NumK*28*28 + Y*28+X]);
			}
			__syncthreads();
		}
	   	__syncthreads();

		///////////////////////////【反向传播】卷积1层---->输入层（卷积核）的权值更新///////////////////////////
		tmpSum = 0;

		for(int NumK = 0; NumK < 100; NumK++)   	
		{
			tmpSum = 0;
			if (X < 5 && Y < 5) 
			{
			 	for (int j = 0; j < 28; j++) 
				{
					for (int i = 0; i < 28; i++) 
					{
						//卷积操作一般是要把卷积核旋转180度再做协相关（一一位置对应相乘），不过，由于 CNN 中的卷积参数本来就是学出来的，所以旋不旋转，关系其实不大，这里默认不旋转
						tmpSum += pPadding_NetInput[(Y+j)*32+(X+i)] * pDelta_Convolution1[NumK*28*28 + (j*28+i)];
					}
				}
				pPD_WeightItoC1[NumK*5*5 + Y*5+X] = tmpSum;
				pWeightItoC1[NumK*5*5 + Y*5+X] = (pWeightItoC1[NumK*5*5 + Y*5+X])*OldWeightPercentage + (pWeightItoC1[NumK*5*5 + Y*5+X] - LearningRate * pPD_WeightItoC1[NumK*5*5 + Y*5+X])*NewWeightPercentage;
			}
		   	__syncthreads();
	   	}	
	   	__syncthreads();
	}
}



//*****************************************	以上为GPU上运行的代码	*****************************************//

int main(void) 
{
	// 为设备副本分配空间
	HANDLE_ERROR( cudaMalloc((void **)&d_pNetInput60000,			sizeof(float)*48249054) ); //sizeof(int) = sizeof(float) = 4字节
		
	// 为主机副本分配空间
	pPicture				= (unsigned char*)malloc(N*TestNumber);
	pNetInput60000			= (float*)malloc(sizeof(float)*48249054);

	/******************** CPU上开的整块内存中的分配用于定位的指针 ********************/	
	AllocatePointer();

	//读图片和标签
	ReadTrainPicData(pPicture, pLabel60000);
//	ReadTest10kPicData(pPicture, pLabel60000);

	//读权重
	ReadWeightData(pWeightItoC1, pWeightS2toO);
//	RandomInitWeight(pWeightItoC1, pWeightS2toO);

	/******************** 强制类型转换 ********************/
	TypeCasting();

	//将输入复制到设备
	HANDLE_ERROR( cudaMemcpy(d_pNetInput60000,  pNetInput60000,  sizeof(float)*48249054, 	cudaMemcpyHostToDevice) );

	/******************** 进入网络训练 ********************/
	MaxCorrect = 0;
	for (int traintimes = 1; traintimes < 50; traintimes++)
	{
	 	correct = 0;
		CorrectRate = 0;
		
		printf("第%d次训练：\n", traintimes);

		RunCUDA(CheckNumber);
		HANDLE_ERROR( cudaMemcpy(pNetInput60000,	d_pNetInput60000,	sizeof(float)*48249054,	cudaMemcpyDeviceToHost) );

		//******************** (1)判断图片被识别为数字几(2)判断识别正误(3)分类 ********************
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
			WriteWeightData(pWeightItoC1, pWeightS2toO);
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
	//内存中的指针分配
	pPadding_NetInput =			pNetInput60000 + 						N*TestNumber;
	pWeightItoC1 =				pPadding_NetInput + 					1024;
	pWeightS2toO =				pWeightItoC1 + 						25*100;
	pNetConvolution1 =			pWeightS2toO + 						196*M*100;
	pSigmoidConvolution1 =			pNetConvolution1 + 					N*100;
	pNetSubsampling2 =			(struct MAXPool*)(pSigmoidConvolution1 + 		N*100);
	pNetOutput =				(float*)(pNetSubsampling2 + 				196*100);
	pSigmoidOutput60000 =			pNetOutput + 						M;
	pTarget =					pSigmoidOutput60000 + 					M*TestNumber;			
	pDelta_NetOutput =			pTarget + 							M;	
	pDelta_Subsampling2 =			pDelta_NetOutput + 					M;
	pUpsample_Delta_Subsampling2 =		pDelta_Subsampling2 + 					196*100;	
	pDelta_Convolution1 =			pUpsample_Delta_Subsampling2 +				N*100;				
	pPD_WeightItoC1 =				pDelta_Convolution1 + 					N*100;	
	pLabel60000 =				(unsigned char*)(pPD_WeightItoC1 + 			25*100);

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

		ForwardAndBackPropagation<<<1, threadsPerBlock>>>(d_pNetInput60000, d_pNetInput, d_pPadding_NetInput, d_pWeightItoC1, d_pNetConvolution1, d_pSigmoidConvolution1, d_pNetSubsampling2, d_pWeightS2toO, d_pNetOutput, d_pSigmoidOutput60000, d_pSigmoidOutput, d_pTarget, d_pDelta_NetOutput, d_pDelta_Subsampling2, d_pUpsample_Delta_Subsampling2, d_pDelta_Convolution1, d_pPD_WeightItoC1, d_pLabel60000, d_pLabel);
	}
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
//		printf("第%d张图片被正确识别为%d\n", iNum, *pLabel);
	}
	else
	{
//		printf("第%d张图片应为%d被错误识别为%d\n", iNum, *pLabel, OCR);
	}
	
}



//************************************************************************************************************************
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


/******************** 读文件【训好的权重】 ********************/
void ReadWeightData(float *WeightItoC1, float *WeightS2toO)
{
	int iNum = 0; //神经元个数

	FILE *fpReadWeightItoC1 = fopen("MNIST01/fWeightItoC1Data201811102304.wet","rb");
	FILE *fpReadWeightS2toO = fopen("MNIST01/fWeightS2toOData201811102304.wet","rb");
	
	if(fpReadWeightItoC1 == NULL)
	{
		printf("fWeightItoC1Data201811102304.wet文件不存在！\n");
		return;
	}
	if(fpReadWeightS2toO == NULL)
	{
		printf("fWeightS2toOData201811102304.wet文件不存在！\n");
		return;
	}

	//下面是读数据，将读到的数据存到数组WeightItoC1[NeuronNumConvolution1]中
	iNum = fread(WeightItoC1, sizeof(float), 5*5*100, fpReadWeightItoC1);
	if(iNum != 5*5*100)
	{
		printf("读数据存到数组WeightItoC1[5*5*100]错误！！！\n");
	}
	printf("读【输入层-卷积1层】间的权重数据成功\n");

	//下面是读数据，将读到的数据存到数组WeightS2toO[14*14*100*10]中
	iNum = fread(WeightS2toO, sizeof(float), 14*14*100*10, fpReadWeightS2toO);
	if(iNum != 14*14*100*10)
	{
		printf("读数据存到数组WeightS2toO[14*14*100*10]错误！！！\n");
	}
	printf("读【池化2层-输出层】间的权重数据成功\n");

	fclose(fpReadWeightItoC1);
	fclose(fpReadWeightS2toO);

	return;
}

/******************** 写文件【训好的权重】 ********************/
void WriteWeightData(float *WeightItoC1, float *WeightS2toO)
{
	int iNum = 0;
	FILE *fpWriteWeightItoC1 = fopen("MNIST02/fWeightItoC1Data201811102304.wet","wb+");
	FILE *fpWriteWeightS2toO = fopen("MNIST02/fWeightS2toOData201811102304.wet","wb+");
	
	if(fpWriteWeightItoC1 == NULL)
	{
		printf("创建WeightItoC1Data201811102304.wet文件失败！\n");
		return;
	}
	if(fpWriteWeightS2toO == NULL)
	{
		printf("创建WeightS2toOData201811102304.wet文件失败！\n");
		return;
	}

	//将输入层-卷积1层间的权重写入到WeightItoC1Data201811102304.wet文件中
	iNum = fwrite(WeightItoC1, sizeof(float), 5*5*100, fpWriteWeightItoC1);
	if (iNum != 5*5*100)
	{
		printf("写WeightItoC1Data201811102304.wet文件失败！");
	}
	printf("所有【输入层-卷积1层】间的权重数据已经保存\n");
	//将池化2层-输出层间的权重写入到WeightS2toOData201811102304.wet文件中
	iNum = fwrite(WeightS2toO, sizeof(float), 14*14*100*10, fpWriteWeightS2toO);
	if (iNum != 14*14*100*10)
	{		
		printf("写WeightS2toOData201811102304.wet文件失败！");
	}
	printf("所有【池化2层-输出层】间的权重数据已经保存\n");

	fclose(fpWriteWeightItoC1);
	fclose(fpWriteWeightS2toO);
}


//随机初始化权重
void RandomInitWeight(float *WeightItoC1, float *WeightS2toO)
{
	for(int m = 0; m < 5*5*100; m++)
	{
		WeightItoC1[m] = (rand()/(float)(RAND_MAX))/1000; //【输入层-卷积1层】之间任意2个神经元间的权重W
	}
	printf("所有【输入层-卷积1层】间的权重数据已经初始化完毕！\n");

	for(int m = 0; m < 14*14*100*10; m++)
	{
		WeightS2toO[m] = (rand()/(float)(RAND_MAX))/20000; //【池化2层-输出层】之间任意2个神经元间的权重W
	}
	printf("所有【池化2层-输出层】间的权重数据已经初始化完毕！\n");
}

/******************** 强制类型转换 ********************/
void TypeCasting()
{
	for(int iNum = 0; iNum < N*TestNumber; iNum++)
	{
		*(pNetInput60000 + iNum) = (float)(*(pPicture + iNum));
	}
}

