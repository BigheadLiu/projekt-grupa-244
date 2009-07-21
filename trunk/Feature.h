#pragma once 

#include <utility>
#include <vector>
using namespace std;

/*
 * Klasa koja definira feature i specificira neke operacije nad njim
 *
 * @author: Ivan Kusalic
 */


class Feature
{
private:
	static pair<int,int> Feature::calcScale(pair<int,int> tocka, float scaleX, float scaleY, int X, int Y);
public:
	Feature(void);
	~Feature(void);

	// konstruktor za inicijalizaciju:
	Feature(int w,int h,vector<pair<int,int> > zb,vector<pair<int,int> > od, int tX, int tY, float tScale, float tWeight, int channel);

	int width;  // osnovna sirina
	int height;  // osnovna visina

	vector<pair<int,int> > add;  // koje (relativne) kordinate treba zbrojiti (iz integralne slike)
	vector<pair<int,int> > subtract;  // koje (relativne) kordinate treba oduzeti (iz integralne slike)

	int x;  // x kordinata lijevog ruba
	int y;  // y kordinata gornjeg ruba

	float scale;  // scale factor naspram osnovne velicine featurea
	float weight;  // tezinska vrijednost ovog featurea u kaskadi	
	int usporedba; // vrijednost koja govori je li potrebno usporedivati val > trehold ili treshold > val, oznaka je p u clanku.
	int treshold;  // treshold za taj feature
	int channel; // kanal (odnosno boja) za koju se racuna taj feature

	static vector<Feature> allBaseFeatures;  // skup svih osnovnih featureova
	static vector<Feature> generatedFeatures; // skup svih generiranih feature-ova

	/*
	 * generira sve featureove za zadani prozor
	 *
	 * prozor je odreden svojim polozajem u slici (gornji, lijevi vrh => x, y) i sirinom i visinom
	 * step odreduje pomak za koliko se pixela pomice feature osnovne velicine
	 * scaleFactor odreduje kakav je odnos izmedu dvije susjedne velicine featurea
	 * numberOfChannels na koliko kanala (odnosno boja) ce se generirati featuri
	 *
	 * veci feature se pomice za isti iznos koliko i feature osnovne velicine
	 */
	static vector<Feature>& generateAll(int width, int height, int step, float scaleFactor, const vector<int>& channels);
	static void loadBaseFeatures(string file);  // dohvaca osnovne featureove iz filea
};
