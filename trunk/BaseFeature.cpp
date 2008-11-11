#include "StdAfx.h"
#include "BaseFeature.h"

#include <utility>
#include <vector>
using namespace std;

BaseFeature::BaseFeature(void)
{
	rb=allBaseFeatures.size();  // automatsko postavljanje
}

BaseFeature::~BaseFeature(void)
{
}

BaseFeature::BaseFeature(int w,int h,vector<pair<int,int> > zb,vector<pair<int,int> > od)
{
	width=w;
	height=h;
	add=zb;
	subtract=od;


	rb=allBaseFeatures.size();  // automatsko postavljanje
}

void BaseFeature::loadBaseFeatures(string file)
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

		allBaseFeatures.push_back(BaseFeature(w,h,zb,od));
	}

	fclose(in);
}

// izgleda da je ovo nuzno da bi vektor allBaseFeatuers bio i definiran, a ne samo deklaniran
vector<BaseFeature> BaseFeature::allBaseFeatures;