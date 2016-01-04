#include "Sticher.h"

vector<Mat> Sticher::readImageFiles(vector<string>& imageFiles)
{
	imageMats.resize(imageFiles.size());
	for(int i=0,len=imageFiles.size();i<len;++i)
	{
		imageMats[i] = imread(imageFiles[i]);
	}
	return imageMats;
}

void Sticher::sortImages()
{

}

void Sticher::extractFeatures()
{
	features.resize(imageMats.size());

	for(int i=0,len=imageMats.size();i<len;++i)
	{
		features[i] = Feature(imageMats[i]);
	}
}

void Sticher::warp()
{
	CylindricalWarper wraper;
	for(int i=0,len=imageMats.size();i<len;++i)
	{
		wraper.backwardWarp(imageMats[i],18);
		//draw Image,for test
		//imshow("CylindricalWarper", imageMat);
		//waitKey(0);
	}
}

void Sticher::matchFeatures()
{
	int threshold = 10;

	for(int i=0,len=features.size()-1;i<len;++i)
	{
		Mat &leftImage = imageMats[i],&rightImage = imageMats[i+1];
		Feature &leftFeature = features[i],&rightFeature = features[i+1];

		vector<DMatch>& matches = leftFeature.match(rightFeature);
		
		vector<Point2f> leftMatchPoints(matches.size());
		vector<Point2f> rightMatchPoints(matches.size());

		for(int k=0;k<matches.size();k++)
		{
			leftMatchPoints[k] = leftFeature.keyPoints[matches[k].queryIdx].pt;
			rightMatchPoints[k] = rightFeature.keyPoints[matches[k].trainIdx].pt;
		}

		homographies.push_back(findHomography(rightMatchPoints,leftMatchPoints,CV_RANSAC));

		//Draw matches,for test
		//Mat img_matches;
		//drawMatches(leftImage, leftFeature.keyPoints,rightImage , rightFeature.keyPoints,
		//	matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		//	vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
		//imshow("Feature Matches", img_matches);
		//waitKey(0);
	}
}

void gainAdjustment(Mat& image,Mat& mask)
{
	//int dir[][2]={{0,1},{0,-1},{-1,0},{1,0}};
	//for(int i=0;i<image.row;++i)
	//{
	//	for(int j=0;j<image.cols;++i)
	//	{
	//		for(int k=0;k<4;++k)
	//		{
	//			if(((int)mask.at<uchar>(i,j))>0)
	//			{
	//				int new_r = i+dir[k][0],newCol = j+dir[k][1];
	//			}
	//		}
	//	}
	//}
}

void Sticher::blend()
{
	MultiBandBlender blender;
	int len = homographies.size();
	int rows = imageMats[0].rows,cols = imageMats[0].cols;

	Mat homography(homographies[0].rows,homographies[0].cols,homographies[0].type(),Scalar::all(1));
	Mat panorama = imageMats[0];
	Mat leftImage = imageMats[0];

	for(int i=0;i<len;++i)
	{
		std::cout<<i<<std::endl;

		Mat &rightImage = imageMats[i+1];
		Mat rightImageWarpped;
		
		Mat temp =rightImage;
		for(int j=i;j>=0;--j)
		{
			Mat newHomography(homographies[0].rows,homographies[0].cols,CV_64F);
			newHomography.at<double>(0,0) = 1;
			newHomography.at<double>(0,1) = 0;
			newHomography.at<double>(0,2) = homographies[j].at<double>(0,2);
			newHomography.at<double>(1,0) = 0;
			newHomography.at<double>(1,1) = 1;
			newHomography.at<double>(1,2) = homographies[j].at<double>(1,2);
			newHomography.at<double>(2,0) = 0;
			newHomography.at<double>(2,1) = 0;
			newHomography.at<double>(2,2) = homographies[j].at<double>(2,2);

			warpPerspective(temp,rightImageWarpped,newHomography, Size(temp.cols+cols, rows), INTER_CUBIC);
			temp = rightImageWarpped.clone();
		}

		Mat rightImageWarpped1(rightImageWarpped,Rect(0,0,rightImageWarpped.cols-cols,rows));

		Mat rightImageGrey;
		cvtColor(rightImageWarpped1,rightImageGrey,CV_BGR2GRAY);

		Mat thrRightImage;
		threshold(rightImageGrey,thrRightImage,0,255,CV_THRESH_BINARY);

		Mat leftImageGrey;
		cvtColor(leftImage,leftImageGrey,CV_BGR2GRAY);

		Mat thrLeftImage;
		threshold(leftImageGrey,thrLeftImage,0,255,CV_THRESH_BINARY);

		Mat thrMask;
		bitwise_and(thrLeftImage,thrRightImage,thrMask);

		int minX=rightImageWarpped1.cols,maxX=0,minY=rightImageWarpped1.rows,maxY=0;
		for(int i=0;i<rightImageWarpped1.rows;++i)
		{
			for(int j=0;j<rightImageWarpped1.cols;++j)
			{
				if(((int)thrMask.at<uchar>(i,j))>0)
				{
					minX = min(minX,j);
					maxX = max(maxX,j);
					minY = min(minY,i);
					maxY = max(maxY,i);
				}
			}
		}

		int blendCols = maxX-minX,blendRows = maxY-minY;
		Mat thrMaskRoi(thrMask,Rect(minX,minY,blendCols,blendRows));

		Mat leftImageBlendRoi(leftImage,Rect(minX,minY,blendCols,blendRows)),leftImageBlend;
		leftImageBlendRoi.copyTo(leftImageBlend,thrMaskRoi);
		
		Mat rightImageBlendRoi(rightImageWarpped,Rect(minX,minY,blendCols,blendRows)),rightImageBlend;
		rightImageBlendRoi.copyTo(rightImageBlend,thrMaskRoi);

		Mat mask(blendRows,blendCols,CV_32FC3,Scalar(0,0,0));
		mask(Range::all(),Range(0,mask.cols/2)) = Scalar(1.0,1.0,1.0);

		Mat& blendImage = blender.blend(leftImageBlend,rightImageBlend,mask);

		leftImage = rightImageWarpped.clone();

		Mat leftRoi(rightImageWarpped,Rect(0,0,rightImageWarpped.cols-cols,rows));
		addWeighted(panorama,1.0,leftRoi,1.0,0,leftRoi);

		Mat blendRoi(rightImageWarpped,Rect(minX,minY,blendCols,blendRows));
		blendImage.copyTo(blendRoi,thrMaskRoi);

		panorama = rightImageWarpped;
		imwrite("panorama.jpg", panorama);
	}

	this->panorama = panorama;
	imshow("panorama",panorama);
	waitKey(0);
}

void getLargestRectangleArea(int*arr,int len,int& maxVal,int& maxStart,int& maxEnd)
{
	stack<int> s;
	maxVal=-1;
	int val,start,end;

	for(int j=0;j<len;++j)
	{
		if(s.empty()||arr[j]>arr[s.top()]) s.push(j);
		else if((!s.empty())&&arr[j]<arr[s.top()])
		{
			do
			{
				int top = s.top();
				s.pop();

				if(s.empty()) start = 0;
				else start = top;

				end = j-1;
				val = arr[top]*(end-start+1);

				if(val>maxVal) 
				{
					maxVal = val;
					maxStart = start;
					maxEnd = end;
				}
			}
			while ((!s.empty())&&arr[j]<arr[s.top()]);
			s.push(j);
		}
	}
}

void Sticher::crop()
{
	Mat greyImage;
	cvtColor(panorama,greyImage,CV_BGR2GRAY);

	Mat thrImage;
	threshold(greyImage,thrImage,0,255,CV_THRESH_BINARY);

	int* arr = new int[thrImage.cols+1];
	memset(arr,0,(thrImage.cols+1)*sizeof(int));

	int maxVal=0,startRow=0,endRow=-1,startCol=0,endCol=-1;
	for(int i=0;i<thrImage.rows;++i)
	{
		for(int j=0;j<thrImage.cols;++j)
		{
			int val = (int)thrImage.at<uchar>(i,j);
			arr[j] = val>0?(arr[j]+1):0;
		}

		int val,start,end;
		getLargestRectangleArea(arr,thrImage.cols+1,val,start,end);

		if(val>maxVal) 
		{
			maxVal = val;
			startCol = start;
			endCol = end;
			startRow = i-maxVal/(endCol-startCol+1)+1;
			endRow = i;
		}
	}

	Mat cropImage(panorama,Rect(startCol,startRow,endCol-startCol+1,endRow-startRow+1));
	panorama = cropImage;

	imwrite("panorama_crop.jpg",panorama);
	imshow("panorama_crop",panorama);
	waitKey(0);

	delete [] arr;
}

int Sticher::stich(vector<string>& imageFiles,string outputDir)
{
	if(!imageFiles.size()) return 0;

	readImageFiles(imageFiles);
	sortImages();
	warp();
	extractFeatures();
	matchFeatures();
	blend();
	crop();

	return 0;
}