#ifndef __IMAGE_STICH_H__
#define __IMAGE_STICH_H__
#include <opencv2/opencv.hpp>
#include <string>
#include "util/Util.h"
#include "feature/Feature.h"
#include "warp/CylindricalWarper.h"
using namespace std;
using namespace cv;


class Sticher
{
public:
	int stich(vector<string>& imageFiles,string outputDir);
	void extractFeatures();
	
private:
	vector<Mat> readImageFiles(vector<string>& imageFiles);
	void warp();
	void matchFeatures();
	void showMatchFeatures();
	void sort();

	vector<Mat> imageMats;
	vector<Feature> features;
};



#endif