#ifndef CLUSTER_H
#define CLUSTER_H

#include <head.h>

class Cluster
{
private:
public:
	cv::Mat centroide;
	int total_puntos;

	Cluster(){ total_puntos= 0; }
	Cluster(int num_coordenadas): total_puntos(0) { centroide = (1, num_coordenadas, CV_32FC1); }
	int getTotalPuntos() const { return total_puntos; }
	void setTotalPuntos(const int& tp) { total_puntos = tp; }

	void imprimeCentroide(const int i, const int num_coordenadas)
	{
		std::cout << "\t";
		for (int j = 0; j < num_coordenadas; j++)
		{
			std::cout << " [" << centroide.at<float>(i,j) << "] -";
		}
		
	}

	~Cluster(){}
};

#endif