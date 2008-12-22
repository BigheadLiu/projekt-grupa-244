#pragma once

#include "StdAfx.h"
#include <algorithm>
#include "BaseFeature.h"
#include "Feature.h"
#include "Image.h"
#include <vector>
#include "AdaBoost.h"
using namespace std;

class AdaBoost
{
private:
	struct triple {
		int value;
		int index;
		bool positive;

		triple( int index, int value, bool positive) {
			this->value = value; this->index = index; this->positive=positive;
		}
		friend bool operator < (const triple &a, const triple &b) {
			return a.value < b.value;
		}
	};

	static pair<float, float> sumFeatureValue( vector< triple > & featureValue);
	static void normalizeWeights( vector< float > &weightPositive, vector<float> &weightNegative);
	static float sumWeight( vector< float > weight);

public:
	AdaBoost(void);
	~AdaBoost(void);

	/**
	* Glavna metoda klase AdaBoost, vraca vector<Feature> sa izracunatim tezinama i treshold-om prema AdaBoost algoritmu
	* @arguments
	*	vector<Image*> positive ... primjerci slika znakova
	*	vector<Image*> negative ... primjerci slika ne-znakova
	*	vector<Feature> &features . vector Feature iz kojeg ce algoritam izabrati najbolje moguce Feature-e za klasifikaciju
	*	T ... broj featura koje algoritam treba vratiti
	*/
	static vector<Feature> startTraining(vector<Image*>&positive, vector<Image*>&negative, vector< Feature > &features, int T);
};
