#include "Feature.h"


Feature::Feature(Mat imageMat)
{
	SiftFeatureDetector dector;
	SiftDescriptorExtractor extractor;

	dector.detect(imageMat,this->keyPoints);
	extractor.compute(imageMat,this->keyPoints,this->descriptor);
	this->imageMat = imageMat;
}

pair<vector<Point2f>,vector<Point2f> > Feature::match(Feature& other)
{
	FlannBasedMatcher matcher;
	vector<DMatch> matches;
	vector<DMatch> good_matches;
	pair<vector<Point2f>,vector<Point2f> > result;

	double max_dist = 0, min_dist = 100;

	matcher.match(this->descriptor,other.descriptor,matches);

	for(int i=0;i<matches.size();i++)
	{
		double dist = matches[i].distance;
		if( dist < min_dist ) min_dist = dist;
		if( dist > max_dist ) max_dist = dist;
	}

	for(int i=0;i<matches.size();i++)
	{
		if( matches[i].distance <= max(2*min_dist, 0.02))
		{ 
			good_matches.push_back(matches[i]);
			result.first.push_back(this->keyPoints[matches[i].queryIdx].pt);
			result.second.push_back(other.keyPoints[matches[i].trainIdx].pt);
		}
	}

	////Draw matches
	//Mat img_matches;
	//drawMatches(this->imageMat, this->keyPoints,other.imageMat , other.keyPoints,
	//	good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
	//	vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
	//imshow( "Good Matches", img_matches );
	//waitKey(0);

	return result;
}