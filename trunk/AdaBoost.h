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

	static vector<Feature> startTraining(vector<Image*>&positive, vector<Image*>&negative, vector< Feature > &features, int T);
};
