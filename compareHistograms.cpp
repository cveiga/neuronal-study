#include "head.h"

#define NCLUSTERS 1000


std::string leerHistograma(int*, std::string &, std::ifstream &);
void compararHistogramas(int *, int *, long &, std::string &, std::string);
bool seguridad(int argc, char* argv[], std::ifstream &fichTest)
{
	if (argc < 3) 
	{
		std::cerr << "Faltan parametros de ejecución" << std::endl;
		return false;
	}

	fichTest.open(argv[argc-1]);
	if ( !fichTest.is_open() )
	{
		std::cerr << "No se ha podido abrir el fichero de imágenes de Test" << std::endl;
		return false;
	}

	return true;
}



int main(int argc, char *argv[])
{
	std::ifstream fichTest, fichTraining;

	if (!seguridad(argc, argv, fichTest)) return 0;

	int hisTraining[NCLUSTERS], hisTest[NCLUSTERS];
	std::string imageTest, imageTraining;

	long posicionTest = 0;
	char basura;
	while( !fichTest.eof() )
	{
		long totalCoincidencias = 0;
		std::string pertenece;

		if (posicionTest != 0) fichTest.seekg(posicionTest, fichTest.beg);
		std::string tipoTest = leerHistograma(hisTest, imageTest, fichTest);

		fichTraining.open(argv[argc-2]);
		if( !fichTraining.is_open() )
		{
			std::cerr << "No se ha podido abrir el fichero de imágenes de Training" << std::endl;
			return false;
		}
		std::cout << "TEST: " << tipoTest << std::endl;
		long posicionTraining = 0;
		while( !fichTraining.eof() )
		{
			if (posicionTraining != 0) fichTraining.seekg(posicionTraining, fichTraining.beg);
			std::string tipoTraining = leerHistograma(hisTraining, imageTraining, fichTraining);
			std::cout << "\tTRAINING: " <<  tipoTraining << std::endl;
		
			compararHistogramas(hisTest, hisTraining, totalCoincidencias, pertenece, tipoTraining);

			posicionTraining = fichTraining.tellg();
			fichTraining >> basura;
		}
		fichTraining.close();

		posicionTest = fichTest.tellg();
		fichTest >> basura;

		std::cout << "El Test: " << tipoTest << ", y el resultado es de Tipo: " << pertenece << std::endl;
	}
	fichTest.close();

	return 1;
}

/**
* Lee un histograma
* @parametro miHistograma, vector de NCLUSTERS donde almacen los datos del histograma
* @parametro img, nombre de la imagen a la que pertenece el histograma
* @parametro ifs, fichero donde se encuentra almacenado el histograma
* @devuelve tipo, tipo de imagen a la que pertenece el histograma leido
*/
std::string leerHistograma(int *miHistograma, std::string &img, std::ifstream &ifs)
{
	std::string tipo;
	//Leer el histograma
	{		
	    // Se abre un archivo de lectura
	    boost::archive::text_iarchive ar(ifs);
	    // Escribe el nombre de la imagen, el tipo al que pertenece y su histograma
	    ar & img & tipo;
	    for (int i = 0; i < NCLUSTERS; i++) { ar & miHistograma[i]; }
	}
	return tipo;
}

/**
* Compara dos histogramas y devuelve el número de coincidencias
* @parametro hisTest, histograma de una imagen de Test
* @parametro hisTraining, histograma de una imagen de Trainin
* @parametro totalCoincidencias, número total de coincidencias de los dos histogramas
* @parametro pertenece, tipo al que pertenece la imagen de Test una vez comparados los histogramas
* @parametro tipoTraining, tipo al que pertenece la imagen de Training
*/
void compararHistogramas(int *hisTest, int *hisTraining, long &totalCoincidencias, std::string &pertenece, std::string tipoTraining)
{
	long coincidencias = 0;

	for (int i = 0; i < NCLUSTERS; i++)
		coincidencias += hisTest[i] >= hisTraining[i] ? hisTraining[i] : hisTest[i];

	if(coincidencias >= totalCoincidencias)
	{
		totalCoincidencias = coincidencias;
		pertenece = tipoTraining;
	}
}