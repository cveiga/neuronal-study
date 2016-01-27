#include "head.h"
#include <time.h>

#define SIZE 256

unsigned long int _vHistogram[SIZE] ;

cv::Mat _img;

int _numeroImagenes;
time_t _initTime, _endTime;

std::string leerImagen(std::ifstream &fich, std::string &ruta, std::string &imagen);
void iniciaVectorHistograma();
void rellenaHistograma();
void imprimeHistograma();
void guardaHistograma(const std::string &img);


int main (int argc, char* argv[])
{
	if (argc < 2)
	{	
		std::cerr << "ERROR: falta la ruta del .txt" << std::endl;
		exit(1);
	}

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
		std::cout << "FILAS: " << _img.rows << ", COLUMNAS: " << _img.cols << std::endl;

		if (!_img.data)
		{
			std::cerr << "Don't read the image" << std::endl;
			return -1;
		}
		//cv::imshow(imagen.substr(0, imagen.size()-4), _img);

		//cv::waitKey(1);

		iniciaVectorHistograma();
		rellenaHistograma();
		imprimeHistograma();
		guardaHistograma(path);

		//cv::destroyWindow(imagen.substr(0, imagen.size()-4));

		posicion = fich.tellg();
		fich >> basura;
	}

	fich.close();
	/*time(&_endTime);
	std::cout << "HA TARDADO: " << (_endTime - _initTime) << std::endl;*/

	return 0;
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

void iniciaVectorHistograma()
{
	for (int i = 0; i < SIZE; i++)
		_vHistogram[i] = 0;
}

void rellenaHistograma()
{int num;
	for(int fila = 0; fila < _img.rows; fila++)
	{
		for(int columna = 0; columna < _img.cols; columna++)
			_vHistogram[_img.at<ushort>(fila, columna) < 256 ? _img.at<ushort>(fila, columna) : _img.at<ushort>(fila, columna)/256] += 1;	/**normalización de datos fuera del rango de 0 a SIZE */
	}
}

void imprimeHistograma()
{
	std::cout << "HISTOGRAMA: [";
	for (int i = 0; i < SIZE; i++)
		std::cout << _vHistogram[i] << "__|";
	
	std::cout << "]" << std::endl << std::endl;
}

void guardaHistograma(const std::string &img)
{
	std::string tipo;
	if(img.find("NO CONDICIONADO") != std::string::npos) tipo = "NO CONDICIONADO";
	else if(img.find("NO EMPAREJADO") != std::string::npos) tipo = "NO EMPAREJADO";
	else 
	{
		tipo = img.substr(img.find_last_of(" "), img.size());
		tipo = tipo.substr(1, tipo.size() - 5);
	}
	//Salvar el histograma
	{		
	    // Se crea un archivo de salida
	    std::ofstream ofs("HistogramasPruebaTraining.txt", std::ios::app);
	    boost::archive::text_oarchive ar(ofs);

	    // Escribe el nombre de la imagen, el tipo al que pertenece y su histograma
	    ar & img & tipo;
	    for (int i = 0; i < SIZE; i++) { ar & _vHistogram[i]; }
	}
}