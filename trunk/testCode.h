#pragma once

#include "stdafx.h"
#include "AdaBoost.h"
#include "ImageLoader.h"
#include "Image.h"
#include "Feature.h"
#include <vector>
#include <iostream>
using namespace std;

namespace testCode {
void testLoader(int colorspace) {
	ImageLoader loader("c:\\images\\", colorspace, true, 1, true);
	vector< Image* > rj = loader.loadNextImages();
	cout << "-----------" << endl;
	 rj = loader.loadNextImages();
	cout << "------------" << endl;
	rj = loader.loadNextImages();
	cout << "------------" << endl;
	rj = loader.loadNextImages();
	cout << "------------" << endl;
	rj = loader.loadNextImages();
	cout << "------------" << endl;
	rj = loader.loadNextImages();
	cout << "------------" << endl;
	rj = loader.loadNextImages();
	cout << "------------" << endl;
	rj = loader.loadNextImages();

	rj[0]->showImage();

	cout << rj.size() << endl;
	system("pause");
}

void testColorSpace() {
	assert( ColorSpace::HSV == ColorSpace::getByName("HSV") );
	assert( ColorSpace::LAB == ColorSpace::getByName("LAB") );
	assert( ColorSpace::RGB == ColorSpace::getByName("RGB") );

	cout << ColorSpace::getName( ColorSpace::HSV ) << endl;
	cout << ColorSpace::getName( ColorSpace::LAB ) << endl;
	cout << ColorSpace::getName( ColorSpace::RGB ) << endl;

	system("pause");
}


void writeFeatures(vector <Feature> tmp ) {
	cout << "All Features: " << tmp.size() << endl;
	for(int i=0; i<tmp.size(); i++) {
		cout << tmp[i].width << " " << tmp[i].height << endl;

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


void testAdaBoost(int colorspace) {
	ImageLoader loaderTrue("c:\\Images\\true", colorspace);
	ImageLoader loaderFalse("c:\\Images\\false", colorspace);
	ImageLoader loaderTest("c:\\Images\\test", colorspace);

	vector< Image* > slikeTrue = loaderTrue.loadNextImages();
	vector< Image* > slikeFalse= loaderFalse.loadNextImages();
	Feature::loadBaseFeatures("bftest.txt");
	
	int tmpChans[] = {0, 1, 2};
	vector< int > channels(tmpChans, tmpChans + 3 ); //kanali nad kojima ce se raditi

	vector < Feature > tmp = Feature::generateAll( 20, 20, 3, 1.25, channels );
	writeFeatures( tmp );

	AdaBoost boost;
	vector< Feature > level = boost.startTraining( slikeTrue, slikeFalse, tmp, 20);	

	cout << "IZABRANI FEATURE-i" << endl; writeFeatures( level );

	vector< Image * > testSlike = loaderTest.loadNextImages();

	//testSlike[0]->showImageOverlappedWithFeature( level[1], true );
	testSlike[0]->evaluirajLevel( level );

	//for(int i=0; i<testSlike.size(); i++) {
	//	testSlike[i]->evaluirajLevel( level );
	//}
}
}