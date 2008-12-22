// ProjektCV.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "AdaBoost.h"
#include "Cascade.h"
#include "ViolaJones.h"
#include "Image.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;

/*
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

*/
void testImageDio() {
	BaseFeature::loadBaseFeatures("bftest.txt");
	
	BaseFeature f = BaseFeature::allBaseFeatures[0];

	Image slika("c:\\Images\\true\\A01_A01_0000.bmp");		
	string tmp = slika.imageData();
	cout << tmp << endl;
	slika.showImage();

	//slika.showImageOverlappedWithFeature( f, 0, 0, 1. );
	//cout << "Suma je: " << slika.evaluateBaseFeature( f, 0, 0, 1 );

	system("pause");
}

void testLoadVise() {	
	vector< Image* > slike = Image::loadAllImagesFromDirectory( "c:\\Images\\true" );	
	slike[0]->showImage();
	cvWaitKey(0);
	system("pause");
}

void writeFeatures(vector <Feature> tmp ) {
	cout << "All Features: " << tmp.size() << endl;
	for(int i=0; i<tmp.size(); i++) {
		cout << "zbroji: ";

		for(int j=0; j<tmp[i].add.size(); j++) {
			int a = (int)((tmp[i].x + tmp[i].add[j].first) * tmp[i].scale);
			int b = (int)((tmp[i].y + tmp[i].add[j].second) * tmp[i].scale);		

			cout << "(" << a << "," << b << ")";
		}

		cout << endl << "oduzmi: ";
		for(int j=0; j<tmp[i].subtract.size(); j++) {
			int a = (int)((tmp[i].x + tmp[i].subtract[j].first) * tmp[i].scale);
			int b = (int)((tmp[i].y + tmp[i].subtract[j].second) * tmp[i].scale);		

			cout << "(" << tmp[i].subtract[j].first << "," << tmp[i].subtract[j].second << ")";
		}
		cout << endl;

	}


}

void testirajKaskadu( Cascade &kaskada ) {
	vector < Image* > testSlike = Image::loadAllImagesFromDirectory( "c:\\Images\\test" );
	for(int i=0; i<10; i++) {
		testSlike[i]->evaluateCascade( kaskada );
	}
}

void testAdaBoost() {
	vector< Image* > slikeTrue = Image::loadAllImagesFromDirectory( "c:\\Images\\true" );	
	vector< Image* > slikeFalse= Image::loadAllImagesFromDirectory( "c:\\Images\\false" );	
	Feature::loadBaseFeatures("bftest.txt");
	
	vector < Feature > tmp = Feature::generateAll( 20, 20, 3, 1.25 );
	writeFeatures( tmp );

	AdaBoost boost;
	vector< Feature > level = boost.startTraining( slikeTrue, slikeFalse, tmp, 20);	

	cout << "IZABRANI FEATURE-i" << endl; writeFeatures( level );

	vector< Image * > testSlike = Image::loadAllImagesFromDirectory( "c:\\Images\\test" );

	//testSlike[0]->showImageOverlappedWithFeature( level[1], true );
	testSlike[0]->evaluirajLevel( level );

	//for(int i=0; i<testSlike.size(); i++) {
	//	testSlike[i]->evaluirajLevel( level );
	//}

}

void testViolaJones() {
	vector< Image* > slikeTrue = Image::loadAllImagesFromDirectory( "c:\\Images\\true" );	
	vector< Image* > slikeFalse= Image::loadAllImagesFromDirectory( "c:\\Images\\false" );	
	Feature::loadBaseFeatures("bftest.txt");
	Feature::generateAll( 20, 20, 3, 1.25 );

	Cascade kaskada;
	ViolaJones kuso( slikeTrue, slikeFalse, slikeTrue, slikeFalse );
	kuso.buildCascade( 0.3, 0.99, 0.000001, kaskada );

	testirajKaskadu( kaskada );
}

int _tmain(int argc, _TCHAR* argv[])
{
	//testFeatureDio();
	//testImageDio();
	//testLoadVise();
	//testAdaBoost();
	testViolaJones();
	system("pause");

	return 0;
}

