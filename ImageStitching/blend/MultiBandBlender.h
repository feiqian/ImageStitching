#ifndef __MULTI_BADN_BLENDER_H__
#define __MULTI_BAND_BLENDER_H__
#include "opencv.h"
using namespace cv;

class MultiBandBlender
{
public:
	Mat blend(Mat& imageMat1,Mat& imageMat2,Mat& mask);
private:
	Mat reconstruct(vector<Mat>& laplacianPymid1,vector<Mat>& laplacianPymid2,vector<Mat>& gaussianPymidMask);
	void buildGaussianPyramid(Mat& imageMat,vector<Mat>& gaussianPymid);
	void buildLaplacianPyramid(vector<Mat>& gaussianPymid,vector<Mat>& laplacianPymid);
};

#endif