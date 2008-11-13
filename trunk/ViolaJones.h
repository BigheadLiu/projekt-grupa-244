#include "StdAfx.h"
#include "Cascade.h"


/*
 * klasa impementira Viola-Jones algoritam i stvara kaskadu
 *
 * @author: Ivan Kusalic
 */
class AdaBoost {
public:
// TODO: treba sve testirati, iako je kod napisan, niti jedan dio nije testiran!!!
// TODO: da li da rade direktno s integralnim slikama ili nekakvim pointerima na njih (brze)
// TODO: implementirati metodu adaBoostTrain()

	vector<IntegralImage> positiveTrain;  // set (integralnih) slika za treniranje koje su pozitivne
	vector<IntegralImage> negativeTrain;  // set (integralnih) slika za treniranje koje su negativne
	
	vector<IntegralImage> positiveTest;  // set (integralnih) slika za testiranje koje su pozitivne
	vector<IntegralImage> negativeTest;  // set (integralnih) slika za testiranje koje su negativne
	
	ViolaJones(void)
	{
	}
	
	~ViolaJones(void)
	{
	}
	
	// konstruktor za inicijalizaciju:	
	ViolaJones(vector<IntegralImage> ptr, vector<IntegralImage>ntr, vector<IntegralImage>pte, vector<IntegralImage> nte);
	
	/*
	 * f - the maximum acceptable false positive rate per layer
	 * d - the minimum acceptable detection rate per layer
	 * targetF - target overall false positive rate
	 * kaskada - kaskada koja se gradi
	 */
	void buildCascade(double f,double d, double targetF, Cascade &kaskada);
	
	// Evaluate current cascaded classifier on validation set to determine tmpF and tmpD
	pair<double,double evaluateOnTest(Cascade kaskada);
	
	// za zadanu integralnu sliku vraca da li je svrstana pozitivno ili negativno
	bool evaluate(IntegralImage iim, Cascade kaskada);
	
	/*
	 * evaluate the current cascaded detector on the set of non-face images
	 * and put any false detections into the set N
	 */
	void evaluateOnTrainNegative(list<IntegralImage> &N, Cascade kaskada);
	
	/*
	 * Decrease threshold for the ith classifier until the current cascaded classifier
	 * has a detection rate of at least minD (this also affects tmpF )
	 *
	 * koristi binary search za pronalazak thresholda
	 */
	void decraseThreshold(int ith, double minD, Cascade kaskada);
};
