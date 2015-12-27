#ifndef __FEATURE_EXTRACTOR_H__
#define __FEATURE_EXTRACTOR_H__
#include "opencv.h"
#include <string>
using namespace std;
using namespace cv;

class Feature
{
public:
	Feature(){};
	Feature(Mat imageMat);

	pair<vector<Point2f>,vector<Point2f> > match(Feature& other);

	vector<KeyPoint> keyPoints;
	Mat descriptor;
private:

};

#endif