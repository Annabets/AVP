#include <iostream>
#include <Windows.h>
#include <ctime>

#define INNER_LINES_AMOUNT_1 4
#define INNER_COMMON_SIDE_AMOUNT 4
#define INNER_COLUMNS_AMOUNT_1 INNER_COMMON_SIDE_AMOUNT
#define INNER_LINES_AMOUNT_2 INNER_COMMON_SIDE_AMOUNT
#define INNER_COLUMNS_AMOUNT_2 4

#define MAIN_LINES_AMOUNT_1 200
#define MAIN_COMMON_SIDE_AMOUNT 200
#define MAIN_COLUMNS_AMOUNT_1 MAIN_COMMON_SIDE_AMOUNT
#define MAIN_LINES_AMOUNT_2 MAIN_COMMON_SIDE_AMOUNT
#define MAIN_COLUMNS_AMOUNT_2 200


using namespace std;

float** createInnerMatrix(int a, int b) {
	float** matrix = (float**)calloc(a, sizeof(float*));
	for (int i = 0; i < a; ++i) {
		matrix[i] = (float*)calloc(b, sizeof(float));
	}
	return matrix;
}

void initializeMatrixWithRandomValues(float** matrix, int a, int b) {
	for (int i = 0; i < a; ++i) {
		for (int j = 0; j < b; ++j) {
			matrix[i][j] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);;
		}
	}
}

float** multiplyInnerMatrix_VLob(float** matrix1, float** matrix2, int m, int n, int k) {

	float** result = createInnerMatrix(m, n);

	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			for (int g = 0; g < k; ++g) {
				result[i][j] += matrix1[i][g] * matrix2[g][j];
			}
		}
	}

	return result;
}

float** multiplyInnerMatrix_optimized_only_line_loops(float** matrix1, float** matrix2, int m, int n, int k) {

	float** result = createInnerMatrix(m, n);

	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < k; ++j) {
			float firstMatrixNum = matrix1[i][j];
			float* r = result[i];
			float* m2 = matrix2[j];
//#pragma loop(hint_parallel(4))
			for (int g = 0; g < n; ++g) {
				r[g] += firstMatrixNum * m2[g];
			}
		}
	}

	return result;
}

float** intrncsMatrix(float** matrix1, float** matrix2, int m, int n, int k) {
	__m128 a_line, b_line, r_line;

	float** result = createInnerMatrix(m, n);

	for (int i = 0; i < m; ++i) {
			r_line = _mm_setzero_ps();
			for (int j = 0; j < k; j++)
			{
				b_line = _mm_load_ps(matrix2[j]);
				a_line = _mm_set1_ps(matrix1[i][j]);
				r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);
			}
			_mm_store_ps(result[i], r_line);
	}

	return result;
}


bool comareToInnerMatrix(float** matrix1, float** matrix2, int m, int n) {
	bool flag = true;

	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			if (matrix1[i][j] != matrix2[i][j]) {
				flag = false;
			}
		}
	}

	return flag;
}


void showInnerMatrix(float** matrix, int a, int b) {
	for (int i = 0; i < a; ++i) {
		for (int j = 0; j < b; ++j) {
			cout << matrix[i][j] << " ";
		}
		cout << endl;
	}
}

int main() {
	LARGE_INTEGER frequency, start, finish;
	float delay;
	QueryPerformanceFrequency(&frequency);
	srand(static_cast <unsigned> (time(0)));

	float**** matrix3 = (float****)calloc(MAIN_LINES_AMOUNT_1, sizeof(float***));
	for (int i = 0; i < MAIN_LINES_AMOUNT_1; ++i) {
		matrix3[i] = (float***)calloc(MAIN_COLUMNS_AMOUNT_1, sizeof(float**));
		for (int j = 0; j < MAIN_COLUMNS_AMOUNT_1; ++j) {
			matrix3[i][j] = createInnerMatrix(INNER_LINES_AMOUNT_1, INNER_COLUMNS_AMOUNT_1);
			initializeMatrixWithRandomValues(matrix3[i][j], INNER_LINES_AMOUNT_1, INNER_COLUMNS_AMOUNT_1);
		}
	}

	float**** matrix4 = (float****)calloc(MAIN_LINES_AMOUNT_2, sizeof(float***));
	for (int i = 0; i < MAIN_LINES_AMOUNT_2; ++i) {
		matrix4[i] = (float***)calloc(MAIN_COLUMNS_AMOUNT_2, sizeof(float**));
		for (int j = 0; j < MAIN_COLUMNS_AMOUNT_2; ++j) {
			matrix4[i][j] = createInnerMatrix(INNER_LINES_AMOUNT_2, INNER_COLUMNS_AMOUNT_2);
			initializeMatrixWithRandomValues(matrix4[i][j], INNER_LINES_AMOUNT_2, INNER_COLUMNS_AMOUNT_2);
		}
	}

	float**** matrix13 = (float****)calloc(MAIN_LINES_AMOUNT_1, sizeof(float***));
	for (int i = 0; i < MAIN_LINES_AMOUNT_1; ++i) {
		matrix13[i] = (float***)calloc(MAIN_COLUMNS_AMOUNT_1, sizeof(float**));
	}
	float** resultMatrix1;
	QueryPerformanceCounter(&start);
	for (int i = 0; i < MAIN_LINES_AMOUNT_1; ++i) {
		for (int j = 0; j < MAIN_COLUMNS_AMOUNT_1; ++j) {
			float** generalConst = matrix3[i][j];
				for (int l = 0; l < MAIN_COLUMNS_AMOUNT_2; ++l) {
					matrix13[i][l] = multiplyInnerMatrix_VLob(generalConst, matrix4[j][l], INNER_LINES_AMOUNT_1, INNER_COLUMNS_AMOUNT_2, INNER_COMMON_SIDE_AMOUNT);
				}
			
		}
	}
	QueryPerformanceCounter(&finish);
	delay = (finish.QuadPart - start.QuadPart) * 1000.0f / frequency.QuadPart;
	cout << "Time v lob cicle in ms: " << delay << endl;

	float**** matrix11 = (float****)calloc(MAIN_LINES_AMOUNT_1, sizeof(float***));
	for (int i = 0; i < MAIN_LINES_AMOUNT_1; ++i) {
		matrix11[i] = (float***)calloc(MAIN_COLUMNS_AMOUNT_1, sizeof(float**));
	}
	QueryPerformanceCounter(&start);
	for (int i = 0; i < MAIN_LINES_AMOUNT_1; ++i) {
		for (int j = 0; j < MAIN_COLUMNS_AMOUNT_1; ++j) {
			float** generalConst = matrix3[i][j];
			for (int l = 0; l < MAIN_COLUMNS_AMOUNT_2; ++l) {
				matrix11[i][l] = multiplyInnerMatrix_optimized_only_line_loops(generalConst, matrix4[j][l], INNER_LINES_AMOUNT_1, INNER_COLUMNS_AMOUNT_2, INNER_COMMON_SIDE_AMOUNT);
			}
		}
	}
	QueryPerformanceCounter(&finish);
	delay = (finish.QuadPart - start.QuadPart) * 1000.0f / frequency.QuadPart;
	cout << "Time vectorize cicle in ms: " << delay << endl;

	float**** matrix12 = (float****)calloc(MAIN_LINES_AMOUNT_1, sizeof(float***));
	for (int i = 0; i < MAIN_LINES_AMOUNT_1; ++i) {
		matrix12[i] = (float***)calloc(MAIN_COLUMNS_AMOUNT_1, sizeof(float**));
	}
	float** resultMatrix3;
	QueryPerformanceCounter(&start);
	for (int i = 0; i < MAIN_LINES_AMOUNT_1; ++i) {
		for (int j = 0; j < MAIN_COLUMNS_AMOUNT_1; ++j) {
			float** generalConst = matrix3[i][j];
			for (int l = 0; l < MAIN_COLUMNS_AMOUNT_2; ++l) {
				//matrix12[i][l] = intrncsMatrix(generalConst, matrix4[j][l], INNER_LINES_AMOUNT_1, INNER_COLUMNS_AMOUNT_2, INNER_COMMON_SIDE_AMOUNT);
			}
		}
	}
	QueryPerformanceCounter(&finish);
	delay = (finish.QuadPart - start.QuadPart) * 1000.0f / frequency.QuadPart;
	cout << "Time with using intrinsics in ms: " << delay << endl;

	bool isEqualBig = true;
	for (int i = 0; i < MAIN_LINES_AMOUNT_1; ++i) {
		for (int l = 0; l < MAIN_COLUMNS_AMOUNT_2; ++l) {
			if (!comareToInnerMatrix(matrix11[i][l], matrix13[i][l], INNER_LINES_AMOUNT_1, INNER_COLUMNS_AMOUNT_2)) {
				isEqualBig = false;
			}
		}
	}
	cout << (isEqualBig ? "matrices are equal" : "matrices aren't equal") << endl;

	float** matrix1 = createInnerMatrix(INNER_LINES_AMOUNT_1, INNER_COLUMNS_AMOUNT_1);
	float** matrix2 = createInnerMatrix(INNER_LINES_AMOUNT_2, INNER_COLUMNS_AMOUNT_2);
	initializeMatrixWithRandomValues(matrix1, INNER_LINES_AMOUNT_1, INNER_COLUMNS_AMOUNT_1);
	initializeMatrixWithRandomValues(matrix2, INNER_LINES_AMOUNT_2, INNER_COLUMNS_AMOUNT_2);

	/*QueryPerformanceCounter(&start);
	float** matrix5 = multiplyInnerMatrix_VLob(matrix1, matrix2, INNER_LINES_AMOUNT_1, INNER_COLUMNS_AMOUNT_2, INNER_COMMON_SIDE_AMOUNT);
	QueryPerformanceCounter(&finish);
	delay = (finish.QuadPart - start.QuadPart) * 1000.0f / frequency.QuadPart;
	cout << "Time \"v lob\" in ms: " << delay << endl;*/
	//showInnerMatrix(matrix5, INNER_LINES_AMOUNT_1, INNER_COLUMNS_AMOUNT_2);


	float** matrix8 = createInnerMatrix(1000, 1000);
	float** matrix9 = createInnerMatrix(1000, 1000);
	initializeMatrixWithRandomValues(matrix8, 1000, 1000);
	initializeMatrixWithRandomValues(matrix9, 1000, 1000);
	QueryPerformanceCounter(&start);
	float** matrix6 = multiplyInnerMatrix_optimized_only_line_loops(matrix8, matrix9, 1000, 1000, 1000);
	QueryPerformanceCounter(&finish);
	delay = (finish.QuadPart - start.QuadPart) * 1000.0f / frequency.QuadPart;
	cout << "Time with line loops optimization in ms: " << delay << endl;
	//showInnerMatrix(matrix6, INNER_LINES_AMOUNT_1, INNER_COLUMNS_AMOUNT_2);
	QueryPerformanceCounter(&start);
	float** matrix10 = multiplyInnerMatrix_VLob(matrix8, matrix9, 1000, 1000, 1000);
	QueryPerformanceCounter(&finish);
	delay = (finish.QuadPart - start.QuadPart) * 1000.0f / frequency.QuadPart;
	cout << "Time \"v lob\" in ms: " << delay << endl;
	//showInnerMatrix(matrix10, INNER_LINES_AMOUNT_1, INNER_COLUMNS_AMOUNT_2);
	bool isEquals = comareToInnerMatrix(matrix6, matrix10, 1000, 1000);
	cout << (isEquals ? "matrices are equal" : "matrices aren't equal") << endl;
	

	/*QueryPerformanceCounter(&start);
	float** matrix7 = intrncsMatrix(matrix1, matrix2, INNER_LINES_AMOUNT_1, INNER_COLUMNS_AMOUNT_2, INNER_COMMON_SIDE_AMOUNT);
	QueryPerformanceCounter(&finish);
	delay = (finish.QuadPart - start.QuadPart) * 1000.0f / frequency.QuadPart;
	cout << "Time with using intrinsics in ms: " << delay << endl;*/
	//showInnerMatrix(matrix7, INNER_LINES_AMOUNT_1, INNER_COLUMNS_AMOUNT_2);

	/*float C[10000] = { 123.456 }, B[10000] = {678.678}, A[10000];

	QueryPerformanceCounter(&start);
	for (int i = 0; i < 10000; ++i)
		A[i] = 5 + C[i];
	QueryPerformanceCounter(&finish);
	delay = (finish.QuadPart - start.QuadPart) * 1000.0f / frequency.QuadPart;
	cout << "Time with line loops optimization in ms: " << delay << endl;

	QueryPerformanceCounter(&start);
#pragma loop(no_vector)
	for (int i = 0; i < 10000; ++i)
		A[i] = 5 + C[i];
	QueryPerformanceCounter(&finish);
	delay = (finish.QuadPart - start.QuadPart) * 1000.0f / frequency.QuadPart;
	cout << "Time with line loops optimization in ms: " << delay << endl;*/
	
	
	system("pause");
	return 0;
}