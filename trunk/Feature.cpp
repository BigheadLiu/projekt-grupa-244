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

Feature::Feature(int w,int h,vector<pair<int,int> > zb,vector<pair<int,int> > od, int tX, int tY, float tScale, float tWeight)
{
	width=w;
	height=h;
	add=zb;
	subtract=od;

	x=tX;
	y=tY;
	scale=tScale;
	width=tWeight;
}

vector<Feature> Feature::generateAll(int x,int y, int width, int height, int step, float scaleFactor)
{
	vector<Feature> ret;
	
	vector<Feature> &bf=allBaseFeatures;
	
	float f;
	for(int i,j,k=0;k<bf.size();k++)
		for(f=1;f*bf[k].width<=width && f*bf[k].height<=height;f*=scaleFactor)
			for(i=0;(int)(i+f*bf[k].width)<width;i+=step)
				for(j=0;j+(int)(f*bf[k].height)<height;j+=step) {
					Feature tmp(bf[k].width,bf[k].height,bf[k].add,bf[k].subtract,i,j,f,-1);  // weight postavljen na -1
					ret.push_back(tmp);
				}
	
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

		allBaseFeatures.push_back(Feature(w,h,zb,od,-1,-1,-1,-1));
	}

	fclose(in);
}

// izgleda da je ovo nuzno da bi vektor allBaseFeatuers bio i definiran, a ne samo deklaniran
vector<Feature> Feature::allBaseFeatures;