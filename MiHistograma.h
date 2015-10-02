#ifndef MIHISTOGRAMA_H
#define MIHISTOGRAMA_H

#include "Cluster.h"

class MiHistograma
{
	private:
		Cluster* vCluster;
		ushort tamanyo, num_coordenadas;
	public:
		MiHistograma(){}
		MiHistograma(ushort tam, ushort nc): tamanyo(tam), num_coordenadas(nc)
		{ 
			vCluster = new Cluster[tamanyo];
			for (int i = 0; i < tamanyo; i++)
				vCluster[i].centroide = cv::Mat::zeros(1, num_coordenadas, CV_32F);
			std::cout << "vector de CLUSTERS creado" << std::endl;
		}
		Cluster getCluster(int i) { return vCluster[i]; }
		ushort getTam() const { return tamanyo; }
		ushort getNumCoordenadas() const { return num_coordenadas; }

		void copyMat(int i, cv::Mat centr)
		{
			vCluster[i].centroide = centr.clone();
		}

		void imprimeHistograma() 
		{ 
			for (int i = 0; i < tamanyo; i++)
			{
				std::cout << "   " << i+1 << "\t  ===> Cantidad:  " << vCluster[i].getTotalPuntos();
				vCluster[i].imprimeCentroide(i, num_coordenadas);
				std::cout << std::endl;
			}
		}
		void setTotalPuntos(const int i, const int tp){ vCluster[i].total_puntos = tp; }

		~MiHistograma(){ delete [] vCluster; }
};

#endif