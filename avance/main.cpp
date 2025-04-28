/*
  * Programa demostrativo de manipulaciónprocesamiento de imágenes BMP en C++ usando Qt.
  *
  * Descripción:
  * Este programa realiza las siguientes tareas:
  * 1. Carga una imagen BMP desde un archivo (formato RGB sin usar estructuras ni STL).
  * 2. Modifica los valores RGB de los píxeles asignando un degradado artificial basado en su posición.
  * 3. Exporta la imagen modificada a un nuevo archivo BMP.
  * 4. Carga un archivo de texto que contiene una semilla (offset) y los resultados del enmascaramiento
  *    aplicados a una versión transformada de la imagen, en forma de tripletas RGB.
  * 5. Muestra en consola los valores cargados desde el archivo de enmascaramiento.
  * 6. Gestiona la memoria dinámicamente, liberando los recursos utilizados.
  *
  * Entradas:
  * - Archivo de imagen BMP de entrada ("I_O.bmp").
  * - Archivo de salida para guardar la imagen modificada ("I_D.bmp").
  * - Archivo de texto ("M1.txt") que contiene:
  *     • Una línea con la semilla inicial (offset).
  *     • Varias líneas con tripletas RGB resultantes del enmascaramiento.
  *
  * Salidas:
  * - Imagen BMP modificada ("I_D.bmp").
  * - Datos RGB leídos desde el archivo de enmascaramiento impresos por consola.
  *
  * Requiere:
  * - Librerías Qt para manejo de imágenes (QImage, QString).
  * - No utiliza estructuras ni STL. Solo arreglos dinámicos y memoria básica de C++.
  *
  * Autores: Augusto Salazar Y Aníbal Guerra
  * Fecha: 06/04/2025
  * Asistencia de ChatGPT para mejorar la forma y presentación del código fuente
  */

#include <fstream>
#include <iostream>
#include <QCoreApplication>
#include <QImage>

using namespace std;

unsigned char* loadPixels(QString input, int &width, int &height);
unsigned char* loadPixelsMask(QString input, int &width_mask, int &height_mask);
bool exportImage(unsigned char* pixelData, int width, int height, QString archivoSalida);
unsigned int* loadSeedMasking(const char* nombreArchivo, int &seed, int &n_pixels);
unsigned char* xorImages(unsigned char* img1, unsigned char* img2, int width, int height);
unsigned char shiftRight(unsigned char byte, int bits);
unsigned char shiftLeft(unsigned char byte, int bits);
unsigned char* shiftImage(unsigned char* img, int width, int height, int bits, bool right);
unsigned char rotateRight(unsigned char byte, int bits);
unsigned char rotateLeft(unsigned char byte, int bits);
unsigned char* rotateImage(unsigned char* img, int width, int height, int bits, bool right);
void enmascararYGuardar(unsigned char* imgTransformada, int imgWidth, int imgHeight,
                        unsigned char* mask, int maskWidth, int maskHeight, int s,
                        const string& filename);

int main()
{
    // Definición de rutas de archivo de entrada (imagen original) y salida (imagen modificada)
    QString imagenOriginal = "I_O.bmp";
    QString imagenSalida = "I_D.bmp";
    QString imagenAleatoria = "I_M.bmp";
    QString imagenMascara = "M.bmp";
    QString imagenPrimeraOperacion = "P1.bmp";
    QString imagenSegundaOperacion = "P2.bmp";
    QString imagenTerceraOperacion = "P3.bmp";
    string archivoTXT1 = "M1.txt";
    string archivoTXT2 = "M2.txt";

    // Variables para almacenar las dimensiones de la imagen
    int height = 0;
    int width = 0;
    int width_mask = 0;
    int height_mask = 0;

    // Variables para almacenar la semilla y el número de píxeles leídos del archivo de enmascaramiento
    int seed = 0;
    int n_pixels = 0;

    // Carga la imagenimagenOriginal BMP en memoria dinámica y obtiene ancho y alto
    unsigned char *pixelDataimagenOriginal = loadPixels(imagenOriginal, width, height);
    // Carga la imagenimagenOriginal BMP en memoria dinámica y obtiene ancho y alto
    unsigned char *pixelDataimagenSalida = loadPixels(imagenSalida, width, height);
    // Carga la imagenAleatoria BMP en memoria dinámica y obtiene ancho y alto
    unsigned char *pixelDataimagenAleatoria = loadPixels(imagenAleatoria, width, height);
    // Carga la imagenMascara original BMP en memoria dinámica y obtiene ancho y alto
    unsigned char *pixelDataimagenMascara = loadPixelsMask(imagenMascara, width_mask, height_mask);
    // Carga la imagenimagenOriginal BMP en memoria dinámica y obtiene ancho y alto
    unsigned char *pixelDataimagenPrimeraOperacion = loadPixels(imagenPrimeraOperacion, width, height);
    // Carga la imagenAleatoria BMP en memoria dinámica y obtiene ancho y alto
    unsigned char *pixelDataimagenSegundaOperacion = loadPixels(imagenSegundaOperacion, width, height);
    // Carga la imagenMascara original BMP en memoria dinámica y obtiene ancho y alto
    unsigned char *pixelDataimagenTerceraOperacion = loadPixels(imagenTerceraOperacion, width, height);

    /*
    En esta seccion se encutran los siguientes puntos:

    [15%] Escribir funciones para realizar operaciones a nivel de bit, tales como XOR, desplazamiento y rotación de bits.
    El máximo número de bits a rotar o desplazar es de 8.
    [10%] Realizar experimentos con las diferentes operaciones a nivel de bit y analizar el efecto de las transformaciones sobre la integridad
    de los datos y su utilidad en un escenario de encriptación básica de información.

    --------------------------------------------------------------------------------------------------------------------------

    // Carga la imagen BMP en memoria dinámica y obtiene ancho y alto
    unsigned char *pixelData1 = loadPixels(imagenPrimeraOperacion, width, height);

    // Carga la imagen BMP en memoria dinámica y obtiene ancho y alto
    unsigned char *pixelData2 = loadPixels(imagenAleatoria, width, height);

    // Carga la imagen original BMP en memoria dinámica y obtiene ancho y alto
    unsigned char *pixelData3 = loadPixels(imagenOriginal, width, height);

    // Carga la imagen BMP en memoria dinámica y obtiene ancho y alto
    unsigned char *pixelData4 = xorImages(pixelData1, pixelData2, width, height);

    // Carga la imagen BMP en memoria dinámica y obtiene ancho y alto
    unsigned char *pixelData5 = shiftImage(pixelData3, width, height, 5, false);

    // Carga la imagen BMP en memoria dinámica y obtiene ancho y alto
    unsigned char *pixelData6 = rotateImage(pixelData3, width, height, 4, true);

    // Exporta la imagen encriptada a un nuevo archivo BMP
    bool exportI = exportImage(pixelData4, width, height, "ExperimentoOperacionXOR.bmp");
    if (exportI) {
        cout << "Imagen guardada correctamente." << endl;
    } else {
        cout << "Error al guardar la imagen." << endl;
    }

    bool exportI2 = exportImage(pixelData5, width, height, "ExperimentoOperacionDesplazamiento.bmp");
    if (exportI2) {
        cout << "Imagen guardada correctamente." << endl;
    } else {
        cout << "Error al guardar la imagen." << endl;
    }

    bool exportI3 = exportImage(pixelData6, width, height, "ExperimentoOperacionRotacion.bmp");
    if (exportI3) {
        cout << "Imagen guardada correctamente." << endl;
    } else {
        cout << "Error al guardar la imagen." << endl;
    }

    // Libera la memoria usada para los píxeles
    delete[] pixelData1;
    pixelData1 = nullptr;
    delete[] pixelData2;
    pixelData2 = nullptr;
    delete[] pixelData3;
    pixelData3 = nullptr;
    delete[] pixelData4;
    pixelData4 = nullptr;
    delete[] pixelData5;
    pixelData5 = nullptr;
    delete[] pixelData6;
    pixelData6 = nullptr;
    */

    /*
    En esta seccion se encutra el siguiente punto, recreando el paso a paso del caso:

    10%] Implementar un algoritmo que permita verificar el resultado del enmascaramiento, comparando la imagen transformada y
    la máscara contra los archivos de resultados.

    --------------------------------------------------------------------------------------------------------------------------

    // se almacena la imagen BMP del XOR ebtre pixelDataimagenOriginal y pixelDataimagenAleatoria
    unsigned char *pixelDataPrimerXOR = xorImages(pixelDataimagenOriginal, pixelDataimagenAleatoria, width, height);

    enmascararYGuardar(pixelDataPrimerXOR, width, height, pixelDataimagenMascara, width_mask, height_mask, 100,"pruebaM1.txt");

    // Exporta la imagen encriptada a un nuevo archivo BMP
    bool exportI = exportImage(pixelDataPrimerXOR, width, height, "pruebaP1.bmp");
    if (exportI) {
        cout << "Imagen guardada correctamente." << endl;
    } else {
        cout << "Error al guardar la imagen." << endl;
    }

    // se almacena la imagen BMP de una rotacion de 3 bits a la derecha de la imagen pixelDataPrimerXOR
    unsigned char *pixelDataRotacionDerecha = rotateImage(pixelDataPrimerXOR, width, height, 3, true);

    enmascararYGuardar(pixelDataRotacionDerecha, width, height, pixelDataimagenMascara, width_mask, height_mask, 15,"pruebaM2.txt");

    bool exportI2 = exportImage(pixelDataRotacionDerecha, width, height, "pruebaP2.bmp");
    if (exportI2) {
        cout << "Imagen guardada correctamente." << endl;
    } else {
        cout << "Error al guardar la imagen." << endl;
    }

    // se almacena la imagen BMP del XOR ebtre pixelDataRotacionDerecha y pixelDataimagenAleatoria
    unsigned char *pixelDataSegungoXOR = xorImages(pixelDataRotacionDerecha, pixelDataimagenAleatoria, width, height);

    // Exporta la imagen encriptada a un nuevo archivo BMP
    bool exportI3 = exportImage(pixelDataSegungoXOR, width, height, "pruebaP3.bmp");
    if (exportI3) {
        cout << "Imagen guardada correctamente." << endl;
    } else {
        cout << "Error al guardar la imagen." << endl;
    }

    delete[] pixelDataPrimerXOR;
    pixelDataPrimerXOR = nullptr;
    delete[] pixelDataRotacionDerecha;
    pixelDataRotacionDerecha = nullptr;
    delete[] pixelDataSegungoXOR;
    pixelDataSegungoXOR = nullptr;

    */

    QString nombrePrimeraImagen = "P3.bmp";
    QString nombreSegundaImagen = "I_M.bmp";
    QString nombreMascara = "M.bmp";
    QString nombreDescargaImagenTransformada = "ImagenTransformada1.bmp";

    // Carga una imagen BMP en memoria dinámica y obtiene ancho y alto
    unsigned char *pixelDataPrimeraImagenBMP = loadPixels(nombrePrimeraImagen, width, height);

    // Carga una imagen BMP en memoria dinámica y obtiene ancho y alto
    unsigned char *pixelDataSegundaImagenBMP = loadPixels(nombreSegundaImagen, width, height);

    // Carga la mascaraascara en memoria dinámica y obtiene ancho y alto
    unsigned char *pixelDataMascara = loadPixelsMask(nombreMascara, width_mask, height_mask);

    /*
    De este punto en adelante se realiza las operaciones a nivel de bit antes del enmascaramiento, a la imagen *pixelDataImagenBMP
    y se almacena la imagen transformada con la operacion. Las opereaciones a utilizar son:
    unsigned char* xorImages(unsigned char* img1, unsigned char* img2, int width, int height);
    unsigned char* shiftImage(unsigned char* img, int width, int height, int bits, bool right);
    unsigned char* rotateImage(unsigned char* img, int width, int height, int bits, bool right);

    NOTA: Recordar que el máximo número de bits a rotar o desplazar es de 8.
    */

    //se almacena la imagen BMP de la operacion entre nombrePrimeraImagen y nombreSegundaImagen
    unsigned char *pixelDataOperacion = xorImages(pixelDataPrimeraImagenBMP, pixelDataSegundaImagenBMP, width, height);

    enmascararYGuardar(pixelDataOperacion, width, height, pixelDataMascara, width_mask, height_mask, 15, "EnmascaramientoImagenTransformada1.txt");

    //Carga los datos de enmascaramiento desde un archivo .txt (semilla + valores RGB)
    unsigned int *maskingData = loadSeedMasking("EnmascaramientoImagenTransformada1.txt", seed, n_pixels);

    // Muestra en consola los primeros valores RGB leídos desde el archivo de enmascaramiento
    for (int i = 0; i < n_pixels * 3; i += 3) {
        if(pixelDataOperacion[i] != maskingData[i] ||
            pixelDataOperacion[i + 1] != maskingData[i + 1] ||
            pixelDataOperacion[i + 2] != maskingData[i + 2]){
            cout << "La imagen transformada no coincide con el enmascaramiento" << endl;
            break;
        }else{
            // Exporta la imagen modificada a un nuevo archivo BMP
            bool exportI = exportImage(pixelDataOperacion, width, height, nombreDescargaImagenTransformada);

            // Muestra si la exportación fue exitosa (true o false)
            cout << exportI << endl;
        }
    }
    //Libera la memoria usada para los píxeles
    delete[] pixelDataOperacion;
    pixelDataOperacion = nullptr;
    delete[] maskingData;
    maskingData = nullptr;







    delete[] pixelDataimagenOriginal;
    pixelDataimagenOriginal = nullptr;
    delete[] pixelDataimagenSalida;
    pixelDataimagenSalida = nullptr;
    delete[] pixelDataimagenAleatoria;
    pixelDataimagenAleatoria = nullptr;
    delete[] pixelDataimagenMascara;
    pixelDataimagenMascara = nullptr;
    delete[] pixelDataimagenPrimeraOperacion;
    pixelDataimagenPrimeraOperacion = nullptr;
    delete[] pixelDataimagenSegundaOperacion;
    pixelDataimagenSegundaOperacion = nullptr;
    delete[] pixelDataimagenTerceraOperacion;
    pixelDataimagenTerceraOperacion = nullptr;

    return 0; // Fin del programa
}


unsigned char* loadPixels(QString input, int &width, int &height){
    /*
  * @brief Carga una imagen BMP desde un archivo y extrae los datos de píxeles en formato RGB.
  *
  * Esta función utiliza la clase QImage de Qt para abrir una imagen en formato BMP, convertirla al
  * formato RGB888 (24 bits: 8 bits por canal), y copiar sus datos de píxeles a un arreglo dinámico
  * de tipo unsigned char. El arreglo contendrá los valores de los canales Rojo, Verde y Azul (R, G, B)
  * de cada píxel de la imagen, sin rellenos (padding).
  *
  * @param input Ruta del archivo de imagen BMP a cargar (tipo QString).
  * @param width Parámetro de salida que contendrá el ancho de la imagen cargada (en píxeles).
  * @param height Parámetro de salida que contendrá la altura de la imagen cargada (en píxeles).
  * @return Puntero a un arreglo dinámico que contiene los datos de los píxeles en formato RGB.
  *         Devuelve nullptr si la imagen no pudo cargarse.
  *
  * @note Es responsabilidad del usuario liberar la memoria asignada al arreglo devuelto usando `delete[]`.
  */

    // Cargar la imagen BMP desde el archivo especificado (usando Qt)
    QImage imagen(input);

    // Verifica si la imagen fue cargada correctamente
    if (imagen.isNull()) {
        cout << "Error: No se pudo cargar la imagen BMP." << endl;
        return nullptr; // Retorna un puntero nulo si la carga falló
    }

    // Convierte la imagen al formato RGB888 (3 canales de 8 bits sin transparencia)
    imagen = imagen.convertToFormat(QImage::Format_RGB888);

    // Obtiene el ancho y el alto de la imagen cargada
    width = imagen.width();
    height = imagen.height();

    // Calcula el tamaño total de datos (3 bytes por píxel: R, G, B)
    int dataSize = width * height * 3;

    // Reserva memoria dinámica para almacenar los valores RGB de cada píxel
    unsigned char* pixelData = new unsigned char[dataSize];

    // Copia cada línea de píxeles de la imagen Qt a nuestro arreglo lineal
    for (int y = 0; y < height; ++y) {
        const uchar* srcLine = imagen.scanLine(y);              // Línea original de la imagen con posible padding
        unsigned char* dstLine = pixelData + y * width * 3;     // Línea destino en el arreglo lineal sin padding
        memcpy(dstLine, srcLine, width * 3);                    // Copia los píxeles RGB de esa línea (sin padding)
    }

    // Retorna el puntero al arreglo de datos de píxeles cargado en memoria
    return pixelData;
}

unsigned char* loadPixelsMask(QString input, int &width_mask, int &height_mask){
    /*
  * @brief Carga una imagen BMP desde un archivo y extrae los datos de píxeles en formato RGB.
  *
  * Esta función utiliza la clase QImage de Qt para abrir una imagen en formato BMP, convertirla al
  * formato RGB888 (24 bits: 8 bits por canal), y copiar sus datos de píxeles a un arreglo dinámico
  * de tipo unsigned char. El arreglo contendrá los valores de los canales Rojo, Verde y Azul (R, G, B)
  * de cada píxel de la imagen, sin rellenos (padding).
  *
  * @param input Ruta del archivo de imagen BMP a cargar (tipo QString).
  * @param width Parámetro de salida que contendrá el ancho de la imagen cargada (en píxeles).
  * @param height Parámetro de salida que contendrá la altura de la imagen cargada (en píxeles).
  * @return Puntero a un arreglo dinámico que contiene los datos de los píxeles en formato RGB.
  *         Devuelve nullptr si la imagen no pudo cargarse.
  *
  * @note Es responsabilidad del usuario liberar la memoria asignada al arreglo devuelto usando `delete[]`.
  */

    // Cargar la imagen BMP desde el archivo especificado (usando Qt)
    QImage imagen(input);

    // Verifica si la imagen fue cargada correctamente
    if (imagen.isNull()) {
        cout << "Error: No se pudo cargar la imagen BMP." << endl;
        return nullptr; // Retorna un puntero nulo si la carga falló
    }

    // Convierte la imagen al formato RGB888 (3 canales de 8 bits sin transparencia)
    imagen = imagen.convertToFormat(QImage::Format_RGB888);

    // Obtiene el ancho y el alto de la imagen cargada
    width_mask = imagen.width();
    height_mask = imagen.height();

    // Calcula el tamaño total de datos (3 bytes por píxel: R, G, B)
    int dataSize = width_mask * height_mask * 3;

    // Reserva memoria dinámica para almacenar los valores RGB de cada píxel
    unsigned char* pixelData = new unsigned char[dataSize];

    // Copia cada línea de píxeles de la imagen Qt a nuestro arreglo lineal
    for (int y = 0; y < height_mask; ++y) {
        const uchar* srcLine = imagen.scanLine(y);              // Línea original de la imagen con posible padding
        unsigned char* dstLine = pixelData + y * width_mask * 3;     // Línea destino en el arreglo lineal sin padding
        memcpy(dstLine, srcLine, width_mask * 3);                    // Copia los píxeles RGB de esa línea (sin padding)
    }

    // Retorna el puntero al arreglo de datos de píxeles cargado en memoria
    return pixelData;
}

bool exportImage(unsigned char* pixelData, int width,int height, QString archivoSalida){
    /*
  * @brief Exporta una imagen en formato BMP a partir de un arreglo de píxeles en formato RGB.
  *
  * Esta función crea una imagen de tipo QImage utilizando los datos contenidos en el arreglo dinámico
  * `pixelData`, que debe representar una imagen en formato RGB888 (3 bytes por píxel, sin padding).
  * A continuación, copia los datos línea por línea a la imagen de salida y guarda el archivo resultante
  * en formato BMP en la ruta especificada.
  *
  * @param pixelData Puntero a un arreglo de bytes que contiene los datos RGB de la imagen a exportar.
  *                  El tamaño debe ser igual a width * height * 3 bytes.
  * @param width Ancho de la imagen en píxeles.
  * @param height Alto de la imagen en píxeles.
  * @param archivoSalida Ruta y nombre del archivo de salida en el que se guardará la imagen BMP (QString).
  *
  * @return true si la imagen se guardó exitosamente; false si ocurrió un error durante el proceso.
  *
  * @note La función no libera la memoria del arreglo pixelData; esta responsabilidad recae en el usuario.
  */

    // Crear una nueva imagen de salida con el mismo tamaño que la original
    // usando el formato RGB888 (3 bytes por píxel, sin canal alfa)
    QImage outputImage(width, height, QImage::Format_RGB888);

    // Copiar los datos de píxeles desde el buffer al objeto QImage
    for (int y = 0; y < height; ++y) {
        // outputImage.scanLine(y) devuelve un puntero a la línea y-ésima de píxeles en la imagen
        // pixelData + y * width * 3 apunta al inicio de la línea y-ésima en el buffer (sin padding)
        // width * 3 son los bytes a copiar (3 por píxel)
        memcpy(outputImage.scanLine(y), pixelData + y * width * 3, width * 3);
    }

    // Guardar la imagen en disco como archivo BMP
    if (!outputImage.save(archivoSalida, "BMP")) {
        // Si hubo un error al guardar, mostrar mensaje de error
        cout << "Error: No se pudo guardar la imagen BMP modificada.";
        return false; // Indica que la operación falló
    } else {
        // Si la imagen fue guardada correctamente, mostrar mensaje de éxito
        cout << "Imagen BMP modificada guardada como " << archivoSalida.toStdString() << endl;
        return true; // Indica éxito
    }

}

unsigned int* loadSeedMasking(const char* nombreArchivo, int &seed, int &n_pixels){
    /*
  * @brief Carga la semilla y los resultados del enmascaramiento desde un archivo de texto.
  *
  * Esta función abre un archivo de texto que contiene una semilla en la primera línea y,
  * a continuación, una lista de valores RGB resultantes del proceso de enmascaramiento.
  * Primero cuenta cuántos tripletes de píxeles hay, luego reserva memoria dinámica
  * y finalmente carga los valores en un arreglo de enteros.
  *
  * @param nombreArchivo Ruta del archivo de texto que contiene la semilla y los valores RGB.
  * @param seed Variable de referencia donde se almacenará el valor entero de la semilla.
  * @param n_pixels Variable de referencia donde se almacenará la cantidad de píxeles leídos
  *                 (equivalente al número de líneas después de la semilla).
  *
  * @return Puntero a un arreglo dinámico de enteros que contiene los valores RGB
  *         en orden secuencial (R, G, B, R, G, B, ...). Devuelve nullptr si ocurre un error al abrir el archivo.
  *
  * @note Es responsabilidad del usuario liberar la memoria reservada con delete[].
  */

    // Abrir el archivo que contiene la semilla y los valores RGB
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        // Verificar si el archivo pudo abrirse correctamente
        cout << "No se pudo abrir el archivo." << endl;
        return nullptr;
    }

    // Leer la semilla desde la primera línea del archivo
    archivo >> seed;

    int r, g, b;

    // Contar cuántos grupos de valores RGB hay en el archivo
    // Se asume que cada línea después de la semilla tiene tres valores (r, g, b)
    while (archivo >> r >> g >> b) {
        n_pixels++;  // Contamos la cantidad de píxeles
    }

    // Cerrar el archivo para volver a abrirlo desde el inicio
    archivo.close();
    archivo.open(nombreArchivo);

    // Verificar que se pudo reabrir el archivo correctamente
    if (!archivo.is_open()) {
        cout << "Error al reabrir el archivo." << endl;
        return nullptr;
    }

    // Reservar memoria dinámica para guardar todos los valores RGB
    // Cada píxel tiene 3 componentes: R, G y B
    unsigned int* RGB = new unsigned int[n_pixels * 3];

    // Leer nuevamente la semilla desde el archivo (se descarta su valor porque ya se cargó antes)
    archivo >> seed;

    // Leer y almacenar los valores RGB uno por uno en el arreglo dinámico
    for (int i = 0; i < n_pixels * 3; i += 3) {
        archivo >> r >> g >> b;
        RGB[i] = r;
        RGB[i + 1] = g;
        RGB[i + 2] = b;
    }

    // Cerrar el archivo después de terminar la lectura
    archivo.close();

    // Mostrar información de control en consola
    cout << "Semilla: " << seed << endl;
    cout << "Cantidad de píxeles leídos: " << n_pixels << endl;

    // Retornar el puntero al arreglo con los datos RGB
    return RGB;
}

unsigned char* xorImages(unsigned char* img1, unsigned char* img2, int width, int height) {
    /*
     * @brief Realiza una operación XOR bit a bit entre dos imágenes en formato RGB.
     *
     * Esta función toma dos arreglos de píxeles (img1 y img2) que representan imágenes en
     * formato RGB (sin padding) y genera una nueva imagen donde cada componente (R, G, B)
     * de cada píxel es el resultado de aplicar XOR entre los correspondientes componentes
     * de las dos imágenes.
     *
     * @param img1 Puntero al primer arreglo de píxeles (unsigned char*).
     * @param img2 Puntero al segundo arreglo de píxeles (unsigned char*).
     * @param width Ancho de las imágenes (en píxeles).
     * @param height Alto de las imágenes (en píxeles).
     * @return Puntero a un nuevo arreglo de píxeles con el resultado del XOR.
     *         Devuelve nullptr si ocurre un error.
     *
     * @note El usuario debe liberar el arreglo devuelto con `delete[]`.
     */

    if (img1 == nullptr || img2 == nullptr) {
        cout << "Error: Una de las imágenes es nula." << endl;
        return nullptr;
    }

    int dataSize = width * height * 3; // Cada píxel tiene 3 componentes (R, G, B)

    // Reserva memoria para la imagen resultante
    unsigned char* result = new unsigned char[dataSize];

    // Realiza el XOR píxel por píxel (canal por canal)
    for (int i = 0; i < dataSize; ++i) {
        result[i] = img1[i] ^ img2[i];
    }

    return result;
}


unsigned char shiftRight(unsigned char byte, int bits) {
    return byte >> bits;
}

unsigned char shiftLeft(unsigned char byte, int bits) {
    return byte << bits;
}

unsigned char* shiftImage(unsigned char* img, int width, int height, int bits, bool right) {
    /*
     * @brief Realiza un desplazamiento de bits (hacia la derecha o izquierda) sobre cada componente RGB de una imagen.
     *
     * @param img Puntero al arreglo de píxeles original (unsigned char*).
     * @param width Ancho de la imagen (en píxeles).
     * @param height Alto de la imagen (en píxeles).
     * @param bits Cantidad de bits a desplazar.
     * @param right true para desplazamiento a la derecha, false para desplazamiento a la izquierda.
     * @return Puntero a un nuevo arreglo de píxeles con el resultado del desplazamiento.
     *         El usuario debe liberar el arreglo con `delete[]`.
     */

    if (img == nullptr) {
        cout << "Error: Imagen nula." << endl;
        return nullptr;
    }

    int dataSize = width * height * 3; // Cada píxel tiene 3 componentes (R, G, B)

    // Reserva memoria para la imagen desplazada
    unsigned char* result = new unsigned char[dataSize];

    // Aplica el desplazamiento a cada byte (canal R, G o B)
    for (int i = 0; i < dataSize; ++i) {
        if (right) {
            result[i] = shiftRight(img[i], bits);
        } else {
            result[i] = shiftLeft(img[i], bits);
        }
    }

    return result;
}

unsigned char rotateRight(unsigned char byte, int bits) {
    return (byte >> bits) | (byte << (8 - bits));
}

unsigned char rotateLeft(unsigned char byte, int bits) {
    return (byte << bits) | (byte >> (8 - bits));
}

unsigned char* rotateImage(unsigned char* img, int width, int height, int bits, bool right) {
    /*
     * @brief Rota los bits de cada componente RGB de una imagen hacia la derecha o izquierda.
     *
     * @param img Puntero al arreglo de píxeles original (unsigned char*).
     * @param width Ancho de la imagen (en píxeles).
     * @param height Alto de la imagen (en píxeles).
     * @param bits Cantidad de bits a rotar.
     * @param right true para rotar a la derecha, false para rotar a la izquierda.
     * @return Puntero a un nuevo arreglo de píxeles con los bits rotados.
     *         El usuario debe liberar el arreglo con `delete[]`.
     */

    if (img == nullptr) {
        cout << "Error: Imagen nula." << endl;
        return nullptr;
    }

    int dataSize = width * height * 3; // Cada píxel tiene 3 componentes (R, G, B)

    // Reserva memoria para la imagen rotada
    unsigned char* result = new unsigned char[dataSize];

    // Aplica la rotación de bits a cada byte (R, G o B)
    for (int i = 0; i < dataSize; ++i) {
        if (right) {
            result[i] = rotateRight(img[i], bits);
        } else {
            result[i] = rotateLeft(img[i], bits);
        }
    }

    return result;
}

void enmascararYGuardar(unsigned char* imgTransformada, int imgWidth, int imgHeight,
                        unsigned char* mask, int maskWidth, int maskHeight, int s,
                        const string& filename) {
    int maskDataSize = maskWidth * maskHeight * 3;
    int imgDataSize = imgWidth * imgHeight * 3;

    // Verificar que la imagen transformada sea lo suficientemente grande
    if (imgDataSize < maskDataSize) {
        cerr << "La imagen es demasiado pequeña para aplicar la máscara." << endl;
        return;
    }

    // Abrir archivo de salida
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cerr << "Error abriendo archivo de salida." << endl;
        return;
    }

    // Escribir el desplazamiento en la primera línea
    outFile << s << endl;

    // Aplicar la máscara sumando los valores RGB
    for (int i = 0; i < maskDataSize; i += 3) {
        int r = imgTransformada[s + i + 0] + mask[i + 0];
        int g = imgTransformada[s + i + 1] + mask[i + 1];
        int b = imgTransformada[s + i + 2] + mask[i + 2];

        outFile << r << " " << g << " " << b << endl;
    }

    outFile.close();
}
