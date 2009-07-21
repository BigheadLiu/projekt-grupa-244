#include "StdAfx.h"
#include "ColorSpace.h"
#include <vector>
#include <string>
#include <assert.h>
using namespace std;

int ColorSpace::HSV = 0, ColorSpace::LAB = 1, ColorSpace::RGB = 2;
string namesTmp[] = {"HSV", "LAB", "RGB"};
vector<string> ColorSpace::names( namesTmp, namesTmp + 3 );
int ColorSpace::convert[] = {CV_HSV2BGR, CV_Lab2BGR, -1 };

string ColorSpace::getName(int value) {
	return names[value];
}

int ColorSpace::getByName(std::string name) {
	for(int i=0; i<names.size(); i++)
		if (name == names[i]) return i;
}

int ColorSpace::convertValue(int colorspace) {
	assert( colorspace < 3);
	return convert[ colorspace ];
}
