#include "StdAfx.h"
#include <algorithm>
#include "BaseFeature.h"
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


vector<BaseFeature> AdaBoost::startTraining(vector<Image>&positive, vector<Image>&negative, vector< BaseFeature > &features, int T) {
	vector< float > weightPositive( positive.size() ), weightNegative( negative.size() );
	vector <BaseFeature> rjesenje;

	//pocetna inicijalizacija tezina
	for(int i=0; i<positive.size(); i++) 
		weightPositive.push_back( 0.5 / positive.size() );
	for(int i=0; i<negative.size(); i++)
		weightNegative.push_back( 0.5 / negative.size() );

	//potrebno je za svaku sliku na pocetku evaluirati svaki feature
	//te zatim sortirati prema vrijednostima featura
	vector < vector < triple > > featureValue;

	for(int i=0; i<features.size(); i++) {
		vector<  triple > tmp;
		for(int j=0; j<positive.size(); j++)  {
			int val = positive[j].evaluateBaseFeature( features[i] );
			tmp.push_back( triple(j, val, true) );
		}

		for(int j=0; j<negative.size(); j++) {
			int val = negative[j].evaluateBaseFeature( features[i] );
			tmp.push_back( triple(j, val, true) );
		}

		featureValue.push_back( tmp );
		sort( featureValue[i].begin(), featureValue[i].end() );
	}


	for(int i=0; i<T; i++) {
		normalizeWeights( weightPositive, weightNegative );

		int bestFeature = -1, p = 0;
		float error = 1e9, treshold = 0.;
		//select best weak classifier

		for(int j=0; j<features.size(); j++) {
			//odredi <T+, T->
			pair< float, float > total = sumFeatureValue( featureValue[j] );
			pair< float, float > curr(0., 0. );

			for(int k=0; k<featureValue[j].size(); j++) {
				int val1 = curr.first + (total.second - curr.second );
				int val2 = curr.second + (total.first - curr.first);

				float curErr = min( val1, val2 );

				if (featureValue[j][k].positive == true)  curr.first += featureValue[j][k].value;
				else									 curr.second += featureValue[j][k].value;
				
				if (curErr < error) {
					if (j == 0) treshold = featureValue[j][k].value;
					else treshold = (featureValue[j - 1][k].value + featureValue[j][k].value) / 2;

					if (val1 < val2) p = -1;
					else p = 1;

					error = curErr;
					bestFeature = j;
				}
			}

		}

		//update the weights
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

		rjesenje.push_back( features[i] );
		rjesenje.back().weight = alpha;		
	}
}

void AdaBoost::normalizeWeights( vector< float > &weightPositive, vector<float> &weightNegative) {
	float sum = 0.;
	for(int i=0; i<weightPositive.size(); i++)
		sum += weightPositive[i];
	for(int i=0; i<weightNegative.size(); i++)
		sum += weightNegative[i];

	for(int i=0; i<weightPositive.size(); i++)
		weightPositive[i] /= sum;
	for(int i=0; i<weightNegative.size(); i++)
		weightNegative[i] /= sum;
}

pair<float, float> AdaBoost::sumFeatureValue( vector< triple > & featureValue) {
	float sumP = 0., sumN = 0.;

	for(int i=0; i<featureValue.size(); i++) {
		if (featureValue[i].positive == true) sumP += featureValue[i].value;
		else								  sumN += featureValue[i].value;
	}

	return make_pair( sumP, sumN );
}
