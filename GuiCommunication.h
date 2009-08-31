#include "Image.h"
#include <string>
#pragma once
using namespace std;

/*
* Trenutno postoji problem sa colorspace-ovim.
* Koristi se samo RGB colorspace. 
* Postoji problem ukoliko se koriste kaskade s razlicitim colorspace-ovima.
* Jer onda je potrebno vrsiti konverziju slike u druge prostore boja.
*
*/
class GuiCommunication
{
	static string file;
	static string outputFile;

	static FILE* fout;

	static vector < Cascade > kaskade;
	static vector < string > images;
	static int brojac;
	static float tresholdUkupni;
	static int isSlijedno;

	static string sredi(char *x);
public:
	GuiCommunication(void);
	~GuiCommunication(void);
	
	static void start();
	static void saveResults( vector < Image::Rectangle > output );
	static void sendResults();
	static vector<Cascade> getCascade();
	static float getPocetniScale();
	static Image* getNextImage();
	static float getStepScale();
	static float getZavrsniScale();
	static int GuiCommunication::getIsSlijedno();
	static float GuiCommunication::getTreshold();
	static int getColorSpace(); //treba napraviti izmjenu ukoliko je potrebna izmjena colorspace-a unutar GUI-a
};
