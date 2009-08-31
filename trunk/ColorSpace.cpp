#include "StdAfx.h"
#include "ColorSpace.h"
#include <vector>
#include <string>
#include <assert.h>
using namespace std;

int ColorSpace::HSV = 0, ColorSpace::LAB = 1, ColorSpace::RGB = 2, ColorSpace::GRAY = 3;
string namesTmp[] = {"HSV", "LAB", "RGB", "GRAY"};
vector<string> ColorSpace::names( namesTmp, namesTmp + 4 );
int ColorSpace::convert[] = {CV_HSV2BGR, CV_Lab2BGR, -1, CV_GRAY2BGR};
int ColorSpace::convertInverse[] = {CV_BGR2HSV, CV_BGR2Lab, -1, CV_BGR2GRAY};
int ColorSpace::numChannels[] = {3, 3, 3, 1 };

string ColorSpace::getName(int value) {
	return names[value];
}

int ColorSpace::getByName(std::string name) {
	for(int i=0; i<names.size(); i++)
		if (name == names[i]) return i;
}

int ColorSpace::convertValue(int colorspace) {
	assert( colorspace < names.size());
	return convert[ colorspace ];
}

int ColorSpace::convertValueInverse(int colorspace) {
	assert( colorspace < names.size());
	return convertInverse[ colorspace ];
}

CvScalar ColorSpace::getMarkerColor(int colorspace) {
	static CvScalar boje[] = {cvScalar(100,100,100), cvScalar(150, 50, 50), cvScalar(0, 0, 255), cvScalar( 200, 0, 0) };
	return boje[ colorspace ];
}

int ColorSpace::getNChannels(int colorspace) {
	return numChannels[ colorspace ];
}