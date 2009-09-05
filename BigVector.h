#pragma once

#include <vector>
#include <cstdio>
using namespace std;

/*
* Container koji sluzi kao vektor koji visak elemenata (sto ne stane u memoriju)
* sprema na disk.
*/

template< typename T> 
class BigVector
{
private:
	vector< int > gdje;
	vector< int > sto;
	T** niz;
	FILE *f;

	int blocksize;
	int numblocks, blocksInMemory;
	int velicina;
public:
	/*
	* Konstruktor klase BigVector. 
	* koliko ... broj mjesta koje treba alocirati. Prilikom koristenja nije dopusteno indeks-om preci dani broj
	* blocksize ... 2 ^ blocksize je broj lokacija koji cine jedan memorijski blok. Blok se ukoliko nema dovoljno mjesta sprema na disk
	* blocksInMemory ... najveci broj blokova koji se u istom trenutku mogu naci u memoriji.
	*/
	BigVector(int koliko = 0, int blocksize = 9, int blocksInMemory = 70);
	~BigVector(void);

	int size();
	void reserve(int koliko);

	T& back();

	void push_back(T value);
	T& operator[] (int index);

private:
	int loadBlock(int index);
	void writeBlock(int mjesto, int kojiBlok);
};
