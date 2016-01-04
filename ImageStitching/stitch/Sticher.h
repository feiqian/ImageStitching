#ifndef __IMAGE_STICH_H__
#define __IMAGE_STICH_H__
#include <opencv2/opencv.hpp>
#include <string>
#include <stack>
#include "util/Util.h"
#include "feature/Feature.h"
#include "warp/CylindricalWarper.h"
#include "blend/MultiBandBlender.h"
using namespace std;
using namespace cv;


class Sticher
{
public:
	int stich(vector<string>& imageFiles,string outputDir);
	void extractFeatures();
	
private:
	vector<Mat> readImageFiles(vector<string>& imageFiles);
	void sortImages();
	void warp();
	void matchFeatures();
	void showMatchFeatures();
	void blend();
	void crop();

	vector<Mat> imageMats;
	vector<Feature> features;
	vector<Mat> homographies;
	Mat panorama;
};



#endif