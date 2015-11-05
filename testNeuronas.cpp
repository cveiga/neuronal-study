#include "MiVocabulario.h"

int main(int argc, char *argv[])
{
	MiVocabulario mVocabu(100, 9, "");
	
	std::ifstream ifs("pruebaGuardar.txt");

	long posicion = 0;
	char basura;
	bool continuar = true;
	while (!ifs.eof())
	{
		if (posicion != 0) ifs.seekg(posicion, ifs.beg);

		continuar = mVocabu.readVocabulary(ifs);
		mVocabu.imprimeVocabulario();

		posicion = ifs.tellg();
		ifs >> basura;
	}

 	std::cout << "FUERA" << std::endl;
	ifs.close();

	return 0;
}