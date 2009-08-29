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
#include "testCode.h" //kod koji sluzi za testiranje

using namespace std;

/**
* Sadrzi main metodu za pokretanje algoritma za ucenje, te testiranje kaskade.
*/

void testirajKaskadu( Cascade &kaskada ) {
	ImageLoader loaderTest("c:\\Images\\test", kaskada.colorSpace(), true, 1, false );
	vector< Image * > testSlike = loaderTest.loadNextImages();

	vector < Cascade > kaskade;
	kaskade.push_back( kaskada );

	while( testSlike.size() ) {
		testSlike[0]->evaluateCascade( kaskade, 1., 1.25, 4., 1, 0 );
		Image::writeTestData();
		Image::clearTestData();
		delete testSlike[0]; //oslobadanje memorije zauzete slikama za testiranje
		testSlike = loaderTest.loadNextImages();
	}
}

void testViolaJones(int colorspace) {
	ImageLoader loaderTrue("c:\\Images\\true", colorspace);
	ImageLoader loaderFalse("c:\\Dokumenti\\Nastava - FER\\Projekt\\ProjektCV - kalafatic\\Images\\false", colorspace, true, 2000, true); //ucitava ih rekurzivno tako da ih je moguce stavit u vise direktorija

	Feature::loadBaseFeatures("basefeatures.txt");

	int tmpChans[] = {0, 1, 2}; //kanali slike za koje se generiraju featuri (za LAB mozda bi bilo dobro izbacit L kanal)
	vector< int > channels(tmpChans, tmpChans + 3 );
	Feature::generateAll(24, 24, 2, 1.3f, channels);

	Cascade kaskada(colorspace);

	ViolaJones kuso( loaderTrue, loaderFalse, 1000 );
	//kuso.buildCascade( 0.1, 0.999, 0.000001, kaskada ); //tesko za izgradit ovakvu kaskadu
	kuso.buildCascade( 0.02, 0.99, 0.0001, kaskada ); //sad je lagana, valjda

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
	//testCode::testColorSpace();
	//testCode::testLoader(ColorSpace::RGB);

	//testViolaJones(ColorSpace::RGB);
	testViolaJonesLoadFromFile("KaskadaTest.cascade");
	//testViolaJonesLoadFromFile("temp.cascade");
	//testCommunicationWithGui();

	return 0;
}

// pokrenit ucenje u LAB sustavu boja ali ne tako naglo(tj ne odjenom dodavat 80 feature-a)
// mozda se zbog toga javlja problem prilikom ucenja.