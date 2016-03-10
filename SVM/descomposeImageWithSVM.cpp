#include "head.h"
#include <time.h>

#define NCLUSTERS 1000

ushort ** _vImage = NULL;
int ** _vPoints = NULL;
float * _vLabeles = NULL;

cv::Mat _vCluster;
cv::Mat _img;
cv::Mat _centers;

int _numeroImagenes;
time_t _initTime, _endTime;

std::string leerHistograma(int*, std::string &, std::ifstream &);
int cuentaImagenes(char* nomFich);
void creaVectorPuntos(int fila, int columna);
void creaVectorEtiquetas(int);
void rellenaVectorPuntos(int, int*);
void imprimeVCluster();
void eliminaVectorPuntos();
bool seguridad(int argc, char* argv[], std::ifstream &fichTest)
{
	if (argc < 2) 
	{
		std::cerr << "ERROR: Faltan parametros de ejecución" << std::endl;
		return false;
	}

	fichTest.open(argv[argc-1]);
	if ( !fichTest.is_open() )
	{
		std::cerr << "ERROR: No se ha podido abrir el fichero de imágenes de TRAINING" << std::endl;
		return false;
	}

	return true;
}



int main (int argc, char* argv[])
{

	std::ifstream fich;

	if (!seguridad(argc, argv, fich)) return 0;

	_numeroImagenes = cuentaImagenes(argv[argc-1]); 
	std::cout << "TOTAL IMAGENES: " << _numeroImagenes << std::endl;

	int boWTraining[NCLUSTERS];
	
	std::string ruta;
	std::string imagen;
	std::string nameImage;

	long posicion = 0;
	char basura;
	int filaVector = 0;

	while (!fich.eof()){
		if (posicion != 0) fich.seekg(posicion, fich.beg);
		
		if (_vPoints == NULL) creaVectorPuntos(_numeroImagenes, NCLUSTERS);
		if (_vLabeles == NULL) creaVectorEtiquetas(_numeroImagenes);

		std::string tipoTraining = leerHistograma(boWTraining, nameImage, fich);
		_vLabeles[filaVector] = tipoTraining == "CONDICIONADO." ? 1.0 : -1.0;

		rellenaVectorPuntos(filaVector++, boWTraining);

		posicion = fich.tellg();
		fich >> basura;
	}

	fich.close();

std::cout << "EXTRACCIÓN DE PUNTOS FINALIZADA" << std::endl;

	cv::Mat labelesMat(_numeroImagenes, 1, CV_32FC1, _vLabeles);
std::cout << "CREACION TRAININGDATAMAT" << std::endl;
	cv::Mat trainingDataMat = cv::Mat::zeros(_numeroImagenes, NCLUSTERS, CV_32FC1);
	for (int i = 0; i < _numeroImagenes; i++)
	{
		for (int j = 0; j < NCLUSTERS; j++)	trainingDataMat.at<float>(i, j) = _vPoints[i][j];	
	}
	//cv::Mat trainingDataMat(_numeroImagenes, NCLUSTERS, CV_32FC1, _vPoints);
std::cout << "TOTAL DE BoW: " << _numeroImagenes << std::endl;
	//Set up SVM's parameters
	CvSVMParams params;
	params.svm_type = CvSVM::C_SVC;
	params.kernel_type = CvSVM::LINEAR;
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

	//Train the SVM
	CvSVM SVM;
std::cout << "EJECUCIÓN TRAINING SVM" << std::endl;
	//cv::Mat response(((_img.rows - 1) * (_img.cols - 1) * _numeroImagenes), 1, CV_32SC1);
	time(&_initTime);
	std::cout << "Training to SVM..." << std::endl;
	SVM.train(trainingDataMat, labelesMat, cv::Mat(), cv::Mat(), params);

	time(&_endTime);

	SVM.save("SVM_TrainingBoW3.txt");

	std::cout << "HA TARDADO: " << (_endTime - _initTime) << std::endl;

	eliminaVectorPuntos();
	delete[] _vLabeles;

	return 0;
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

int cuentaImagenes(char* nomFich)
{
	std::ifstream fich(nomFich);

	if (!fich)
	{
		std::cout << "Error no se puede abrir el archivo: " << std::endl;
        exit(1);
	}
	std::string linea;
	int totalImagenes = 0, totalRutas = 0;
	
	int posicion = 0;
	char basura;
	while(!fich.eof())
	{
		if (posicion != 0) fich.seekg(posicion, fich.beg);
		getline(fich, linea);
		if (!linea.find("/")) totalRutas++;
		else totalImagenes++;
		posicion = fich.tellg();
		fich >> basura;
	}

	fich.close();
	std::cout << "TOTAL BOLSA DE PALABRAS: " << totalImagenes << 	std::endl;

	return totalImagenes;
}

void creaVectorPuntos(int fila, int columna)
{  std::cout << "VECTOR PUNTOS: " << std::endl << "FILAS: " << fila << ", COLUMNAS: " << columna << std::endl;
	_vPoints = new int*[fila];
	for (int i = 0; i < fila; i++)
		_vPoints[i] = new int[columna];
}

void creaVectorEtiquetas(int fila)
{  std::cout << "VECTOR ETIQUETAS: " << std::endl << "FILAS: " << fila << std::endl;
	_vLabeles = new float[fila];
}

/** En esta función se almacenan el valor de cada uno de las palabras para cada una de las imágenes */
void rellenaVectorPuntos(int fila, int *miHistograma)
{	
	for (int i = 0; i < NCLUSTERS; i++)	_vPoints[fila][i] = miHistograma[i];
}

void eliminaVectorPuntos()
{
	for (int i = 0; i < _numeroImagenes; i++)
		delete[] _vPoints[i];
	delete[] _vPoints;
}