// ProjektCV.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Cascade.h"

//#include "Image.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;

void testFeatureDio()
{
	Feature::loadBaseFeatures("bftest.txt");

	for(int i=0;i<Feature::allBaseFeatures.size();i++)
		printf("%d: w:%d h:%d (%d,%d), (%d,%d)\n",i,Feature::allBaseFeatures[i].width,
			Feature::allBaseFeatures[i].height,
			Feature::allBaseFeatures[i].add[0].first,Feature::allBaseFeatures[i].add[0].second,
			Feature::allBaseFeatures[i].subtract[1].first,Feature::allBaseFeatures[i].subtract[1].second);


	vector<Feature> genFs=Feature::generateAll(100,100,125,90,3,1.25f);
	cout << "genFs.size(): " << genFs.size() << endl;
	printf("%d: scale:%f x:%d y:%d\n",555,genFs[555].scale,genFs[555].x,genFs[555].y);
	printf("%d: scale:%f x:%d y:%d\n",5555,genFs[5555].scale,genFs[5555].x,genFs[5555].y);
	printf("%d: scale:%f x:%d y:%d\n",1555,genFs[1555].scale,genFs[1555].x,genFs[1555].y);
	printf("%d: scale:%f x:%d y:%d\n",3555,genFs[3555].scale,genFs[3555].x,genFs[3555].y);
	printf("%d: scale:%f x:%d y:%d\n",4555,genFs[4555].scale,genFs[4555].x,genFs[4555].y);
	
	printf("%d: scale:%f x:%d y:%d w:%d\n",2345,genFs[2345].scale,genFs[2345].x,
		genFs[2345].y,genFs[2345].width);


	Cascade kaskada;
	vector<Feature> vv;
	vector<pair<int,int> > ais;
	ais.push_back(make_pair(1,2));
	ais.push_back(make_pair(3,4));
	ais.push_back(make_pair(5,6));
	vv.push_back(Feature(1,2,ais,ais,3,4,1.2f,3.4f));
	vv.push_back(Feature(1,2,ais,ais,3,4,1.2f,3.4f));
	kaskada.cascade.push_back(vv);
	
	kaskada.levelThreshold.push_back(2);
	kaskada.levelThreshold.push_back(59);

	kaskada.saveCascade("cascadeout.txt");
	
	Cascade k2;
	k2.loadCascade("cascadeout.txt");
	printf("\n%f %d\n",k2.cascade[0][1].weight,k2.cascade[0][1].x);

	k2.saveCascade("k2out.txt");
	system("pause");
}

/*
void testImageDio() {
	Feature::loadBaseFeatures("bftest.txt");
	Feature f = BaseFeature::allBaseFeatures[0];

	Image slika("c:\\Images\\true\\A01_A01_0000.bmp");		
	string tmp = slika.imageData();
	cout << tmp << endl;
	slika.showImage();

	//slika.showImageOverlappedWithFeature( f, 0, 0, 1. );
	//cout << "Suma je: " << slika.evaluateBaseFeature( f, 0, 0, 1 );

	system("pause");
}

void testLoadVise() {	
	vector< Image > slike = Image::loadAllImagesFromDirectory("c:\\Images\\true\\" );
	//slike[0].showImage();
	cvWaitKey(0);
	system("pause");
}
*/

int _tmain(int argc, _TCHAR* argv[])
{
	testFeatureDio();
	//testImageDio();
	//testLoadVise();


	return 0;
}

