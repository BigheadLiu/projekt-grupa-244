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
	this->kolikoUkupno = koliko;

	int numblocks = (koliko + (1<<blocksize)-1) / (1<<blocksize);
	blocksInMemory = min( blocksInMemory, numblocks ); //ne treba u memoriji drzat vise od maksimalnog broja blokova
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
	//cout << "Unistavam BigVector...";
	for(int i=0; i<blocksInMemory; i++) {
		//cout << "Brisem... " << i << " " << niz[i] << endl;
		cout.flush();
		delete[] niz[i];
	}
	//cout << "Brisem cijeli: " << endl; cout.flush();
	delete[] niz;

	fclose(f);
	//cout << "Gotovo" << endl;
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
int BigVector<T>::pickPlace() {
	static int mjesto = 0;
    int pos = mjesto;
	mjesto = (mjesto + 1) % blocksInMemory; //makni se na sljedeci blok za mjesto load-anja

	return pos;
}

template< typename T >  
void BigVector<T>::load(int koji, int mjesto) {
	fseek( f,koji * sizeof(T) * (1<<blocksize), SEEK_SET );
	fread( (void *)niz[mjesto], sizeof(T), (1<<blocksize), f );
}

template< typename T >
int BigVector<T>::loadBlock(int koji) {	
	int mjesto = pickPlace();
	writeBlock( mjesto, sto[mjesto] );	
	gdje[koji] = mjesto;
	sto[mjesto] = koji;

	if (gdje[koji] != -2) {
		load( koji, mjesto );
	} 

	return mjesto;
}

template< typename T >
void BigVector<T>::write(int koji, int mjesto) {
	fseek( f,koji * sizeof(T) * (1<< blocksize), SEEK_SET );
	fwrite( (void *)niz[mjesto], sizeof(T), (1<<blocksize), f );
}

template< typename T >
void BigVector<T>::writeBlock(int mjesto, int kojiBlok) {
	gdje[ kojiBlok ] = -1;
	write( kojiBlok, mjesto );
}


template< typename T >
T& BigVector<T>::back() {
	assert( velicina > 0 );
	return (*this)[velicina-1];
}

template< typename T >
void BigVector<T>::push_back(const T& value) {	
	velicina++;
	assert( velicina <= kolikoUkupno );
	(*this)[velicina-1] = value;
}

template< typename T >
void BigVector<T>::reserve(int koliko) {
	//niz.reserve( koliko );
}


template BigVector<int>;