#include "StdAfx.h"
#include "ViolaJones.h" 
#include <iostream>
using namespace std;

#define MAX 1000000000
#define debug(x) cout << #x << ": " << x << endl;

ViolaJones::ViolaJones(ImageLoader &positiveTestLoader, ImageLoader& negativeTestLoader, int minNumber):negativeTestLoader(negativeTestLoader), positiveTestLoader(positiveTestLoader)
{
	positiveTest = positiveTestLoader.loadNextImages();
	negativeTest = negativeTestLoader.loadNextImages();

	clearedNegativeTestSize = 0;

	this->minNumber = minNumber;
}

/*
 * f - the maximum acceptable false positive rate per layer
 * d - the minimum acceptable detection rate per layer
 * targetF - target overall false positive rate
 * kaskada - kaskada koja se gradi
 */
void ViolaJones::buildCascade(double f,double d, double targetF,Cascade &kaskada) {
	//P - set of positive examples; N - set of negative examples:
	vector<Image*> P(positiveTest.begin(),positiveTest.end());
	vector<Image*> N(negativeTest.begin(),negativeTest.end());	

	double tmpF=1.0;  // trenutni false positive rate
	double lastF=1.0;  // false positive rate prethodnog nivoa
	double tmpD=1.0;  // trenutni detaction rate
	double lastD=1.0;  // detection rate prethodnog nivoa
	
	int i=-1;  // i - level kaskade
	int n;  // broj featureova u trenutnom levelu kaskade
	
	pair<double,double> tmpRet;  // pomocna varijabla

	recoverFromError(i, lastD, lastF,N ); // oporavak od greske u slucaju ako je prethodni postupak ucenja prekinut greskom na racunalu

	while(tmpF>targetF) {
		cout << "nivo kaskade: " << i+2 << ". "<< " Broj znakova: " << P.size() << " " << "Broj ne znakova: " << N.size() << " " << endl;
		i++;
		n=0;
		tmpF=lastF;
				
		kaskada.cascade.push_back( vector<Feature>(0) ); // dodajem prazan vektor na kraj da napravim mjesto za sljedeci level kaskade
		while(tmpF>f*lastF) {
			n+=80;
			cout << "broj featura: " << n << " false positive: " << tmpF << " " << "trazimo: " << targetF << endl;
			//cout << n << endl;

			//adaBoostTrain(P,N,n,kaskada);  // trenira level kaskade s n featureova
			
			kaskada.cascade[i] = AdaBoost::startTraining(P,N,Feature::generatedFeatures,n);
			kaskada.levelThreshold.push_back(MAX); /////////////////////////10e6
									
			/*
			 * Decrease threshold for the i-th classifier until the current cascaded classifier
			 * has a detection rate of at least d × lastD (this also affects tmpF ):
			 */
			
			decraseThreshold(i,d*lastD,kaskada);	

			// Evaluate current cascaded classifier on validation set to determine tmpF and tmpD:
			tmpRet=evaluateOnTest(kaskada);
			
			tmpF=tmpRet.first;
			tmpD=tmpRet.second;					

			debug(tmpF);
			debug(tmpD);
		}
		
		N.clear();
		lastD = tmpD;		
		lastF = tmpF;
		/*
		 * If tempF > targetF then evaluate the current cascaded detector on the set of non-face images
		 * and put any false detections into the set N:
		 */
		if(tmpF>targetF) evaluateOnTrainNegative(N,kaskada);
		cout << "Trenutno imam: " << negativeTest.size() << " negativnih primjera.";
	
		spremiPodatke(kaskada, i, lastD, lastF); // spremanje kaskade zbog mogucnosti nastavka treniranja u slucaju greske na racunalu
	}
	clearTempData(); //brise file-ove nastale privremenim spremanjem kaskade na disk zbog mogucnosti nastavka procesa ucenja
}

void ViolaJones::clearTempData() {
	system("del podaci.temp");
}

void ViolaJones::spremiPodatke(Cascade kaskada, int &i, double &lastD, double &lastF) {
	kaskada.saveCascade("temp.cascade");
	FILE *fout = fopen("podaci.temp", "w");
		fprintf(fout, "%lf %lf %d", lastD, lastF, i );
	fclose(fout);
}

void ViolaJones::recoverFromError(int &i, double &lastD, double &lastF, vector< Image* > &N) {
	Cascade kaskada( ColorSpace::RGB );	//samo default vrijednost, prilikom ucitavanja se ucita prava vrijednost
	FILE *fin = fopen("podaci.temp", "r");
		if (fin == NULL) return;
		cout << "RECOVERING FROM ERROR!!!" << endl;
		fscanf(fin, "%lf %lf %d", &lastD, &lastF, &i);
	fclose(fin);	
	kaskada.loadCascade("temp.cascade");
	N.clear();
	evaluateOnTrainNegative( N, kaskada);
}

// Evaluate current cascaded classifier on validation set to determine tmpF and tmpD
pair<double,double> ViolaJones::evaluateOnTest(Cascade &kaskada) { 
	int corrP=0, errN=0;  // broj gresaka na P i na N
	
	for(int i=0;i<positiveTest.size();i++)
		if(evaluate(positiveTest[i],kaskada)) corrP++;
	
	for(int i=0;i<negativeTest.size();i++)
		if(evaluate(negativeTest[i],kaskada)) errN++;
	
	debug( positiveTest.size() );
	debug( negativeTest.size() );
	debug( corrP );
	debug( errN );
	//system("pause");
	
	return make_pair(errN/(double)(negativeTest.size()+clearedNegativeTestSize),corrP/(double)positiveTest.size());//?????????????
}

// za zadanu integralnu sliku vraca da li je svrstana pozitivno ili negativno
bool ViolaJones::evaluate(Image *iim, Cascade &kaskada) {
	double sum=0;

	for(int k,j,i=0;i<kaskada.cascade.size();i++) {//////////////////////////////-1?
		sum=0;  // suma trenutno obradivanog nivoa kaskade
	
		for(j=0;j<kaskada.cascade[i].size();j++) {
			sum+=iim->evaluateTrainedFeature(kaskada.cascade[i][j]);
		}

		//cout << iim << " " << sum << " " << kaskada.levelThreshold[i] << endl;		
		
		if(sum < kaskada.levelThreshold[i]) return false;  // nije proso i-ti level kaskade
	}
	return true;
}

/*
 * evaluate the current cascaded detector on the set of non-face images
 * and put any false detections into the set N
 */
void ViolaJones::evaluateOnTrainNegative(vector<Image*> &N, Cascade &kaskada) {
	for(int i=0;i<negativeTest.size();i++) {
		if(evaluate(negativeTest[i],kaskada)) N.push_back(negativeTest[i]);
		else {
			delete negativeTest[i];
			clearedNegativeTestSize ++;
		}
	}

	negativeTest = N;
	if (negativeTest.size() < minNumber) {			
		vector < Image* > tmp = this->negativeTestLoader.loadNextImages();

		cout << "Ucitao sam nove negativne primjere, njih: " << tmp.size() << endl;
		if (tmp.size() == 0) return;

		for(int i=0; i<tmp.size(); i++)
			negativeTest.push_back( tmp[i] );
	
		N.clear();
		evaluateOnTrainNegative( N, kaskada );
    }
}

/*
 * Decrease threshold for the ith classifier until the current cascaded classifier
 * has a detection rate of at least minD (this also affects tmpF )
 *
 * koristi binary search za pronalazak thresholda
 */
void ViolaJones::decraseThreshold(int ith, double minD, Cascade &kaskada) {
	int corrP;  // broj tocnih na P
	double tmpD;  // detection rade s trenutnim thresholdom
	
	
	// varijable za trazenje thersholda
	double mid,up=kaskada.cascade[ith].size() * 10, down = 0;/////////////////////////////100100
	// up je 10^5 sto je dovoljno za 250 20x20 featureova po levelu kaskade
	// TODO: granicu up odabrati na pametniji nacin
	
	debug( ith ); debug( minD );
	while(up - down > 1e-3) {
		mid=kaskada.levelThreshold[ith]=(down+up)/2;		
		
		corrP=0;
		for(int i=0;i<positiveTest.size();i++)
			if(evaluate(positiveTest[i],kaskada)) corrP++;

		//tmpD=errP/(double)negativeTest.size(); >??????????????????????? ili positiveTest
		
		tmpD=corrP/(double)positiveTest.size(); 				

		if(tmpD<minD+1e-9) up=mid;
		else down=mid;

	}
	mid=kaskada.levelThreshold[ith] = down;
	debug(mid);
	debug(tmpD);
}
