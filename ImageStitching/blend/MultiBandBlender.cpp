#include "MultiBandBlender.h"

Mat MultiBandBlender::reconstruct(vector<Mat>& laplacianPymid1,vector<Mat>& laplacianPymid2,vector<Mat>& gaussianPymidMask)
{
	Mat destMat,tempMat;
	for(int level= laplacianPymid1.size()-1;level>=0;--level)
	{
		int rows =gaussianPymidMask[level].rows,cols = gaussianPymidMask[level].cols;
		if(destMat.empty())
		{
			destMat = Mat::zeros(rows,cols,CV_32FC3);
		}
		else 
		{
			tempMat = destMat;
			pyrUp(tempMat,destMat,Size(cols,rows));
		}
		destMat+=(laplacianPymid1[level].mul(gaussianPymidMask[level])+laplacianPymid2[level].mul(Scalar(1.0,1.0,1.0) - gaussianPymidMask[level]));
	}
	return destMat;
}

void MultiBandBlender::buildGaussianPyramid(Mat& imageMat,vector<Mat>& gaussianPymid)
{
	Mat currMat = imageMat,tempMat;
	gaussianPymid.push_back(imageMat);

	while(currMat.rows>=2&&currMat.cols>=2)
	{
		pyrDown(currMat,tempMat);
		gaussianPymid.push_back(tempMat);
		currMat = tempMat;
	}
}

void MultiBandBlender::buildLaplacianPyramid(vector<Mat>& gaussianPymid,vector<Mat>& laplacianPymid)
{
	int len=gaussianPymid.size();
	if(len<=0) return;

	for(int i=1;i<len;++i)
	{
		Mat upsampleMat;
		pyrUp(gaussianPymid[i],upsampleMat,gaussianPymid[i-1].size());
		laplacianPymid.push_back(gaussianPymid[i-1]-upsampleMat);
	}
	laplacianPymid.push_back(gaussianPymid[len-1]);
}

Mat MultiBandBlender::blend(Mat& imageMat1,Mat& imageMat2,Mat& mask)
{
	assert(imageMat1.rows==imageMat2.rows&&imageMat2.rows==mask.rows);
	assert(imageMat1.cols==imageMat2.cols&&imageMat2.cols==mask.cols);

	vector<Mat> gaussianPymid1,gaussianPymid2,gaussianPymidMask;
	vector<Mat> laplacianPymid1,laplacianPymid2;

	buildGaussianPyramid(imageMat1,gaussianPymid1);
	buildLaplacianPyramid(gaussianPymid1,laplacianPymid1);
	buildGaussianPyramid(imageMat2,gaussianPymid2);
	buildLaplacianPyramid(gaussianPymid2,laplacianPymid2);
	buildGaussianPyramid(mask,gaussianPymidMask);
	
	return reconstruct(laplacianPymid1,laplacianPymid2,gaussianPymidMask);
}

