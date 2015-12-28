#ifndef __CYLINDRICAL_WARPER_H__
#define __CYLINDRICAL_WARPER_H__
#include "opencv.h"
#include "feature/Feature.h"
#include <string>
using namespace std;
using namespace cv;

class CylindricalWarper
{
public:
	void backwardWarp(Mat& imageMat,Feature& features,double focalLength);
private:
	Point2d project(const Point2d& p);
	Point2d project_reverse(const Point2d& p);
	Vec3b bilinear_interpolate(Mat& imageMat, double row, double col);

	double radius;
	double centerX,centerY;
};

#endif