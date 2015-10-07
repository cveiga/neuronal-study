#include "MiHistograma.h"

#define NCLUSTER 100

ushort ** _vImage;
ushort ** _vPoints;
//int _vCluster[NCLUSTER];
cv::Mat _vCluster;
cv::Mat _img;
cv::Mat _centers;	//(NCLUSTER, 1, CV_32FC1);


void creaVectorImagen(int fila, int columna);
void creaVectorPuntos(int fila, int columna);
void rellenaVectorImagen();
void rellenaVectorPuntos(int fila, int f, int c, cv::Mat);
void rellenaMiHistograma(MiHistograma &mh);
void imprimeVectorImagen();
void imprimeVCluster();
void muestraHistograma();
void eliminaVectorImagen();
void eliminaVectorPuntos();


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

	getline(fich, ruta);
	getline(fich, imagen);

	std::cout << "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\" << std::endl;
	std::cout << ruta << std::endl;

	fich.close();

	cv::namedWindow("Neuronas", cv::WINDOW_AUTOSIZE);
	
	std::string path = ruta.substr(ruta.find("IMG"), ruta.length()) + "/" + imagen;
	std::cout << path << std::endl;

	_img = cv::imread("../IMG/NEURONAS/PNNs_granular_condicionamiento/RECORTADAS/F8C1/corte 1 lob3 40x2.5 foto1_2 CONDICIONADO.tif"/*path*/, cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);
	creaVectorImagen(_img.rows, _img.cols);

	cv::imshow("Neuronas", _img);

	cv::waitKey(0);

	rellenaVectorImagen();

	std::cout << _img.dims << std::endl;

	/** 9 columnas, por que utilizamos una mascara de 3x3, que genera 9 coordenadas por cada pixel
	**  los pixeles de los extremos se descartan */
	cv::Mat data = cv::Mat::zeros((_img.rows - 1) * (_img.cols - 1), 9, CV_32FC1);
	creaVectorPuntos((_img.rows - 1) * (_img.cols - 1), 9);

	int cont = 0;
	for (int f = 1; f < 399; f++){
		for (int c = 1; c < 399; c++)
		{
			rellenaVectorPuntos(cont++, f, c, data);
		}
	}
	/*for (int i = 0; i < (_img.rows - 1) * (_img.cols - 1); i++)
	{
		std::cout << "[";
		for (int j = 0; j < 9; j++)
			std::cout << "," << data.at<float>(i,j) << " ";
		std::cout  << "]" << std::endl;
		if(i == 40) cv::waitKey(0);
	}
	cv::waitKey(0);*/

	std::cout << "TOTAL: " << cont << std::endl;

	//imprimeVectorImagen();
	//cv::Mat indices, dists;
	double * dists;
	double compactness = cv::kmeans(data, NCLUSTER, _vCluster, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 10, 0.01), 3, cv::KMEANS_PP_CENTERS, _centers);
	//cv::parallel_for_(cv::Range(0, NCLUSTER), KMeansDistanceComputer(dists, _vCluster, data, _centers));
	//cv::ParallelLoopBody::kmeansPPDistanceComputer();
	//int::knnSearch(data, indices, dists, NCLUSTER, 32);

	std::cout << "Filas de _centers: " << _centers.rows << ", Columnas de _centers: " << _centers.cols << std::endl;
	std::cout << "Filas de _vCluster: " << _vCluster.rows << ", Columnas de _vCluster: " << _vCluster.cols << std::endl;
	//std::cout << "Filas de indices: " << indices.rows << ", Columnas de indices: " << indices.cols << std::endl;
	//std::cout << "Filas de dists: " << dists.rows << ", Columnas de dists: " << dists.cols << std::endl;

	//MiHistograma mHisto(NCLUSTER, _centers.cols, cv::Mat(1,_centers.cols, CV_32FC1));
	MiHistograma mHisto(NCLUSTER, _centers.cols);
	rellenaMiHistograma(mHisto);
	mHisto.imprimeHistograma();
	cv::waitKey(0);
	for(int i = 0; i < _centers.rows; i++)
	{
		std::cout << "   " << i+1 << "\t  ===>  ";
		for (int j = 0; j < _centers.cols; j++)
		{
			std::cout << " [" << _centers.at<float>(i,j) << "] -";
		}
		std::cout << std::endl;
	}

	//imprimeVCluster();
	//muestraHistograma();
	std::cout << (int)compactness << std::endl;

	eliminaVectorImagen();
	eliminaVectorPuntos();

	return 0;
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
			{
				numerito = _img.at<ushort>(fila, columna) < 256 ? _img.at<ushort>(fila, columna) : _img.at<ushort>(fila, columna)/256;
				_vImage[fila][j] = numerito;
				//std::cout << numerito  << ", ";
			}
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

void muestraHistograma()
{
 	/// Establish the number of bins
  	int histSize = 100;

  	/// Set the ranges ( for B,G,R) )
  	float range[] = { 1, 100 } ;
  	const float* histRange = { range };

	cv::Mat miHistograma;
	cv::calcHist(&_centers, 1, 0, cv::Mat(), miHistograma, 1, &histSize, &histRange, true, false);
	//cv::calcHist(_centers, 1, 0, cv::Mat(), miHistograma, 1, &histSize, &histRange, uniform, accumulate );

	for( int h = 0; h < histSize; h++ )
    {
            float binVal = miHistograma.at<float>(h);
            std::cout<<" "<<binVal << ",";
    }
    std::cout << std::endl;

	// Draw the histograms for R, G and B
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound( (double) hist_w/histSize );

	cv::Mat histImage( hist_h, hist_w, CV_8UC1);

	/// Normalize the result to [ 0, histImage.rows ]
	cv::normalize(miHistograma, miHistograma, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );

	/// Draw for each channel
	for( int i = 1; i < histSize; i++ )
	{
    	cv::line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(miHistograma.at<float>(i-1)) ) ,
                     cv::Point( bin_w*(i), hist_h - cvRound(miHistograma.at<float>(i)) ),
                     cv::Scalar( 255, 0, 0), 2, 8, 0  );
    }

	cv::namedWindow("calcHist Demo", cv::WINDOW_AUTOSIZE );
	cv::imshow("calcHist Demo", histImage );

	cv::waitKey(0);
}

void eliminaVectorImagen()
{
	for (int i = 0; i < _img.rows; i++)
		delete[] _vImage[i];
	delete[] _vImage;
}

void eliminaVectorPuntos()
{
	for (int i = 0; i < (_img.rows - 1) * (_img.cols - 1); i++)
		delete[] _vPoints[i];
	delete[] _vPoints;
}