#ifndef __IMAGE_STICH_H__
#define __IMAGE_STICH_H__
#include <opencv2/opencv.hpp>
#include <string>
#include <stack>
#include <time.h>
#include "util/Util.h"
#include "feature/Feature.h"
#include "warp/CylindricalWarper.h"
#include "blend/MultiBandBlender.h"
#include "camera/camera.h"
using namespace std;

class Sticher
{
public:
	void stich(vector<string>& imageFiles,string outputDir);
	void extractFeatures();
	
private:
	vector<Mat> readImageFiles(vector<string>& imageFiles);
	void sortImages();
	void warp();
	void matchFeatures();
	void showMatchFeatures();
	void getHomography();
	void blend();
	void crop();
	void outputResult(string outputDir);

	vector<Mat> imageMats;
	vector<Mat> floatImageMats;
	vector<ImageFeatures> features;
	vector<Mat> homographies;
	vector<MatchesInfo> pairwise_matches;
	vector<MatchesInfo>	matches; 
	Mat panorama;
	Mat cropPanorama;
};



#endif