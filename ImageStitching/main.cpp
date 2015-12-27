#include <iostream>
#include <string>

#include "stitch/Sticher.h"

using namespace std;

int main(int argc,char* argv[])
{
	const char* imageDir = "..//data";
	const char* outputDir = "..//result";
	if(argc>1) imageDir = argv[1];
	if(argc>2) outputDir = argv[2];

	vector<string> imageFiles = Utils::getFiles(imageDir,"JPG");
	Sticher sticher;
	cout<<sticher.stich(imageFiles,outputDir)<<endl;
	
	return 0;
}