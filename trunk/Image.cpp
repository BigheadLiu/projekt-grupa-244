#include "StdAfx.h"
#include <sstream>
#include <windows.h>
#include <vector>
#include "BaseFeature.h"
#include <algorithm>
#include "Image.h"
#include <string>
#include <cv.h>
#include <iostream>
#include <highgui.h>
#include <sstream>
#include <conio.h>
using namespace std;

#define stupac(x,y)		stupac[ (x) * image->width + (y) ]
#define IntegralImage(x,y)	IntegralImage[ (x) * image->width + (y) ]
#define data(i,j,k)			data[ i * step + j * channels + k ]


Image::Image(string fileName)
{	
	//TODO: u kasnijim verzijama prepravit na BGR umjesto grayscale-a
	image = cvLoadImage( fileName.c_str() );		

	if (image == NULL) cout << "Nisam uspio ucitati sliku";
	IntegralImage =(int *) malloc( sizeof(int) * image->height * image->width );
	int *stupac = (int *) malloc( sizeof(int) * image->height * image->width );
	char *data = image->imageData;
	int step = image->widthStep;

	//integralnu sliku trenutno radim samo na Y komponenti(zbog toga sliku pretvaram u grayscale)
	//u finalnoj verziji to treba izbaciti
	IplImage* grayImage = cvCreateImage( cvSize(image->width, image->height),image->depth, 1);		
	cvCvtColor( image, grayImage, CV_BGR2GRAY );
	int channels = 1;
	
	for(int j=0; j<image->width; j++)
		stupac(0,j) = data(0,j,0);

	for(int i=1; i<image->height; i++)
		for(int j=0; j<image->width; j++)
			stupac(i,j) = stupac(i-1, j) + data(i,j,0);		

	for(int i=0; i<image->height; i++)
		IntegralImage(i,0) = stupac(i,0);

	for(int i=0; i<image->height; i++)
		for(int j=1; j<image->width; j++)
			IntegralImage(i,j) = IntegralImage(i, j-1) + stupac(i,j);

	free( stupac );
	cvReleaseImage( &grayImage );
}

Image::~Image(void)
{
	if( image != NULL) cvReleaseImage( &image );
	free(IntegralImage);
}

string Image::imageData() {
	char rj[100];
	sprintf(rj, "Velicina: (%d,%d)\nChannels: %d\nInterleaved: %d\nDepth: %d\n", 
		image->height, image->width, image->nChannels, image->dataOrder, image->depth);
		
	return string(rj);
}

int Image::getHeight(void) {
	return image->height;
}

int Image::getWidth(void) {
	return image->width;
}

void Image::showImage(void) {
	const int minVelicina = 100;
	cvNamedWindow("PRIKAZ SLIKA", CV_WINDOW_AUTOSIZE);
	
	if ( min(image->height, image->width) < minVelicina) { //povecaj sliku za bolji pogled
		int h = image->height * minVelicina / min(image->height, image->width);
		int w = image->width * minVelicina / min(image->height, image->width);
		IplImage *tmpSlika = cvCreateImage( cvSize(h, w), image->depth, image->nChannels);
		cvResize( image, tmpSlika );

		cvShowImage("PRIKAZ SLIKA", tmpSlika );	
	} else {
		cvShowImage("PRIKAZ SLIKA", image );	
	}

	cvWaitKey(0);
	cvDestroyWindow("PRIKAZ SLIKA");
}

void Image::showImageOverlappedWithFeature(const BaseFeature &f, int x, int y, float scale) {
	const int minVelicina = 500;
	cvNamedWindow("PRIKAZ SLIKA", CV_WINDOW_AUTOSIZE);
	
	float scaleSlike = 1.;
	if ( min(image->height, image->width) < minVelicina)
		scaleSlike = minVelicina / min(image->height, image->width);	

	int h = (int)( image->height * scaleSlike );
	int w = (int)( image->width * scaleSlike );

	IplImage *tmpImage = cvCreateImage( cvSize(w,h), image->depth, image->nChannels );
	cvResize( image, tmpImage );
	
	nacrtajTocke( tmpImage, f.add, scale * scaleSlike, x, y );
	nacrtajTocke( tmpImage, f.subtract, scale * scaleSlike, x, y );

	cvShowImage( "PRIKAZ SLIKA", tmpImage );
	cvWaitKey(0);
	cvDestroyWindow("PRIKAZ SLIKA");
}

int Image::evaluateBaseFeature(const BaseFeature &F, int x, int y, float scale) {	
	int rj = 0;
		
	for(int i=0; i<F.add.size(); i++) {
		int a = (int)((x + F.add[i].first) * scale);
		int b = (int)((y + F.add[i].second) * scale);
		rj += IntegralImage( a, b);
	}
	
	for(int j=0; j<F.subtract.size(); j++) {
		int a = (int)((x + F.subtract[j].first) * scale);
		int b = (int)((y + F.subtract[j].second) * scale);
		rj -= IntegralImage(a, b);

	}

	return rj;
}


void Image::nacrtajTocke( IplImage *slika, vector < pair<int,int> > tocke, float scale, int pomX, int pomY) {
	char *data = slika->imageData;
	int step = slika->widthStep;
	int chan = slika->nChannels;
	int sirina = 3;

	for(int i=0; i<tocke.size(); i++) {
		int a = (int) (tocke[i].first * scale) + pomX;
		int b = (int) (tocke[i].second * scale) + pomY;

		for(int x=a-sirina; x<a+sirina; x++)
			for(int y=b-sirina; y<b+sirina; y++) {
				if (x < 0 || y < 0 || x >= (int)( image->height * scale) || y >= (int)( image->width * scale)) continue;

				data[ x * step + y * chan + 0 ] = 200;
				data[ x * step + y * chan + 1 ] = 200;
				data[ x * step + y * chan + 2 ] = 200;
			}
	}
}

//TODO: funkcija trenutno ne radi kako treba
vector<Image*> Image::loadAllImagesFromDirectory(string dir) {
	string file = dir + "\\files.txt";
	vector < Image* > rjesenje;
	char ime[100];

	FILE *fin = fopen(file.c_str(), "r");
	while( fscanf(fin, "%s", ime) == 1) {
		string tmp = dir + "\\" + string( ime );
		
		rjesenje.push_back( new Image( tmp ) );
	}
	fclose(fin);

	return rjesenje;
}

int Image::SearchDirectory(LPCWSTR dir, std::vector<std::string> &refvecFiles,
                    const std::string        &refcstrRootDirectory,
                    const std::string        &refcstrExtension,
                    bool                     bSearchSubdirectories)
{
  std::string     strFilePath;             // Filepath
  std::string     strPattern;              // Pattern
  std::string     strExtension;            // Extension
  HANDLE          hFile;                   // Handle to file
  WIN32_FIND_DATA FileInformation;         // File information


  strPattern = refcstrRootDirectory + "\\*.*";
  wchar_t* text = new wchar_t[100];
  MultiByteToWideChar(CP_ACP, NULL, strPattern.c_str(), -1, text, strPattern.size() );
  
  hFile = FindFirstFile( (LPCWSTR)dir, &FileInformation);  
  if(hFile != INVALID_HANDLE_VALUE)
  {
    do
    {
      if(FileInformation.cFileName[0] != '.')
      {
        strFilePath.erase();
        strFilePath = refcstrRootDirectory + "\\" + (char *)(FileInformation.cFileName);

        if(FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
        }
        else
        {		  
		  
          // Check extension
          strExtension = (char *)FileInformation.cFileName;
          strExtension = strExtension.substr(strExtension.rfind(".") + 1);

		  cout << strFilePath << endl;
          if(strExtension == refcstrExtension)
          {
            // Save filename

            refvecFiles.push_back(strFilePath);
          }
        }
      }
    } while(::FindNextFile(hFile, &FileInformation) == TRUE);

    // Close handle
    ::FindClose(hFile);

    DWORD dwError = ::GetLastError();
    if(dwError != ERROR_NO_MORE_FILES)
      return dwError;
  }

  return 0;
}
