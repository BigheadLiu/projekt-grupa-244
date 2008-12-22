#include "StdAfx.h"
#include "Feature.h"

#include <vector>
#include <iostream>

#define debug(x) cout << #x << ": " << x << endl;
using namespace std;

Feature::Feature(void)
{
}

Feature::~Feature(void)
{
}

Feature::Feature(int w,int h,vector<pair<int,int> > zb,vector<pair<int,int> > od, int tX, int tY, float tScale, float tWeight, int channel)
{
	this->channel = channel;
	width=w;
	height=h;
	add=zb;
	subtract=od;

	x=tX;
	y=tY;
	scale=tScale;
	width=tWeight;
}

vector<Feature> Feature::generateAll(int width, int height, int step, float scaleFactor, int numberOfChannels)
{
	vector<Feature> ret;
	
	vector<Feature> &bf=allBaseFeatures;
	
	float f;
	for(int chan=0; chan<numberOfChannels; chan++)
		for(int i,j,k=0;k<bf.size();k++)
			for(f=1;f*bf[k].width<width && f*bf[k].height<height;f*=scaleFactor)
				for(i=0;(int)(i+f*bf[k].width)<width;i+=step)
					for(j=0;j+(int)(f*bf[k].height)<height;j+=step) {
						Feature tmp(bf[k].width,bf[k].height,bf[k].add,bf[k].subtract,i,j,f,0, chan);  // weight postavljen na 0					
						ret.push_back(tmp);
					}

	generatedFeatures = ret;
	return ret;
}

void Feature::loadBaseFeatures(string file)
{
	FILE *in=fopen(file.c_str(),"r");
	
	allBaseFeatures.clear();

	int n;
	fscanf(in,"%d",&n);
	
	for(int a,b,m,w,h,j,i=0;i<n;i++) {
		fscanf(in,"%d %d",&w,&h);

		vector<pair<int,int> > zb;
		fscanf(in,"%d",&m);
		for(j=0;j<m;j++) {
			fscanf(in,"%d %d",&a,&b);
			zb.push_back(make_pair(a,b));
		}

		vector<pair<int,int> > od;
		fscanf(in,"%d",&m);
		for(j=0;j<m;j++) {
			fscanf(in,"%d %d",&a,&b);
			od.push_back(make_pair(a,b));
		}

		allBaseFeatures.push_back(Feature(w,h,zb,od,0,0,1,0,0));				
	}

	fclose(in);
}

// izgleda da je ovo nuzno da bi vektor allBaseFeatuers bio i definiran, a ne samo deklaniran
vector<Feature> Feature::allBaseFeatures;
vector<Feature> Feature::generatedFeatures;