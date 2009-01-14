#include "StdAfx.h"
#include "Cascade.h"
#include "Image.h"
#include "AdaBoost.h"
#include <list>
using namespace std;


/*
 * klasa impementira Viola-Jones algoritam i stvara kaskadu
 *
 * @author: Ivan Kusalic
 */
class ViolaJones {
public:
// TODO: treba sve testirati, iako je kod napisan, niti jedan dio nije testiran!!!
// TODO: da li da rade direktno s integralnim slikama ili nekakvim pointerima na njih (brze)
// TODO: implementirati metodu adaBoostTrain()

	vector<Image*> positiveTrain;  // set (integralnih) slika za treniranje koje su pozitivne
	vector<Image*> negativeTrain;  // set (integralnih) slika za treniranje koje su negativne
	
	vector<Image*> positiveTest;  // set (integralnih) slika za testiranje koje su pozitivne
	vector<Image*> negativeTest;  // set (integralnih) slika za testiranje koje su negativne
	string negativeTestDirectory;
	int minNumber; // minimalni broj negativnih test primjera u jednom trenutku
	int loadNumber; // broj negativnih test primjera koje ce ucitavati u jednom koraku ukoliko ih ima manje od minNumber

	int clearedNegativeTestSize; // broj negativnih primjera koje je obrisao zbog stednje memorije
	
	ViolaJones(void)
	{
	}
	
	~ViolaJones(void)
	{
	}
	
	// konstruktor za inicijalizaciju:	
	ViolaJones(vector<Image*> pte, vector<Image*> nte, string negativeTestDirectory, int minNumber, int loadNumber);
	
	/*
	 * f - the maximum acceptable false positive rate per layer
	 * d - the minimum acceptable detection rate per layer
	 * targetF - target overall false positive rate
	 * kaskada - kaskada koja se gradi
	 */
	void buildCascade(double f,double d,double targetF,Cascade &kaskada);
	
	// Evaluate current cascaded classifier on validation set to determine tmpF and tmpD
	pair<double,double> evaluateOnTest(Cascade &kaskada);
	
	// za zadanu integralnu sliku vraca da li je svrstana pozitivno ili negativno 
	bool evaluate(Image *iim, Cascade &kaskada);
	
	/*
	 * evaluate the current cascaded detector on the set of non-face images
	 * and put any false detections into the set N
	 */
	void evaluateOnTrainNegative(vector<Image*> &N, Cascade &kaskada);
	
	/*
	 * Decrease threshold for the ith classifier until the current cascaded classifier
	 * has a detection rate of at least minD (this also affects tmpF )
	 *
	 * koristi binary search za pronalazak thresholda
	 */
	void decraseThreshold(int ith, double minD, Cascade &kaskada);

	/*
	* Funkcija koja se koristi u slucaju da dode do greske na racunalu prilikom postupka ucenja.
	* Nakon svakog novo stvorenog levela kaskade vrsi se spremanje kaskade u file temp.cascade.
	* Ovo funkcija cita zadnji level kaskade iz tog file-a i nastavlja sa ucenjem	
	*/
	void recoverFromError(int &i, double &lastD, double &lastF, vector< Image* > &N);
	/*
	* Brise podatke nastale pohranom privremenih podataka za oporavak od pogreske.
	*/
	void clearTempData();

    /*
	* Sprema podatke bitne za nastavak procesa ucenja na disk u fileove: podaci.temp, temp.cascade
	*/
	void ViolaJones::spremiPodatke(Cascade kaskada, int &i, double &lastD, double &lastF);

};
