#ifndef __FEATURE_EXTRACTOR_H__
#define __FEATURE_EXTRACTOR_H__
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/stitching/stitcher.hpp>
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
	Mat imageMat;
private:

};

#endif