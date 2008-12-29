#include <string>
#include "Cascade.h"
#include <vector> 
#include <cv.h>
#include <highgui.h>
using namespace std;

#pragma once

/**
* Klasa koja predstavlja jednu sliku (ili za testiranje ili za ucenje )
* Ima metode za evaluaciju featura i cijele kaskade nad slikom te
* mogucnost iscrtavanja kvadrata nad oznacenom regijom slike.
*/

class Image
{
private:
	/*
	* Uzima vector < pair<int,int> > tocke i na tim koordinatama crta tocke
	* scale - sluzi za skalarianje feature-a prilikom iscrtavanja, jer se slika ukoliko je previse mala povecava
	* x - pomak svih koordinata u vectoru tocke za x
	* y - pomak svih koordinata u vectoru tocke za x
	*/
	void nacrtajTocke( IplImage *slika, vector< pair<int,int> > tocke, float scale, int x, int y );
	int *IntegralImage;

public:
	struct Rectangle {
		int x, y, height, width;
		Rectangle(int x, int y, int height, int width) {
			this->x = x;
			this->y = y;
			this->height = height;
			this->width = width;
		}
		Rectangle() {x = 0; y = 0; height = 0; width = 0;}
	};

	IplImage *image;

	/**
	* Konstruktor slike, ucitava sliku iz danog file-a
	*/
	Image(string);

	/*
	* destruktor slike, brise dinamicki zauzete resurse.
	*/
	~Image(void);

	/*
	* Evaluira feature na danoj slici
	* Feature &f ... referenca na feature koji se treba evaluirati
	* X - 
	* Y - 
	* ispisi - debug opcija za ispis (false ako nije definirana)
	*/

	/* 
	* Evaluira zadani feature nad slikom, te vraca izracunatu vrijednost
	*/
	int evaluateBaseFeature(const Feature &F, int X = 0, int Y = 0, bool ispisi=false, float scale=1.0);

	/*
	* prikazuje sliku na ekranu
	*/
	void showImage();

	/*
	* Crta sliku na ekran, te karakteristicne tocke za dani feature
	* Feature f... referenca na feature koje ce se iscrtati
	* wait - bool vrijednost koja oznacava hoce li nakon iscrtavanja slike cekati na pritisak tipke korisnika za nastavak rada
	*/
	void Image::showImageOverlappedWithFeature(const Feature &f, int X = 0, int Y = 0, bool wait=true);
	void Image::nacrtajOkvir(IplImage *slika, int X, int Y, int velicina, int b, int g, int r);

	/*
	* Ucitava sve slike iz danog direktorija. U tom direktoriju se treba nalazi file "files.txt" u kojem
	* se nalaze imena svih datoteka sa slikama za ucitavanje. Taj file najlakse je stvoriti
	* sa naredbom dir /w > files.txt, te zatim obrisati podatke u zaglavlju.
	*/
	static vector<Image*> Image::loadAllImagesFromDirectory(string directory);
	
	/*
	* Trazi sve znakove na zadanoj slici. Vraca vector< Rectangle > koji obiljezava sva moguca pojavljivanja znaka na slici	
	*/
	vector < Rectangle > findSigns();

	/*
	* Ispisuje osnovne podatke o slici: visina, sirina, broj kanala...
	*
	*/
	string imageData();
	int getHeight();
	int getWidth();	

	/*
	* Evaluira Feature na danom slikom, uracunava i njegov weight koji je prije izracunat pomocu AdaBoost-a
	* scale - default je 1., radi se scale-u unutar velike slike za testiranje. Uvijek prilikom ucenje taj scale-e treba biti 1. Sluzi
	* za skaliranje treshold-a
	*/
	float Image::evaluateTrainedFeature(const Feature &F, int X=0, int Y=0, bool ispisi=false, float scale=1.);

	/*
	* Evaluira jedan level kaskade. Ova metoda sluzi samo za testiranje AdaBoost algoritma i u konacnoj verziji se moze izbaciti
	*/
	void Image::evaluirajLevel( vector< Feature > features ); // ovo sluzi samo za testiranje, prava funkcija za evaluaciju level kaskade je navedena kasnije	
	
	/*
	* Evaluacija cijele kaskade, sluzi za testiranje cjelokupnog algoritma. Na slici oznacava 
	* regije za koje algoritam kaze da su znakovi	
	*/
	void Image::evaluateCascade(Cascade kaskada, float pocetniScale, float stepScale, float zavrsniScale);
	bool Image::evaluateCascadeLevel( int X, int Y, int velicinaProzora, float scale, Cascade &kaskada, int index);
};
