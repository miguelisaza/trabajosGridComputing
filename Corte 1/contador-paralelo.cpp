#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>

using namespace std;

int main () {
    vector<string> totalLineas;
    string linea;
    fstream archivo;

    // variables para OpenMP

    int threadId; // thread id
    int numThreads; // # de threads del CPU

    // variables globales del programa

    int palabras = 0; // contador de palabras
    int totalLineasArchivo = 0; //

    // fstream in del archivo
    // principal cuello de botella al ser una IO
    archivo.open( "texto.txt", ios::in );

    // si hay un archivo...
    if ( archivo ) {
        /* 
        Se cuentan la cantidad total de líneas 
        que tiene el archivo para posteriormente
        dividir las lineas por thread.
        */
        while (getline(archivo, linea)) {
            totalLineas.push_back(linea);
            totalLineasArchivo += 1;
        }

        // cerramos el archivo
        archivo.close();

        // se ejecuta la directiva omp parallel private
        #pragma omp parallel private(threadId) reduction(+:palabras)
        {
            threadId = omp_get_thread_num(); // OMP thread id (variable privada)
            numThreads = omp_get_num_threads(); // OMP # de threads

            /* 
            Se obtienen las lineas por thread para que el trabajo
            sea igual para cada thread.

            Este vendría siendo el tamaño de las tareas
            */
            int lineasPorThread = totalLineasArchivo / numThreads;

            /*
            Limite superior:
            Este límite define hasta cuál línea
            leerá cada thread.
            */
            int limiteSuperior = lineasPorThread * (threadId + 1);

            // el último límite llega hasta las lineas totales del archivo
            if ((threadId + 1) == numThreads) {
                limiteSuperior = totalLineasArchivo;
            }
             /*
             Se recorren las linas por thread
             hasta su límite superior.
             
             Este es el hotspot al ser la zona de 
             alta complejidad computacional y es
             la que se paralelizará
              */
            for (int i = lineasPorThread * threadId; i < limiteSuperior; i++){
                /*
                Cada ejecucion interna de este for
                representa una tarea
                */
                for (int j = 0; j < totalLineas[i].size(); j++) {
                    if(totalLineas[i][j-1] == ' ' && totalLineas[i][j]!=' ' ) {
                        ++palabras;
                    }
                }

                if(totalLineas[i][0]!= ' ') palabras++;
            }
            /*
            Region crítica:
            se ejecuta por un solo thread a la vez

            Esta salida se considera un bloque no
            paralelizable
             */
            #pragma omp critical
            {
            cout<<"Palabras en el hilo "<<threadId+1<<": "<<palabras<<"\n";
            }
        }

        cout<<"Palabras en el texto: "<<palabras<<"\n";
    }

    else {
        cout<<"Error: Archivo no encontrado. \n";
    }

    return 0;
}

