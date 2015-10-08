#ifndef CLUSTER_H
#define CLUSTER_H

#include <head.h>

class Cluster
{
private:
	/*cv::Mat*/float* centroide;
	int total_puntos;

public:
	Cluster(): total_puntos(0) {}
	Cluster(int num_coordenadas): total_puntos(0) { centroide = new float[num_coordenadas]; } /*centroide = (1, num_coordenadas, CV_32FC1);*/
	int getTotalPuntos() const { return total_puntos; }
	void setTotalPuntos(const int tp) { total_puntos = tp; }
	void setCoordenadasCentroide(const float vectorCoordenadas[]) 
	{
		for (int i = 0; i <= sizeof(vectorCoordenadas); i++)
			this->centroide[i] = vectorCoordenadas[i];
	}

	Cluster operator=(Cluster c){ this->centroide = c.centroide; }

	void imprimeCentroide(const int num_coordenadas)
	{
		std::cout << "\t";
		for (int i = 0; i < num_coordenadas; i++)
			std::cout << " [" << /*centroide.at<float>(i,j)*/ centroide[i] << "] -";
	}

	~Cluster(){}
};

#endif