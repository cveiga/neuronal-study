#include "head.h"

#define NCLUSTERS 256 //1000
#define LIMITMATCHES 10
#define KVECINOS 3 	//5 mínimo para propiciar la repetición

/** Estructura para los k-vecinos */
struct Vecinos
{
	double distancia;
	std::string tipoVecino;
};

struct NodeCandidate
{
	std::string tipo;
	std::string img;
	int hisCandidate[NCLUSTERS];
};


std::string leerHistograma(int*, std::string &, std::ifstream &);
//void rellenaLista(ListHistrograms &, std::ifstream &);
void rellenaLista(std::list<NodeCandidate> &, std::ifstream &);
//void guardaResultado(std::string, std::string, std::string, std::string);
void guardaHistograma(int*, std::string &);
void compararHistogramas(int *, int *, long &, std::string &, std::string);
double distanciaEuclidea(int *, int *, double &, std::string &, std::string);
void encuentraKVecinos(Vecinos *, int &, const std::string, const double);
std::string votaVecino(Vecinos *);
void imprimeKVecinos(Vecinos *);
bool seguridad(int argc, char* argv[], std::ifstream &fichCandidate)
{
	if (argc < 2) 
	{
		std::cerr << "Faltan parametros de ejecución" << std::endl;
		return false;
	}

	fichCandidate.open(argv[argc-1]);
	if ( !fichCandidate.is_open() )
	{
		std::cerr << "No se ha podido abrir el fichero de imágenes de Candidate" << std::endl;
		return false;
	}

	return true;
}



int main(int argc, char *argv[])
{
	std::ifstream fichCandidate, fichTraining;
	//long aciertos = 0;
	Vecinos vecinosMasProximos[KVECINOS];

	if (!seguridad(argc, argv, fichCandidate)) return 0;
	//ListHistrograms listHistCandidate;
	std::list<NodeCandidate> listHistCandidate;
	rellenaLista(listHistCandidate, fichCandidate);
	fichCandidate.close();


	std::cout << "HAY : " << listHistCandidate.size() << " ELEMENTOS" << std::endl;
	std::list<NodeCandidate>::iterator it = listHistCandidate.begin();
	//std::cout << "the first is the number " << it->img /*<< ", and the last is the number " << listHistCandidate.end()*/ << std::endl;
	int hisTraining[NCLUSTERS], hisCandidate[NCLUSTERS];
	std::string /*imageCandidate,*/ imageTraining;
	int noMatch = 0;

	bool endOfWilsone = false;
	int positionCandidate = 0;
	char basura;
	while( !endOfWilsone )
	{
		long totalCoincidencias = 0;
		double minimo = -1;
		int posicionVecino = 0;
		std::string pertenece, perteneceEuclidea, perteneceKVecinos;
		for (int i = 0; i < KVECINOS; i++) vecinosMasProximos[i].distancia = 0;

		fichTraining.open(argv[argc-1]);
		if( !fichTraining.is_open() )
		{
			std::cerr << "No se ha podido abrir el fichero de imágenes de Training" << std::endl;
			return false;
		}
		//std::cout << "Candidate: " << tipoCandidate << std::endl;
		std::cout << "Calculando tipo de la imagen: " << it->img << std::endl;
		long posicionTraining = 0, countTraining = 0;
		while( !fichTraining.eof() )
		{
			if (posicionTraining != 0) fichTraining.seekg(posicionTraining, fichTraining.beg);
			std::string tipoTraining = leerHistograma(hisTraining, imageTraining, fichTraining);
			//std::cout << "\tTRAINING: " <<  tipoTraining << std::endl;
		
			//compararHistogramas(hisCandidate, hisTraining, totalCoincidencias, pertenece, tipoTraining);
			//distanciaEuclidea(hisCandidate, hisTraining, minimo, perteneceEuclidea, tipoTraining);
			if (it->img.compare(imageTraining))
				encuentraKVecinos(vecinosMasProximos, posicionVecino, tipoTraining, distanciaEuclidea(it->hisCandidate, hisTraining, minimo, perteneceEuclidea, tipoTraining));
			//imprimeKVecinos(vecinosMasProximos);
			posicionTraining = fichTraining.tellg();
			fichTraining >> basura;
			countTraining++;
		}
		fichTraining.close();
		perteneceKVecinos = votaVecino(vecinosMasProximos);
		//guardaResultado(imageCandidate.substr(imageCandidate.find_last_of("/"), imageCandidate.size()), pertenece, perteneceEuclidea, perteneceKVecinos);

		if(it->tipo != perteneceKVecinos) 
		{
			listHistCandidate.erase(it++);
			noMatch++;
std::cout << "		---->>>> ELIMINADO! <<<<----	" << std::endl;
		}
		else{
			positionCandidate++; //it++;

			if (++it == listHistCandidate.end())
			{
				usleep(2000);
				if(noMatch < LIMITMATCHES) break;//endOfWilsone = true; //break;
				it = listHistCandidate.begin();
				noMatch = positionCandidate = 0;
	std::cout << "|||||||||||||||||||||||||||||||||||||||||||||| OTRA VEZZZZZ! ||||||||||||||||||||||||||||||||||||||||||||||" << std::endl;
			}
		}
		
		//std::cout << "El Candidate: " << tipoCandidate << ", y el resultado por Coincidencias es: " /*<< pertenece << ", y por Proximidad es: "*/ << perteneceEuclidea << std::endl;
		//std::cout << "El Candidate: " << it->tipo << ", y el resultado por Coincidencias es: " << perteneceKVecinos << std::endl;
	}
	for(it = listHistCandidate.begin(); it != listHistCandidate.end(); it++)
		guardaHistograma(it->hisCandidate, it->img); ////////////////////ESCRIBIMOS EL HISTOGRAMA EN EL NUEVO FICHERO PARA WILSON ALGORITHM////////////////
	//std::cout << "TOTAL DE ACIERTOS: " << aciertos << std::endl;
	std::cout << "TOTAL DE FALLOS: " << noMatch << ", y tamaño de la lista: " << listHistCandidate.size() << std::endl;

	return 1;
}

void rellenaLista(std::list<NodeCandidate> &listHistCandidate, std::ifstream &fichCandidate)
{
	NodeCandidate nodeAux;
	long positionCandidate = 0;
	char basura;
	while( !fichCandidate.eof() )
	{
		if (positionCandidate != 0) fichCandidate.seekg(positionCandidate, fichCandidate.beg);
		nodeAux.tipo = leerHistograma(nodeAux.hisCandidate, nodeAux.img, fichCandidate);

		listHistCandidate.push_back(nodeAux);

		positionCandidate = fichCandidate.tellg();
		fichCandidate >> basura;
	}

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
* @parametro hisCandidate, histograma de una imagen de Candidate
* @parametro hisTraining, histograma de una imagen de Trainin
* @parametro totalCoincidencias, número total de coincidencias de los dos histogramas
* @parametro pertenece, tipo al que pertenece la imagen de Candidate una vez comparados los histogramas
* @parametro tipoTraining, tipo al que pertenece la imagen de Training
*/
void compararHistogramas(int *hisCandidate, int *hisTraining, long &totalCoincidencias, std::string &pertenece, std::string tipoTraining)
{
	long coincidencias = 0;

	for (int i = 0; i < NCLUSTERS; i++)
		coincidencias += hisCandidate[i] >= hisTraining[i] ? hisTraining[i] : hisCandidate[i];

	if(coincidencias >= totalCoincidencias)
	{
		totalCoincidencias = coincidencias;
		pertenece = tipoTraining;
	}
}


double distanciaEuclidea(int *hisCandidate, int *hisTraining, double &min, std::string &perteneceEuclidea, std::string tipoTraining)
{
	double resultado = 0;
	///////////////////////////////// DISTANCIA EUCLIDEA /////////////////////////////////////////////	
	for (int i = 0; i < NCLUSTERS; i++) resultado += pow(hisCandidate[i] - hisTraining[i], 2);			
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

/*void guardaResultado(std::string img, std::string tipoMayorCoincidencia, std::string tipoMasProximo, std::string tipoKVecino)
{
	//Salvar el histograma
	{		
	    // Se crea un archivo de salida
	    std::ofstream ofs("ResultadosExperimenSinNoEmparejado_Once_K-Vecinos.txt", std::ios::app);
	    boost::archive::text_oarchive ar(ofs);

	    // Escribe el nombre de la imagen, el tipo al que pertenece y su histograma
	    ar & img /*& tipoMayorCoincidencia; & tipoMasProximo; */ /*& tipoKVecino;
	}
}*/


void guardaHistograma(int *miHistograma, std::string &img)
{
	std::string tipo;
	if(img.find("NO CONDICIONADO") != std::string::npos) tipo = "C1";
	else if(img.find("NO EMPAREJADO") != std::string::npos) tipo = "NO EMPAREJADO";
	else 
	{
		tipo = img.substr(img.find_last_of(" "), img.size());
		tipo = tipo.substr(1, tipo.size() - 4);
		tipo = tipo == "SALINA." ? "C1" : "C2";
	}
	//Salvar el histograma
	{		
	    // Se crea un archivo de salida
	    std::ofstream ofs("Wilson_3_TwoClases_256Elem.txt", std::ios::app);
	    boost::archive::text_oarchive ar(ofs);

	    // Escribe el nombre de la imagen, el tipo al que pertenece y su histograma
	    ar & img & tipo;
	    for (int i = 0; i < NCLUSTERS; i++) { ar & miHistograma[i]; }
	}
}
