#include "StdAfx.h"
#include "Feature.h"

#include <vector>
using namespace std;

Feature::Feature(void)
{
}

Feature::~Feature(void)
{
}

Feature::Feature(int tBase, int tX, int tY, float tScale)
{
	baseRb=tBase;
	x=tX;
	y=tY;
	scale=tScale;	
}

vector<Feature> Feature::generateAll(int x,int y, int width, int height, int step, float scaleFactor)
{
	vector<Feature> ret;
	
	vector<BaseFeature> &bf=BaseFeature::allBaseFeatures;
	
	float f;
	for(int i,j,k=0;k<bf.size();k++)
		for(f=1;f*bf[k].width<=width && f*bf[k].height<=height;f*=scaleFactor)
			for(i=0;(int)(i+f*bf[k].width)<width;i+=step)
				for(j=0;j+(int)(f*bf[k].height)<height;j+=step) {
					Feature tmp(k,i,j,f);
					ret.push_back(tmp);
				}
	
	return ret;
}
