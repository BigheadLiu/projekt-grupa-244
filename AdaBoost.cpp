#include "StdAfx.h"
#include <iostream>
#include <algorithm>
#include "Feature.h"
#include "Image.h"
#include <vector>
#include "AdaBoost.h"
#include <climits>
#include "BigVector.h"
#include "BigVector.cpp"
using namespace std;

#define INF INT_MAX / 2
AdaBoost::AdaBoost(void)
{
}

AdaBoost::~AdaBoost(void)
{
}

/**
* glavna metoda klase AdaBoost koja obavlja treniranje jednog jakog klasifikatora
* @arguments: 
*		vector<Image> positive - referenca na vektor koji sadrze sve primjerke za ucenje znakova;
*		vector<Image> negative - referenca na vektor koji sadrzi sve primjerke za ucenje neznakova
*		vector<BaseFeature> features - referenca na vektor koji sadrzi feature-e koje metoda pokusava nauciti
*		int T - broj featura koji se ocekuje kao rezultat
* @return:
*		vraca T naucenih featura sa pripadnim tezinama
*/

#ifndef NODEBUG
	#define debug(x) cout << x << endl;
	#define debug2(x) cout << #x << ": " << x << endl;
#else
	#define debug(x) ;
	#define debug2(x) ;
#endif


void ispisi(vector <float> ispis) {
	for(int i=0; i<ispis.size(); i++) cout << ispis[i] << " ";
	cout << endl;	
}

vector<Feature> AdaBoost::startTraining( vector<Image*>&positive, vector<Image*> &negative, vector< Feature > &features, int T) {
	return train( positive, negative, features, T );
	
	int dio = features.size() / 3;
	vector < Feature > najbolji;
	vector < Feature > tren;

	for(int i=0; i<features.size(); i++) {
		tren.push_back( features[i] );

		if (tren.size() >= dio || i == features.size() - 1) { //nadi najbolji feature po dijelovima			
			int koliko  = tren.size() / dio;
			if (koliko < T) koliko = T;
			if (koliko > tren.size() ) koliko = tren.size();

			vector < Feature > tmp = train( positive, negative, tren, koliko);				

			najbolji.insert( najbolji.end(), tmp.begin(), tmp.end() );
			tren.clear();
		}
	}

	cout << "NAJBOLJI FEATURI UKUPNO";
	return train( positive, negative, najbolji, T ); // od najboljih featura po dijelovima, nadi najbolje ukupno
}

vector<Feature> AdaBoost::train(vector<Image*>&positive, vector<Image*>&negative, vector< Feature > &features, int T) {
	int brNegative = min( positive.size(), negative.size() );

	vector< float > weightPositive, weightNegative;
	//vector < Feature > dobriFeaturi;
	vector< bool > validFeature;
	vector <Feature> rjesenje;

	debug2( positive.size() );
	debug2( negative.size() );
	debug2( features.size() );
	debug2(T);

	debug("POSTAVLJAM DEFAULT TEZINA" );
	//pocetna inicijalizacija tezina	
	for(int i=0; i<positive.size(); i++) 
		weightPositive.push_back( 0.5 / positive.size() );	
	for(int i=0; i<brNegative; i++)
		weightNegative.push_back( 0.5 / brNegative );

	//potrebno je za svaku sliku na pocetku evaluirati svaki feature
	//te zatim sortirati prema vrijednostima featura
	//vector < vector < triple > > featureValue( features.size() );
	BigVector< vector<triple> > featureValue( features.size() );			

	debug("EVALUIRAM FEATURE NA SLIKAMA" );
	for(int i=0; i<features.size(); i++) {
		vector<  triple > tmp;		
		
		tmp.reserve( positive.size() + brNegative );
		bool broken = false;

		for(int j=0; j<positive.size(); j++)  {
			int val = positive[j]->evaluateBaseFeature( features[i] );
			if (val == -INF) broken = true; //izlaz ako feature nije dobar

			tmp.push_back( triple(j, val, true) );
		}		

		for(int j=0; j<brNegative; j++) {
			int val = negative[j]->evaluateBaseFeature( features[i] );
			if (val == -INF) broken = true; //izlaz ako feature nije ispravan

			tmp.push_back( triple(j, val, false) );
		}

		if ( broken == true ) { //ako feature nije ispravan sve vrijednosti postavi na random, to ce ka iskljucit iz odabira			
			for(int i=0; i<tmp.size(); i++)
				tmp[i].value = rand() % 100;			
		}

		validFeature.push_back( !broken );
		featureValue.push_back( tmp );
		
		sort( featureValue.back().begin(), featureValue.back().end() );
	}

	debug("ZAPOCINJE ODABIR NAJBOLJIH FEATURA" );

	for(int i=0; i<T; i++) {		
		normalizeWeights( weightPositive, weightNegative );

		int bestFeature = -1, p = 0;
		float error = 1e9, treshold = 0.;
		//select best weak classifier

		for(int j=0; j<featureValue.size(); j++) {
			if (validFeature[j] == false) continue; //provjerit je li feature ispravan

			//odredi <T+, T->
			pair< float, float > total = make_pair( sumWeight( weightPositive ), sumWeight( weightNegative ));

			//ispisi( weightPositive );
			pair< float, float > curr(0., 0. );

			for(int k=0; k<featureValue[j].size(); k++) {		
				int index = featureValue[j][k].index;
				int value = featureValue[j][k].value;				

				float val1 = curr.first + (total.second - curr.second );
				float val2 = curr.second + (total.first - curr.first);

				float curErr = min( val1, val2 );

				if (featureValue[j][k].positive == true)  curr.first += weightPositive[ index ];
				else									  curr.second += weightNegative[ index ];

				if (k != 0 && featureValue[j][k-1].value == featureValue[j][k].value) continue; //sluzi da nije moguce postaviti granicu izmedu istih vrijednosti. Jako vazan uvjet!

				if (curErr < error) {
					if (k == 0) treshold = featureValue[j][k].value;
					else treshold = ( featureValue[j][k-1].value + featureValue[j][k].value) / 2.;

					if (val1 < val2) p = -1;
					else p = 1;

					error = (float)curErr;
					bestFeature = j;
				}
			}
		}
		
		//update the weights
		#ifndef NODEBUG
			cout << "Error: " << error << "(" << bestFeature << ")" << " | ";
		#endif

		if (error != 0) {
			float beta = error / (1 - error);	
			float alpha = log( 1 / beta );			

			for(int k=0; k<featureValue[ bestFeature ].size(); k++) {
				int val = featureValue[ bestFeature ][k].value;
				int index = featureValue[ bestFeature ][k].index;
				int correct = featureValue[ bestFeature ][k].positive;
			
				//if classified correctly, multiply by beta
				if (correct == ( p * val < p * treshold )) {
					if (correct == true) weightPositive[ index ] *= beta;
					else				 weightNegative[ index ] *= beta;
				}			
			}

			rjesenje.push_back( features[bestFeature] );		
			rjesenje.back().treshold = treshold;	
			rjesenje.back().weight = alpha;		
			rjesenje.back().usporedba = p;

		} else {
			rjesenje.push_back( features[bestFeature] );		
			rjesenje.back().treshold = treshold;	
			rjesenje.back().weight = 1e5;		
			rjesenje.back().usporedba = p;
			break;
		}
	}
	#ifndef NODEBUG
		cout << endl;
	#endif

	return rjesenje;
}

void AdaBoost::normalizeWeights( vector< float > &weightPositive, vector<float> &weightNegative) {
	float sum = 0.;
	for(int i=0; i<weightPositive.size(); i++) 
		sum += weightPositive[i];
	for(int i=0; i<weightNegative.size(); i++)
		sum += weightNegative[i];

	for(int i=0; i<weightPositive.size(); i++) {
		weightPositive[i] /= sum;		
	}
	for(int i=0; i<weightNegative.size(); i++) {
		weightNegative[i] /= sum;
	}
}

float AdaBoost::sumWeight( const vector< float > &weight) {
	float rj = 0;
	for(int i=0; i<weight.size(); i++) rj += weight[i];
	return rj;
}
