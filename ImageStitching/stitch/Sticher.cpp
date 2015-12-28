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

void Sticher::sort()
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
		wraper.backwardWarp(imageMats[i],features[i],27);
		//draw Image,for test
		//imshow("CylindricalWarper", imageMat);
		//waitKey(0);
	}
}

void Sticher::matchFeatures()
{
	Mat destImage;
	for(int i=0,len=features.size();i<len-1;++i)
	{
		Mat &leftImage = imageMats[i],&rightImage = imageMats[i+1];
		Feature &leftFeature = features[i],&rightFeature = features[i+1];

		vector<DMatch>& matches = leftFeature.match(rightFeature);
		vector<Point2f> leftMatchPoints(matches.size());
		vector<Point2f> rightMatchPoints(matches.size());

		for(int i=0;i<matches.size();i++)
		{
			leftMatchPoints[i] = leftFeature.keyPoints[matches[i].queryIdx].pt;
			rightMatchPoints[i] = rightFeature.keyPoints[matches[i].trainIdx].pt;
		}

		//Draw matches,for test
		//Mat img_matches;
		//drawMatches(leftImage, leftFeature.keyPoints,rightImage , rightFeature.keyPoints,
		//	matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		//	vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
		//imshow("Feature Matches", img_matches);
		//waitKey(0);


		Mat homography = findHomography(leftMatchPoints,rightMatchPoints,CV_RANSAC);

		//TODO MultiBand
	}
}

int Sticher::stich(vector<string>& imageFiles,string outputDir)
{
	readImageFiles(imageFiles);
	extractFeatures();
	warp();
	matchFeatures();
	

	return 0;
}