#include "head.h"

#define DIMENSIONES 3

void leerDato(std::string &, std::string &, std::ifstream &);
void construyeMatriz(int [][DIMENSIONES], int *, int *, const std::string, const std::string );
void imprimeMatriz(int [][DIMENSIONES], int *, int *);
bool seguridad(int argc, char* argv[], std::ifstream &fichero)
{

	if (argc < 2) 
	{
		std::cerr << "Faltan parametros de ejecución" << std::endl;
		return false;
	}

	fichero.open(argv[argc-1]);
	if ( !fichero.is_open() )
	{
		std::cerr << "No se ha podido abrir el fichero de imágenes de Test" << std::endl;
		return false;
	}

	return true;
}



int main (int argc, char* argv[])
{
	std::ifstream fichResultado;

	int totales[DIMENSIONES][DIMENSIONES];
	int totalesOriginal[DIMENSIONES], totalesResultado[DIMENSIONES];
	for(int i = 0; i < DIMENSIONES; i++)
	{
		totalesOriginal[i] = totalesResultado[i] = 0;
		for(int j = 0; j < DIMENSIONES; j++) totales[i][j] = 0;
	}

	if ( !seguridad(argc, argv, fichResultado) ) return 0;

	std::string tipoOriginal, tipoResultado;

	long posicionTest = 0;
	char basura;
	while( !fichResultado.eof() )
	{
		std::string pertenece, perteneceEuclidea;

		if (posicionTest != 0) fichResultado.seekg(posicionTest, fichResultado.beg);
		leerDato(tipoOriginal, tipoResultado, fichResultado);

		std::cout << "Original: " << tipoOriginal << ", y Resultado: " << tipoResultado << std::endl;

		construyeMatriz(totales, totalesOriginal, totalesResultado, tipoOriginal, tipoResultado);
		
		posicionTest = fichResultado.tellg();
		fichResultado >> basura;
	}
	fichResultado.close();

	imprimeMatriz(totales, totalesOriginal, totalesResultado);

	return 1;
}

void leerDato(std::string &tOrigen, std::string &tResultado, std::ifstream &ifs)
{
	std::string nomImagen;

	{
	    // Se abre un archivo de lectura
	    boost::archive::text_iarchive ar(ifs);
	    // Escribe el nombre de la imagen, el tipo al que pertenece y su histograma
	    ar & nomImagen & tResultado;
	}
	if(nomImagen.find("NO CONDICIONADO") != std::string::npos) tOrigen = "NO CONDICIONADO";
	//else if(nomImagen.find("NO EMPAREJADO") != std::string::npos) tOrigen = "NO EMPAREJADO";
	else 
	{
		tOrigen = nomImagen.substr(nomImagen.find_last_of(" "), nomImagen.size());
		tOrigen = tOrigen.substr(1, tOrigen.size() - 5);
	}
}

void construyeMatriz(int totales[][DIMENSIONES], int * tOrigen, int * tResultado, const std::string tipoOriginal, const std::string tipoResultado)
{
	if(tipoOriginal == "SALINA")
	{
		tOrigen[0]++;

		if(tipoResultado == "SALINA") totales[0][0]++;
	//	if(tipoResultado == "NO EMPAREJADO") totales[0][1]++;
		if(tipoResultado == "NO CONDICIONADO") totales[0][1]++;
		if(tipoResultado == "CONDICIONADO") totales[0][2]++;
	}
	if(tipoOriginal == "NO CONDICIONADO")
	{
		tOrigen[1]++;

		if(tipoResultado == "SALINA") totales[1][0]++;
	//	if(tipoResultado == "NO EMPAREJADO") totales[2][1]++;
		if(tipoResultado == "NO CONDICIONADO") totales[1][1]++;
		if(tipoResultado == "CONDICIONADO") totales[1][2]++;
	}
	if(tipoOriginal == "CONDICIONADO")
	{
		tOrigen[2]++;

		if(tipoResultado == "SALINA") totales[2][0]++;
	//	if(tipoResultado == "NO EMPAREJADO") totales[3][1]++;
		if(tipoResultado == "NO CONDICIONADO") totales[2][1]++;
		if(tipoResultado == "CONDICIONADO") totales[2][2]++;
	}
	 
	if(tipoResultado == "SALINA") tResultado[0]++;
	if(tipoResultado == "NO CONDICIONADO") tResultado[1]++;
	if(tipoResultado == "CONDICIONADO") tResultado[2]++;
}

void imprimeMatriz(int totales[][DIMENSIONES], int *tOrigen, int *tResultado)
{
	std::cout << std::endl << "|||||||||| MATRIZ DE CONFUSIÓN ||||||||||" << std::endl;
	std::cout << "_________________________________________" << std::endl;

	std::cout << "\t|  ";
	for (int i = 0; i < DIMENSIONES; i++) std::cout << tResultado[i] << "\t|  ";
	std::cout << std::endl << "________|_______|_______|_______|_______|";	

	for (int i = 0; i < DIMENSIONES; i++)
	{
		std::cout << std::endl << "| " << tOrigen[i] << "\t|  ";
		for (int j = 0; j < DIMENSIONES; j++)	std::cout << totales[i][j] << "\t|  ";
	}
	std::cout << std::endl << std::endl;
}