#pragma once
#include <string>
#include <cv.h>
#include <highgui.h>
#include <vector>
using namespace std;


/**
* Razred koji enkapsulira prostor boja koristen u slikama.
* Razred omogucava pristup prostorima boja preko imena te preko vrijednosti.
* Sve metode razreda su staticke te se razred ne moze instancirati.
*/
class ColorSpace
{
private:
	//bitno je da su vrijednost-i ovih int-ova uskladene s nizom stringova

	static vector<string> names;
	static int convert[3];
	static int convertInverse[3];

	ColorSpace() {} //klasa se zbog ovoga ne moze instancirati
public:
	static int HSV, LAB, RGB;
	static int getByName(string name);
	static string getName(int value);
	static int convertValue(int colorspace); // prilikom prikaza slike treba colorspace pretvoriti u BGR, tome sluzi ova f-ja
	static int convertValueInverse(int colorspace); // prilikom prikaza slike treba colorspace pretvoriti u BGR, tome sluzi ova f-ja
};
