#include "StdAfx.h"
#include "ImageLoader.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <algorithm>
using namespace boost;
using namespace std;

ImageLoader::ImageLoader(string dir, int colorspace, bool limitLoading, int maxNumber, bool recursivly)
{
	this->dir = dir;
	this->colorspace = colorspace;
	this->limitLoading = limitLoading;
	this->maxNumber = maxNumber;
	this->recursivly = recursivly;
	this->dosadUcitanoTrenutno = 0;
	this->dosadUcitanoUkupno = 0;
}

ImageLoader::~ImageLoader(void)
{
}

void ImageLoader::loadFromDir(string dir, vector<Image*> &rj) {
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

vector< Image * > ImageLoader::loadNextImages() {
	vector < Image * > rj;
	dosadUcitanoTrenutno = 0;
	loadFromDir( dir, rj );
	dosadUcitanoUkupno += rj.size();

	return rj;
}
