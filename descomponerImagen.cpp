#include "MiHistograma.h"

#define NCLUSTER 100

ushort ** _vImage = NULL;
ushort ** _vPoints = NULL;

cv::Mat _vCluster;
cv::Mat _img;
cv::Mat _centers;

int _numeroImagenes;

int cuentaImagenes(char* nomFich);
std::string leerImagen(std::ifstream &fich, std::string &ruta, std::string &imagen);
void creaVectorImagen(int fila, int columna);
void creaVectorPuntos(int fila, int columna);
void rellenaVectorImagen();
void rellenaVectorPuntos(int fila, int f, int c, cv::Mat);
void rellenaMiHistograma(MiHistograma &mh);
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

	getline(fich, ruta);

	long posicion = fich.tellg();;
	char basura;
	int filaVectorPuntos = 0;

	do{
		fich.seekg(posicion, fich.beg);

		nameImage = leerImagen(fich, ruta, imagen);

		//cv::namedWindow(imagen.substr(0, imagen.size()-4), cv::WINDOW_AUTOSIZE);
		
		const std::string & path = "../" + ruta.substr(ruta.find("IMG"), ruta.length()) + "/" + imagen;
		std::cout << "PATH: " << path << std::endl;

		_img = cv::imread(path, cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);
		if (_vImage == NULL) creaVectorImagen(_img.rows, _img.cols);

		/** 9 columnas, por que utilizamos una mascara de 3x3, que genera 9 coordenadas por cada pixel
		**  los pixeles de los extremos se descartan */
		if(data.empty()) data = cv::Mat::zeros(((_img.rows - 1) * (_img.cols - 1) * _numeroImagenes), 9, CV_32FC1);
		if (_vPoints == NULL) creaVectorPuntos((((_img.rows - 1) * (_img.cols - 1)) * _numeroImagenes), 9);

		//cv::imshow(imagen.substr(0, imagen.size()-4), _img);
		//std::cout << "NO LLEGA" << std::endl;

		//cv::waitKey(1);

		rellenaVectorImagen();

		std::cout << _img.dims << std::endl;

		for (int f = 1; f < 399; f++)
		{ 
			for (int c = 1; c < 399; c++)
			{
				rellenaVectorPuntos(filaVectorPuntos++, f, c, data);
			}
		}

		std::cout << "TOTAL: " << filaVectorPuntos << std::endl;
		std::cout << "TIPO: -> " << nameImage.substr(nameImage.find_last_of(" "), nameImage.size()) << std::endl;

		//cv::destroyWindow(imagen.substr(0, imagen.size()-4));

		posicion = fich.tellg();
		fich >> basura;
	} while (!fich.eof());
	//std::cout << "Total De Totales: " << mHisto.getTotaltes() << std::endl;

	fich.close();

	double compactness = cv::kmeans(data, NCLUSTER, _vCluster, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 10, 0.01), 3, cv::KMEANS_PP_CENTERS, _centers);

	MiHistograma mHisto(NCLUSTER, _centers.cols, nameImage.substr(nameImage.find_last_of(" "), nameImage.size()));
	rellenaMiHistograma(mHisto);
	mHisto.imprimeHistograma();
	mHisto.saveHistograma();

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
	
	while(!fich.eof())
	{
		getline(fich, linea);
		if (!linea.find("/")) totalRutas++;
		else totalImagenes++;
	}

	fich.close();

	return totalImagenes -  totalRutas;
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
{  std::cout << "FILA: " << fila << ", COLUMNA: " << columna << std::endl;
	_vPoints = new ushort*[fila];
	for (int i = 0; i < fila; i++)
		_vPoints[i] = new ushort[columna];
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
	}std::cout << "SALIMOS" << std::endl;
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

void rellenaMiHistograma(MiHistograma &mh)
{
	for (int i = 0; i < _centers.rows; i++)
	{
		float vCoordenadas[_centers.cols];
		for (int j = 0; j < _centers.cols; j++)
		{
			vCoordenadas[j] = _centers.at<float>(i,j);
			//mh.getCluster(i).centroide.at<float>(j) = _centers.at<float>(i,j);
		}
		mh.setCoordenadasCentroide(i, vCoordenadas);
	}
	for (int i = 0; i < _vCluster.rows; i++)
		mh.setTotalPuntos(_vCluster.at<ushort>(i), mh.getCluster(_vCluster.at<ushort>(i)).getTotalPuntos() + 1);
	std::cout << "HISTOGRAMA RELLENO" << std::endl;
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

void imprimeVCluster()
{
	for (int i = 0; i < _vCluster.rows; i++)
	{
		std::cout << i+1 << "->" << _vCluster.at<ushort>(i) << "__|";

		if (i % 18 == 0) std::cout << std::endl;
		//if (i % 200 == 0) cv::waitKey(0);
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