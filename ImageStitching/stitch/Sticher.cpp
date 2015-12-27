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

int Sticher::stich(vector<string>& imageFiles,string outputDir)
{
	readImageFiles(imageFiles);
	extractFeatures();

	CylindricalWarper wraper;
	for(int i=0,len=imageMats.size();i<len;++i)
	{
		wraper.backwardWarp(imageMats[i],features[i]);
	}


	Mat destImage;
	for(int i=0,len=features.size();i<len-1;++i)
	{
		Mat&rightImage = imageMats[i+1];
		
		pair<vector<Point2f>,vector<Point2f> >& matches = features[i].match(features[i+1]);
		Mat homography = findHomography(matches.second,matches.first,CV_RANSAC);

		//TODO MultiBand
		

	}
	 
	return 0;
}