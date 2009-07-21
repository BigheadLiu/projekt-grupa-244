#include "stdafx.h"
#include "ColorSpace.h"
#include "GuiCommunication.h"
#include "AdaBoost.h"
#include "Cascade.h"
#include "ViolaJones.h"
#include "Image.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "ImageLoader.h"

//#include "testCode.cpp" //kod koji sluzi za testiranje
//using namespace test

using namespace std;

/**
* Sadrzi main metodu za pokretanje algoritma za ucenje, te testiranje kaskade.
*/

#define COLORSPACE ColorSpace::HSV

void testirajKaskadu( Cascade &kaskada ) {
	ImageLoader loaderTrue("c:\\Images\\test", kaskada.colorSpace() );
	vector< Image * > testSlike = loaderTrue.loadNextImages();

	vector < Cascade > kaskade;
	kaskade.push_back( kaskada );

	for(int i=0; i<testSlike.size(); i++) 
		testSlike[i]->evaluateCascade( kaskade, 1., 1.25, 4., 1, 0 );
	Image::writeTestData();
}

void testViolaJones() {
	ImageLoader loaderTrue("c:\\Images\\true", COLORSPACE);
	ImageLoader loaderFalse("c:\\Dokumenti\\Nastava - FER\\Projekt\\ProjektCV - kalafatic\\Images\\false", COLORSPACE, true, 2000, true); //ucitava ih rekurzivno tako da ih je moguce stavit u vise direktorija

	Feature::loadBaseFeatures("basefeatures.txt");

	int tmpChans[] = {0, 1, 2};
	vector< int > channels(tmpChans, tmpChans + 3 );
	Feature::generateAll(24, 24, 2, 1.3f, channels);

	Cascade kaskada(COLORSPACE);

	ViolaJones kuso( loaderTrue, loaderFalse, 1000 );
	//kuso.buildCascade( 0.1, 0.999, 0.000001, kaskada ); //tesko za izgradit ovakvu kaskadu
	kuso.buildCascade( 0.2, 0.9, 0.05, kaskada );

	kaskada.saveCascade("KaskadaTest.cascade" );
	testirajKaskadu( kaskada );
}

void testViolaJonesLoadFromFile(string file) {
	Cascade kaskada( ColorSpace::RGB ); // default je RGB ali prilikom ucitavanja to se pravilno ucita
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
	//testColorSpace();
	//testLoader(COLORSPACE);

	testViolaJones();
	//testViolaJonesLoadFromFile("KaskadaTest.cascade");
	//testViolaJonesLoadFromFile("temp.cascade");
	//testCommunicationWithGui();

	return 0;
}

//TODO: vidjet zasto se unutar slike broje prozori koji su 
//		evaluirani true i false i to onda ispravit