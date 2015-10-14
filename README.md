# neuronal-study
For compile, you need say it the path of Opencv and others external libs:
 i.e. g++ descomponerImagen.cpp -o prueba1 `pkg-config opencv --cflags --libs` -I/home/usuario/dlib -I/home/usuario/NEURONAS/neuronal-study -lboost_serialization

 pkg-config opencv --cflags --libs --------->> for OpenCV libraries
 -I/home/usuario/dlib  --------------------->> for dlib (is not necessary)
 -I/home/usuario/NEURONAS/neuronal-study  -->> for head of includes
 -lboost_serialization---------------------->> for boost libraries to serialize objects


For run program:
 it need the path of file-path
 i.e. ./prueba1 ../rutas/totalSalina.txt