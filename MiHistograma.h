#ifndef MIHISTOGRAMA_H
#define MIHISTOGRAMA_H

#include "Cluster.h"

class MiHistograma : public Cluster
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
				vCluster[i].centroide = new float[num_coordenadas];/*cv::Mat::zeros(1, num_coordenadas, CV_32F);*/
			std::cout << "vector de CLUSTERS creado" << std::endl;
		}
		Cluster getCluster(int i) { return vCluster[i]; }
		ushort getTam() const { return tamanyo; }
		ushort getNumCoordenadas() const { return num_coordenadas; }

		/*void copyMat(int i, cv::Mat centr)
		{
			vCluster[i].centroide = centr.clone();
		}*/

		void imprimeHistograma() 
		{ 
			for (int i = 0; i < tamanyo; i++)
			{
				std::cout << "   " << i+1 << "\t  ===> Cantidad:  " << vCluster[i].getTotalPuntos();
				vCluster[i].imprimeCentroide(num_coordenadas);
				std::cout << std::endl;
			}
		}
		void setTotalPuntos(const int i, const int tp){ vCluster[i].total_puntos = tp; }
		void setCoordenadasCentroide(const int i, const float vCoordenadas[]) { vCluster[i].setCoordenadasCentroide(vCoordenadas);	}

		~MiHistograma()
		{ 
			for (int i = 0; i < tamanyo; i++) 
				delete [] vCluster[i].centroide; 
			delete [] vCluster; 
			std::cout << "HISTOGRAMA LIBERADO" << std::endl;
		}
};

#endif