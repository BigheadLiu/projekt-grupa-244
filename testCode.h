#pragma once

#include "stdafx.h"
#include "AdaBoost.h"
#include "ImageLoader.h"
#include "Image.h"
#include "Feature.h"
#include <vector>
#include <iostream>
#include "BigVector.h"
#include "BigVectorVector.h"
#include "BigVectorVector.cpp"
#include "BigVector.cpp"
using namespace std;

namespace testCode {
	vector<int> createRandomVector(int);
	void test(void) {
		vector < int > v = createRandomVector(10);
		vector< int > prazni;

		cout << "Velicina vektora: " << sizeof(v) << ", a prazni: " << sizeof(prazni) << endl;
		prazni.resize( v.size() );

		FILE *f = fopen("test.dat", "w+b");
			fwrite( &v[0], sizeof(int), v.size(), f);
			fseek( f, 0, SEEK_SET );
			fread( &prazni[0], sizeof(int), v.size(), f);

		fclose(f);
		cout << "v: \t\t";
		for(int i=0; i<v.size(); i++)
			cout << v[i] << " ";
		cout << endl;

		cout << "prazni: \t\t";
		for(int i=0; i<prazni.size(); i++)
			cout << prazni[i] << " ";
		cout << endl;
		system("pause");
	}

	vector< int > createRandomVector(int n) {
		vector< int > rj;
		for(int i=0; i<n; i++)
			rj.push_back( rand() % 1000 );

		return rj;
	}

	void testBigVector2(){
		int broj = 100000;
		int duljina = 3;
		BigVectorVector< vector<int> > big(broj, duljina);
		vector< vector<int> > normal;

		for(int i=0; i<broj; i++) {
			vector< int > v = createRandomVector( duljina );
			big.push_back( v );
			normal.push_back( v);
			assert( big.back() == normal.back() );
			sort( big.back().begin(), big.back().end() );
			sort( normal.back().begin(), normal.back().end() );

			assert( big.back() == normal.back() );
		}

		for(int i=0; i<broj; i++)
			assert( big[i] == normal[i] );

		cout << "GOTOV TEST2" << endl;
	}

	void testBigVector() {
		int broj = 1000;
		BigVector< int > big(broj);
		vector< int > v;

		for(int i=0; i<broj; i++) {
			int value = rand() % 1000;
			v.push_back( value );
			big.push_back( value );
			assert( big.back() == v.back() );
		}

		for(int i=0; i<broj; i++) {
			int value = rand() % 1000;
			int gdje = rand() % v.size();
			v[gdje] = value;
			big[gdje] = value;
		}


		//sort(big.begin(), big.end() );

		for(int i=0; i<broj; i++)
			assert( v[i] == big[i] );		

		for(int i=0; i<broj; i++) {
			int value = rand();
			assert( v[ value % v.size() ] == big[ value % big.size() ] );
		}

		cout << "GOTOVO" << endl;
		//system("pause");

	}

void testLoader(int colorspace) {
	DirectoryLoader loader("c:\\images\\", colorspace, true, 1, true);
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

void testNegativeLoader() {
	NegativeTestLoader loader("c:\\Images\\test", ColorSpace::LAB, true, 2000, false);

	int ukupno = 0;
	while(true) {
		vector< Image* > slika = loader.loadNextImages();
		ukupno += slika.size();
		if (slika.size() == 0) break;
		
	for(int i=0; i<slika.size(); i++)
		delete slika[i];

		cout << "Ucitao sam: " << slika.size() << " slika." << endl;
	}

	cout << "Ukupno ih ima: " << ukupno << endl;
	system("pause");
}

void testMultipleLoader() {
	DirectoryLoader loader("c:\\Images\\true", ColorSpace::LAB, true, 10, true );
	DirectoryLoader loader2("c:\\Images\\true2", ColorSpace::LAB, true, 10, true );
	MultipleDirectoryLoader multiple( loader, loader2 );

	long long ukupno = 0;
	while(true) {
		vector< Image* > slika = multiple.loadNextImages();
		ukupno += slika.size();
		if (slika.size() == 0) break;
		cout << "Ucitao sam: " << slika.size() << " slika." << endl;
	}

	cout << "Ukupno ih ima: " << ukupno << endl;
	system("pause");
}

void testColorSpace() {
	assert( ColorSpace::HSV == ColorSpace::getByName("HSV") );
	assert( ColorSpace::LAB == ColorSpace::getByName("LAB") );
	assert( ColorSpace::RGB == ColorSpace::getByName("RGB") );
	assert( ColorSpace::GRAY == ColorSpace::getByName("GRAY") );

	assert( ColorSpace::getName( ColorSpace::HSV) == "HSV");
	assert( ColorSpace::getName( ColorSpace::LAB) == "LAB");
	assert( ColorSpace::getName( ColorSpace::RGB) == "RGB");
	assert( ColorSpace::getName( ColorSpace::GRAY) == "GRAY");

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
	DirectoryLoader loaderTrue("c:\\Images\\true", colorspace);
	DirectoryLoader loaderFalse("c:\\Images\\false", colorspace);
	DirectoryLoader loaderTest("c:\\Images\\test", colorspace);

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