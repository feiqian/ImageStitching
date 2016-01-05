#include "camera.h"

void Camera::estimateParameters(vector<ImageFeatures>& features,vector<MatchesInfo>& matches)
{
	HomographyBasedEstimator estimator;
	
	estimator(features, matches, cameras);

	focal = cameras[0].focal;
	ppx = cameras[0].ppx;
	ppy = cameras[0].ppy;

	//for (size_t i = 0; i < cameras.size(); ++i)
	//{
	//	Mat R;
	//	cameras[i].R.convertTo(R, CV_32F);
	//	cameras[i].R = R;
	//}

	//BundleAdjusterReproj adjuster;
	//adjuster.setConfThresh(1.f);

	//Mat_<uchar> refine_mask = Mat::zeros(3, 3, CV_8U);
	//refine_mask(0,0) = 1;
	//refine_mask(0,1) = 1;
	//refine_mask(0,2) = 1;
	//refine_mask(1,1) = 1;
	//refine_mask(1,2) = 1;

	//adjuster.setRefinementMask(refine_mask);
	//adjuster(features, matches, cameras);

	//// Find median focal length
	//vector<double> focals;
	//for (size_t i = 0; i < cameras.size(); ++i)
	//{
	//	focals.push_back(cameras[i].focal);
	//}

	//sort(focals.begin(), focals.end());

	//if (focals.size() % 2 == 1) focal = static_cast<float>(focals[focals.size() / 2]);
	//else focal = static_cast<float>(focals[focals.size() / 2 - 1] + focals[focals.size() / 2]) * 0.5f;

}