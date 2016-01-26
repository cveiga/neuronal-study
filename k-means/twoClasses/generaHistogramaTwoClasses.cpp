#include <MiVocabulario.h>
#include <time.h>

#define NCLUSTERS 1000
#define NCOORDENADAS 9

ushort ** _vImage = NULL;
ushort ** _vPoints = NULL;

cv::Mat _img;
MiVocabulario _vocabulario(NCLUSTERS, NCOORDENADAS);
std::ifstream _fichImagenes;
std::string _ruta;

bool capturaVocabulario(char * nomFich);
void leeImagen(std::string &img);
void creaVectorImagen(int fila, int columna);
void creaVectorPuntos(int fila, int columna);
void rellenaVectorImagen();
void rellenaVectorPuntos(int fila, int f, int c);
void calculaHistograma(int*, int);
void imprimeHistograma(int*);
void guardaHistograma(int*, std::string &);
bool seguridad(int argc, char* argv[])
{
	if (argc < 3) 
	{
		std::cerr << "Faltan parametros de ejecución" << std::endl;
		return false;
	}

	if( !capturaVocabulario(argv[argc-2]) )
	{
		std::cerr << "No se ha podido recuperar el vacabulario" << std::endl;
		return false;
	}

	_fichImagenes.open(argv[argc-1]);
	if ( !_fichImagenes.is_open())
	{
		std::cerr << "No se ha podido abrir el fichero de imágenes" << std::endl;
		return false;
	}
	_ruta = "";

	return true;
}


int main(int argc, char* argv[])
{
	if (!seguridad(argc, argv)) return 0;

	std::string image;
	long posicion = 0;
	char basura;
	int miHistograma[NCLUSTERS];
	time_t initTime, currentTime;

	time(&initTime);
	while(!_fichImagenes.eof()) 
	{ 
		if (posicion != 0) _fichImagenes.seekg(posicion, _fichImagenes.beg);
		leeImagen(image); 


		_img = cv::imread(image, cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);
		if (_vImage == NULL) creaVectorImagen(_img.rows, _img.cols);

		/**  los pixeles de los extremos se descartan */
		if (_vPoints == NULL) creaVectorPuntos((_img.rows - 1) * (_img.cols - 1), NCOORDENADAS);

		/** se cogen los valores de la matriz y se normalizan para tenerlos en el rango (0,255) */
		rellenaVectorImagen();
		/** se rellena el vector con cada una de las coordenadas de la imagen */
		int cont = 0;
		for (int f = 1; f < (_img.rows - 1); f++)
			for (int c = 1; c < (_img.cols - 1); c++) rellenaVectorPuntos(cont++, f, c);

		for(int i = 0; i < NCLUSTERS; i++) miHistograma[i] = 0;	//histograma a 0
		calculaHistograma(miHistograma, cont);
		imprimeHistograma(miHistograma);
		guardaHistograma(miHistograma, image);

		posicion = _fichImagenes.tellg();
		_fichImagenes >> basura;
	}
	_fichImagenes.close();
	std::cout << "Ha tardado: " << time(&currentTime) - initTime << " Segundos" << std::endl;

	return 1;
}


/** Almacenamiento del vocabulario en una variable MiVocabulario */
bool capturaVocabulario(char * nomFich)
{
	std::ifstream fich(nomFich);
	if(fich.is_open()) _vocabulario.readVocabulary(fich);
	else return false;
	fich.close();	

	_vocabulario.imprimeVocabulario();
	std::cout << _vocabulario.getTipo() << std::endl;

	return true;
}

void leeImagen(std::string &img)
{	
	getline(_fichImagenes, img);
	if (!img.find("/"))
	{
		_ruta = "../" + img.substr(img.find("IMG"), img.length()) + "/";
		getline(_fichImagenes, img);
	}
	img = _ruta + img;
	std::cout << img << std::endl;
}

void creaVectorImagen(int fila, int columna)
{
	_vImage = new ushort*[fila];
	for (int i = 0; i < fila; i++)
		_vImage[i] = new ushort[columna];
}

void creaVectorPuntos(int fila, int columna)
{ 
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

/** En esta función se almacenan el valor de cada uno de los puntos de 9 pixeles resultantes 
**  de aplicar una mascara de 3x3 */
void rellenaVectorPuntos(int fila, int f, int c)
{	
	_vPoints[fila][0] = _vImage[f - 1][c - 1];
	_vPoints[fila][1] = _vImage[f - 1][c];
	_vPoints[fila][2] = _vImage[f - 1][c + 1];
	_vPoints[fila][3] = _vImage[f][c - 1];
	_vPoints[fila][4] = _vImage[f][c];
	_vPoints[fila][5] = _vImage[f][c + 1];
	_vPoints[fila][6] = _vImage[f + 1][c - 1];
	_vPoints[fila][7] = _vImage[f + 1][c];
	_vPoints[fila][8] = _vImage[f + 1][c + 1];
}

void calculaHistograma(int *miHistograma, int filas)
{
	std::cout << "Calculando histograma de la imagen... " << std::endl;
	for (int i = 0; i < filas; ++i)	//cada uno de los f filas de la imagen de Training
	{ 	//coordenadas de cada uno de los 100 centroides de la imagen de Test
		double resultado = 0;
		int min = -1;
		int colocarEn = 0;
		for (int j = 0; j < NCLUSTERS; j++)	//cada uno de los 1000 clusters del vocabulario
		{ 	//coordenadas de cada uno de los 1000 centroides de la imagen de Test
			float *coordenadasCentroide = _vocabulario.getCluster(j).getCoordenadasCentroide();
			///////////////////////////////// DISTANCIA EUCLIDEA /////////////////////////////////////////////	
			for (int k = 0; k < NCOORDENADAS; k++) resultado += pow(_vPoints[i][k] - coordenadasCentroide[k], 2);			
			resultado = sqrt(resultado);
			//////////////////////////////////////////////////////////////////////////////////////////////////			
			if ( resultado <= min or min == -1)
			{
				min = resultado;
				colocarEn = j;
			}
		}
		miHistograma[colocarEn] += 1;
	}
}

void imprimeHistograma(int *miHistograma)
{
	for (int i = 0; i < NCLUSTERS; i++)
	{
		std::cout << i+1 << "-> " << miHistograma[i] << " | ";
		if((i+1) % 11 == 0) std::cout << std::endl;
	}
 	std::cout << std::endl << std::endl;
}

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
	    std::ofstream ofs("HistogramasSinNoEmparejadoTest.txt", std::ios::app);
	    boost::archive::text_oarchive ar(ofs);

	    // Escribe el nombre de la imagen, el tipo al que pertenece y su histograma
	    ar & img & tipo;
	    for (int i = 0; i < NCLUSTERS; i++) { ar & miHistograma[i]; }
	}
}