#include "StdAfx.h"
#include "Cascade.h"
#include <vector> 
#include "Feature.h"
#include <algorithm>
#include "Image.h"
#include <string>
#include <cv.h>
#include <iostream>
#include <highgui.h>
#include <climits>
using namespace std;

#define stupac(x,y,c)		stupac[ ((x) * image->width + (y) ) * NUM_CHANNELS + (c)]
#define IntegralImage(x,y,c)	IntegralImage[ ((x) * image->width + (y)) * NUM_CHANNELS + (c) ]
#define data(i,j,k)			data[ (i) * step + (j) * NUM_CHANNELS + (k) ]
#define INF INT_MAX / 2
#define debug(x) cout << #x << ": " << x << endl;
int NUM_CHANNELS;

Image::Image(string fileName) 
{		
	image = cvLoadImage( fileName.c_str() );		

	//cout << "(" << image->height << " " << image->width << ")   ";

	NUM_CHANNELS = image->nChannels;

	if (image == NULL) cout << "Nisam uspio ucitati sliku";
	IntegralImage =(int *) malloc( sizeof(int) * image->height * image->width * NUM_CHANNELS );
	int *stupac = (int *) malloc( sizeof(int) * image->height * image->width * NUM_CHANNELS );
	unsigned char *data = (unsigned char *) image->imageData;	

	int step = image->widthStep;
	
	for(int j=0; j<image->width; j++) 
		for(int c=0; c<NUM_CHANNELS; c++) 
			stupac(0,j,c) = data(0,j,c);

	for(int i=1; i<image->height; i++)
		for(int j=0; j<image->width; j++)
			for(int c=0; c<NUM_CHANNELS; c++)
				stupac(i,j,c) = stupac(i-1, j, c) + data(i,j,c);		

	for(int i=0; i<image->height; i++)
		for(int c=0; c<NUM_CHANNELS; c++)
			IntegralImage(i,0,c) = stupac(i,0,c);

	for(int i=0; i<image->height; i++)
		for(int j=1; j<image->width; j++)
			for(int c=0; c<NUM_CHANNELS; c++)
				IntegralImage(i,j, c) = IntegralImage(i, j-1, c) + stupac(i,j, c);

	free( stupac );	
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

void Image::showImageOverlappedWithFeature(const Feature &f, int X, int Y, bool wait) {
	float scale = f.scale;
	int x = f.x * scale + X;
	int y = f.y * scale + Y;

	const int minVelicina = 500;
	cvNamedWindow("PRIKAZ SLIKA", CV_WINDOW_AUTOSIZE);
	
	float scaleSlike = 1.;
	if ( min(image->height, image->width) < minVelicina)
		scaleSlike = minVelicina / min(image->height, image->width);	

	int h = (int)( image->height * scaleSlike );
	int w = (int)( image->width * scaleSlike );

	IplImage *tmpImage = cvCreateImage( cvSize(w,h), image->depth, image->nChannels );
	cvResize( image, tmpImage );
	
	//debug( scale );
	//debug( scaleSlike );
	int pomX = (X + x * scale) * scaleSlike;
	int pomY = (Y + y * scale) * scaleSlike;

	nacrtajTocke( tmpImage, f.add, scale * scaleSlike, pomX, pomY );
	nacrtajTocke( tmpImage, f.subtract, scale * scaleSlike, pomX, pomY );

	cvShowImage( "PRIKAZ SLIKA", tmpImage );
	if (wait == true) cvWaitKey(0);

	cvDestroyWindow("PRIKAZ SLIKA");
	cvReleaseImage( &tmpImage );
}

float Image::evaluateTrainedFeature(const Feature &F, int X, int Y, bool ispisi, float scale) {	
	int val = evaluateBaseFeature( F, X, Y, ispisi );
	return  (F.usporedba * val < F.usporedba * F.treshold * scale) * F.weight;
}

int Image::evaluateBaseFeature(const Feature &F, int X, int Y, bool ispisi) {	
	int rj = 0;
	int x = F.x;
	int y = F.y;
	float scale = F.scale;
			
	for(int i=0; i<F.add.size(); i++) {
		int a = (int)((x + F.add[i].first * scale) + X);
		int b = (int)((y + F.add[i].second * scale) + Y);		

		if (a >= getHeight() || b >= getWidth() ) return -INF;				
		rj += IntegralImage( a, b, F.channel);
	}
	
	for(int j=0; j<F.subtract.size(); j++) {
		int a = (int)((x + F.subtract[j].first) * scale + X);
		int b = (int)((y + F.subtract[j].second) * scale + Y);

		if (a >= getHeight() || b >= getWidth() ) return -INF;

		rj -= IntegralImage(a, b, F.channel);
	}

	return rj;
}


void Image::nacrtajTocke( IplImage *slika, vector < pair<int,int> > tocke, float scale, int pomX, int pomY) {
	char *data = slika->imageData;
	int step = slika->widthStep;
	int chan = slika->nChannels;
	int sirina = 1;
	
	for(int i=0; i<tocke.size(); i++) {
		int a = (int)( (tocke[i].first) * scale + pomX);
		int b = (int)( (tocke[i].second) * scale + pomY);
		//cout << "Crtam tocke: " << a << " " << b << " scale: " << scale << endl;

		for(int x=a-sirina; x<a+sirina; x++)
			for(int y=b-sirina; y<b+sirina; y++) {
				//cout << "rubovi: " <<  x << " " << y << endl;

				if (x < 0 || y < 0 || x >= (int)( slika->height) || y >= (int)( slika->width)) continue;

				data[ x * step + y * chan + 0 ] = 0;
				data[ x * step + y * chan + 1 ] = 0;
				data[ x * step + y * chan + 2 ] = 250;
			}
	}
}

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

void Image::evaluirajLevel( vector< Feature > features ) {
	float trenScale = 1;
	float stepScale = 1.25;

	//sumiraj weight za sve feature-e
	float ukupniTreshold = 0;
	for(int i=0; i<features.size(); i++)
		ukupniTreshold += features[i].weight;
	ukupniTreshold /= 2;

	IplImage* tmpImage = cvCreateImage( cvSize(image->width, image->height),image->depth, image->nChannels);		
	cvCopyImage( image, tmpImage );


	for(;trenScale<10; trenScale *= stepScale) {	
	//{ trenScale = 8;
 		int velicinaSkoka = ( trenScale + 1 ) * 4;
		int velicinaProzora = trenScale * 20;
		int brFalse = 0, brTrue = 0;

		for(int i=0; i<getHeight(); i+= velicinaSkoka) {
			for(int j=0; j<getWidth(); j+=velicinaSkoka) {
				float suma = 0;

				for(int k=0; k<features.size(); k++) {					
					features[k].scale *= trenScale;
					int val = evaluateBaseFeature( features[k], i, j );
					features[k].scale /= trenScale;

					if (val == -INF) { suma =-INF; break; }

					suma += ( features[k].usporedba * val > features[k].usporedba * features[k].treshold ) * features[k].weight;

					

				}
				if (suma == -INF) break;

				if (suma > ukupniTreshold) {					
						//cout << "DA!!!" << i << " " << j << " " << " " << velicinaProzora << endl;

						//showImageOverlappedWithFeature( features[0], false);
						nacrtajOkvir( image, i , j, velicinaProzora, 0, 255, 0);

						for(int k=0; k<features.size(); k++) {
						//{int k = 0;
						int pomX = (i + features[k].x * trenScale);
						int pomY = (j + features[k].y * trenScale);

							nacrtajTocke( image, features[k].add, features[k].scale *trenScale, pomX, pomY );
							nacrtajTocke( image, features[k].subtract, features[k].scale * trenScale, pomX, pomY );
						}
						showImage();
						cvWaitKey(0);

						brTrue++;
				} else {
						brFalse++;
				}
			}
		} 
		//showImageOverlappedWithFeature( features[0], true);

		cout << "OD UKUPNO: " << brFalse + brTrue << " PROZORA, JA SAM ZA: " << brTrue << " rekao da su znakovi" << endl;
		showImage();
		cvCopyImage( tmpImage, image );
		//cvWaitKey(0);

	}
	cvWaitKey(0);
	features[0].scale = 1;
	features[0].x = 0;
	features[0].y = 0;
		
	showImageOverlappedWithFeature( features[0], true);
}

void Image::nacrtajOkvir(IplImage *slika, int X, int Y, int velicina, int b, int g, int r) {
	char *data = slika->imageData;
	int step = slika->widthStep;
	int chan = slika->nChannels;

	vector < pair<int,int> > tocke;
	for(int i=X; i<X+velicina; i++) {
		tocke.push_back( make_pair( i, Y + velicina ) );
		tocke.push_back( make_pair( i, Y) );
	}
	for(int j=Y; j<Y+velicina; j++) {
		tocke.push_back( make_pair( X, j) );
		tocke.push_back( make_pair( X + velicina, j) );
	}

	for(int i=0; i<tocke.size(); i++) {
		int x = tocke[i].first;
		int y = tocke[i].second;

		data[ x * step + y * chan + 0 ] = b;
		data[ x * step + y * chan + 1 ] = g;
		data[ x * step + y * chan + 2 ] = r;
	}

}

void Image::evaluateCascade(Cascade kaskada, float pocetniScale, float stepScale, float zavrsniScale) {

	debug("EVALUIRAM KASKADU NA SLICI");

	float trenScale = pocetniScale;
	//float stepScale = 1.25;
	int k;

	IplImage* tmpImage = cvCreateImage( cvSize(image->width, image->height),image->depth, image->nChannels);		
	cvCopyImage( image, tmpImage );

	int brFalse = 0, brTrue = 0;
	for(;trenScale<zavrsniScale; trenScale *= stepScale) {	
		int velicinaSkoka = ( trenScale + 1 ) * 4;
		int velicinaProzora = trenScale * 20;

		for(int i=0; i+velicinaProzora<getHeight(); i+= velicinaSkoka) {
			for(int j=0; j+velicinaProzora<getWidth(); j+=velicinaSkoka) {

				for(k=0; k<kaskada.cascade.size(); k++) {
					if (evaluateCascadeLevel( i, j, velicinaProzora, trenScale, kaskada, k) == false) break;					
				}

				if ( k == kaskada.cascade.size() ) { //prosao je sve elemente kaskade, ovo je pronadeni znak
//#ifndef NODEBUG
//					cout << "NASAO!!!" << i << " " << j << " " << velicinaProzora << " " << velicinaSkoka << endl;
//#endif
					brTrue ++;
                    nacrtajOkvir( image, i, j, velicinaProzora, 0, 0, 255 );
		
				} else {
					brFalse ++;
				}
			}
		}
	}

#ifndef NODEBUG
		cout << "OD UKUPNO: " << brFalse + brTrue << " PROZORA, JA SAM ZA: " << brTrue << " rekao da su znakovi. To je: " << (float)brTrue / (brFalse + brTrue) << " od ukupnog broja." << endl;
#endif


	showImage();
	cvCopyImage( tmpImage, image );
}

bool Image::evaluateCascadeLevel( int X, int Y, int velicinaProzora, int scale, Cascade &kaskada, int index) {
	double sum = 0.;
	for(int i=0; i<kaskada.cascade[index].size(); i++) {
		kaskada.cascade[index][i].scale *= scale;
		sum += evaluateTrainedFeature( kaskada.cascade[index][i], X, Y, false, scale );
		kaskada.cascade[index][i].scale /= scale;
	}

	if (sum > kaskada.levelThreshold[index]) return true;
	else return false;
}