#pragma once
#include <string>
#include <vector>
#include "Image.h"
using namespace std;

class ImageLoader
{
private:
	string dir;
	bool recursivly;
	int maxNumber;
	bool limitLoading;

	void loadFromDir(string dir, vector<Image*> &rj);
	int dosadUcitanoTrenutno;
	int dosadUcitanoUkupno;
	int colorspace; //oznacavan koristen prostor boja, klasa ColorSpace
public:
	ImageLoader::ImageLoader(string dir, int colorspace, bool limitLoading=false, int maxNumber=10000, bool recursivly=false);
	~ImageLoader(void);

	vector< Image* > loadNextImages();
};
