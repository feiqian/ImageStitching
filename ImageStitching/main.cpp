#include <iostream>
#include <string>

#include "stitch/ImageStich.h"

using namespace std;

int main(int argc,char* argv[])
{
	const char* imageDir = "..//data";
	if(argc>1) imageDir = argv[1];

	cout<<ImageStich::stich(imageDir)<<endl;
	

	return 0;
}