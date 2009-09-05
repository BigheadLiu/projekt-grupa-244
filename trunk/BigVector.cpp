#include "stdafx.h"
#include "BigVector.h"
#include "AdaBoost.h"
#include  <iostream>

template< typename T >
BigVector<T>::BigVector(int koliko, int blocksize, int blockInMemory)
{
	f = fopen("memory.dat", "w+b");

	this->blocksize = blocksize;
	this->blocksInMemory = blockInMemory;

	int numblocks = (koliko + (1<<blocksize)-1) / (1<<blocksize);
	gdje.resize( numblocks, 0 );
	sto.resize( blocksInMemory, 0 );

	for(int i=0; i<blocksInMemory; i++) {
		gdje[i] = i;
		sto[i] = i;
	}

	for(int i=blocksInMemory; i<numblocks; i++) 
		gdje[i] = -2;
	
	cout << "Stvaram BigVector: numblocks = " << numblocks << ", blocksInMemory = " << blocksInMemory << "...";
	niz = new T*[blocksInMemory];
	for(int i=0; i<blocksInMemory; i++)
		niz[i] = new T[ (1<<blocksize) ];

	velicina = 0;
	cout << "Gotovo" << endl;
}

template< typename T >
BigVector<T>::~BigVector(void)
{
	cout << "Unistavam BigVector...";
	for(int i=0; i<blocksInMemory; i++)
		delete[] niz[i];
	delete[] niz;

	fclose(f);
	cout << "Gotovo" << endl;
}

template< typename T >
int BigVector<T>::size() {
	return velicina;
}

template< typename T >
T& BigVector<T>::operator[] (int index) {
	assert( index < velicina );
	int koji = gdje[ index >> blocksize ];
	if (koji < 0) koji = loadBlock( index >> blocksize );

	return niz[koji][ index & ( (1<<blocksize)-1)];
}

template< typename T >
int BigVector<T>::loadBlock(int koji) {
	//treba return na koje mjesto sam ga load-a
	//stavi ga na prvo mjesto
	static int mjesto = 0;

	writeBlock( mjesto, sto[mjesto] );	
	gdje[koji] = mjesto;
	sto[mjesto] = koji;

	if (gdje[koji] != -2) {
		fseek( f,koji * sizeof(T) * (1<<blocksize), SEEK_SET );
		fread( (void *)niz[mjesto], sizeof(T), (1<<blocksize), f );
	} else {//nije jos alociran
		//mislim da nije potrebno.
		//for(int i=0; i<(1<<blocksize); i++)
		//	niz[mjesto][i].
		//fill(niz[mjesto], niz[mjesto] + (1<<blocksize), T() );
	}
	int pos = mjesto;
	mjesto = (mjesto + 1) % blocksInMemory; //makni se na sljedeci blok za mjesto load-anja
	return pos;
}

template< typename T >
void BigVector<T>::writeBlock(int mjesto, int kojiBlok) {
	gdje[ kojiBlok ] = -1;
	fseek( f,kojiBlok * sizeof(T) * (1<< blocksize), SEEK_SET );
	fwrite( (void *)niz[mjesto], sizeof(T), (1<<blocksize), f );
}


template< typename T >
T& BigVector<T>::back() {
	return (*this)[velicina-1];
}

template< typename T >
void BigVector<T>::push_back(T value) {
	velicina++;
	(*this)[velicina-1] = value;
}

template< typename T >
void BigVector<T>::reserve(int koliko) {
	//niz.reserve( koliko );
}


template BigVector<int>;