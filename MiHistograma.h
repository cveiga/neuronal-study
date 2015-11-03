#ifndef MIHISTOGRAMA_H
#define MIHISTOGRAMA_H

#include "Cluster.h"

class MiHistograma : public Cluster
{
	private:
		Cluster* vCluster;
		std::string tipo;
		ushort tamanyo, num_coordenadas;
	public:
		MiHistograma(){}
		MiHistograma(ushort tam, ushort nc, std::string tp): tamanyo(tam), num_coordenadas(nc), tipo(tp)
		{ 
			vCluster = new Cluster[tamanyo];
			for (int i = 0; i < tamanyo; i++)
				vCluster[i] = Cluster(tamanyo);			
				//vCluster[i].centroide = new float[num_coordenadas];/*cv::Mat::zeros(1, num_coordenadas, CV_32F);*/
			std::cout << "vector de CLUSTERS creado" << std::endl;
		}
		Cluster getCluster(int i) { return vCluster[i]; }
		ushort getTam() const { return tamanyo; }
		ushort getNumCoordenadas() const { return num_coordenadas; }
		std::string getTipo() const{ return tipo; }
		int getTotaltes() const
		{
			int total = 0;
			for (int i = 0; i < tamanyo; i++)
				total += vCluster[i].getTotalPuntos();
			return total;
		}

		void imprimeHistograma() 
		{ 
			std::cout << "|||||||||||||||||||||||||||||||||" << std::endl;
			std::cout << "| TIPO DE NEURONA: " << tipo << "\t|"<< std::endl;
			std::cout << "|||||||||||||||||||||||||||||||||" << std::endl;
			for (int i = 0; i < tamanyo; i++)
			{
				std::cout << "   " << i+1 << "\t  ===> Cantidad:  " << vCluster[i].getTotalPuntos();
				vCluster[i].imprimeCentroide(num_coordenadas);
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
		void setTotalPuntos(const int i, const int tp){ vCluster[i].setTotalPuntos(tp); }
		void setCoordenadasCentroide(const int i, const float vCoordenadas[]) { vCluster[i].setCoordenadasCentroide(vCoordenadas);	}
		bool saveHistograma(){
			//creo y abro un fichero de caracteres para la salida
			//std::ofstream ofs("filename.dat", std::ios::binary);
			std::ofstream ofs("pruebaGuardar.txt", std::ios::app);	//abre con cursor a final de fichero
			ofs << tipo.substr(1, tipo.size()) << std::endl;
			boost::archive::text_oarchive oa(ofs);
			//escriblo la instancia de la clase en el archivo
			for (int i = 0; i < tamanyo; i++){
				oa << vCluster[i];
			}
	
			//el archivo y el stream se cierran cuando se llama al destructor
			return true;
		}

		bool readHistograma(std::ifstream &ifs)
		{
			ifs >> tipo;

			boost::archive::text_iarchive ia(ifs);

			for(int i = 0; i < tamanyo; i++)
				ia >> vCluster[i];
			return true;
		}

		~MiHistograma()
		{ 
			for (int i = 0; i < tamanyo; i++) 
				vCluster[i].~Cluster(); 
			delete [] vCluster; 
		}
};

#endif