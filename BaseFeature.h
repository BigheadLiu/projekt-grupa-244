#pragma once

#include <utility>
#include <vector>
using namespace std;

/*
 * Klasa koja definira osnovni feature i specificira neke operacije nad njim
 *
 * @author: Ivan Kusalic
 */
class BaseFeature
{
public:
	BaseFeature(void);
	~BaseFeature(void);

	BaseFeature(int w,int h,vector<pair<int,int> > zb,vector<pair<int,int> > od);  // konstruktor za inicijalizaciju
	
	int width;  // osnovna sirina
	int height;  // osnovna visina
	int rb;  // redni broj ovog BaseFeaturea

	vector<pair<int,int> > add;  // koje (relativne) kordinate treba zbrojiti (iz integralne slike)
	vector<pair<int,int> > subtract;  // koje (relativne) kordinate treba oduzeti (iz integralne slike)

	static vector<BaseFeature> allBaseFeatures;  // skup svih osnovnih featureova

	static void loadBaseFeatures(string file);  // dohvaca osnovne featureove iz filea
};
