#include "Feature.h"


Feature::Feature(Mat imageMat)
{
	SiftFeatureDetector dector;
	SiftDescriptorExtractor extractor;

	dector.detect(imageMat,this->keyPoints);
	extractor.compute(imageMat,this->keyPoints,this->descriptor);
}

vector<DMatch> Feature::match(Feature& other)
{
	FlannBasedMatcher matcher;
	vector<DMatch> matches;
	vector<DMatch> good_matches;

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
		}
	}

	return good_matches;
}