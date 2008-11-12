#include <string>
#include "BaseFeature.h"
#include <vector>
#include <cv.h>
#include <highgui.h>
using namespace std;

#pragma once

class Image
{
private:
	void nacrtajTocke( IplImage *slika, vector< pair<int,int> > tocke, float scale, int x, int y );
	static string convertWCharArrayToString(const WCHAR * const wcharArray);
	int *IntegralImage;
	static int SearchDirectory(std::vector<std::string> &refvecFiles,
                    const std::string        &refcstrRootDirectory,
                    const std::string        &refcstrExtension,
                    bool                     bSearchSubdirectories = true);

public:
	struct Rectangle {
		int x, y, height, width;
	};

	IplImage *image;

	Image(string);
	~Image(void);

	int evaluateBaseFeature(const BaseFeature &F, int x, int y, float scale);
	void showImage();
	void showImageOverlappedWithFeature(const BaseFeature &f, int x, int y, float scale);
	static vector<Image> Image::loadAllImagesFromDirectory(string directory);
	vector < Rectangle > findSigns();
	string imageData();
	int getHeight();
	int getWidth();
};
