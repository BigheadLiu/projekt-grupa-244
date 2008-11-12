#pragma once

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
public:
	AdaBoost(void);
	~AdaBoost(void);

	static vector<BaseFeature> startTraining(vector<Image>&positive, vector<Image>&negative, vector< BaseFeature > &features, int T);
};
