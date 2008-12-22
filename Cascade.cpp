#include "StdAfx.h"
#include "Cascade.h"

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
	
	vector<Feature> &bf=Feature::allBaseFeatures;
	
	fprintf(out,"%d\n",bf.size());
	
	for(int n,j,i=0;i<bf.size();i++) {
		fprintf(out,"%d %d\n",bf[i].width,bf[i].height);

		fprintf(out,"%d\n",n=bf[i].add.size());
		for(j=0;j<n;j++)
			fprintf(out,"%d %d\n",bf[i].add[j],bf[i].add[j]);

		fprintf(out,"%d\n",n=bf[i].subtract.size());
		for(j=0;j<n;j++)
			fprintf(out,"%d %d\n",bf[i].subtract[j],bf[i].subtract[j]);
	}
	

	// snimanje same kaskade:
	
	if(cascade.size()==0) { fclose(out); return; }
	
	fprintf(out,"%d\n",cascade.size());
	for(int j,i=0;i<cascade.size();i++) {
		fprintf(out,"%d\n",cascade[i].size());
		
		for(j=0;j<cascade[i].size();j++){
			fprintf(out,"%d %d %d %d ",cascade[i][j].width,cascade[i][j].height,cascade[i][j].x,cascade[i][j].y);
			fprintf(out,"%f %f %d\n",cascade[i][j].scale,cascade[i][j].weight, cascade[i][j].channel);

			fprintf(out,"%d\n",cascade[i][j].add.size());
			for(int tn=cascade[i][j].add.size(),k=0;k<tn;k++)
				fprintf(out,"%d %d\n",cascade[i][j].add[k],cascade[i][j].add[k]);

			fprintf(out,"%d\n",cascade[i][j].subtract.size());
			for(int tn=cascade[i][j].subtract.size(),k=0;k<tn;k++)
				fprintf(out,"%d %d\n",cascade[i][j].subtract[k],cascade[i][j].subtract[k]);
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
	
	vector<Feature> &bf=Feature::allBaseFeatures;

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

		bf.push_back(Feature(w,h,zb,od,-1,-1,-1,-1, 0)); //na kanalu 0 je default
	}


	// ucitava samu kaskadu:
	
	cascade.clear();
	
	int m;
	fscanf(in,"%d",&n);

	cascade.resize(n);

	for(int tn,j,i=0;i<n;i++) {
		fscanf(in,"%d",&m);
		
		cascade[i].resize(m);
		for(j=0;j<m;j++){
			fscanf(in,"%d %d %d %d",&(cascade[i][j].width),&(cascade[i][j].height),&(cascade[i][j].x),&(cascade[i][j].y));
			fscanf(in,"%f %f %d",&(cascade[i][j].scale),&(cascade[i][j].weight), &(cascade[i][j].channel));

			fscanf(in,"%d",&tn);
			cascade[i][j].add.resize(tn);
			for(int k=0;k<tn;k++)
				fscanf(in,"%d %d",&(cascade[i][j].add[k]),&(cascade[i][j].add[k]));

			fscanf(in,"%d",&tn);
			cascade[i][j].subtract.resize(tn);
			for(int k=0;k<tn;k++)
				fscanf(in,"%d %d",&(cascade[i][j].subtract[k]),&(cascade[i][j].subtract[k]));
		}
	}

	levelThreshold.clear();

	fscanf(in,"%d",&m);
	levelThreshold.resize(m);
	for(int i=0;i<m;i++)
		fscanf(in,"%d", &levelThreshold[i]);

	fclose(in);
}
