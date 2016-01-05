#include "Sticher.h"

vector<Mat> Sticher::readImageFiles(vector<string>& imageFiles)
{
	imageMats.resize(imageFiles.size());
	floatImageMats.resize(imageFiles.size());

	double baseHeight,baseWidth;
	for(int i=0,len=imageFiles.size();i<len;++i)
	{
		Mat tempImage= imread(imageFiles[i]);
		if(!i) 
		{
			baseHeight = tempImage.rows;
			baseWidth = tempImage.cols;
		}

		if(tempImage.rows!=baseHeight||tempImage.cols!=baseWidth) resize(tempImage,imageMats[i],Size(baseWidth,baseHeight));
		else imageMats[i] = tempImage;

		imageMats[i].convertTo(floatImageMats[i],CV_32FC3,1.0/255.0);
	}

	return imageMats;
}

void Sticher::outputResult(string outputDir)
{
	time_t t = time(NULL);
	struct tm *p;
	p = gmtime(&t);
	char timeStamp[32] = {0};

	sprintf_s(timeStamp, sizeof(timeStamp), "%d%d%d%d%d%d", 1900 + p->tm_year, 1+ p->tm_mon, p->tm_mday,p->tm_hour,
		p->tm_min, p->tm_sec);

	Mat ucharPanorama;
	panorama.convertTo(ucharPanorama,CV_8UC3,255);
	imwrite(outputDir+"//panorama_"+timeStamp+".JPG",ucharPanorama);

	Mat ucharCropPanorama;
	cropPanorama.convertTo(ucharCropPanorama,CV_8UC3,255);
	imwrite(outputDir+"//panorama_crop_"+timeStamp+".JPG",ucharCropPanorama);
}

void Sticher::sortImages()
{

}

void Sticher::extractFeatures()
{
	features.resize(imageMats.size());
	SurfFeaturesFinder finder;

	for(int i=0,len=imageMats.size();i<len;++i)
	{
		finder(imageMats[i], features[i]);
		features[i].img_idx = i;
	}
}

void Sticher::warp()
{
	Camera camera;
	camera.estimateParameters(features,pairwise_matches);

	//focal_pixel = (focal_mm / sensor_width_mm) * image_width_in_pixels
	//focal_pixel = (37 / 36 )*imageMats[0].cols;

	CylindricalWarper wraper;
	for(int i=0,len=imageMats.size();i<len;++i)
	{
		wraper.backwardWarp(floatImageMats[i],features[i],camera);
		floatImageMats[i].convertTo(imageMats[i],CV_8UC3,255);

		//draw Image,for test
		//imshow("CylindricalWarper", imageMat);
		//waitKey(0);
	}
}

void Sticher::matchFeatures()
{
	BestOf2NearestMatcher matcher;
	Mat matchMask(features.size(),features.size(),CV_8U,Scalar(0));

	for(int i = 0,len=features.size(); i < len-1; ++i)
	{
		matchMask.at<char>(i,i+1) =1;
	}
	matcher(features, pairwise_matches,matchMask);

	matches.resize(features.size()-1);
	for(int i=0,len=features.size();i<len-1;++i)
	{
		Mat &leftImage = imageMats[i],&rightImage = imageMats[i+1];
		ImageFeatures &leftFeature = features[i],&rightFeature = features[i+1];
	
		//matcher(leftFeature,rightFeature,matches[i]);
		matches[i] = pairwise_matches[i*len+i+1];

		//Draw matches,for test
		//Mat img_matches;
		//drawMatches(leftImage, leftFeature.keyPoints,rightImage , rightFeature.keyPoints,
		//	matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		//	vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
		//imshow("Feature Matches", img_matches);
		//waitKey(0);
	}
}

void Sticher::getHomography()
{
	for(int i=0,len=features.size()-1;i<len;++i)
	{
		ImageFeatures &leftFeature = features[i],&rightFeature = features[i+1];
		vector<DMatch>& match_points = matches[i].matches;

		vector<Point2f> leftMatchPoints(match_points.size());
		vector<Point2f> rightMatchPoints(match_points.size());

		for(int k=0;k<match_points.size();k++)
		{
			leftMatchPoints[k] = leftFeature.keypoints[match_points[k].queryIdx].pt;
			rightMatchPoints[k] = rightFeature.keypoints[match_points[k].trainIdx].pt;
		}
		homographies.push_back(findHomography(rightMatchPoints,leftMatchPoints,CV_RANSAC));
	}
}

void Sticher::blend()
{
	MultiBandBlender blender;
	int len = homographies.size();
	int rows = imageMats[0].rows,cols = imageMats[0].cols;

	Mat homography(homographies[0].rows,homographies[0].cols,homographies[0].type(),Scalar::all(1));
	Mat leftImage = floatImageMats[0];
	Mat panorama = floatImageMats[0];

	for(int i=0;i<len;++i)
	{
		std::cout<<i<<std::endl;

		Mat rightImage = floatImageMats[i+1];
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

			warpPerspective(temp,rightImageWarpped,newHomography, Size(temp.cols+cols, rows), INTER_LINEAR,BORDER_CONSTANT,Scalar(-1,-1,-1));
			temp = rightImageWarpped.clone();
		}

		Mat rightImageWarpped1(rightImageWarpped,Rect(0,0,rightImageWarpped.cols-cols,rows));

		Mat rightImageGrey;
		cvtColor(rightImageWarpped1,rightImageGrey,CV_BGR2GRAY);

		Mat thrRightImage;
		threshold(rightImageGrey,thrRightImage,-1,255,CV_THRESH_BINARY);
		thrRightImage.convertTo(thrRightImage,CV_8U);

		Mat leftImageGrey;
		cvtColor(leftImage,leftImageGrey,CV_BGR2GRAY);

		Mat thrLeftImage;
		threshold(leftImageGrey,thrLeftImage,-1,255,CV_THRESH_BINARY);
		thrLeftImage.convertTo(thrLeftImage,CV_8U);

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

		int blendCols = maxX-minX+1,blendRows = maxY-minY+1;
		Mat thrMaskRoi(thrMask,Rect(minX,minY,blendCols,blendRows));

		Mat leftImageBlendRoi(leftImage,Rect(minX,minY,blendCols,blendRows)),leftImageBlend;
		leftImageBlendRoi.copyTo(leftImageBlend,thrMaskRoi);
		
		Mat rightImageBlendRoi(rightImageWarpped,Rect(minX,minY,blendCols,blendRows)),rightImageBlend;
		rightImageBlendRoi.copyTo(rightImageBlend,thrMaskRoi);

		Mat mask(blendRows,blendCols,CV_32FC3,Scalar(0,0,0));
		mask(Range::all(),Range(0,mask.cols/2)) = Scalar(1.0,1.0,1.0);

		Mat& blendImage = blender.blend(leftImageBlend,rightImageBlend,mask);

		Mat leftRoi(rightImageWarpped,Rect(0,0,rightImageWarpped.cols-cols,rows));
		panorama.copyTo(leftRoi,thrLeftImage);

		Mat blendRoi(rightImageWarpped,Rect(minX,minY,blendCols,blendRows));
		blendImage.copyTo(blendRoi,thrMaskRoi);

		panorama = rightImageWarpped;
		leftImage = rightImageWarpped;
			
		Mat ucharPanorama;
		panorama.convertTo(ucharPanorama,CV_8UC3,255);
		imwrite("panorama.jpg",ucharPanorama);
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
	threshold(greyImage,thrImage,-1,255,CV_THRESH_BINARY);
	thrImage.convertTo(thrImage,CV_8U);

	int* arr = new int[panorama.cols+1];
	memset(arr,0,(panorama.cols+1)*sizeof(int));

	int maxVal=0,startRow=0,endRow=-1,startCol=0,endCol=-1;
	for(int i=0;i<panorama.rows;++i)
	{
		for(int j=0;j<panorama.cols;++j)
		{
			int val = (int)thrImage.at<uchar>(i,j);
			arr[j] = val>0?(arr[j]+1):0;
		}

		int val,start,end;
		getLargestRectangleArea(arr,panorama.cols+1,val,start,end);

		if(val>maxVal) 
		{
			maxVal = val;
			startCol = start;
			endCol = end;
			startRow = i-maxVal/(endCol-startCol+1)+1;
			endRow = i;
		}
	}

	cropPanorama = Mat(panorama,Rect(startCol,startRow,endCol-startCol+1,endRow-startRow+1));

	imshow("panorama_crop",cropPanorama);
	waitKey(0);

	delete [] arr;
}

void Sticher::stich(vector<string>& imageFiles,string outputDir)
{
	if(!imageFiles.size()) return;

	readImageFiles(imageFiles);
	extractFeatures();
	matchFeatures();
	sortImages();
	warp();
	getHomography();
	blend();
	crop();
	outputResult(outputDir);

}