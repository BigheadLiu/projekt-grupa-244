#include "StdAfx.h"
#include "BigVectorVector.h"

template< typename T >
BigVectorVector<T>::BigVectorVector(int koliko, int duljinaVektora, int blocksize, int blocksInMemory):BigVector<T>(koliko, blocksize, blocksInMemory)
{
	this->duljinaVektora = duljinaVektora;
}

template< typename T >
BigVectorVector<T>::~BigVectorVector(void)
{
}

template< typename T >
void BigVectorVector<T>::load(int koji, int mjesto) {
	//cout << koji << ", " << duljinaVektora << " " << sizeof( niz[mjesto][0][0] ) << " " << (1 << blocksize) << endl;

	fseek( f, koji * duljinaVektora * sizeof(niz[mjesto][0][0] ) * (1 << blocksize), SEEK_SET  );
	for(int i=0; i<(1<<blocksize); i++)
		fread( &niz[mjesto][i][0], sizeof( niz[mjesto][0][0] ), duljinaVektora, f );
}

template< typename T >
void BigVectorVector<T>::write(int koji, int mjesto) {
	
	fseek( f, koji * duljinaVektora * sizeof( niz[mjesto][0][0] ) * (1 << blocksize), SEEK_SET );
	for(int i=0; i<(1<<blocksize); i++) {
		fwrite( &niz[mjesto][i][0], sizeof( niz[mjesto][0][0] ), duljinaVektora, f );
		//fill( niz[mjesto][i].begin(), niz[mjesto][i].end(), 0 );
	}
	//load( koji, mjesto );
}
