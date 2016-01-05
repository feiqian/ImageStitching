#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <vector>
#include "opencv.h"
#include "feature/Feature.h"
#include <opencv2\stitching\detail\motion_estimators.hpp>
#include <opencv2\stitching\detail\camera.hpp>
using namespace std;

class Camera
{
public:
	void estimateParameters(vector<ImageFeatures>& features,vector<MatchesInfo>& matches);

	Mat K,R;
	float focal;
	vector<CameraParams> cameras;
	float ppx,ppy;
};


#endif
