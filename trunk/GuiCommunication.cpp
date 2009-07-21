#include "StdAfx.h"
#include <string>
#include "Image.h"
#include "Cascade.h"
#include <vector>
#include <algorithm>
#include "ColorSpace.h"
#include "GuiCommunication.h"
using namespace std;

vector < Cascade > GuiCommunication::kaskade;
vector < string > GuiCommunication::images;
FILE * GuiCommunication::fout = NULL;
int GuiCommunication::brojac = 0;
int GuiCommunication::isSlijedno = 0;
float GuiCommunication::tresholdUkupni= 0;
string GuiCommunication::outputFile = "rez.dat";
string GuiCommunication::file = "postavke.dat";

GuiCommunication::GuiCommunication(void)
{
}

GuiCommunication::~GuiCommunication(void)
{
}

void GuiCommunication::saveResults(vector< Image::Rectangle > output ) {
	if (fout == NULL) {
		fout = fopen( GuiCommunication::outputFile.c_str(), "w" );
		fprintf(fout, "%d\n", images.size());
	}
	fprintf(fout, "%d\n", output.size() );
	for(int i=0; i<output.size(); i++) {
		fprintf(fout, "%d %d %d %d\n", output[i].x, output[i].y, output[i].width, output[i].width );
	}
	fflush(fout);
}

void GuiCommunication::sendResults() {
	fclose(fout);
}

string GuiCommunication::sredi(char *x) {
	string ok = "-_:.\\ ";
	char tmp[1000];
	int d = strlen(x);
	for(int i=0; i<d; i++) {
		if ( x[i] <= 'z' && x[i] >= 'a') continue;
		if ( x[i] <= 'Z' && x[i] >= 'A') continue;
		if ( x[i] <= '9' && x[i] >= '0') continue;
		if ( ok.find( x[i] ) != string::npos) continue;
		
		x[i] = 0;
		return string(x);
	}
	return string(x);
}

void GuiCommunication::start() {
	int brojKaskada, level, brojSlika;
	float tezina;
	char ime[1000];

	FILE *fin = fopen((GuiCommunication::file).c_str(), "r");
		fscanf(fin, "%d", &brojKaskada );
		//kaskade.reserve( brojKaskada );
		for(int i=0; i<brojKaskada; i++) {
			fscanf(fin, "%d %f %f ", &level, &tezina);
			fgets( ime, 500, fin );

			Cascade tmp = Cascade(ColorSpace::RGB);
			tmp.loadCascade( sredi( ime ) );
			tmp.weight = tezina;
			
			//printf(">>%s<<\n",ime);
			//printf(">>%s<<\n",sredi(ime).c_str());
			//for(int i=0; i<tmp.cascade.size(); i++) 
			//	for(int j=0; j<tmp.cascade[i].size(); j++)
			//		printf("%d\n", tmp.cascade[i][j].treshold );

			//printf(">>%d %f %f<<\n",level, tezina, tresholdUkupni);
			//fflush(stdout);

			tmp.redniBroj = level;
			kaskade.push_back( tmp );
		}
		sort( kaskade.begin(), kaskade.end() );

		fscanf(fin, "%d", &isSlijedno );
		fscanf(fin, "%d ", &brojSlika );

		for(int i=0; i<brojSlika; i++) {			
			fgets(ime, 500, fin );
			images.push_back( sredi(ime) );
		}

		fscanf(fin, "%f", &tresholdUkupni );
	fclose(fin);
}

vector<Cascade> GuiCommunication::getCascade() {
	return GuiCommunication::kaskade;
}

Image* GuiCommunication::getNextImage() {	
	if (brojac >= images.size()) return NULL;
	Image *slika = new Image( images[ brojac ] );	
	GuiCommunication::brojac ++;
	return slika;
}

float GuiCommunication::getPocetniScale() {
	return 1.;
}

float GuiCommunication::getZavrsniScale() {
	return 4.;
}

float GuiCommunication::getStepScale() {
	return 1.25;
}

int GuiCommunication::getIsSlijedno() {
	return isSlijedno;
}

float GuiCommunication::getTreshold() {
	return tresholdUkupni;
}