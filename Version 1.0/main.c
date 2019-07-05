#include "include/matrix.c"
#include "include/matrixadv.c"
#include <stdio.h>

int main(){
    matrix *filedata = readMatrix("test.txt");
    // printMatrix(filedata);
    matrix *B = submatrix(filedata,1,filedata->width,1,filedata->height);
    matrix *A = submatrix(filedata,1,1,filedata->width-1,filedata->height);
    // matrix* X = multiplyMatrix(transposeMatrix(A),A);
    matrix* X = multiplyMatrix(
        inverseMatrix(
            multiplyMatrix(transposeMatrix(A),A)
            ),
        multiplyMatrix(transposeMatrix(A),B)
        );
    printMatrix(X);
}

