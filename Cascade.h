#pragma once

#include "Feature.h"

#include <utility>
#include <vector>
using namespace std;

/*
 * Klasa koja definira kaskadu i specificira neke operacije nad njom
 *
 * @author: Ivan Kusalic
 */
class Cascade
{
public:
	Cascade(void);
	~Cascade(void);

	/*
	 * sama kaskada
	 *
	 * vanjski vektor predstavlja nivo kaskade
	 * unutarnji vektor sadrzi featurove pojedinog nivoa
	 */
	vector<vector<Feature> > cascade;

	vector<double> levelThreshold;  // thresholdi za pojedine levele kaskade

	/*
	 * snima kaskadu u file
	 *
	 * ujedno snima i sve osnovne feature u isti file na pocetak
	 * tako da je moguce pozvati BaseFeature.loadBaseFeatures() nad istim fileom
	 */
	void saveCascade(string file);

	/*
	 * loada kaskadu iz filea
	 *
	 * usput loada i osnovne featureove
	 */
	void loadCascade(string file);
};
