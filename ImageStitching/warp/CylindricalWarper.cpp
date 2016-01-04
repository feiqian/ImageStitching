#include "CylindricalWarper.h"

Point2d CylindricalWarper::project(const Point2d& p)
{
	Point2d coord;
	coord.x = radius*atan((p.x-centerX)/radius);
	coord.y = radius*(p.y-centerY)/sqrt(pow(p.x-centerX,2)+pow(radius,2));
	return coord;
}

Point2d CylindricalWarper::project_reverse(const Point2d& p)
{
	Point2d coord;
	double radius_inv = 1/radius;

	coord.x = radius*tan(p.x*radius_inv)+centerX;
	coord.y = p.y/cos(p.x*radius_inv) + centerY;
	return coord;
}

Vec3b CylindricalWarper::bilinear_interpolate(Mat& imageMat, double row, double col)
{
	Vec3b result(0,0,0);
	if(row<0||row>imageMat.rows-1||col<0||col>imageMat.cols-1) return result;

	if(row==imageMat.rows-1) --row;
	if(col==imageMat.cols-1) --col;

	int row1 = floor(row),col1 = floor(col);

	Vec3b color[4];
	Vec4d weight;
	double dr = row-row1,dc = col-col1;

	weight[0] = (1-dr)*(1-dc);
	weight[1] = dr*(1-dc);
	weight[2] = dr*dc;
	weight[3] = (1-dr)*dc;

	color[0] = imageMat.at<Vec3b>(row1,col1);
	color[1] = imageMat.at<Vec3b>(row1+1,col1);
	color[2] = imageMat.at<Vec3b>(row1+1,col1+1);
	color[3] = imageMat.at<Vec3b>(row1,col1+1);

	for(int i=0;i<4;++i)
	{
		result+=color[i]*weight[i];
	}

	return result;
}

void CylindricalWarper::backwardWarp(Mat& imageMat,double focalLength)
{
	//focal_pixel = (focal_mm / sensor_width_mm) * image_width_in_pixels
	radius = (37 / 36 )*imageMat.cols;
	centerX = imageMat.cols/2;
	centerY = imageMat.rows/2;

	double minX=1<<20,maxX=-1<<20,minY=1<<20,maxY=-1<<20;
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

	int rows = maxY-minY, cols = maxX-minX;
	Mat destMat(rows,cols,CV_8UC3,Scalar(0,0,0));

	for(int i=0;i<destMat.rows;++i)
	{
		for(int j=0;j<destMat.cols;++j)
		{
			Point2d& coord = project_reverse(Point2d(j+minX,i+minY));
			destMat.at<Vec3b>(i,j) = bilinear_interpolate(imageMat,coord.y,coord.x);
		}
	}

	imageMat = destMat;
}