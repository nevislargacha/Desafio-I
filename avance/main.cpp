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
#include <bitset>

using namespace std;

// Declaración de funciones
unsigned char* loadPixels(QString input, int &width, int &height);
bool exportImage(unsigned char* pixelData, int width, int height, QString archivoSalida);
unsigned int* loadSeedMasking(const char* nombreArchivo, int &seed, int &n_pixels);
unsigned char bitwiseXOR(unsigned char a, unsigned char b);
unsigned char shiftBits(unsigned char a, int n, bool left);
unsigned char rotateBits(unsigned char a, int n, bool left);
void verificarEnmascaramiento(unsigned char* imagen, unsigned int* enmascarado, int width, int height, int seed);

int main()
{
    unsigned char a = 0b10101010;
    unsigned char b = 0b11001100;

    cout << "Valor de a: " << bitset<8>(a) << endl;
    cout << "Valor de b: " << bitset<8>(b) << endl;
    cout << "Resultado de a XOR b: " << bitset<8>(bitwiseXOR(a, b)) << endl;
    cout << "Resultado de a desplazado 3 bits a la izquierda: " << bitset<8>(shiftBits(a, 3, true)) << endl;
    cout << "Resultado de a desplazado 3 bits a la derecha: " << bitset<8>(shiftBits(a, 3, false)) << endl;
    cout << "Resultado de a rotado 3 bits a la izquierda: " << bitset<8>(rotateBits(a, 3, true)) << endl;
    cout << "Resultado de a rotado 3 bits a la derecha: " << bitset<8>(rotateBits(a, 3, false)) << endl;

    QString archivoEntrada = "I_O.bmp";
    QString archivoSalida = "I_D.bmp";
    QString archivoEncriptado = "I_D_decrypted33.bmp";

    int height = 0, width = 0;
    unsigned char *pixelData = loadPixels(archivoEntrada, width, height);

    if (!pixelData) {
        cerr << "Error: No se pudo cargar la imagen de entrada." << endl;
        return -1;
    }

    for (int i = 0; i < width * height * 3; i += 3) {
        pixelData[i] = i;
        pixelData[i + 1] = i;
        pixelData[i + 2] = i;
    }

    bool exportI = exportImage(pixelData, width, height, archivoSalida);
    cout << (exportI ? "Exportación exitosa." : "Exportación fallida.") << endl;

    //Reemplazar datos en memoria por la imagen final escrita en disco
    unsigned char *encryptedData = loadPixels(archivoEncriptado, width, height);
    if (!encryptedData) {
        cerr << "Error: No se pudo volver a cargar la imagen modificada (I_D_decrypted33.bmp)." << endl;
        return -1;
    }

    int seed = 0, n_pixels = 0;
    unsigned int *maskingData = loadSeedMasking("M1.txt", seed, n_pixels);

    if (!maskingData) {
        cerr << "Error: No se pudo cargar el archivo de enmascaramiento." << endl;
        delete[] pixelData;
        return -1;
    }

    cout << "Dimensiones imagen: " << width << " x " << height << endl;
    cout << "Total píxeles esperados: " << width * height << ", enmascarados leídos: " << n_pixels << endl;

    for (int i = 0; i < n_pixels * 3; i += 3) {
        cout << "Pixel " << i / 3 << ": (" << maskingData[i] << ", "
             << maskingData[i + 1] << ", " << maskingData[i + 2] << ")" << endl;
    }

    verificarEnmascaramiento(encryptedData, maskingData, width, height, seed);

    delete[] pixelData;
    delete[] maskingData;
    delete[] encryptedData;

    return 0;
}

unsigned char bitwiseXOR(unsigned char a, unsigned char b) {
    return a ^ b;
}

unsigned char shiftBits(unsigned char a, int n, bool left) {
    return left ? (a << n) : (a >> n);
}

unsigned char rotateBits(unsigned char a, int n, bool left) {
    return left ? ((a << n) | (a >> (8 - n))) : ((a >> n) | (a << (8 - n)));
}

unsigned char* loadPixels(QString input, int &width, int &height) {
    QImage imagen(input);
    if (imagen.isNull()) {
        cout << "Error: No se pudo cargar la imagen BMP." << endl;
        return nullptr;
    }

    imagen = imagen.convertToFormat(QImage::Format_RGB888);
    width = imagen.width();
    height = imagen.height();

    int dataSize = width * height * 3;
    unsigned char* pixelData = new unsigned char[dataSize];

    for (int y = 0; y < height; ++y) {
        const uchar* srcLine = imagen.scanLine(y);
        unsigned char* dstLine = pixelData + y * width * 3;
        memcpy(dstLine, srcLine, width * 3);
    }

    return pixelData;
}

bool exportImage(unsigned char* pixelData, int width, int height, QString archivoSalida) {
    QImage outputImage(width, height, QImage::Format_RGB888);

    for (int y = 0; y < height; ++y) {
        memcpy(outputImage.scanLine(y), pixelData + y * width * 3, width * 3);
    }

    if (!outputImage.save(archivoSalida, "BMP")) {
        cout << "Error: No se pudo guardar la imagen BMP modificada." << endl;
        return false;
    }

    return true;
}

unsigned int* loadSeedMasking(const char* nombreArchivo, int &seed, int &n_pixels) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo." << endl;
        return nullptr;
    }

    archivo >> seed;
    int r, g, b;
    n_pixels = 0;
    while (archivo >> r >> g >> b) {
        n_pixels++;
    }

    archivo.close();
    archivo.open(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "Error al reabrir el archivo." << endl;
        return nullptr;
    }

    unsigned int* RGB = new unsigned int[n_pixels * 3];
    archivo >> seed;

    for (int i = 0; i < n_pixels * 3; i += 3) {
        archivo >> r >> g >> b;
        RGB[i] = r;
        RGB[i + 1] = g;
        RGB[i + 2] = b;
    }

    archivo.close();
    cout << "Semilla: " << seed << endl;
    cout << "Cantidad de píxeles leídos: " << n_pixels << endl;
    return RGB;
}

void verificarEnmascaramiento(unsigned char* imagen, unsigned int* enmascarado, int width, int height, int seed) {
    bool hayErrores = false;
    int errores = 0;
    int totalPixels = width * height;

    for (int i = 0; i < totalPixels && i < seed; ++i) {
        int idx = i * 3;

        unsigned char r_ori = imagen[idx];
        unsigned char g_ori = imagen[idx + 1];
        unsigned char b_ori = imagen[idx + 2];

        unsigned int r_msk = enmascarado[idx];
        unsigned int g_msk = enmascarado[idx + 1];
        unsigned int b_msk = enmascarado[idx + 2];

        if (r_ori != r_msk || g_ori != g_msk || b_ori != b_msk) {
            hayErrores = true;
            errores++;

            if (errores <= 10) {
                cout << "Diferencia en píxel " << i << ": "
                     << "Original (" << (int)r_ori << "," << (int)g_ori << "," << (int)b_ori << ") - "
                     << "Enmascarado (" << r_msk << "," << g_msk << "," << b_msk << ")" << endl;
            }
        }
    }

    if (!hayErrores) {
        cout << "Enmascaramiento verificado correctamente. No se encontraron diferencias." << endl;
    } else {
        cout << "Se encontraron " << errores << " píxeles diferentes entre la imagen y la máscara." << endl;
    }
}
