#include "head.h"

#define NCLUSTERS 1000

ushort _tCondicionados = 0, _tResto = 0;

std::string _fichTrainig;
int _hisTest[NCLUSTERS];
std::string _imageTest;
cv::Mat _img;

std::string leerHistograma(std::ifstream &);
void clasificaImagen(std::string);
bool seguridad(int argc, char* argv[], std::ifstream &fichTest)
{
	if (argc < 3) 
	{
		std::cerr << "Faltan parametros de ejecución" << std::endl;
		return false;
	}

	fichTest.open(argv[argc-1]);
	if ( !fichTest.is_open())
	{
		std::cerr << "No se ha podido abrir el fichero de BoS" << std::endl;
		return false;
	}
	_fichTrainig = argv[argc-2];

	return true;
}


int main(int argc, char* argv[])
{
	std::ifstream fichTest;
	if (!seguridad(argc, argv, fichTest)) return 0;

	long posicion = 0;
	char basura;
	time_t initTime, currentTime;

	time(&initTime);
	while(!fichTest.eof()) 
	{ 
		if (posicion != 0) fichTest.seekg(posicion, fichTest.beg);
		std::string tipoTest = leerHistograma(fichTest);

		clasificaImagen(tipoTest);

		posicion = fichTest.tellg();
		fichTest >> basura;
	}
	fichTest.close();
	std::cout << "Ha tardado: " << time(&currentTime) - initTime << " Segundos" << std::endl;

	std::cout << "TOTAL: CONDICIONADOS = " << _tCondicionados << " | RESTO = " << _tResto << std::endl;

	return 1;
}

/**
* Lee un histograma
* @parametro _hisTest, vector de NCLUSTERS donde almacen los datos del histograma
* @parametro _imageTest, nombre de la imagen a la que pertenece el histograma
* @parametro ifs, fichero donde se encuentra almacenado el histograma
* @devuelve tipo, tipo de imagen a la que pertenece el histograma leido
*/
std::string leerHistograma(std::ifstream &ifs)
{
	std::string tipo;
	//Leer el histograma
	{		
	    // Se abre un archivo de lectura
	    boost::archive::text_iarchive ar(ifs);
	    // Escribe el nombre de la imagen, el tipo al que pertenece y su histograma
	    ar & _imageTest & tipo;
	    for (int i = 0; i < NCLUSTERS; i++) { ar & _hisTest[i]; }
	}
	return tipo;
}

/**
*Clasificar una imagen
*@parametro tipo, tipo de la imagen de test
*@parametro _hisTest, histograma de la imagen de test
*/
void clasificaImagen(std::string tipo)
{
	int result[] = {0,0};

	CvSVM SVM;
	SVM.load(_fichTrainig.c_str());
	
	cv::Mat sample(NCLUSTERS, 1, CV_32FC1, _hisTest);
	float response = SVM.predict(sample);

	std::cout << tipo << " --> RESULTADO: ";
	response != 1 ? std::cout << "OTRO" : std::cout << "CONDICIONADO" ;
	std::cout << std::endl;
}