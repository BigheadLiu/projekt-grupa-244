#include "StdAfx.h"
#include "Cascade.h"
#include <vector> 
#include "Feature.h"
#include <algorithm>
#include "Image.h"
#include <string>
#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <climits>
using namespace std;

#define stupac(x,y,c)		stupac[ ((x) * image->width + (y) ) * NUM_CHANNELS + (c)]
#define IntegralImage(x,y,c)	IntegralImage[ ((x) * image->width + (y)) * NUM_CHANNELS + (c) ]
#define data(i,j,k)			data[ (i) * step + (j) * NUM_CHANNELS + (k) ]
#define INF INT_MAX / 2
#define debug(x) cout << #x << ": " << x << endl;

void Image::createIntegralImage(IplImage* slika, int colorspace) {
	image = slika;
	this->colorspace = colorspace;
	if (colorspace != ColorSpace::RGB) { //nakon ovog bloka slika je u colorspace-u kojeg zelim
		IplImage *image2 = cvCreateImage( cvSize( image->width, image->height), image->depth, ColorSpace::getNChannels(colorspace) );		
	
		cvCvtColor( image, image2, ColorSpace::convertValueInverse(colorspace) );
		//cvCopyImage( image2, image );
		swap( image, image2 );
		
		cvReleaseImage( &image2 );
	} 

	NUM_CHANNELS = image->nChannels;
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

Image::Image(IplImage *slika, int colorspace) {
	createIntegralImage( slika, colorspace );
}

Image::Image(string fileName, int colorspace ) 
{		
	Image::ukupanBroj ++;
	image = cvLoadImage( fileName.c_str() );		

	if (image == NULL) cout << "Nisam uspio ucitati sliku";
	createIntegralImage( image, colorspace );
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

IplImage* Image::getRgbImage() {
	IplImage *tmpSlika = cvCreateImage( cvSize( image->width, image->height), image->depth, ColorSpace::getNChannels(ColorSpace::RGB) );
	int convert = ColorSpace::convertValue( colorspace );

	if ( convert != -1 ) //nakon ovog bloka slika je u RGB colorspace-u
		cvCvtColor( image, tmpSlika, convert );		
	else
		cvCopyImage( image, tmpSlika );


	return tmpSlika;
}

void Image::showImage(void) {
	const int minVelicina = 150;
	cvNamedWindow("PRIKAZ SLIKA", CV_WINDOW_AUTOSIZE);
	IplImage* imageRGB = getRgbImage();

	if (  min(imageRGB->height, imageRGB->width) < minVelicina) { //povecaj sliku za bolji pogled
		int h = imageRGB->height * minVelicina / min(imageRGB->height, imageRGB->width);
		int w = imageRGB->width * minVelicina / min(imageRGB->height, imageRGB->width);
		IplImage *tmpSlika = cvCreateImage( cvSize(h, w), imageRGB->depth, imageRGB->nChannels);
		cvResize( imageRGB, tmpSlika );

		cvShowImage("PRIKAZ SLIKA", tmpSlika );	
	} else { //ako ne zelim skalirat
		cvShowImage("PRIKAZ SLIKA", imageRGB );	
	}

	cvReleaseImage( &imageRGB);
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

	IplImage *imageRGB = getRgbImage();
	IplImage *tmpImage = cvCreateImage( cvSize(w,h), imageRGB->depth, imageRGB->nChannels );
	cvResize( imageRGB, tmpImage );
	
	//debug( scale );
	//debug( scaleSlike );
	int pomX = (X + x * scale) * scaleSlike;
	int pomY = (Y + y * scale) * scaleSlike;

	nacrtajTocke( tmpImage, f.add, scale * scaleSlike, pomX, pomY );
	nacrtajTocke( tmpImage, f.subtract, scale * scaleSlike, pomX, pomY );

	cvShowImage( "PRIKAZ SLIKA", tmpImage );
	if (wait == true) cvWaitKey(0);

	cvReleaseImage( &imageRGB );
	cvDestroyWindow("PRIKAZ SLIKA");
	cvReleaseImage( &tmpImage );
}

float Image::evaluateTrainedFeature(const Feature &F, int X, int Y, bool ispisi, float scale) {	
	int val = evaluateBaseFeature( F, X, Y, ispisi, scale );
	return  (F.usporedba * val < F.usporedba * F.treshold * scale * scale) * F.weight;
}

int Image::evaluateBaseFeature(const Feature &F, int X, int Y, bool ispisi, float scale) {	
	int rj = 0;
	int x = F.x;
	int y = F.y;	
			
	for(int i=0; i<F.add.size(); i++) {
		int a = (int)((x + F.add[i].first * F.scale) * scale + X);
		int b = (int)((y + F.add[i].second * F.scale) * scale + Y);			

		if (a >= getHeight() || b >= getWidth() ) return -INF;				
		rj += IntegralImage( a, b, F.channel);
	}
	
	for(int j=0; j<F.subtract.size(); j++) {
		int a = (int)((x + F.subtract[j].first * F.scale) * scale + X);
		int b = (int)((y + F.subtract[j].second * F.scale) * scale + Y);

		if (a >= getHeight() || b >= getWidth() ) return -INF;

		rj -= IntegralImage(a, b, F.channel);
	}

	return rj;
}


void Image::nacrtajTocke( IplImage *slika, vector < pair<int,int> > tocke, float scale, int pomX, int pomY, int r, int g, int b) {
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

				data[ x * step + y * chan + 0 ] = r;
				data[ x * step + y * chan + 1 ] = g;
				data[ x * step + y * chan + 2 ] = b;
			}
	}
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
		int velicinaSkoka = trenScale * 2;
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

bool Image::evaluateSystem(int i, int j, int velicinaProzora, int trenScale, vector<Cascade>& kaskade, int order, float tresholdUkupno) {
	int k;
	if (order == 1) {
		for(int n=0; n<kaskade.size(); n++) {
			Cascade& kaskada = kaskade[n];

			for(k=0; k<kaskada.cascade.size(); k++) 
				if (evaluateCascadeLevel( i, j, velicinaProzora, trenScale, kaskada, k) == false)
					return false;
			
		}
		return true;
	} else {
		float suma = 0.;
		for(int n=0; n<kaskade.size(); n++) {
			Cascade& kaskada  = kaskade[n];

			for(k=0; k<kaskada.cascade.size(); k++) 
				if (evaluateCascadeLevel( i, j, velicinaProzora, trenScale, kaskada, k) == false)
					break;
			
			if (k == kaskada.cascade.size())
				suma += kaskada.weight;
		}
		if (suma >= tresholdUkupno) return true;
		else return false;
	}

	
}

vector<Image::Rectangle> Image::evaluateCascade(vector<Cascade>& kaskade, float pocetniScale, float stepScale, float zavrsniScale, int order, float ukupniTreshold, bool showImages) {
//#define SHOW_PICTURE_FOR_EACH_SCALE
//#define SAVE_SELECTED_IMAGES
//#define SHOW_FEATURE
	vector < Rectangle > rjesenje;
	debug("EVALUIRAM KASKADU NA SLICI");

	float trenScale = pocetniScale;
	int k, n;
	IplImage* tmpImage = NULL;

	if (showImages == true) {
		 tmpImage = cvCreateImage( cvSize(image->width, image->height),image->depth, image->nChannels);		
		cvCopyImage( image, tmpImage );
	}


	int brFalse = 0, brTrue = 0;
	for(;trenScale<zavrsniScale; trenScale *= stepScale) {	
		int velicinaSkoka = ( trenScale + 1 ) * 3; //ovdje je bilo 5
		int velicinaProzora = trenScale * 24;

		for(int i=0; i+velicinaProzora<getHeight(); i+= velicinaSkoka) {
			for(int j=0; j+velicinaProzora<getWidth(); j+=velicinaSkoka) {
				this->ukupnoEvaluirano++;

				//if ( n == kaskade.size() ) { //prosao je sve elemente kaskade, ovo je pronadeni znak
				if (evaluateSystem(i, j, velicinaProzora, trenScale, kaskade, order, ukupniTreshold) == true) {
					brTrue ++;
					this->ukupnoEvaluiranoTrue++;

					if (showImages == true) {
						CvScalar &boja = ColorSpace::getMarkerColor( this->colorspace );
						nacrtajOkvir( image, i, j, velicinaProzora, boja.val[0], boja.val[1], boja.val[2] );
					}

					rjesenje.push_back( Rectangle(i, j, velicinaProzora, velicinaProzora ) );
#ifdef SHOW_FEATURE 
					nacrtajTocke( image, kaskade[0].cascade[0][0].add, trenScale, i, j, 255, 0, 0);
					nacrtajTocke( image, kaskade[0].cascade[0][0].subtract, trenScale, i, j, 0, 0, 255);
#endif


#ifdef SAVE_SELECTED_IMAGES
					char tmpBroj[20], tmp[100];
					strcpy( tmp, "c:\\Images\\false2\\" );
					itoa( ukupnoEvaluiranoTrue, tmpBroj, 10 );
					strcat( tmp, tmpBroj );					

					saveImageExtraction( tmp, i, j, velicinaProzora, 24, 24, tmpImage );
#endif
				} else {
					brFalse ++;
					this->ukupnoEvaluiranoFalse++;
				}
			}
		}

#ifdef SHOW_PICTURE_FOR_EACH_SCALE
		cout << "OD UKUPNO: " << brFalse + brTrue << " PROZORA, JA SAM ZA: " << brTrue << " rekao da su znakovi. To je: " << (float)brTrue / (brFalse + brTrue) << " od ukupnog broja." << endl;
		showImage();
		cvCopyImage( tmpImage, image );				
		brTrue = 0; brFalse = 0;
#endif


	}

#ifndef NODEBUG
		cout << "OD UKUPNO: " << brFalse + brTrue << " PROZORA, JA SAM ZA: " << brTrue << " rekao da su znakovi. To je: " << (float)brTrue / (brFalse + brTrue) << " od ukupnog broja." << endl;
#endif

	if (showImages == true) {
		showImage();
		cvCopyImage( tmpImage, image );
	}

	return rjesenje;
}

bool Image::postProcess( int x, int y, int velicina ) {
	unsigned char *data = (unsigned char *) image->imageData;
	int step = image->widthStep;
	int pocX = x + velicina * 0.1;
	int krajX = x + velicina * 0.9;

	int pocY = y + velicina * 0.1;
	int krajY = y + velicina * 0.9;

	cout << endl;
	for(int i=pocX; i<krajX; i++) {
		int brojac = 0;
		for(int j=pocY; j<krajY; j++) {
			if (data(i, j, 2) > 150) brojac++;
		}

		cout << "(" << brojac << " " << 0.1 * velicina << ")" << endl;
		if ( (float) brojac < 0.1 * velicina ) return false;		
	}


	return true;
}

bool Image::evaluateCascadeLevel( int X, int Y, int velicinaProzora, float scale, Cascade &kaskada, int index) {
	double sum = 0.;
	for(int i=0; i<kaskada.cascade[index].size(); i++) {
		//kaskada.cascade[index][i].scale *= scale;
		sum += evaluateTrainedFeature( kaskada.cascade[index][i], X, Y, false, scale );
		//kaskada.cascade[index][i].scale /= scale;
	}

	if (sum > kaskada.levelThreshold[index]) return true;
	else return false;
}

void Image::saveImage(string file, int width, int height) {
	if (width == 0 || height == 0) {
		cvSaveImage( file.c_str(), image );
		return; 
	}

	IplImage *tmp = cvCreateImage( cvSize(width, height), image->depth, image->nChannels );
	cvResize( image, tmp ); 
	cvSaveImage( file.c_str(), tmp );

	cvReleaseImage( &tmp );
}

void Image::saveImageExtraction(string file, int x, int y, int velicina, int width, int height, IplImage* image) {	
	IplImage *tmp = cvCreateImage( cvSize(velicina, velicina), image->depth, image->nChannels );

	unsigned char *data = (unsigned char *) image->imageData;	
	int step = image->widthStep;

	for(int i=0; i<velicina; i++)
		for(int j=0; j<velicina; j++) 
			for(int k=0; k<image->nChannels; k++) 
				tmp->imageData[i * tmp->widthStep + j * tmp->nChannels + k] = data( (x+i), (y+j), k );

	IplImage *spremi = cvCreateImage( cvSize(width, height), image->depth, image->nChannels );
	cvResize( tmp, spremi );
    string fileTmp = file + ".bmp";
	cvSaveImage( fileTmp.c_str(), spremi );
	
	cvReleaseImage( &tmp );
	cvReleaseImage( &spremi );
}


void Image::writeTestData() {
	cout << "Ukupan broj prozora za trazenje: " << Image::ukupnoEvaluirano << endl;
	cout << "Ukupan broj pronadenih znakova: " << Image::ukupnoEvaluiranoTrue << endl;
	cout << "Ukupan broj isjecaka koji su proglaseni ne-znakovima: " << Image::ukupnoEvaluiranoFalse << endl;
}

void Image::clearTestData() {
	ukupnoEvaluirano = ukupnoEvaluiranoTrue =ukupnoEvaluiranoFalse = 0;
}


int Image::dosadUcitano = 0;
int Image::ukupanBroj = 0;

int Image::ukupnoEvaluiranoTrue = 0;
int Image::ukupnoEvaluiranoFalse = 0;
int Image::ukupnoEvaluirano = 0;