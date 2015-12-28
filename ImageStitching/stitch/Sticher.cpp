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
		wraper.backwardWarp(imageMats[i],features[i],27);
		//draw Image,for test
		//imshow("CylindricalWarper", imageMat);
		//waitKey(0);
	}
}

void Sticher::matchFeatures()
{
	homographies.resize(features.size());

	for(int i=0,len=features.size();i<len;++i)
	{
		Mat &leftImage = imageMats[i],&rightImage = imageMats[(i+1)%len];
		Feature &leftFeature = features[i],&rightFeature = features[(i+1)%len];

		vector<DMatch>& matches = leftFeature.match(rightFeature);
		vector<Point2f> leftMatchPoints(matches.size());
		vector<Point2f> rightMatchPoints(matches.size());

		for(int k=0;k<matches.size();k++)
		{
			leftMatchPoints[k] = leftFeature.keyPoints[matches[k].queryIdx].pt;
			rightMatchPoints[k] = rightFeature.keyPoints[matches[k].trainIdx].pt;
		}

		homographies[i] = findHomography(rightMatchPoints,leftMatchPoints,CV_RANSAC);

		//Draw matches,for test
		//Mat img_matches;
		//drawMatches(leftImage, leftFeature.keyPoints,rightImage , rightFeature.keyPoints,
		//	matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		//	vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
		//imshow("Feature Matches", img_matches);
		//waitKey(0);
	}
}

void Sticher::blend()
{
	MultiBandBlender blender;
	for(int i=0,len=homographies.size();i<len-1;++i)
	{
		Mat& leftImage = imageMats[i],rightImage = imageMats[(i+1)%len];
		int rows = leftImage.rows,cols = leftImage.cols;
		Mat rightImageWarpped,maskMats(rows,cols,CV_8UC3,Scalar(1,1,1));

		warpPerspective(rightImage, rightImageWarpped, homographies[i], Size(cols*2, rows), INTER_LINEAR);

		Mat rightImageBlend(rightImageWarpped,Rect(0,0,cols,rows));
		Mat& blendImage = blender.blend(leftImage,rightImageBlend,maskMats);
		
		Mat panorama = rightImageWarpped.clone();
		Mat roi(panorama , Rect(0, 0, cols, rows));
		blendImage.copyTo(roi);
		imshow("Panorama", panorama);
		waitKey(0);
	}
}

void Sticher::crop()
{

}

int Sticher::stich(vector<string>& imageFiles,string outputDir)
{
	readImageFiles(imageFiles);
	sortImages();
	extractFeatures();
	warp();
	matchFeatures();
	blend();
	crop();

	return 0;
}