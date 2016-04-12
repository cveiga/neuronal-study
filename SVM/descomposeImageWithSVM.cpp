#include "head.h"
#include <time.h>

#define NCLUSTERS 1000

int _numeroImagenes;
time_t _initTime, _endTime;

std::string leerHistograma(int*, std::string &, std::ifstream &);
int cuentaImagenes(char* nomFich);
void imprimeVCluster();
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

	cv::Mat samples;
	cv::Mat labelesMat(0, 1, CV_32FC1);

	long posicion = 0;
	char basura;
	int filaVector = 0;
	int imagenNum = 0;

	while (!fich.eof()){
		float label;
		if (posicion != 0) fich.seekg(posicion, fich.beg);

		std::string tipoTraining = leerHistograma(boWTraining, nameImage, fich);
		cv::Mat imgDescriptor(NCLUSTERS, 1, CV_8U, boWTraining); 

		if(tipoTraining.compare("SALINA.") == 0) label = -1.0;
		if(tipoTraining.compare("NO CONDICIONADO") == 0) label = 0.0;
		if(tipoTraining.compare("CONDICIONADO.") == 0) label = 1.0;
		labelesMat.push_back(label);

		if(samples.empty()) samples.create(_numeroImagenes, NCLUSTERS, CV_32F);
		//samples.push_back(imgDescriptor);
		for (int j = 0; j < NCLUSTERS; j++)	samples.at<float>(imagenNum, j) = boWTraining[j];	
		imagenNum++;
		
		posicion = fich.tellg();
		fich >> basura;
	}

	fich.close();

std::cout << "EXTRACCIÓN DE PUNTOS FINALIZADA" << std::endl;
std::cout << "TAMAÑO DE LOS DESCRIPTORES: " << samples.size() << ", y de las ETIQUETAS: " << labelesMat.size() << std::endl;

	//Set up SVM's parameters
	CvSVMParams params;
	params.svm_type 	= CvSVM::C_SVC;
	params.degree 		= 0.4;
    params.gamma 		= 1;
    params.coef0 		= 0;

    params.nu 			= 0.0;
    params.p 			= 2;
	params.C 			= std::numeric_limits<double>::infinity();
	params.kernel_type 	= CvSVM::LINEAR;	//CvSVM::RBF;
	params.term_crit 	= cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, /*1000000*/(int)1e7, /*1e-6*/FLT_EPSILON);

	cv::Mat samples_32f;
	samples.convertTo(samples_32f, CV_32F);

	//Train the SVM
	CvSVM SVM;
std::cout << "EJECUCIÓN TRAINING SVM" << std::endl;

	time(&_initTime);
std::cout << "Training to SVM..." << std::endl;
	SVM.train(samples_32f, labelesMat, cv::Mat(), cv::Mat(), params);

	time(&_endTime);

	SVM.save("SVM_TrainingBoW_Wilson3.txt");

std::cout << "HA TARDADO: " << (_endTime - _initTime) << std::endl;

	for (int i = 0; i < _numeroImagenes; i++)
	{
		std::cout << labelesMat.at<float>(0,i) << "," ;
		if(i % 10 == 0) std::cout << std::endl;
	}std::cout << std::endl;

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