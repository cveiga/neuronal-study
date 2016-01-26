#include "head.h"

#define NCLUSTERS 1000  //1000
#define KVECINOS 23 	//5 mínimo para propiciar la repetición

/** Estructura para los k-vecinos */
struct Vecinos
{
	double distancia;
	std::string tipoVecino;
};


std::string leerHistograma(int*, std::string &, std::ifstream &);
void guardaResultado(std::string, std::string, std::string, std::string);
void compararHistogramas(int *, int *, long &, std::string &, std::string);
double distanciaEuclidea(int *, int *, double &, std::string &, std::string);
void encuentraKVecinos(Vecinos *, int &, const std::string, const double);
std::string votaVecino(Vecinos *);
void imprimeKVecinos(Vecinos *);
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
	long aciertos = 0;
	Vecinos vecinosMasProximos[KVECINOS];

	if (!seguridad(argc, argv, fichTest)) return 0;

	int hisTraining[NCLUSTERS], hisTest[NCLUSTERS];
	std::string imageTest, imageTraining;

	long posicionTest = 0;
	char basura;
	while( !fichTest.eof() )
	{
		long totalCoincidencias = 0;
		double minimo = -1;
		int posicionVecino = 0;
		std::string pertenece, perteneceEuclidea, perteneceKVecinos;
		for (int i = 0; i < KVECINOS; i++) vecinosMasProximos[i].distancia = 0;

		if (posicionTest != 0) fichTest.seekg(posicionTest, fichTest.beg);
		std::string tipoTest = leerHistograma(hisTest, imageTest, fichTest);

		fichTraining.open(argv[argc-2]);
		if( !fichTraining.is_open() )
		{
			std::cerr << "No se ha podido abrir el fichero de imágenes de Training" << std::endl;
			return false;
		}
		//std::cout << "TEST: " << tipoTest << std::endl;
		std::cout << "Calculando tipo de la imagen: " << imageTest << std::endl;
		long posicionTraining = 0;
		while( !fichTraining.eof() )
		{
			if (posicionTraining != 0) fichTraining.seekg(posicionTraining, fichTraining.beg);
			std::string tipoTraining = leerHistograma(hisTraining, imageTraining, fichTraining);
			//std::cout << "\tTRAINING: " <<  tipoTraining << std::endl;
		
			//compararHistogramas(hisTest, hisTraining, totalCoincidencias, pertenece, tipoTraining);
			//distanciaEuclidea(hisTest, hisTraining, minimo, perteneceEuclidea, tipoTraining);
			encuentraKVecinos(vecinosMasProximos, posicionVecino, tipoTraining, distanciaEuclidea(hisTest, hisTraining, minimo, perteneceEuclidea, tipoTraining));
			//imprimeKVecinos(vecinosMasProximos);
			posicionTraining = fichTraining.tellg();
			fichTraining >> basura;
		}
		fichTraining.close();
		perteneceKVecinos = votaVecino(vecinosMasProximos);
		guardaResultado(imageTest.substr(imageTest.find_last_of("/"), imageTest.size()), pertenece, perteneceEuclidea, perteneceKVecinos);

		//if(tipoTest == pertenece) aciertos++;
		//if(tipoTest == perteneceEuclidea) aciertos++;
		if(tipoTest == perteneceKVecinos) aciertos++;

		posicionTest = fichTest.tellg();
		fichTest >> basura;

		//std::cout << "El Test: " << tipoTest << ", y el resultado por Coincidencias es: " << pertenece /*<< ", y por Proximidad es: " << perteneceEuclidea*/ << std::endl;
		std::cout << "El Test: " << tipoTest << ", y el resultado por Coincidencias es: " << perteneceKVecinos << std::endl;
	}
	fichTest.close();

	std::cout << "TOTAL DE ACIERTOS: " << aciertos << std::endl;

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


double distanciaEuclidea(int *hisTest, int *hisTraining, double &min, std::string &perteneceEuclidea, std::string tipoTraining)
{
	double resultado = 0;
	///////////////////////////////// DISTANCIA EUCLIDEA /////////////////////////////////////////////	
	for (int i = 0; i < NCLUSTERS; i++) resultado += pow(hisTest[i] - hisTraining[i], 2);			
	resultado = sqrt(resultado);
	//////////////////////////////////////////////////////////////////////////////////////////////////			
	if ( resultado <= min or min == -1)
	{
		min = resultado;
		perteneceEuclidea = tipoTraining;
	}
	return resultado;
}

void encuentraKVecinos(Vecinos *vVecinos, int &posVecinos, const std::string tTraining, const double res)
{
	if(posVecinos < KVECINOS)
	{
		vVecinos[posVecinos].distancia = res;
		vVecinos[posVecinos++].tipoVecino = tTraining;
	}
	else
	{
		for (int i = 0; i < KVECINOS; i++)
			if (vVecinos[i].distancia > res)
			{
				vVecinos[i].distancia = res;
				vVecinos[i].tipoVecino = tTraining;

				break;	//evitamos que modifique mas de un vecino
			}
	}
}

std::string votaVecino(Vecinos *vecinosMasProximos)
{
	Vecinos votacion[2];
	for (int i = 0; i < 2; i++) votacion[i].distancia = 0;
	/*votacion[0].tipoVecino = "SALINA.";
	votacion[1].tipoVecino = "NO EMPAREJADO";
	votacion[2].tipoVecino = "NO CONDICIONADO";
	votacion[3].tipoVecino = "CONDICIONADO.";*/
	votacion[0].tipoVecino = "C1";
	votacion[1].tipoVecino = "C2";

	for (int i = 0; i < KVECINOS; i++)
	{
		/*if(vecinosMasProximos[i].tipoVecino == "SALINA.") votacion[0].distancia++;
		if(vecinosMasProximos[i].tipoVecino == "NO EMPAREJADO") votacion[1].distancia++;
		if(vecinosMasProximos[i].tipoVecino == "NO CONDICIONADO") votacion[2].distancia++;
		if(vecinosMasProximos[i].tipoVecino == "CONDICIONADO.") votacion[3].distancia++;*/
		if(vecinosMasProximos[i].tipoVecino == "C1") votacion[0].distancia++;
		if(vecinosMasProximos[i].tipoVecino == "C2") votacion[1].distancia++;
	}

	bool iguales = false;
	int max = -1, posicion = 0;
	for (int i = 0; i < 2; i++)
	{
		if(votacion[i].distancia > max)
		{
			max = votacion[i].distancia;
			posicion = i;
			iguales = false;
		}
		else if(votacion[i].distancia == max) iguales = true;
	}
	std::cout << "VOTACIÓN VECINOS: " << std::endl;
	for (int i = 0; i < 2; i++) std::cout << "\t" << votacion[i].tipoVecino << " " << votacion[i].distancia << " votos" << std::endl;
	std::cout << "POSICION: " << posicion << std::endl;
	std::cout << "\t______________________________________________" << std::endl;

	if (iguales)
	{
		std::string tipoDefinitivo;
		int minDistanciaDefinitiva = -1;
		for (int i = posicion; i < 2; i++)
		{
			if(votacion[i].distancia == max)
			{
				for(int j = 0; j < KVECINOS; j++)
				{
					if ( (votacion[i].tipoVecino == vecinosMasProximos[j].tipoVecino) and (vecinosMasProximos[j].distancia < minDistanciaDefinitiva or minDistanciaDefinitiva == -1))
					{
						minDistanciaDefinitiva = vecinosMasProximos[j].distancia;
						tipoDefinitivo = vecinosMasProximos[j].tipoVecino;
					}
				}
			}
		}
		return tipoDefinitivo;
	}

	return votacion[posicion].tipoVecino;
}

void imprimeKVecinos(Vecinos *vecinosMasProximos)
{
	for (int i = 0; i < KVECINOS; ++i) std::cout << "\t Tipo: " << vecinosMasProximos[i].tipoVecino << ", y distancia = " << vecinosMasProximos[i].distancia << std::endl;
	std::cout << "\t______________________________________________" << std::endl;
}

void guardaResultado(std::string img, std::string tipoMayorCoincidencia, std::string tipoMasProximo, std::string tipoKVecino)
{
	//Salvar el histograma
	{		
	    // Se crea un archivo de salida
	    std::ofstream ofs("ResultadosExperimenSinNoEmparejado_VeintiTres_K-Vecinos.txt", std::ios::app);
	    boost::archive::text_oarchive ar(ofs);

	    // Escribe el nombre de la imagen, el tipo al que pertenece y su histograma
	    ar & img /*& tipoMayorCoincidencia;& tipoMasProximo; */ & tipoKVecino;
	}
}
