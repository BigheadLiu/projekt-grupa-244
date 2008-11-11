#include "StdAfx.h"
#include "Cascade.h"

#include <utility>
#include <vector>
using namespace std;

Cascade::Cascade(void)
{
}

Cascade::~Cascade(void)
{
}

void Cascade::saveCascade(string file)
{
	FILE *out=fopen(file.c_str(),"w");
	
	// snimnje osnovnih featureove:
	
	vector<BaseFeature> &bf=BaseFeature::allBaseFeatures;
	
	fprintf(out,"%d\n",bf.size());
	
	for(int n,j,i=0;i<bf.size();i++) {
		fprintf(out,"%d %d\n",bf[i].width,bf[i].height);

		fprintf(out,"%d\n",n=bf[i].add.size());
		for(j=0;j<n;j++)
			fprintf(out,"%d %d\n",bf[i].add[j].first,bf[i].add[j].second);

		fprintf(out,"%d\n",n=bf[i].subtract.size());
		for(j=0;j<n;j++)
			fprintf(out,"%d %d\n",bf[i].subtract[j].first,bf[i].subtract[j].second);
	}
	

	// snimanje same kaskade:
	
	if(cascade.size()==0) { fclose(out); return; }
	
	fprintf(out,"%d\n",cascade.size());
	for(int j,i=0;i<cascade.size();i++) {
		fprintf(out,"%d\n",cascade[i].size());
		
		for(j=0;j<cascade[i].size();j++){
			fprintf(out,"%f\n",cascade[i][j].first);
			fprintf(out,"%d %d %d %f\n",cascade[i][j].second.baseRb,cascade[i][j].second.x,
				cascade[i][j].second.y,cascade[i][j].second.scale);
		}
	}
	fprintf(out,"%d\n",levelThreshold.size());
	for(int i=0;i<levelThreshold.size();i++)
		fprintf(out,"%d\n", levelThreshold[i]);

	fclose(out);
}


void Cascade::loadCascade(string file) {
	FILE *in=fopen(file.c_str(),"r");
	
	// ucitava osnovne featureove:
	
	vector<BaseFeature> &bf=BaseFeature::allBaseFeatures;

	bf.clear();
	
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

		bf.push_back(BaseFeature(w,h,zb,od));
	}


	// ucitava samu kaskadu:
	
	cascade.clear();
	
	int m;
	fscanf(in,"%d",&n);

	cascade.resize(n);

	for(int j,i=0;i<n;i++) {
		fscanf(in,"%d",&m);
		
		cascade[i].resize(m);
		for(j=0;j<m;j++){
			fscanf(in,"%f",&(cascade[i][j].first));
			fscanf(in,"%d %d %d %f",&(cascade[i][j].second.baseRb),&(cascade[i][j].second.x),
				&(cascade[i][j].second.y),&(cascade[i][j].second.scale));
		}
	}

	levelThreshold.clear();

	fscanf(in,"%d",&m);
	levelThreshold.resize(m);
	for(int i=0;i<m;i++)
		fscanf(in,"%d", &levelThreshold[i]);

	fclose(in);
}
