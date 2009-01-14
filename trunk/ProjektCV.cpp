// ProjektCV.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "GuiCommunication.h"
#include "AdaBoost.h"
#include "Cascade.h"
#include "ViolaJones.h"
#include "Image.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;

/**
* Sadrzi main metodu za pokretanje algoritma za ucenje, te testiranje kaskade.
*
*/

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

void testAdaBoost() {
	vector< Image* > slikeTrue = Image::loadAllImagesFromDirectory( "c:\\Images\\true" );	
	vector< Image* > slikeFalse= Image::loadAllImagesFromDirectory( "c:\\Images\\false" );	
	Feature::loadBaseFeatures("bftest.txt");
	
	vector < Feature > tmp = Feature::generateAll( 20, 20, 3, 1.25, 3 );
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

void testirajKaskadu( Cascade &kaskada ) {
	vector < Image* > testSlike = Image::loadAllImagesFromDirectory( "c:\\Images\\test" );
	vector < Cascade > kaskade;
	kaskade.push_back( kaskada );

	for(int i=0; i<testSlike.size(); i++) 
		testSlike[i]->evaluateCascade( kaskade, 1., 1.25, 4., 1, 0 );
	Image::writeTestData();
}

void testViolaJones() {
	vector< Image* > slikeTrue = Image::loadAllImagesFromDirectory( "c:\\Images\\true2" );	
	vector< Image* > slikeFalse= Image::loadAllImagesFromDirectory( "c:\\Images\\false2", true, 10000 );	
	Feature::loadBaseFeatures("basefeatures.txt");

	Feature::generateAll(24, 24, 2, 1.3f, 3);

	Cascade kaskada;
	ViolaJones kuso( slikeTrue, slikeFalse, "c:\\Images\\false", 1000, 10000 );
	kuso.buildCascade( 0.1, 0.999, 0.000001, kaskada );

	kaskada.saveCascade("KaskadaTest.cascade" );
	testirajKaskadu( kaskada );
}

void testViolaJonesLoadFromFile(string file) {
	Cascade kaskada;
	kaskada.loadCascade(file);
	testirajKaskadu( kaskada );
}

void testCommunicationWithGui() {
	Image *slika;
	GuiCommunication::start();
	while( (slika = GuiCommunication::getNextImage()) != NULL ) {
		vector <Image::Rectangle > rj = slika->evaluateCascade( GuiCommunication::getCascade(), GuiCommunication::getPocetniScale(), 
																GuiCommunication::getStepScale(), GuiCommunication::getZavrsniScale(),
																GuiCommunication::getIsSlijedno(), GuiCommunication::getTreshold() );		
		GuiCommunication::saveResults( rj );
		delete slika;
	}
	GuiCommunication::sendResults();
}

int _tmain(int argc, _TCHAR* argv[])
{
	//testViolaJones();
	//testViolaJonesLoadFromFile("KaskadaTest.cascade");
	testCommunicationWithGui();

	return 0;
}

