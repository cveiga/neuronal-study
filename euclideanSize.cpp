#include <math.h>
#include <head.h>

#include "MiHistograma.h"

#define NCLUSTER 100
#define NCOORDENADAS 9

//int NCOORDENADAS = 9;
int _numPuntos = 100;

void Punto(double *p)
{
	std::cout << "[";
	for (int i = 0; i < NCOORDENADAS; i++){
		std::cout << ", " << p[i];
	}
	std::cout << "]" << std::endl;;
}

void imprimePoints(double *p) {}

int main(int argc, char * argv[])
{
	std::ifstream fichTest(argv[argc-2]);

	MiHistograma mHistoTest(NCLUSTER, NCOORDENADAS, "");
	MiHistograma mHistoTrainig(NCLUSTER, NCOORDENADAS, "");

	//double **Point_P, **Point_Q, *posicion;
	double resultado = 0, *vectorPosition;
	int min, colocarEn;

	//Point_P = new double*[NCOORDENADAS];
	//Point_Q = new double*[NCOORDENADAS];
	vectorPosition = new double[NCLUSTER];

	/*for (int i = 0; i < NCOORDENADAS; i++)
		for (int j = 0; j < NCOORDENADAS; j++)
		{
			Point_P[i][j] = rand() % 100;
			Point_Q[i][j] = rand() % 100;
		}
	*/
	long posicionTest = 0;
	char basura;
	bool continuar = true;
	while (!fichTest.eof())
	{
		for (int i = 0; i < NCOORDENADAS; i++) vectorPosition[i] = 0;	//posiciones a 0 para cada imagen de Test

		if (posicionTest != 0) fichTest.seekg(posicionTest, fichTest.beg);
		//if (fichTest.is_open()) std::cout << "Adios" << posicionTest << std::endl;

		mHistoTest.readHistograma(fichTest);

		//std::cout << mHistoTest.getTipo() << std::endl;
		std::ifstream fichTraining(argv[argc-1]);	//cada imagen de Test que se lee, se leen todas las de training
		long posicionTraining = 0;
		char basura2;
		while(!fichTraining.eof())
		{		
			if (posicionTraining != 0) fichTraining.seekg(posicionTraining, fichTraining.beg);
	
			mHistoTrainig.readHistograma(fichTraining);
			for (int i = 0; i < NCLUSTER; ++i)	//cada uno de los 100 clusters de la imagen de Test
			{ 		//coordenadas de cada uno de los 100 centroides de la imagen de Test
				float *centroideTest = mHistoTest.getCluster(i).getCoordenadasCentroide();
				resultado = 0;
				min = -1;
				for (int j = 0; j < NCLUSTER; j++)	//cada uno de los 100 clusters de la imagen de Training
				{ 		//coordenadas de cada uno de los 100 centroides de la imagen de Test
					float *centroideTraining = mHistoTrainig.getCluster(j).getCoordenadasCentroide();
				/////////////////////////////////////  DISTANCIA EUCLIDEA  ///////////////////////////////////////
					for (int k = 0; k < NCOORDENADAS; k++) resultado += pow(centroideTest[k] - centroideTraining[k], 2);
					//+= pow(Point_P[i][k] - Point_Q[j][k], 2);				
					resultado = sqrt(resultado);
				//////////////////////////////////////////////////////////////////////////////////////////////////
					//std::cout << "RESULTADO: " << resultado << ", MINIMO: " << min << ", POSICION: " << colocarEn << std::endl;			
					if ( resultado <= min or min == -1)
					{
						min = resultado;
						colocarEn = j;
						//std::cout << "colocar en: " << i << std::endl;
					}
				}
				vectorPosition[colocarEn] += 1;
				/*std::cout << i << "--> " ;
				for(int i = 0; i < NCOORDENADAS; i++) std::cout << "[" << centroideTest[i] << ",";
				std::cout << "]" << std::endl;*/
			}
			//std::cout << "\t TIPO TRAINING: -> " << mHistoTrainig.getTipo() << std::endl;

			posicionTraining = fichTraining.tellg();
			fichTraining >> basura2	;
		}
		fichTraining.close();

		std::cout << "TIPO TEST: -> " << mHistoTest.getTipo() << std::endl;
		for (int i = 0; i < NCLUSTER; i++) std::cout << "\t" << i+1 << ": " << vectorPosition[i] << std::endl;
		std::cout << std::endl << std::endl;

		posicionTest = fichTest.tellg();
		fichTest >> basura;
	}
	fichTest.close();


	/*std::cout << "Punto_P ---> " << std::endl;
	for (int i = 0; i < NCOORDENADAS; i++)
	{
		Punto(Point_P[i]);
	}
	std::cout << "Punto_Q ---> " << std::endl;
	for (int i = 0; i < NCOORDENADAS; i++)
	{
		Punto(Point_Q[i]);
	}*/

	//std::cout << "Distancia del Punto_P al Punto_Q es: " << resultado << std::endl;
	std::cout << "COINCIDENCIAS: ";
	//Punto(posicion);



	//liberando memoria
	/*for (int i = 0; i < NCOORDENADAS; i++)
	{
		delete [] Point_P[i];
		delete [] Point_Q[i];
	}*/

	return 1;
}