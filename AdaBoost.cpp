#include "StdAfx.h"
#include <iostream>
#include <algorithm>
#include "Feature.h"
#include "Image.h"
#include <vector>
#include "AdaBoost.h"
using namespace std;

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

#define debug(x) cout << x << endl;
#define debug2(x) cout << #x << ": " << x << endl;

void ispisi(vector <float> ispis) {
	for(int i=0; i<ispis.size(); i++) cout << ispis[i] << " ";
	cout << endl;	
}

vector<Feature> AdaBoost::startTraining(vector<Image*>&positive, vector<Image*>&negative, vector< Feature > &features, int T) {
	vector< float > weightPositive, weightNegative;
	vector <Feature> rjesenje;

	debug("ZAPOCINJE ADABOOST TRAINING" );

	//pocetna inicijalizacija tezina	
	for(int i=0; i<positive.size(); i++) 
		weightPositive.push_back( 0.5 / positive.size() );	
	for(int i=0; i<negative.size(); i++)
		weightNegative.push_back( 0.5 / negative.size() );

	debug("POSTAVLJAM DEFAULT TEZINA" );
	//potrebno je za svaku sliku na pocetku evaluirati svaki feature
	//te zatim sortirati prema vrijednostima featura
	vector < vector < triple > > featureValue;

	for(int i=0; i<features.size(); i++) {
		vector<  triple > tmp;
		for(int j=0; j<positive.size(); j++)  {
			int val = positive[j]->evaluateBaseFeature( features[i] );
//			int val = (rand() % 100)* 20 + 1000;			
			tmp.push_back( triple(j, val, true) );
		}

		for(int j=0; j<negative.size(); j++) {
			int val = negative[j]->evaluateBaseFeature( features[i] );
//			int val = (rand() % 100) * 20;
			tmp.push_back( triple(j, val, false) );
		}

		featureValue.push_back( tmp );
		sort( featureValue[i].begin(), featureValue[i].end() );
	}

	debug("EVALUIRAM FEATURE NA SLIKAMA" );

	for(int i=0; i<T; i++) {		
		normalizeWeights( weightPositive, weightNegative );

		int bestFeature = -1, p = 0;
		float error = 1e9, treshold = 0.;
		//select best weak classifier

		for(int j=0; j<features.size(); j++) {
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

				
				if (curErr < error) {
					if (k == 0) treshold = featureValue[j][k].value;
					else treshold = (featureValue[j][k-1].value + featureValue[j][k].value) / 2;

					if (val1 < val2) p = -1;
					else p = 1;

					error = (float)curErr;
					bestFeature = j;
				}
			}
		}
		
		//update the weights
		cout << "Current error at this step: ";
		debug2( error );
		debug2(treshold);
		float beta = error / (1 - error);	
		float alpha = log( 1 / beta );		
		system("pause");

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
	}

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

float AdaBoost::sumWeight( vector< float > weight) {
	float rj = 0;
	for(int i=0; i<weight.size(); i++) rj += weight[i];
	return rj;
}

/*
pair<float, float> AdaBoost::sumFeatureWeight( vector< triple > & featureValue) {
	float sumP = 0., sumN = 0.;

	for(int i=0; i<featureValue.size(); i++) {		
		if (featureValue[i].positive == true) sumP += featureValue[i].value;
		else								  sumN += featureValue[i].value;
	}

	return make_pair( sumP, sumN );
}*/
