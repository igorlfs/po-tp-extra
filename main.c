#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_basis(int *basis, int size) {
  for (int i = 0; i < size; ++i) {
    printf("%d ", basis[i]);
  }
  putchar('\n');
  putchar('\n');
}

void print_tableau(double **tableau, int num_rows, int num_cols) {
  for (int i = 0; i < num_rows; i++) {
    for (int j = 0; j < num_cols; ++j) {
      printf("%+06.2lf ", tableau[i][j]);
    }
    putchar('\n');
  }
  putchar('\n');
}

double **init_tableau(int n, int m, const double *c, double **a,
                      const double *b) {
  int num_rows = n + 1;
  int num_cols = n + m + n + 1;

  double **tableau = (double **)malloc(sizeof(double *) * num_rows);

  for (int i = 0; i < num_rows; ++i) {
    tableau[i] = (double *)malloc(sizeof(double) * num_cols);
    for (int j = 0; j < num_cols; ++j) {
      tableau[i][j] = 0;
    }
  }

  // Vetor de custos
  for (int i = 0; i < m; ++i) {
    tableau[0][i + n] = -c[i];
  }

  // Matriz A
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      tableau[i + 1][n + j] = a[i][j];
    }
  }

  // Registro de Operações
  for (int i = 0; i < n; ++i) {
    tableau[i + 1][i] = 1;
  }

  // Variáveis de folga
  for (int i = 0; i < n; ++i) {
    tableau[i + 1][n + m + i] = 1;
  }

  // Vetor de b
  for (int i = 0; i < n; ++i) {
    // É necessário inverter a linha se b < 0
    if (b[i] < 0) {
      tableau[i + 1][num_cols - 1] = -b[i];
      for (int j = 0; j < num_cols - 1; ++j) {
        tableau[i + 1][j] *= -1;
      }
    } else {
      tableau[i + 1][num_cols - 1] = b[i];
    }
  }

  return tableau;
}

int get_min_ratio_index(int num_cols, int col, double **tableau, int n) {
  double min_ratio = INFINITY;
  int min_ratio_index = -1;

  for (int j = 1; j <= n; ++j) {
    if (tableau[j][col] == 0) {
      continue;
    }

    double ratio = tableau[j][num_cols - 1] / tableau[j][col];

    // Regra de Bland: pegar a variável de menor índice dentre as que podem
    // entrar na base.
    //
    // Basta não trocar o índice quando quando há empate
    if (ratio >= 0 && ratio < min_ratio && tableau[j][col] > 0) {
      min_ratio = ratio;
      min_ratio_index = j;
    }
  }

  return min_ratio_index;
}

void pivoting(int num_cols, int num_rows, double **tableau, int min_ratio_index,
              int col) {
  // print_tableau(tableau, num_rows, num_cols);
  // Fazendo a elemento da linha do índice ser 1
  // Essa variável é necessária pois sobrescrevemos o valor dela
  double ratio = tableau[min_ratio_index][col];
  for (int j = 0; j < num_cols; ++j) {
    tableau[min_ratio_index][j] = tableau[min_ratio_index][j] / ratio;
  }

  // print_tableau(tableau, num_rows, num_cols);

  // Zerando outras linhas
  for (int j = 0; j < num_rows; ++j) {
    if (j == min_ratio_index) {
      continue;
    }

    double multiplier = tableau[j][col] / tableau[min_ratio_index][col];

    for (int k = 0; k < num_cols; ++k) {
      tableau[j][k] -= multiplier * tableau[min_ratio_index][k];
    }
  }

  // print_tableau(tableau, num_rows, num_cols);
}

void result_optimal(double **tableau, int num_cols, const int *basis, int n,
                    int m) {
  printf("otima\n");
  printf("%05.2lf\n", tableau[0][num_cols - 1]);
  for (int j = 0; j < m; ++j) {
    for (int k = 0; k < n; ++k) {
      if (basis[k] == j + n) {
        printf("%05.2lf ", tableau[k + 1][num_cols - 1]);
        break;
      }
      if (k == n - 1) {
        printf("%05.2f ", 0.);
      }
    }
  }
  putchar('\n');
  for (int j = 0; j < n; ++j) {
    printf("%05.2f ", tableau[0][j]);
  }
  putchar('\n');
}

void result_unlimited(double **tableau, int col, const int *basis, int n,
                      int m) {
  printf("ilimitada\n");
  for (int i = 0; i < m; ++i) {
    if (i + n == col) {
      printf("%05.2f ", 1.);
      continue;
    }
    for (int j = 0; j < n; ++j) {
      if (basis[j] == i + n) {
        printf("%05.2lf ", -tableau[j][col]);
        break;
      }
      if (j == n - 1) {
        printf("%05.2f ", 0.);
      }
    }
  }
  putchar('\n');
}

double **init_auxiliary(int n, int m, double **a, const double *b) {
  int num_rows = n + 1;
  int num_cols = n + m + n + n + 1;

  double **tableau = (double **)malloc(sizeof(double *) * num_rows);

  for (int i = 0; i < num_rows; ++i) {
    tableau[i] = (double *)malloc(sizeof(double) * num_cols);
    for (int j = 0; j < num_cols; ++j) {
      tableau[i][j] = 0;
    }
  }

  // Vetor de custos
  for (int i = n + m + n; i < num_cols - 1; ++i) {
    tableau[0][i] = 1;
  }

  // Matriz A
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      tableau[i + 1][n + j] = a[i][j];
    }
  }

  // Registro de Operações
  for (int i = 0; i < n; ++i) {
    tableau[i + 1][i] = 1;
  }

  // Variáveis de folga da PL original
  for (int i = 0; i < n; ++i) {
    tableau[i + 1][n + m + i] = 1;
  }

  // Vetor de b
  for (int i = 0; i < n; ++i) {
    // É necessário inverter a linha se b < 0
    if (b[i] < 0) {
      tableau[i + 1][num_cols - 1] = -b[i];
      for (int j = 0; j < num_cols - 1; ++j) {
        tableau[i + 1][j] *= -1;
      }
    } else {
      tableau[i + 1][num_cols - 1] = b[i];
    }
  }

  // Variáveis do auxiliar
  // Não devem ser invertida com o 'b'
  for (int i = 0; i < n; ++i) {
    tableau[i + 1][n + m + n + i] = 1;
  }

  // Colocando na base canônica
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < num_cols; ++j) {
      tableau[0][j] -= tableau[i + 1][j];
    }
  }

  return tableau;
}

void simplex(int n, int m, const double *c, double **a, const double *b,
             int *basis) {
  int num_rows = n + 1;
  int num_cols = n + m + n + 1;
  double **tableau = init_tableau(n, m, c, a, b);
  for (int i = 0; i < n; ++i) {
    basis[i] = -1;
  }
  // print_basis(basis, n);
  // print_tableau(tableau, num_rows, num_cols);
  // Vetor de custos
  // for (int i = 0; i < num_cols; ++i) {
  //   tableau[0][i] = 0;
  // }
  // for (int i = 0; i < m; ++i) {
  //   tableau[0][i + n] = -c[i];
  // }

  bool loop = true;
  // print_basis(basis, n);
  // print_tableau(tableau, num_rows, num_cols);
  // for (int i = 0; i < n; ++i) {
  //   pivoting(num_cols, num_rows, tableau, i, basis[i]);
  // }
  // print_tableau(tableau, num_rows, num_cols);
  while (loop) {
    for (int i = n; i < num_cols - 1; ++i) {
      if (tableau[0][i] < 0) {
        int min_ratio_index = get_min_ratio_index(num_cols, i, tableau, n);

        if (min_ratio_index != -1) {
          basis[min_ratio_index - 1] = i;
          pivoting(num_cols, num_rows, tableau, min_ratio_index, i);
          // print_basis(basis, n);
          // print_tableau(tableau, num_rows, num_cols);
        }
        // A solução é ilimitada quando nenhuma das razões é maior que 0
        else {
          // TODO certificado
          // print_basis(basis, n);
          // print_tableau(tableau, num_rows, num_cols);
          result_unlimited(tableau, i, basis, n, m);
          loop = false;
        }

        // Regra de Bland: para escolher a variável que vai sair da base,
        // nós escolhemos a de menor índice
        //
        // Um jeito de fazer isso é breakar o loop
        break;
      }
      // Se ninguém pode sair da base, sair do loop
      if (i == n + m + n - 1 && loop) {
        // TODO certificado
        // print_basis(basis, n);
        // print_tableau(tableau, num_rows, num_cols);
        result_optimal(tableau, num_cols, basis, n, m);
        loop = false;
      }
    }
  }
  for (int i = 0; i < num_rows; i++) {
    free(tableau[i]);
  }
  free(tableau);
}

void auxiliary(int n, int m, double **a, const double *b, bool *is_viable,
               int *basis) {
  int num_rows = n + 1;
  int num_cols = n + m + n + n + 1;
  double **auxiliary_tableau = init_auxiliary(n, m, a, b);
  // print_tableau(auxiliary_tableau, num_rows, num_cols);
  // print_basis(basis, n);

  bool loop = true;
  while (loop) {
    for (int i = n; i < num_cols - 1; ++i) {
      if (auxiliary_tableau[0][i] < 0) {
        int min_ratio_index =
            get_min_ratio_index(num_cols, i, auxiliary_tableau, n);

        if (min_ratio_index != -1) {
          basis[min_ratio_index - 1] = i;
          pivoting(num_cols, num_rows, auxiliary_tableau, min_ratio_index, i);
          // print_tableau(auxiliary_tableau, num_rows, num_cols);
          // print_basis(basis, n);

          // Regra de Bland: para escolher a variável que vai sair da base, nós
          // escolhemos a de menor índice
          //
          // Um jeito de fazer isso é breakar o loop
          break;
        }
      }
      // Se ninguém pode sair da base, sair do loop
      if (i == num_cols - 2) {
        // Problema é inviável
        if (auxiliary_tableau[0][num_cols - 1] < 0) {
          *is_viable = false;
          printf("inviavel\n");
          for (int j = 0; j < n; ++j) {
            printf("%04.2f ", auxiliary_tableau[0][j]);
          }
          putchar('\n');
        }
        loop = false;
      }
    }
  }
  // print_basis(basis, n);
  // print_tableau(auxiliary_tableau, num_rows, num_cols);
  for (int i = 0; i < num_rows; i++) {
    free(auxiliary_tableau[i]);
  }
  free(auxiliary_tableau);
}

int main(void) {
  int n; // Número de Restrições
  int m; // Número de Variáveis

  scanf("%d %d", &n, &m);

  double *c = (double *)malloc(sizeof(double) * m);
  double *b = (double *)malloc(sizeof(double) * n);

  for (int i = 0; i < m; ++i) {
    scanf("%lf", &c[i]);
  }

  double **a = (double **)malloc(sizeof(double *) * n);

  bool run_auxiliary = false;
  for (int i = 0; i < n; ++i) {
    a[i] = (double *)malloc(sizeof(double) * m);
    for (int j = 0; j < m; ++j) {
      scanf("%lf", &a[i][j]);
    }
    scanf("%lf", &b[i]);
    if (b[i] < 0) {
      run_auxiliary = true;
    }
  }

  bool is_viable = true;
  int basis[n];
  for (int i = 0; i < n; ++i) {
    basis[i] = n + m + i;
  }

  if (run_auxiliary) {
    for (int i = 0; i < n; ++i) {
      basis[i] = n + m + n + i;
    }
    auxiliary(n, m, a, b, &is_viable, basis);
  }

  if (is_viable) {
    simplex(n, m, c, a, b, basis);
  }

  free(a);
  free(c);
  free(b);

  return EXIT_SUCCESS;
}
