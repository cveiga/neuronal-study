#ifndef CLUSTER_H
#define CLUSTER_H

#include <head.h>

class Cluster
{
private:
	friend std::ostream & operator<<(std::ostream &os, const Cluster &cl);
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version/* file_version */)
	{ 
		for (int i = 0; i < 9; i++)
			ar & centroide[i];
		ar & total_puntos;
	}

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

std::ostream & operator<<(std::ostream &os, const Cluster &cl)
{
	for (int i = 0; i < 9; i++)
			os << cl.centroide[i];
		
    return os << cl.total_puntos;
}

#endif