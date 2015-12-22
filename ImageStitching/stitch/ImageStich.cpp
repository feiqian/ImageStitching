#include "ImageStich.h"

vector<Mat> ImageStich::readFiles(string srcImageDir)
{
	vector<string> imageFiles = Utils::getFiles(srcImageDir);
	vector<Mat> imageMats;
	for(int i=0,len=imageFiles.size();i<len;++i)
	{
		imageMats.push_back(imread(imageFiles[i]));
	}
	return imageMats;
}

void ImageStich::sort(vector<Mat>& imageMats,vector<Mat>& features)
{

}

vector<Feature> ImageStich::getFeatures(vector<Mat>& imageMats)
{
	vector<Feature> features(imageMats.size());

	for(int i=0,len=imageMats.size();i<len;++i)
	{
		features[i] = Feature(imageMats[i]);
	}

	return features;
}

int ImageStich::stich(string srcImageDir,string outputDir)
{
	if(outputDir.empty()) outputDir = srcImageDir;
		
	vector<Mat>& imageMats = readFiles(srcImageDir);
	vector<Feature>& features = getFeatures(imageMats);

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