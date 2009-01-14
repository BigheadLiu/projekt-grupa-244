#pragma once

#include "StdAfx.h"
#include <algorithm>
#include "BaseFeature.h"
#include "Feature.h"
#include "Image.h"
#include <vector>
#include "AdaBoost.h"
using namespace std;
/**
* Klasa koja obavlja algoritma AdaBoost prema clanku
*
*/

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
		triple() { value = 0; index = 0; positive= 0; }
	};

	static float sumWeight( vector< float > weight);
	static void AdaBoost::normalizeWeights( vector< float > &weightPositive, vector<float> &weightNegative);

	static vector<Feature> train(vector<Image*>&positive, vector<Image*>&negative, vector< Feature > &features, int T);
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
