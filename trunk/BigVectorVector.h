#pragma once
#include "bigvector.h"
#include <vector>
#include <cstdio>
using namespace std;

/*
* Klasa koja sluzi za apstrakciju vektora koji moze imati vrlo velik broj podataka.
* Klasa je prilagodena iskljuciva za istanciranje s: BigVectorVector< vector<T> >.
* Podaci 
*/
template< typename T >
class BigVectorVector :
	public BigVector< T >
{
private:
	int duljinaVektora;
public:
	BigVectorVector(int koliko, int duljinaVektora, int blocksize = 9, int blocksInMemory = 70);	
	~BigVectorVector(void);

	void load(int koji, int mjesto); //bas cisto citanje iz file-a
	void write(int koji, int mjesto); //bas cisto pisanje u file
};
