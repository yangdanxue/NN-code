/****************************************************************************************************************
父文件：/home/yangdanxue/CUDA_MNIST201811081625（202010202200，【CNN-784-784-196-10@1】将扩充图片pPadding_NetInput移至GPU完成。已成功跑通，且正确率能在震荡中提升）.cu

参考文件：/home/yangdanxue/201811081625.c
		/home/yangdanxue/CUDA_MNIST201810161953（202008292315，优化代码***** KEY *****一次上传一张图片到GPU，1个global，随机权重。pPD_WeightHiddenToOutput层改为784个线程并行后，成功跑通啦。训1轮用时4'25''~~~!!!）.cu
		/home/yangdanxue/CUDA_MNIST201811081625（202010132255，【CNN-784-784-196-10@1】截至到所有权重均已更新完毕。13s左右60000张图训练1轮。问题：正确率在80%附近震荡。pPD_WeightInputToConvolution1值和手动计算不一致）.cu

当前版本进度： 20220212更新：读取【正确率：90.6467%】测试测试集：【正确数：9097 正确率：90.9700%】。
*****************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define Padding				1024
#define N					784
#define M					10
#define K					25
#define S					196



#define pixels				28		//图片像素是28*28
#define Padding_pixels			32		//填充后图片像素是32*32
#define ConvolutionKernel		5		//卷积核，大小是5*5
#define NeuronNumConvolution1		28		//卷积1层神经元个数
#define NeuronNumSubsampling2		14		//池化2层神经元个数
#define Filter				2		//池化层过滤器大小
#define NeuronNumOutput			10		//输出层神经元个数

#define TestNumber			10000		//总测试图片的数量

#define OldWeightPercentage		0.9   	//反向传播更新权重时旧权重占比
#define NewWeightPercentage		0.1   	//反向传播更新权重时新权重占比
#define LearningRate			0.01		//学习速率

int ReadPicData(unsigned char*, unsigned char*); 	//读图片数据文件

void WriteWeightData(float*, float*);			//保留当前调好的权重
void ReadWeightData(float*, float*);			//使用保存的权重
void InitializationWeight(float*, float*);		//随机初始化各值

unsigned char iPicture[pixels][pixels] = {0};		//导入的MNIST数据库图片数组
unsigned char *pLabel60000;					//指向读取的60000张标签数组的一维指针
unsigned char *pLabel;					//用于反向传播的指向60000张标签的指针，用于当前具体指向哪张图片+标签进行测试+训练（因为是用于定位的指针，不是为了指向开了空间的指针，所以无需开空间+释放空间）

float *pNetInput60000;							 	 	 		//指向读取的60000张图片数组的一维指针
float *pNetInput,						*d_pNetInput;					//指向每次只上传1张图片数据至GPU的指针，的主机副本，与，设备副本
float *pPadding_NetInput,					*d_pPadding_NetInput;			//对于5*5的过滤器，在导入的MNIST数据库图片数组iPicture[y][x]周围，填充宽度为2的0时，就可以保证输出图像和输入图像一样大

//各层权重
float *pWeightInputToConvolution1,				*d_pWeightInputToConvolution1;		//【输入层-卷积1层】卷积核（权重）
float *pWeightSubsampling2ToOutput,			*d_pWeightSubsampling2ToOutput;		//【池化2层-输出层】权重

//各层神经元值 & 激活函数值（Sigmoid函数）
float *pNetConvolution1,					*d_pNetConvolution1;				//卷积1层神经元的值
float *pSigmoidConvolution1,				*d_pSigmoidConvolution1;			//卷积1层激活函数值（Sigmoid函数）

//设定最大池化值的坐标结构体数组
struct MAXPool 
{
	float Value;
	float x;
	float y;
	float i;
	float j;
};struct MAXPool *pNetSubsampling2, *d_pNetSubsampling2;							//池化2层神经元的值

float *pSigmoidSubsampling2,				*d_pSigmoidSubsampling2;			//池化2层激活函数值（Sigmoid函数）
float *pNetOutput,						*d_pNetOutput;				//输出层神经元的值
float *pSigmoidOutput,					*d_pSigmoidOutput;				//输出层激活函数值（Sigmoid函数）

//目标输出值
float *pTarget,						*d_pTarget;					//Target的主机副本，与，设备副本	

//输出层---->池化2层的权值更新
float *pPD_ErrorToSigmoidOutput,				*d_pPD_ErrorToSigmoidOutput;
float *pPD_SigmoidOutputToNetOutput,			*d_pPD_SigmoidOutputToNetOutput;
float *pDelta_NetOutput,					*d_pDelta_NetOutput;
float *pPD_NetOutputToWeightSubsampling2ToOutput,	*d_pPD_NetOutputToWeightSubsampling2ToOutput;
float *pPD_WeightSubsampling2ToOutput,			*d_pPD_WeightSubsampling2ToOutput;

//池化2层---->卷积1层的权值更新
float *pPD_NetOutputToSigmoidSubsampling2,		*d_pPD_NetOutputToSigmoidSubsampling2;
float *pPD_SigmoidSubsampling2ToNetSubsampling2,		*d_pPD_SigmoidSubsampling2ToNetSubsampling2;
float *pDelta_Subsampling2,					*d_pDelta_Subsampling2;
float *pUpsample_Delta_Subsampling2,			*d_pUpsample_Delta_Subsampling2;
float *pPD_SigmoidConvolution1ToNetConvolution1,		*d_pPD_SigmoidConvolution1ToNetConvolution1;
float *pDelta_Convolution1,					*d_pDelta_Convolution1;
float *pPD_WeightInputToConvolution1,			*d_pPD_WeightInputToConvolution1;

int Correct = 0;//识别正确数
//***********************************************************************************************************

////////////////////////////////////////////	前向传播（开始）	////////////////////////////////////////////
//计算sigmoid的值
__device__ __forceinline__ float sigmoid (float a)
{
    return 1.0 / (1.0 + exp (-a));
}

__global__ void ForwardAndBackPropagation(float *pNetInput, float *pPadding_NetInput, float *pWeightInputToConvolution1, float *pNetConvolution1, float *pSigmoidConvolution1, struct MAXPool *pNetSubsampling2, float *pSigmoidSubsampling2, float *pWeightSubsampling2ToOutput, float *pNetOutput, float *pSigmoidOutput, float *pTarget, float *pPD_ErrorToSigmoidOutput, float *pPD_SigmoidOutputToNetOutput, float *pDelta_NetOutput, float *pPD_NetOutputToWeightSubsampling2ToOutput, float *pPD_WeightSubsampling2ToOutput, float *pPD_NetOutputToSigmoidSubsampling2, float *pPD_SigmoidSubsampling2ToNetSubsampling2, float *pDelta_Subsampling2, float *pUpsample_Delta_Subsampling2, float *pPD_SigmoidConvolution1ToNetConvolution1, float *pDelta_Convolution1, float *pPD_WeightInputToConvolution1)
{
	int Y = blockIdx.y*blockDim.y+threadIdx.y;
 	int X = blockIdx.x*blockDim.x+threadIdx.x;

	//在iPicture[y][x]周围，填充2圈0——因为在图像边缘的像素在输出中采用较少，为了不丢掉了很多图像边缘的信息，在图像卷积操作之前，沿着图像边缘用0进行图像填充。对于5*5的过滤器，填充宽度为2时，就可以保证输出图像和输入图像一样大。
    	if (X < 28 && Y < 28) 
	{
		pPadding_NetInput[(Y+2)*32+(X+2)] = pNetInput[Y*28+X];
	}
     	__syncthreads();


	float tmpSum = 0;
   	///////////////////////////【前向传播】计算卷积1层NetConvolution1神经元的值///////////////////////////
    	if (X < 28 && Y < 28) 
	{
     		for (int j = 0; j < 5; j++) 
		{
	     		for (int i = 0; i < 5; i++) 
			{
		   		tmpSum += pPadding_NetInput[(Y+j)*32+(X+i)] * pWeightInputToConvolution1[j*5+i];
			}
		} 
     		pNetConvolution1[Y*28+X] = tmpSum;
		
	   	///////////////////////////【前向传播】计算卷积1层NetConvolution1的sigmoid函数值///////////////////////////
	    	pSigmoidConvolution1[Y*28+X] = sigmoid (pNetConvolution1[Y*28+X]);
     	}
     	__syncthreads();


	///////////////////////////【前向传播】计算池化2层NetSubsampling2神经元的值///////////////////////////
    	if (X < 14 && Y < 14) 
	{
		pNetSubsampling2[Y*14+X].Value = 0;
		pNetSubsampling2[Y*14+X].i = 0;
		pNetSubsampling2[Y*14+X].j = 0;

		for(int j = 0; j < 2; j++)
		{
			for (int i = 0; i < 2; i++)
			{
				//求出每2*2过滤器中的最大值，组成池化层
				if (pSigmoidConvolution1[(Y*2+j)*28+(X*2+i)] > pNetSubsampling2[Y*14+X].Value)
				{
					pNetSubsampling2[Y*14+X].Value = pSigmoidConvolution1[(Y*2+j)*28+(X*2+i)];
					pNetSubsampling2[Y*14+X].i = i;
					pNetSubsampling2[Y*14+X].j = j;
				}
			}
		}

	   	///////////////////////////【前向传播】计算池化2层NetSubsampling2的sigmoid函数值///////////////////////////
//	    	pSigmoidSubsampling2[Y*14+X] = pNetSubsampling2[Y*14+X].j;		//验证i、j值都正确啦（与【pNetSubsampling2[Y*14+X].i = i】与【pNetSubsampling2[Y*14+X].j = j】两行代码匹配）。
	    	pSigmoidSubsampling2[Y*14+X] = pNetSubsampling2[Y*14+X].Value;	//全部写成数组形式，pSigmoidSubsampling2值全部正确
	}
     	__syncthreads();


	////////////////【前向传播】计算输出层NetOutput神经元的值////////////////
	tmpSum = 0;

	if (X < M && Y == 0) 
	{
		for (int i = 0; i < 196; i++)
		{
           		tmpSum += pWeightSubsampling2ToOutput[X * 196 + i] * pSigmoidSubsampling2[i];
		}
    		pNetOutput[X] = tmpSum;
    		
    		///////////////////////////【前向传播】计算输出层NetOutput的sigmoid函数值///////////////////////////
    		pSigmoidOutput[X] = sigmoid (pNetOutput[X]);
    	}
	__syncthreads();
}



//*****************************************	以上为GPU上运行的代码	*****************************************//

int main(void) 
{
	unsigned char Size = sizeof(float);
	unsigned char *pPicture;
	

	// a、b、c设备副本的分配空间
	cudaMalloc((void **)&d_pWeightInputToConvolution1, Size*K);
	cudaMalloc((void **)&d_pWeightSubsampling2ToOutput, Size*(S*M));
	cudaMalloc((void **)&d_pNetInput, Size*N);
	cudaMalloc((void **)&d_pPadding_NetInput, Size*Padding);
	cudaMalloc((void **)&d_pNetConvolution1, Size*N);
	cudaMalloc((void **)&d_pSigmoidConvolution1, Size*N);
	cudaMalloc((void **)&d_pNetSubsampling2, Size*S*5);
	cudaMalloc((void **)&d_pSigmoidSubsampling2, Size*S);
	cudaMalloc((void **)&d_pNetOutput, Size*M);
	cudaMalloc((void **)&d_pSigmoidOutput, Size*M);
	////////////////////////////////////////////////////////////////////////
	cudaMalloc((void **)&d_pTarget, Size*M);
	cudaMalloc((void **)&d_pPD_ErrorToSigmoidOutput, Size*M);
	cudaMalloc((void **)&d_pPD_SigmoidOutputToNetOutput, Size*M);	
	cudaMalloc((void **)&d_pDelta_NetOutput, Size*M);					
	cudaMalloc((void **)&d_pPD_NetOutputToWeightSubsampling2ToOutput, Size*(S*M));
	cudaMalloc((void **)&d_pPD_WeightSubsampling2ToOutput, Size*(S*M));
	////////////////////////////////////////////////////////////////////////	
	cudaMalloc((void **)&d_pPD_NetOutputToSigmoidSubsampling2, Size*(S*M));
	cudaMalloc((void **)&d_pPD_SigmoidSubsampling2ToNetSubsampling2, Size*S);
	cudaMalloc((void **)&d_pDelta_Subsampling2, Size*S);
	cudaMalloc((void **)&d_pUpsample_Delta_Subsampling2, Size*N);
	cudaMalloc((void **)&d_pPD_SigmoidConvolution1ToNetConvolution1, Size*N);	
	cudaMalloc((void **)&d_pDelta_Convolution1, Size*N);		
	////////////////////////////////////////////////////////////////////////	
	cudaMalloc((void **)&d_pPD_WeightInputToConvolution1, Size*K);		


	//为a、b、c和设置输入值的主机副本分配空间
	pWeightInputToConvolution1 = (float*)malloc(Size*K);
	pWeightSubsampling2ToOutput = (float*)malloc(Size*(S*M));
	pPicture = (unsigned char*)malloc(N*TestNumber);
	pLabel60000 = (unsigned char*)malloc(TestNumber);
	pNetInput60000 = (float*)malloc(Size*(N*TestNumber));
	pPadding_NetInput = (float*)malloc(Size*Padding);
	pNetConvolution1 = (float*)malloc(Size*N);
	pSigmoidConvolution1 = (float*)malloc(Size*N);
	pNetSubsampling2 = (struct MAXPool*)malloc(Size*S*5);
	pSigmoidSubsampling2 = (float*)malloc(Size*S);
	pNetOutput = (float*)malloc(Size*M);	
	pSigmoidOutput = (float*)malloc(Size*M);
	////////////////////////////////////////////////////////////////////////
	pTarget = (float*)malloc(Size*M);			
	pPD_ErrorToSigmoidOutput = (float*)malloc(Size*M);	
	pPD_SigmoidOutputToNetOutput = (float*)malloc(Size*M);
	pDelta_NetOutput = (float*)malloc(Size*M);	
	pPD_NetOutputToWeightSubsampling2ToOutput = (float*)malloc(Size*(S*M));
	pPD_WeightSubsampling2ToOutput = (float*)malloc(Size*(S*M));
	////////////////////////////////////////////////////////////////////////	
	pPD_NetOutputToSigmoidSubsampling2 = (float*)malloc(Size*(S*M));
	pPD_SigmoidSubsampling2ToNetSubsampling2 = (float*)malloc(Size*S);
	pDelta_Subsampling2 = (float*)malloc(Size*S);
	pUpsample_Delta_Subsampling2 = (float*)malloc(Size*N);		
	pPD_SigmoidConvolution1ToNetConvolution1 = (float*)malloc(Size*N);
	pDelta_Convolution1 = (float*)malloc(Size*N);				
	////////////////////////////////////////////////////////////////////////		
	pPD_WeightInputToConvolution1 = (float*)malloc(Size*K);	
	
			
	
	//读权重
	ReadWeightData(pWeightInputToConvolution1, pWeightSubsampling2ToOutput);
//	InitializationWeight(pWeightInputToConvolution1, pWeightSubsampling2ToOutput);

	//将输入复制到设备
	cudaMemcpy(d_pWeightInputToConvolution1,  pWeightInputToConvolution1,  Size*K, cudaMemcpyHostToDevice);
	cudaMemcpy(d_pWeightSubsampling2ToOutput, pWeightSubsampling2ToOutput, Size*(S*M), cudaMemcpyHostToDevice);
			
	//读图片和标签
	ReadPicData(pPicture, pLabel60000);

	for (int k = 1; k < 2; k++)
	{
		int result = 0;	//声明在main()开始处，会导致pSigmoidOutput[0]不参与if判断，从而所有的图片0都会被识别为除pSigmoidOutput[0]值外的最大值对应的元素数字。由i=1，改为i=0即可。
		int Correct = 0;	//识别正确数
		float CorrectRate = 0;


		printf("第%d次训练：\n", k);
		
		for(int i = 0; i < N*TestNumber; i++)
		{
			*(pNetInput60000 + i) = (float)(*(pPicture + i));	//新开一个指针pNetInput60000，指向一次性读取60000张的图片数组。该指针始终指向60000张的图片数组的首地址。
		}


		pLabel = pLabel60000;	//pLabel：用于反向传播的指向60000张标签的指针，用于当前具体指向哪张图片+标签进行测试+训练（因为是用于定位的指针，不是为了指向开了空间的指针，所以无需开空间+释放空间）。
						//在未声明指针位置时，指针默认指向内存0地址。【pLabel = pLabel60000;】此赋值语句表示，让pLabelBP60000指向与pLabel60000相同的地址。

		for(int iNum = 0; iNum < TestNumber; iNum++)
		{
			pNetInput = pNetInput60000 + iNum*N;	

			for (int i = 0; i < 10; i++)
			{
				*(pTarget + i) = 0;	//清理数组Target[]
			}

			pLabel = pLabel60000 + iNum;
			*(pTarget + *pLabel) = 1;	//语句想要实现的目的：给数组Target的第label个元素赋值为1，之前的写法是【*(pTarget + pLabel60000) = 1】
								//【pLabel】是反向传播中用于指向60000张标签的定位用的指针，【*pLabel】是该指针指向的地址中存放的值。
								//比如【pLabel】指向第一张图片5的地址，【*pLabel】=5。【&pLabel】的意思是存放指针pLabelBP60000变量的地址。
			//将输入复制到设备
			cudaMemcpy(d_pNetInput,			pNetInput,			Size*N,		cudaMemcpyHostToDevice);
			cudaMemcpy(d_pTarget,			pTarget,			Size*M,		cudaMemcpyHostToDevice);
			
			dim3 blocksPerGrid(1, 1);
			dim3 threadsPerBlock(28, 28);
			
			//在带有N个块的GPU上启动add（）内核
			ForwardAndBackPropagation<<<blocksPerGrid, threadsPerBlock>>>(d_pNetInput, d_pPadding_NetInput, d_pWeightInputToConvolution1, d_pNetConvolution1, d_pSigmoidConvolution1, d_pNetSubsampling2, d_pSigmoidSubsampling2, d_pWeightSubsampling2ToOutput, d_pNetOutput, d_pSigmoidOutput, d_pTarget, d_pPD_ErrorToSigmoidOutput, d_pPD_SigmoidOutputToNetOutput, d_pDelta_NetOutput, d_pPD_NetOutputToWeightSubsampling2ToOutput, d_pPD_WeightSubsampling2ToOutput, d_pPD_NetOutputToSigmoidSubsampling2, d_pPD_SigmoidSubsampling2ToNetSubsampling2, d_pDelta_Subsampling2, d_pUpsample_Delta_Subsampling2, d_pPD_SigmoidConvolution1ToNetConvolution1, d_pDelta_Convolution1, d_pPD_WeightInputToConvolution1);


			//将结果复制回主机
//			cudaMemcpy(pNetInput,						d_pNetInput,						Size*N,		cudaMemcpyDeviceToHost);
			cudaMemcpy(pPadding_NetInput,					d_pPadding_NetInput,					Size*Padding,	cudaMemcpyDeviceToHost);
			cudaMemcpy(pNetConvolution1, 		  	 		d_pNetConvolution1, 		   			Size*N, 		cudaMemcpyDeviceToHost);
			cudaMemcpy(pSigmoidConvolution1, 	   			d_pSigmoidConvolution1, 	  	 			Size*N, 		cudaMemcpyDeviceToHost);
			cudaMemcpy(pNetSubsampling2, 		  	 		d_pNetSubsampling2, 		   			Size*S*5, 		cudaMemcpyDeviceToHost);
			cudaMemcpy(pSigmoidSubsampling2, 	   			d_pSigmoidSubsampling2, 	  	 			Size*S, 		cudaMemcpyDeviceToHost);
			cudaMemcpy(pNetOutput, 		   				d_pNetOutput, 		   				Size*M, 		cudaMemcpyDeviceToHost);	
			cudaMemcpy(pSigmoidOutput, 	   				d_pSigmoidOutput, 	   				Size*M, 		cudaMemcpyDeviceToHost);
			cudaMemcpy(pPD_ErrorToSigmoidOutput,				d_pPD_ErrorToSigmoidOutput,				Size*M,		cudaMemcpyDeviceToHost);
			cudaMemcpy(pPD_SigmoidOutputToNetOutput,			d_pPD_SigmoidOutputToNetOutput,			Size*M,		cudaMemcpyDeviceToHost);
			cudaMemcpy(pDelta_NetOutput,					d_pDelta_NetOutput,					Size*M,		cudaMemcpyDeviceToHost);
			cudaMemcpy(pPD_NetOutputToWeightSubsampling2ToOutput,	d_pPD_NetOutputToWeightSubsampling2ToOutput,	Size*(S*M),		cudaMemcpyDeviceToHost);
			cudaMemcpy(pPD_WeightSubsampling2ToOutput, 		d_pPD_WeightSubsampling2ToOutput, 			Size*(S*M),		cudaMemcpyDeviceToHost);
			cudaMemcpy(pWeightSubsampling2ToOutput, 			d_pWeightSubsampling2ToOutput, 			Size*(S*M),		cudaMemcpyDeviceToHost);
			cudaMemcpy(pPD_NetOutputToSigmoidSubsampling2, 		d_pPD_NetOutputToSigmoidSubsampling2, 		Size*(S*M),		cudaMemcpyDeviceToHost);
			cudaMemcpy(pPD_SigmoidSubsampling2ToNetSubsampling2, 	d_pPD_SigmoidSubsampling2ToNetSubsampling2, 	Size*S,		cudaMemcpyDeviceToHost);
			cudaMemcpy(pDelta_Subsampling2, 				d_pDelta_Subsampling2, 					Size*S,		cudaMemcpyDeviceToHost);				
			cudaMemcpy(pUpsample_Delta_Subsampling2, 			d_pUpsample_Delta_Subsampling2, 			Size*N,		cudaMemcpyDeviceToHost);
			cudaMemcpy(pPD_SigmoidConvolution1ToNetConvolution1, 	d_pPD_SigmoidConvolution1ToNetConvolution1,	Size*N,		cudaMemcpyDeviceToHost);
			cudaMemcpy(pDelta_Convolution1, 				d_pDelta_Convolution1,					Size*N,		cudaMemcpyDeviceToHost);
			cudaMemcpy(pPD_WeightInputToConvolution1, 		  	d_pPD_WeightInputToConvolution1, 		   	Size*K, 		cudaMemcpyDeviceToHost);
			cudaMemcpy(pWeightInputToConvolution1, 			d_pWeightInputToConvolution1, 				Size*K,		cudaMemcpyDeviceToHost);



			for (int i = 0; i < M; i++)	//声明在main()开始处，会导致pSigmoidOutput[0]不参与if判断，从而所有的图片0都会被识别为除pSigmoidOutput[0]值外的最大值对应的元素数字。由i=1，改为i=0即可（20200705改）。
			{
				if (pSigmoidOutput[result] < pSigmoidOutput[i]) //10个值比大小，哪个大说明网络把图片识别为几
				{
					result = i;
				}	
			}
/*			printf("读取第%d张图片是%d \n",iNum+1, *pLabel);
			printf("被网络识别为[%d] \n", result);
			
			for(int i = 0; i < 10; i++)
			{
				printf("pSigmoidOutput[%d]=%e\n", i, *(pSigmoidOutput + i));
			}
*/	
			
			if (result == *pLabel)
			{	
				Correct++;
			}
		}	
		CorrectRate = ((float)Correct) / TestNumber;
		printf("正确数：%d 正确率：%.4f%%\n", Correct, CorrectRate * 100);
	}

}








//************************************************************************************************************************
int ReadPicData(unsigned char*data, unsigned char*pLabel60000)
{
	int iCheck = 0;

//	FILE *fp_image = fopen("MNIST/train-images.idx", "rb");//打开这个二进制(b)文件train-images.idx，以只读(r)的方式打开，
//	FILE *fp_label = fopen("MNIST/train-labels.idx", "rb");//读类标
	FILE *fp_image = fopen("MNIST/t10k_images.idx", "rb");//打开这个二进制(b)文件t10k-images.idx，以只读(r)的方式打开，
	FILE *fp_label = fopen("MNIST/t10k_labels.idx", "rb");//读类标
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


//读权重——使用保存的权重
void ReadWeightData(float *WeightInputToConvolution1, float *WeightSubsampling2ToOutput)
{
	int iNum = 0; //神经元个数

	FILE *fpReadItoC1 = fopen("MNIST01/fWeightItoC1Data201811081625.wet","rb");
	FILE *fpReadS2toO = fopen("MNIST01/fWeightS2toOData201811081625.wet","rb");
	
	if(fpReadItoC1 == NULL)
	{
		printf("fWeightItoC1Data201811081625.wet文件不存在！\n");
		return;
	}
	if(fpReadS2toO == NULL)
	{
		printf("fWeightS2toOData201811081625.wet文件不存在！\n");
		return;
	}

	//下面是读数据，将读到的数据存到数组WeightInputToConvolution1[NeuronNumConvolution1]中
	iNum = fread(WeightInputToConvolution1, sizeof(float), ConvolutionKernel * ConvolutionKernel, fpReadItoC1);
	if(iNum != ConvolutionKernel * ConvolutionKernel)
	{
		printf("读数据存到数组WeightInputToConvolution1[ConvolutionKernel * ConvolutionKernel]错误！！！\n");
	}
	printf("读[输入层-卷积1层]间的权重数据成功\n");

	//下面是读数据，将读到的数据存到数组WeightSubsampling2ToOutput[NeuronNumSubsampling2 * NeuronNumOutput]中
	iNum = fread(WeightSubsampling2ToOutput, sizeof(float), NeuronNumSubsampling2 * NeuronNumSubsampling2 * NeuronNumOutput, fpReadS2toO);
	if(iNum != NeuronNumSubsampling2 * NeuronNumSubsampling2 * NeuronNumOutput)
	{
		printf("读数据存到数组WeightSubsampling2ToOutput[NeuronNumSubsampling2 * NeuronNumSubsampling2 * NeuronNumOutput]错误！！！\n");
	}
	printf("读[池化2层-输出层]间的权重数据成功\n");

	fclose(fpReadItoC1);
	fclose(fpReadS2toO);

	return;
}



//写权重——保留当前训好的权重
void WriteWeightData(float *WeightInputToConvolution1, float *WeightSubsampling2ToOutput)
{
	int iNum = 0;
	FILE *fpWriteInputToConvolution1 = fopen("MNIST02/fWeightItoC1Data201811081625.wet","wb+");
	FILE *fpWriteSubsampling2ToOutput = fopen("MNIST02/fWeightS2toOData201811081625.wet","wb+");

	if(fpWriteInputToConvolution1 == NULL)
	{
		printf("创建WeightItoC1Data201811081625.wet文件失败！\n");
		return;
	}
	if(fpWriteSubsampling2ToOutput == NULL)
	{
		printf("创建WeightS2toOData201811081625.wet文件失败！\n");
		return;
	}

	//将输入层-卷积1层间的权重写入到WeightItoC1Data201811081625.wet文件中
	iNum = fwrite(WeightInputToConvolution1, sizeof(float), ConvolutionKernel * ConvolutionKernel, fpWriteInputToConvolution1);
	if (iNum != ConvolutionKernel * ConvolutionKernel)
	{
		printf("写WeightItoC1Data201811081625.wet文件失败！");
	}
	printf("所有输入层-卷积1层间的权重数据已经保存\n");

	//将池化2层-输出层间的权重写入到WeightS2toOData201811081625.wet文件中
	iNum = fwrite(WeightSubsampling2ToOutput, sizeof(float), NeuronNumSubsampling2 * NeuronNumSubsampling2 * NeuronNumOutput, fpWriteSubsampling2ToOutput);
	if (iNum != NeuronNumSubsampling2 * NeuronNumSubsampling2 * NeuronNumOutput)
	{		
		printf("写WeightS2toOData201811081625.wet文件失败！");
	}
	printf("所有池化2层-输出层间的权重数据已经保存\n");

	fclose(fpWriteInputToConvolution1);
	fclose(fpWriteSubsampling2ToOutput);
}



/*
//写权重——保留当前调好的[输入层-卷积1层]权重
void WriteWeightInputToConvolution1Data(float *WeightInputToConvolution1)
{
	int iNum = 0;
	FILE *fpWriteInputToConvolution1 = fopen("MNIST/fWeightItoC1Data201811081625.wet","wb+");

	if(fpWriteInputToConvolution1 == NULL)
	{
		printf("创建WeightItoC1Data201811081625.wet文件失败！\n");
		return;
	}

	//将输入层-卷积1层间的权重写入到WeightItoC1Data201811081625.wet文件中
	iNum = fwrite(WeightInputToConvolution1, sizeof(float), ConvolutionKernel * ConvolutionKernel, fpWriteInputToConvolution1);
	if (iNum != ConvolutionKernel * ConvolutionKernel)
	{
		printf("写WeightItoC1Data201811081625.wet文件失败！");
	}
	printf("所有输入层-卷积1层间的权重数据已经保存\n");

	fclose(fpWriteInputToConvolution1);
}


//写权重——保留当前调好的[池化2层-输出层]权重
void WriteWeightSubsampling2ToOutputData(float *WeightSubsampling2ToOutput)
{
	int iNum = 0;
	FILE *fpWriteSubsampling2ToOutput = fopen("MNIST/fWeightS2toOData201811081625.wet","wb+");

	if(fpWriteSubsampling2ToOutput == NULL)
	{
		printf("创建WeightS2toOData201811081625.wet文件失败！\n");
		return;
	}

	//将池化2层-输出层间的权重写入到WeightS2toOData201811081625.wet文件中
	iNum = fwrite(WeightSubsampling2ToOutput, sizeof(float), NeuronNumSubsampling2 * NeuronNumSubsampling2 * NeuronNumOutput, fpWriteSubsampling2ToOutput);
	if (iNum != NeuronNumSubsampling2 * NeuronNumSubsampling2 * NeuronNumOutput)
	{		
		printf("写WeightS2toOData201811081625.wet文件失败！");
	}
	printf("所有池化2层-输出层间的权重数据已经保存\n");

	fclose(fpWriteSubsampling2ToOutput);
}			


//随机初始化[输入层-卷积1层]权重
void InitializationWeightInputToConvolution1(float *WeightInputToConvolution1)
{
//	int m = 0;
//	int n = 0;

	for(int m = 0; m < ConvolutionKernel * ConvolutionKernel; m++)
	{
		//WeightInputToConvolution1[m] = 0.5/2000; //【输入层-卷积1层】之间任意2个神经元间的权重W
		WeightInputToConvolution1[m] = (rand()/(float)(RAND_MAX))/1000; //【输入层-卷积1层】之间任意2个神经元间的权重W
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
void InitializationWeightSubsampling2ToOutput(float *WeightSubsampling2ToOutput)
{
//	int m = 0;

	for(int m = 0; m < NeuronNumSubsampling2 * NeuronNumSubsampling2 * NeuronNumOutput; m++)
	{
		//WeightSubsampling2ToOutput[m] = 0; //【池化2层-输出层】之间任意2个神经元间的权重W
		WeightSubsampling2ToOutput[m] = (rand()/(float)(RAND_MAX))/200; //【池化2层-输出层】之间任意2个神经元间的权重W
		//WeightSubsampling2ToOutput[m] = -0.065319700309; ///89.9467%版权重的中位数
		//WeightSubsampling2ToOutput[m] = -0.060853877; ///89.9467%版权重的平均数
	}
}
*/

//随机初始化权重
void InitializationWeight(float *WeightInputToConvolution1, float *WeightSubsampling2ToOutput)
{
	for(int m = 0; m < ConvolutionKernel * ConvolutionKernel; m++)
	{
		WeightInputToConvolution1[m] = (rand()/(float)(RAND_MAX))/1000; //【输入层-卷积1层】之间任意2个神经元间的权重W
	}

	for(int m = 0; m < NeuronNumSubsampling2 * NeuronNumSubsampling2 * NeuronNumOutput; m++)
	{
		WeightSubsampling2ToOutput[m] = (rand()/(float)(RAND_MAX))/200; //【池化2层-输出层】之间任意2个神经元间的权重W
	}
}

