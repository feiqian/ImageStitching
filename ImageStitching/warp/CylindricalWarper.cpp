#include "CylindricalWarper.h"

Point2d CylindricalWarper::project(const Point2d& p)
{
	Point2d coord;
	coord.x = atan((p.x-centerX)/radius);
	coord.y = (p.y-centerY)/sqrt(pow(p.y-centerX,2)+pow(radius,2));
	return coord;
}

Point2d CylindricalWarper::project_reverse(const Point2d& p)
{
	Point2d coord;
	coord.x = radius*tan(p.x)+centerX;
	coord.y = p.y*sqrt(pow(p.x-centerX,2)+pow(radius,2))+centerY;
	return coord;
}

Vec3b CylindricalWarper::bilinear_interpolate(Mat& imageMat, double x, double y)
{
	Vec3b result;
	int x1 = floor(x),y1 = floor(y);
	double dx = x-x1,dy = y-dy;

	if(x1==imageMat.cols-1) --x1;
	if(y1==imageMat.rows-1) --y1;

	Vec3b color[4];
	Vec4d weight;

	weight[0] = (1-dx)*(1-dy);
	weight[1] = dx*(1-dy);
	weight[2] = dx*dy;
	weight[3] = (1-dx)*dy;

	color[0] = imageMat.at<Vec3b>(x1,y1);
	color[1] = imageMat.at<Vec3b>(x1+1,y1);
	color[2] = imageMat.at<Vec3b>(x1+1,y1+1);
	color[3] = imageMat.at<Vec3b>(x1,y1+1);

	for(int i=0;i<4;++i)
	{
		result+=color[i]*weight[i];
	}

	return result;
}

void CylindricalWarper::backwardWarp(Mat& imageMat,Feature& features)
{
	centerX = imageMat.cols/2;
	centerY = imageMat.rows/2;

	double minX=-1<<20,maxX=1>>20,minY=-1<<20,maxY=1>>20;
	for(int i=0;i<imageMat.rows;++i)
	{
		for(int j=0;j<imageMat.cols;++j)
		{
			Point2d& coord = project(Point2d(j,i));
			minX = min(minX,coord.x);
			maxX = max(maxX,coord.x);
			minY = min(minY,coord.y);
			maxY = max(maxY,coord.y);
		}
	}

	Mat destMat(maxX-minX+0.5,maxY-minY+0.5,CV_8UC3,Scalar(0,0,0));
	for(int i=0;i<destMat.rows;++i)
	{
		for(int j=0;j<destMat.cols;++j)
		{
			Point2d& coord = project_reverse(Point2d(j+minY,i+minX));
			if(coord.x>=0&&coord.x<=imageMat.rows&&coord.y>=0&&coord.y<=imageMat.cols)
			{
				destMat.at<Vec3b>(i,j) = bilinear_interpolate(imageMat,j,i);
			}
		}
	}

	//更正特征关键点坐标
	imageMat = destMat;
	vector<KeyPoint>& keyPoints = features.keyPoints;
	for(int i=0,len = keyPoints.size();i<len;++i)
	{
		keyPoints[i].pt = project(keyPoints[i].pt);
	}

}