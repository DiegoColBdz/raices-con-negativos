#include "mbed.h"
#include <math.h>
#include <stdbool.h>

const int numRows = 4;
const int numCols = 4;

char keyMap[numRows][numCols] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

DigitalOut rowPins[numRows] = {DigitalOut(D2), DigitalOut(D3), DigitalOut(D4), DigitalOut(D5)};
DigitalIn colPins[numCols] = {DigitalIn(D6), DigitalIn(D7), DigitalIn(D8), DigitalIn(D9)};

char inputBuffer[50];
int bufferIndex = 0;
bool recording = false;

int coefficients[3]; // Array para almacenar los coeficientes a, b y c
int coefficientIndex = 0; // Índice para el coeficiente actual

void calculateRoots(int a, int b, int c) {
    int discriminant = b * b - 4 * a * c;
    if (discriminant > 0) {
        // Dos raíces enteras
        int root1 = (-b + sqrt(discriminant)) / (2 * a);
        int root2 = (-b - sqrt(discriminant)) / (2 * a);
        printf("Raíces enteras: %d y %d\n", root1, root2);
    } else if (discriminant == 0) {
        // Una raíz real
        int root = -b / (2 * a);
        printf("Raíz entera única: %d\n", root);
    } else {
        // Raíces complejas (no aplicable en este caso)
        printf("El polinomio no tiene raíces enteras.\n");
    }
}

void processKey(char key) {
    if (recording) {
        if (key == '*') {
            inputBuffer[bufferIndex] = '\0';  // Null-terminate the string
            printf("Coeficiente %c: %s\n", 'a' + coefficientIndex, inputBuffer);

            // Interpretar '#' como signo negativo al inicio
            int coefficientValue;
            if (inputBuffer[0] == '#') {
                if (sscanf(inputBuffer + 1, "%d", &coefficientValue) == 1) {
                    coefficientValue = -coefficientValue;
                }
            } else {
                if (sscanf(inputBuffer, "%d", &coefficientValue) != 1) {
                    printf("Entrada inválida. Por favor, ingrese un número válido.\n");
                    bufferIndex = 0;
                    return;
                }
            }

            coefficients[coefficientIndex] = coefficientValue;
            coefficientIndex++;
            if (coefficientIndex < 3) {
                printf("Por favor, ingrese el coeficiente %c: ", 'a' + coefficientIndex);
                bufferIndex = 0;
            } else {
                printf("Coeficientes ingresados: a=%d, b=%d, c=%d\n", coefficients[0], coefficients[1], coefficients[2]);
                // Calcula las raíces
                calculateRoots(coefficients[0], coefficients[1], coefficients[2]);
                coefficientIndex = 0;
                recording = false;
            }
        } else {
            inputBuffer[bufferIndex++] = key;
        }
    } else {
        if (key == '*') {
            recording = true;
            printf("Por favor, ingrese el coeficiente %c: ", 'a' + coefficientIndex);
            bufferIndex = 0;
        }
    }
}

int main() {
    printf("Ingrese los coeficientes del polinomio de grado 2.\n");
    while (true) {
        for (int i = 0; i < numRows; i++) {
            rowPins[i] = 0;

            for (int j = 0; j < numCols; j++) {
                if (!colPins[j]) {
                    processKey(keyMap[i][j]);
                    ThisThread::sleep_for(500ms);  // Evita lecturas múltiples mientras la tecla está presionada
                }
            }

            rowPins[i] = 1;
        }
    }
}
