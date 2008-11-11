#pragma once

#include "BaseFeature.h"

#include <vector>
using namespace std;

/*
 * Klasa koja definira feature i specificira neke operacije nad njim
 *
 * @author: Ivan Kusalic
 */
class Feature
{
public:
	Feature(void);
	~Feature(void);

	Feature(int tBase, int tX, int tY, float tScale);  // konstruktor za inicijalizaciju
	
	int baseRb;  // index BaseFeaturea na kojem se temelji ovaj Feature u vektoru allBaseFeatures;
	int x;  // x kordinata lijevog ruba
	int y;  // y kordinata gornjeg ruba
	float scale;  // scale factor naspram BaseFeature
		
	/*
	 * generira sve featureove za zadani prozor
	 *
	 * prozor je odreden svojim polozajem u slici (gornji, lijevi vrh => x, y) i sirinom i visinom
	 * step odreduje pomak za koliko se pixela pomice feature osnovne velicine
	 * scaleFactor odreduje kakav je odnos izmedu dvije susjedne velicine featurea
	 *
	 * veci feature se pomice za isti iznos koliko i feature osnovne velicine
	 */
	static vector<Feature> generateAll(int x,int y, int width, int height, int step, float scaleFactor);
};
