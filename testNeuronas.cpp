#include "MiHistograma.h"

int main(int argc, char *argv[])
{
	MiHistograma mHisto(100, 9, "");
	
	std::ifstream ifs("pruebaGuardar.txt");

	long posicion = 0;
	char basura;
	bool continuar = true;
	while (!ifs.eof())
	{
		if (posicion != 0) ifs.seekg(posicion, ifs.beg);

		continuar = mHisto.readHistograma(ifs);
		mHisto.imprimeHistograma();

		posicion = ifs.tellg();
		ifs >> basura;
	}

 	std::cout << "FUERA" << std::endl;
	ifs.close();

	return 0;
}