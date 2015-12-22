#ifndef __IMAGE_STICH_H__
#define __IMAGE_STICH_H__
#include <opencv2/opencv.hpp>
#include <string>
#include "util/Util.h"
#include "Feature.h"
using namespace std;
using namespace cv;


class ImageStich
{
public:
	static int stich(string srcImageDir,string outputDir=string(""));
	static vector<Feature> getFeatures(vector<Mat>& imageMats);
	
private:
	static vector<Mat> readFiles(string srcImageDir);
	static void sort(vector<Mat>& imageMats,vector<Mat>& features);
};



#endif