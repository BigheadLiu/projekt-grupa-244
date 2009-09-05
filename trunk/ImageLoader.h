#pragma once
#include <string>
#include <vector>
#include "Image.h"
using namespace std;

class ImageLoader {
public:
	virtual vector < Image * > loadNextImages() = 0;
};

class DirectoryLoader: public ImageLoader
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
	DirectoryLoader::DirectoryLoader(string dir, int colorspace, bool limitLoading=false, int maxNumber=10000, bool recursivly=false);
	~DirectoryLoader(void);

	vector< Image* > loadNextImages();
};

class MultipleDirectoryLoader : public ImageLoader {
private:
	bool potrosenPrvi; // je li potrosen prvi izvor slika
	ImageLoader &prvi;
	ImageLoader &drugi;
	
public:
	MultipleDirectoryLoader( ImageLoader &a, ImageLoader &b );
	vector < Image* > loadNextImages();
};

class NegativeTestLoader : public ImageLoader {
	ImageLoader &loader;
	int colorspace;
	bool limitLoading;
	int maxNumber;
	vector< Image* > created;

	void transferFromCreated( vector<Image*> &rj );
	vector< Image* > createSmallImages( Image &image);

	bool check(int x1, int x2, int y1, int y2);
public:
	NegativeTestLoader( string dir, int colorspace, bool limitLoading=false, int maxNumber=10000, bool recursivly=false);
	vector< Image *> loadNextImages();
};