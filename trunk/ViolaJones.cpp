#include "ViolaJones.h"


ViolaJones::ViolaJones(void)
{
}

ViolaJones::~ViolaJones(void)
{
}

ViolaJones::ViolaJones(vector<Image> ptr, vector<Image>ntr, vector<Image>pte, vector<Image> nte)
{
	positiveTrain=ptr;
	negativeTrain=ntr;
	positiveTest=pte;
	negativeTest=nte;
}

/*
 * f - the maximum acceptable false positive rate per layer
 * d - the minimum acceptable detection rate per layer
 * targetF - target overall false positive rate
 * kaskada - kaskada koja se gradi
 */
void ViolaJones::buildCascade(double f,double d, double targetF, Cascade &kaskada) {
	//P - set of positive examples; N - set of negative examples:
	list<Image> P(positiveTrain.begin(),positiveTrain.end());
	list<Image> N(negativeTrain.begin(),negativeTrain.end());
	
	double tmpF=1.0;  // trenutni false positive rate
	double lastF=1.0;  // false positive rate prethodnog nivoa
	double tmpD=1.0;  // trenutni detaction rate
	double lastD=1.0;  // detection rate prethodnog nivoa
	
	int i=0;  // i - level kaskade
	int n;  // broj featureova u trenutnom levelu kaskade
	
	pair<double,double> tmpRet;  // pomocna varijabla
	
	while(tmpF>targetF) {
		i++;
		n=0;
		tmpF=lastF;
		
		while(tmpF>f*lastF) {
			n++;

			//adaBoostTrain(P,N,n,kaskada);  // trenira level kaskade s n featureova
			AdaBoost::startTraining(P,N,kaskada[i],n)
			
			// Evaluate current cascaded classifier on validation set to determine tmpF and tmpD:
			tmpRet=evaluateOnTest(kaskada);
			tmpF=tmpRet.first;
			tmpD=tmpRet.second;
			
			/*
			 * Decrease threshold for the i-th classifier until the current cascaded classifier
			 * has a detection rate of at least d × lastD (this also affects tmpF ):
			 */
			decraseThreshold(i,d*lastD,kaskada);
		}
		
		N.clear();
		
		/*
		 * If tempF > targetF then evaluate the current cascaded detector on the set of non-face images
		 * and put any false detections into the set N:
		 */
		if(tmpF>targetF) evaluateOnTrainNegative(N,kaskada);
	}
}

// Evaluate current cascaded classifier on validation set to determine tmpF and tmpD
pair<double,double ViolaJones::evaluateOnTest(Cascade kaskada) {
	int errP=0, errN=0;  // broj gresaka na P i na N
	
	for(int i=0;i<positiveTest.size();i++)
		if(!evaluate(positiveTest[i],kaskada)) errP++;
	
	for(int i=0;i<negativeTest.size();i++)
		if(evaluate(negativeTest[i],kaskada)) errN++;
	
	
	return make_pair(errN/(double)positiveTest.size(),errP/(double)negativeTest.size());
}

// za zadanu integralnu sliku vraca da li je svrstana pozitivno ili negativno
bool ViolaJones::evaluate(Image iim, Cascade kaskada) {
	double sum=0;
	for(int k,j,i=0;i<kaskada.cascade.size();i++) {
		sum=0;  // suma trenutno obradivanog nivoa kaskade
		
		
		for(j=0;j<kaskada.cascade[i].size();j++) {
			sum+=imm.evaluateBaseFeature(kaskada.cascade[i][j])*kaskada.cascade[i][j].weight;
						
//			for(k=0;k<kaskada.cascade[i][j].add.size();k++)
//				sum+=kaskada.cascade[i][j].weight*
//					iim.get(kaskada.cascade[i][j].add[k].first,kaskada.cascade[i][j].add[k].second);
			
//			for(k=0;k<kaskada.cascade[i][j].subtract.size();k++)
//				sum-=kaskada.cascade[i][j].weight*
//					iim.get(kaskada.cascade[i][j].subtract[k].first,kaskada.cascade[i][j].subtract[k].second);
		}
		
		if(sum<kaskada.levelThreshold[i]) return false;  // nije proso i-ti level kaskade
	}
	
	return true;
}

/*
 * evaluate the current cascaded detector on the set of non-face images
 * and put any false detections into the set N
 */
void ViolaJones::evaluateOnTrainNegative(list<Image> &N, Cascade kaskada) {
	for(int i=0;i<negativeTrain.size();i++)
		if(evaluate(negativeTrain[i],kaskada)) N.push_back(negativeTrain[i]);
}

/*
 * Decrease threshold for the ith classifier until the current cascaded classifier
 * has a detection rate of at least minD (this also affects tmpF )
 *
 * koristi binary search za pronalazak thresholda
 */
void ViolaJones::decraseThreshold(int ith, double minD, Cascade kaskada) {
	int errP;  // broj gresaka na P
	double tmpD;  // detection rade s trenutnim thresholdom
	
	int down=0,mid,up=100100;  // varijable za trazenje thersholda
	// up je 10^5 sto je dovoljno za 250 20x20 featureova po levelu kaskade
	// TODO: granicu up odabrati na pametniji nacin
	
	while(down<=up) {
		mid=kaskada.levelThreshold[ith]=(down+up)/2;
		
		errP=0;
		for(int i=0;i<positiveTest.size();i++)
			if(!evaluate(positiveTest[i],kaskada)) errP++;
		tmpD=errP/(double)negativeTest.size();
		
		if(tmpD<minD+1e-9) up=mid-1;
		else down=mid;
	}
}
