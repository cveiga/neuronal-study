#include "head.h"

#define NCLUSTERS 1000

ushort ** _vImage = NULL;
ushort ** _vPoints = NULL;

std::string _fichTrainig;
int _hisTest[NCLUSTERS];
std::string _imageTest;
cv::Mat _img;
//MiVocabulario _vocabulario(NCLUSTERS, NCOORDENADAS);

//std::string _ruta;

//void leeImagen(std::string &img);
std::string leerHistograma(std::ifstream &);
void clasificaImagen();
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

	//std::string image;
	long posicion = 0;
	char basura;
	//int miHistograma[NCLUSTERS];
	time_t initTime, currentTime;

	time(&initTime);
	while(!fichTest.eof()) 
	{ 
		if (posicion != 0) fichTest.seekg(posicion, fichTest.beg);
		std::string tipoTest = leerHistograma(fichTest);

		clasificaImagen();

		posicion = fichTest.tellg();
		fichTest >> basura;
	}
	fichTest.close();
	std::cout << "Ha tardado: " << time(&currentTime) - initTime << " Segundos" << std::endl;

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

void clasificaImagen()
{
	int result[] = {0,0};

	CvSVM SVM;
	SVM.load(_fichTrainig.c_str());

	/*for (int i = 0; i < _img.rows - 1; i++)
		{ 
			cv::Mat sample(1, 9, CV_32FC1, _vPoints[i]);
			for (int j = 0; j < _img.cols -1; j++)
			{
				float response = SVM.predict(sample);

				response == 0 ? result[0]++ : result[1]++;
				//std::cout << response == 1 ? "OTRO" : "CONDICIONADO" << std::endl;
				//std::cout << '[' << response << ']' << '\t';
				//std::cout << std::string(response == 1 ? "OTRO" : "CONDICIONADO") << std::endl;
			}
	}*/
	cv::Mat response (1,1,CV_32F);
	//for (int i = 0; i < NCLUSTERS; i++)
	//{
	cv::Mat sample(NCLUSTERS, 1, CV_32FC1, _hisTest);
	response = SVM.predict(sample);

	for (int i = 0; i < NCLUSTERS; i++)
	{
		response.at<float>(i) == 0 ? result[0]++ : result[1]++;
		//if (response != -1)	
			//std::cout << "[" << response.at<float>(i) << "]  ";
	}
	//}

	std::cout << "RESULTADO: " << result[0] << " | " << result[1];
	if (result[0] > result[1]) std::cout << "--> SALINA" << std::endl;
	else std::cout << "--> CONDICIONADO" << std::endl;
}