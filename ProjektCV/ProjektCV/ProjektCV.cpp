// ProjektCV.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Cascade.h"

#include "Image.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;

void testFeatureDio()
{
	BaseFeature::loadBaseFeatures("bftest.txt");

	for(int i=0;i<BaseFeature::allBaseFeatures.size();i++)
		printf("%d: w:%d h:%d rb:%d (%d,%d), (%d,%d)\n",i,BaseFeature::allBaseFeatures[i].width,
			BaseFeature::allBaseFeatures[i].height,BaseFeature::allBaseFeatures[i].rb,
			BaseFeature::allBaseFeatures[i].add[0].first,BaseFeature::allBaseFeatures[i].add[0].second,
			BaseFeature::allBaseFeatures[i].subtract[1].first,BaseFeature::allBaseFeatures[i].subtract[1].second);


	vector<Feature> genFs=Feature::generateAll(100,100,125,90,3,1.25f);
	cout << "genFs.size(): " << genFs.size() << endl;
	printf("%d: rb:%d scale:%f x:%d y:%d\n",555,genFs[555].baseRb,genFs[555].scale,genFs[555].x,genFs[555].y);
	printf("%d: rb:%d scale:%f x:%d y:%d\n",5555,genFs[5555].baseRb,genFs[5555].scale,genFs[5555].x,genFs[5555].y);
	printf("%d: rb:%d scale:%f x:%d y:%d\n",1555,genFs[1555].baseRb,genFs[1555].scale,genFs[1555].x,genFs[1555].y);
	printf("%d: rb:%d scale:%f x:%d y:%d\n",3555,genFs[3555].baseRb,genFs[3555].scale,genFs[3555].x,genFs[3555].y);
	printf("%d: rb:%d scale:%f x:%d y:%d\n",4555,genFs[4555].baseRb,genFs[4555].scale,genFs[4555].x,genFs[4555].y);
	
	printf("%d: rb:%d scale:%f x:%d y:%d base.w:%d\n",2345,genFs[2345].baseRb,genFs[2345].scale,genFs[2345].x,
		genFs[2345].y,BaseFeature::allBaseFeatures[genFs[2345].baseRb].width);


	Cascade kaskada;
	vector<pair<float,Feature> > vv;
	vv.push_back(make_pair(1.234f,Feature(1,2,3,4.5f)));
	vv.push_back(make_pair(1.234f,Feature(5,6,7,8.9f)));
	kaskada.cascade.push_back(vv);
	
	kaskada.levelThreshold.push_back(2);
	kaskada.levelThreshold.push_back(59);

	kaskada.saveCascade("cascadeout.txt");
	
	Cascade k2;
	k2.loadCascade("cascadeout.txt");
	printf("\n%f %d\n",k2.cascade[0][1].first,k2.cascade[0][1].second.x);

	k2.saveCascade("k2out.txt");
	system("pause");
}

void testImageDio() {
	BaseFeature::loadBaseFeatures("bftest.txt");
	BaseFeature f = BaseFeature::allBaseFeatures[0];

	Image slika("c:\\Images\\true\\A01_A01_0000.bmp");		
	string tmp = slika.imageData();
	cout << tmp << endl;
	//slika.showImage();

	slika.showImageOverlappedWithFeature( f, 0, 0, 1. );
	cout << "Suma je: " << slika.evaluateBaseFeature( f, 0, 0, 1 );

	system("pause");
}

void testLoadVise() {	
	vector< Image > slike = Image::loadAllImagesFromDirectory("c:\\Images\\true\\" );
	//slike[0].showImage();
	cvWaitKey(0);
	system("pause");
}

int _tmain(int argc, _TCHAR* argv[])
{
	//testFeatureDio();
	//testImageDio();
	testLoadVise();


	return 0;
}

