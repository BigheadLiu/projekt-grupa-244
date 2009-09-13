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
protected:
	vector< int > gdje;
	vector< int > sto;
	T** niz;
	FILE *f;

	int blocksize;
	int numblocks, blocksInMemory;
	int velicina; //koliko ih trenutno ima u vektoru
	int kolikoUkupno; //maksimalan broj koliko ih moze biti
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

	void push_back(const T& value);
	T& operator[] (int index);

	int pickPlace(); //odabire mjesto gdje ce biti spremljen sljedeci load-ani blok

private:
	int loadBlock(int index);
	void writeBlock(int mjesto, int kojiBlok);

	virtual void load(int koji, int mjesto); //bas cisto citanje iz file-a
	virtual void write(int koji, int mjesto); //bas cisto pisanje u file
};
