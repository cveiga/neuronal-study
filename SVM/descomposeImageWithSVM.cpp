#include "MiVocabulario.h"
#include <time.h>

#define NCLUSTER 1000
#define NLABEL 2

ushort ** _vImage = NULL;
ushort ** _vPoints = NULL;
ushort ** _vLabeles = NULL;

cv::Mat _vCluster;
cv::Mat _img;
cv::Mat _centers;

int _numeroImagenes;
time_t _initTime, _endTime;

int cuentaImagenes(char* nomFich);
std::string leerImagen(std::ifstream &fich, std::string &ruta, std::string &imagen);
void creaVectorImagen(int fila, int columna);
void creaVectorPuntos(int fila, int columna);
void creaVectorEtiquetas(int fila, int columna);
void rellenaVectorImagen();
void rellenaVectorPuntos(int fila, int f, int c, cv::Mat);
void rellenaMiVocabulario(MiVocabulario &mv);
void imprimeVectorImagen();
void imprimeVCluster();
void eliminaVectorImagen();
void eliminaVectorPuntos();


int main (int argc, char* argv[])
{
	if (argc < 2)
	{	
		std::cerr << "ERROR: falta la ruta del .txt" << std::endl;
		exit(1);
	}

	_numeroImagenes = cuentaImagenes(argv[argc-1]); 
	std::cout << "TOTAL IMAGENES: " << _numeroImagenes << std::endl;

	std::ifstream fich(argv[argc-1]);

	if (!fich)
	{
		std::cout << "Error no se puede abrir el archivo: " << std::endl;
        exit(1);
	}

	std::string ruta;
	std::string imagen;
	std::string nameImage;

	cv::Mat data;

	long posicion = 0;
	char basura;
	int filaVectorPuntos = 0;

	while (!fich.eof()){
		if (posicion != 0) fich.seekg(posicion, fich.beg);
		nameImage = leerImagen(fich, ruta, imagen);

		//cv::namedWindow(imagen.substr(0, imagen.size()-4), cv::WINDOW_AUTOSIZE);
		
		const std::string & path = "../../" + ruta.substr(ruta.find("IMG"), ruta.length()) + "/" + imagen;
		std::cout << "PATH: " << path << std::endl;

		_img = cv::imread(path, cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);
		if (_vImage == NULL) creaVectorImagen(_img.rows, _img.cols);

		/** 9 columnas, por que utilizamos una mascara de 3x3, que genera 9 coordenadas por cada pixel
		**  los pixeles de los extremos se descartan */
		if(data.empty()) data = cv::Mat::zeros(((_img.rows - 1) * (_img.cols - 1) * _numeroImagenes), 9, CV_32FC1);
		if (_vPoints == NULL) creaVectorPuntos((((_img.rows - 1) * (_img.cols - 1)) * _numeroImagenes), 9);
		if (_vLabeles == NULL) creaVectorEtiquetas((((_img.rows - 1) * (_img.cols - 1)) * _numeroImagenes), 1);

		//cv::imshow(imagen.substr(0, imagen.size()-4), _img);
		//cv::waitKey(1);

		rellenaVectorImagen();

		for (int f = 1; f < 399; f++)
			for (int c = 1; c < 399; c++)
			{
				_vLabeles[filaVectorPuntos] = nameImage.substr(nameImage.find_last_of(" "), nameImage.size()) == "CONDICIONADO" ? (unsigned short*)1 : (ushort*)2;
				rellenaVectorPuntos(filaVectorPuntos++, f, c, data);
			}

		std::cout << "TOTAL: " << filaVectorPuntos << std::endl;
		std::cout << "TIPO: -> " << nameImage.substr(nameImage.find_last_of(" "), nameImage.size()) << std::endl;

		//cv::destroyWindow(imagen.substr(0, imagen.size()-4));

		posicion = fich.tellg();
		fich >> basura;
	}

	fich.close();

std::cout << "EXTRACCIÓN DE PUNTOS FINALIZADA" << std::endl;

	float labels[NLABEL] = {1.0, -1.0};
	//cv::Mat labelesMat(NLABEL,1, CV_32FC1, labels);
	cv::Mat labelesMat(((_img.rows - 1) * (_img.cols - 1) * _numeroImagenes), 1, CV_32SC1, _vLabeles);
std::cout << "CREACION TRAININGDATAMAT" << std::endl;
	cv::Mat trainingDataMat (((_img.rows - 1) * (_img.cols - 1) * _numeroImagenes), 9, CV_32FC1, _vPoints);

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
	SVM.train(trainingDataMat, labelesMat, /*response*/cv::Mat(), cv::Mat(), params);

	time(&_endTime);

	SVM.save("SVM_Training.txt");

	std::cout << "HA TARDADO: " << (_endTime - _initTime) << std::endl;

	eliminaVectorImagen();
	eliminaVectorPuntos();

	return 0;
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
	std::cout << "TOTAL RUTAS: " << totalRutas << 	std::endl;

	return totalImagenes;
}



std::string leerImagen(std::ifstream &fich, std::string &ruta, std::string &imagen)
{
	getline(fich, imagen);

	if (!imagen.find("/"))	//nueva ruta de acceso a las imágenes
	{
		ruta = imagen;
		getline(fich, imagen);
	}

	std::cout << "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\" << std::endl;
	std::cout << ruta << std::endl;

	return imagen.substr(0, imagen.size()-4);
}


void creaVectorImagen(int fila, int columna)
{
	_vImage = new ushort*[fila];
	for (int i = 0; i < fila; i++)
		_vImage[i] = new ushort[columna];
}

void creaVectorPuntos(int fila, int columna)
{  std::cout << "VECTOR PUNTOS: " << std::endl << "FILAS: " << fila << ", COLUMNAS: " << columna << std::endl;
	_vPoints = new ushort*[fila];
	for (int i = 0; i < fila; i++)
		_vPoints[i] = new ushort[columna];
}

void creaVectorEtiquetas(int fila, int columna)
{  std::cout << "VECTOR ETIQUETAS: " << std::endl << "FILAS: " << fila << ", COLUMNAS: " << columna << std::endl;
	_vLabeles = new ushort*[fila];
	for (int i = 0; i < fila; i++)
		_vLabeles[i] = new ushort[columna];
}

void rellenaVectorImagen()
{
	ushort numerito;
	int columna = 1;
	int mostrado = 0;
	int j = 0;
	int cont = 0;
	bool mostrar = false;
	while (true){
		if (mostrar)
		{
			for(int fila = 0; fila < _img.rows; fila++)
				_vImage[fila][j] = _img.at<ushort>(fila, columna) < 256 ? _img.at<ushort>(fila, columna) : _img.at<ushort>(fila, columna)/256;   /**normalización de datos fuera del rango de 0 a 255 */

			j++;
			columna++;
			mostrado++;
			//std::cout << std::endl;
			//if (columna % 50 == 0)
			//	cv::waitKey(0);

			if(cont > 1)
			{
				mostrar = false;
				cont = 0;
				columna++;
			}
		}
		if (mostrado >= 400) break;		
		if (cont < 1) mostrar = true;
		cont++;
	}
}

/** En esta función se almacenan el valor de cada uno de los puntos de 9 pixeles resultantes de aplicar una mascara de 3x3 */
void rellenaVectorPuntos(int fila, int f, int c, cv::Mat data)
{	
	data.at<float>(fila, 0) = _vPoints[fila][0] = _vImage[f - 1][c - 1];
	data.at<float>(fila, 1) = _vPoints[fila][1] = _vImage[f - 1][c];
	data.at<float>(fila, 2) = _vPoints[fila][2] = _vImage[f - 1][c + 1];
	data.at<float>(fila, 3) = _vPoints[fila][3] = _vImage[f][c - 1];
	data.at<float>(fila, 4) = _vPoints[fila][4] = _vImage[f][c];
	data.at<float>(fila, 5) = _vPoints[fila][5] = _vImage[f][c + 1];
	data.at<float>(fila, 6) = _vPoints[fila][6] = _vImage[f + 1][c - 1];
	data.at<float>(fila, 7) = _vPoints[fila][7] = _vImage[f + 1][c];
	data.at<float>(fila, 8) = _vPoints[fila][8] = _vImage[f + 1][c + 1];
}

void imprimeVectorImagen()
{
	for (int f = 1; f < 399; f++){
		for (int c = 1; c < 399; c++)
		{
			std::cout << "[" << _vImage[f - 1][c - 1] << ", " << _vImage[f - 1][c] << ", " << _vImage[f - 1][c + 1] << ", "
					  		 << _vImage[f][c - 1] << ", " << _vImage[f][c] << ", " << _vImage[f][c + 1] << ", "
					  		 << _vImage[f + 1][c - 1] << ", " << _vImage[f + 1][c] << ", " << _vImage[f + 1][c + 1] << "]" << std::endl; //<< std::endl;
		}
		std::cout << std::endl;
		if (f % 100 == 0)
			cv::waitKey(0);
	}
}

void eliminaVectorImagen()
{
	for (int i = 0; i < _img.rows; i++)
		delete[] _vImage[i];
	delete[] _vImage;
}

void eliminaVectorPuntos()
{
	for (int i = 0; i < (_img.rows - 1) * (_img.cols - 1) * _numeroImagenes; i++)
		delete[] _vPoints[i];
	delete[] _vPoints;
}