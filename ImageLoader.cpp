#include "StdAfx.h"
#include "ImageLoader.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <algorithm>
using namespace boost;
using namespace std;

DirectoryLoader::DirectoryLoader(string dir, int colorspace, bool limitLoading, int maxNumber, bool recursivly)
{
	this->dir = dir;
	this->colorspace = colorspace;
	this->limitLoading = limitLoading;
	this->maxNumber = maxNumber;
	this->recursivly = recursivly;
	this->dosadUcitanoTrenutno = 0;
	this->dosadUcitanoUkupno = 0;
}

DirectoryLoader::~DirectoryLoader(void)
{
}

void DirectoryLoader::loadFromDir(string dir, vector<Image*> &rj) {
	string tmp[6] = {".bmp", ".jpeg", ".jpg", ".gif", ".tiff", ".tif" };
	int br = 6;
	
	filesystem::path p( dir );

	assert( filesystem::exists( p ) );
	assert( filesystem::is_directory( p ) );

	filesystem::directory_iterator iter(p);

	while( iter != filesystem::directory_iterator() ) {
		const filesystem::path&p = iter->path();

		if ( is_directory(p) == true) 
			if (recursivly == true) {
				loadFromDir( p.directory_string(), rj);
				if (rj.size() == maxNumber) return;
			}
			
		if ( is_directory(p) == false) {
			
			string ext = p.extension();
			if (find( tmp, tmp + br, ext) - tmp <br) {
				dosadUcitanoTrenutno++;

				if (dosadUcitanoUkupno >= dosadUcitanoTrenutno) {iter++; continue;}
				//cout << iter->path().filename() << endl;
				rj.push_back( new Image( p.native_file_string(), colorspace) );

				if (limitLoading == true && rj.size() == maxNumber) return;
				
			}
			
		}
		++iter;
	}
}

vector< Image * > DirectoryLoader::loadNextImages() {
	vector < Image * > rj;
	dosadUcitanoTrenutno = 0;
	loadFromDir( dir, rj );
	dosadUcitanoUkupno += rj.size();

	return rj;
}

MultipleDirectoryLoader::MultipleDirectoryLoader(ImageLoader &a, ImageLoader &b): prvi(a), drugi(b) {
	potrosenPrvi = false;
}

vector < Image * > MultipleDirectoryLoader::loadNextImages() {
	vector< Image* > rj;

	if (potrosenPrvi == false) 
		rj = prvi.loadNextImages();

	if (rj.size() == 0)  {
		potrosenPrvi = true;
		rj = drugi.loadNextImages();
	}
	return rj;
}

NegativeTestLoader::NegativeTestLoader(std::string dir, int colorspace, bool limitLoading, int maxNumber, bool recursivly):loader( *new DirectoryLoader( dir, ColorSpace::RGB, true, 1, recursivly) ) {
	this->colorspace = colorspace;
	this->limitLoading = limitLoading;
	this->maxNumber = maxNumber;
}

vector< Image *> NegativeTestLoader::loadNextImages() {
	vector< Image * > rj;
	
	transferFromCreated( rj );
	if (rj.size() >= maxNumber) return rj;

	vector< Image * > velike = loader.loadNextImages();
	if (velike.size() == 0) return rj;
	
	for(int i=0; i<velike.size(); i++) {
		vector < Image * > tmp = createSmallImages( *velike[i] );
		created.insert( created.end(), tmp.begin(), tmp.end() );
		delete velike[i];
	}

	transferFromCreated( rj );
	if (rj.size() >= maxNumber) return rj;

	return rj;
}

void NegativeTestLoader::transferFromCreated(std::vector<Image*> &rj) {
	int koliko = min( created.size(), maxNumber - rj.size() );
	if (koliko <= 0) return;
	rj.insert( rj.end(), created.begin(), created.begin() + koliko );
	created.erase( created.begin(), created.begin() + koliko ) ;
}

vector<  Image * > NegativeTestLoader::createSmallImages(Image &image) {
	vector< Image * > rj;


	for( float scale=1.;scale<10; scale*=1.25)
		for(int x=1; x<image.getHeight(); x+= (int)(scale*10))
			for(int y=1; y<image.getWidth(); y+= (int)(scale*10)) {
				int velicina = (int)(scale * 24);
				if (x + velicina >= image.getHeight()) continue;
				if (y + velicina >= image.getWidth() ) continue;
				if (check(x, y, x + velicina, y + velicina) == false) continue;

				IplImage *slika = cvCreateImage( cvSize(velicina, velicina), image.depth(), image.channels() );				

				cvSetImageROI( image.image, cvRect(y, x, velicina, velicina) );
				cvResize( image.image, slika);
				cvResetImageROI( image.image );
				
				Image *tmp = new Image(slika, colorspace );
				//tmp->showImage();
				rj.push_back( tmp );
			}
	return rj;
}

bool NegativeTestLoader::check(int x1, int x2, int y1, int y2) {
	//TODO: provjera je li dio slike predstavlja upravo prometni znak
	return true;
}