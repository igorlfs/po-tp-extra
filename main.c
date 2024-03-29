#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EPSILON 10e-6

void print_basis(int *basis, int size) {
  for (int i = 0; i < size; ++i) {
    printf("%d ", basis[i]);
  }
  putchar('\n');
  putchar('\n');
}

void print_tableau(double **tableau, int n, int m, bool auxiliary) {
  int num_rows = n + 1;
  int num_cols = n + m + n + 1;
  if (auxiliary) {
    num_cols += n;
  }
  for (int i = 0; i < num_rows; i++) {
    for (int j = 0; j < num_cols; ++j) {
      if (j == n) {
        printf("||  ");
      }
      if (j == num_cols - 1) {
        printf("|  ");
      }
      printf("%+06.2lf", tableau[i][j]);
      if (j != num_cols - 1) {
        printf("  ");
      } else if (i == 0) {
        putchar('\n');
      }
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
}

void get_basis(double **tableau, int *basis, int n, int m, int num_rows) {
  for (int i = 0; i < n; ++i) {
    basis[i] = -1;
  }
  for (int j = n; j < n + m; ++j) {
    int num_ones = 0;
    int last_one = -1;
    for (int i = 1; i < num_rows; ++i) {
      if (tableau[i][j] == 1) {
        num_ones++;
        last_one = i;
      } else if (tableau[i][j] != 0) {
        break;
      }
      if (i == num_rows - 1 && num_ones == 1) {
        basis[last_one - 1] = j;
      }
    }
  }
}

void result_optimal(double **tableau, int num_cols, int num_rows, int n,
                    int m) {
  printf("otima\n");
  printf("%.7lf\n", tableau[0][num_cols - 1]);
  int basis[n];
  get_basis(tableau, basis, n, m, num_rows);
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      if (basis[j] == i + n) {
        printf("%.7lf ", tableau[j + 1][num_cols - 1]);
        break;
      }
      if (j == n - 1) {
        printf("%.7lf ", 0.);
      }
    }
  }
  putchar('\n');
  for (int i = 0; i < n; ++i) {
    printf("%.7lf ", tableau[0][i]);
  }
  putchar('\n');
}

void result_unlimited(double **tableau, int col, int n, int m, int num_cols,
                      int num_rows) {
  printf("ilimitada\n");
  int basis[n];
  get_basis(tableau, basis, n, m, num_rows);
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      if (basis[j] == i + n) {
        printf("%.7lf ", tableau[j + 1][num_cols - 1]);
        break;
      }
      if (j == n - 1) {
        printf("%.7lf ", 0.);
      }
    }
  }
  putchar('\n');
  for (int i = 0; i < m; ++i) {
    if (i + n == col) {
      printf("%.7f ", 1.);
      continue;
    }
    for (int j = 0; j < n; ++j) {
      if (basis[j] == i + n) {
        printf("%.7f ", -tableau[j + 1][col]);
        break;
      }
      if (j == n - 1) {
        printf("%.7f ", 0.);
      }
    }
  }
  putchar('\n');
}

void result_unviable(int n, double **tableau) {
  printf("inviavel\n");
  for (int j = 0; j < n; ++j) {
    printf("%.7f ", tableau[0][j]);
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

void simplex(int n, int m, double **tableau, bool inherits_auxiliary) {
  int num_rows = n + 1;
  int num_cols = n + m + n + 1;

  if (inherits_auxiliary) {
    num_cols += n;
  }

  bool loop = true;
  while (loop) {
    for (int i = n; i < num_cols - 1; ++i) {
      if (tableau[0][i] < -EPSILON) {
        int min_ratio_index = get_min_ratio_index(num_cols, i, tableau, n);

        if (min_ratio_index != -1) {
          pivoting(num_cols, num_rows, tableau, min_ratio_index, i);
        }
        // A solução é ilimitada quando nenhuma das razões é maior que 0
        else {
          result_unlimited(tableau, i, n, m, num_cols, num_rows);
          loop = false;
        }

        // Regra de Bland: para escolher a variável que vai sair da base,
        // nós escolhemos a de menor índice
        //
        // Um jeito de fazer isso é breakar o loop
        break;
      }
      // Se ninguém pode sair da base, sair do loop
      if (i == num_cols - 2 && loop) {
        result_optimal(tableau, num_cols, num_rows, n, m);
        loop = false;
      }
    }
  }
  for (int i = 0; i < num_rows; i++) {
    free(tableau[i]);
  }
  free(tableau);
}

double **auxiliary(int n, int m, double **a, const double *b, bool *is_viable) {
  int num_rows = n + 1;
  int num_cols = n + m + n + n + 1;
  double **auxiliary_tableau = init_auxiliary(n, m, a, b);

  bool loop = true;
  while (loop) {
    for (int i = n; i < num_cols - 1; ++i) {
      if (auxiliary_tableau[0][i] < -EPSILON) {
        int min_ratio_index =
            get_min_ratio_index(num_cols, i, auxiliary_tableau, n);

        if (min_ratio_index != -1) {
          pivoting(num_cols, num_rows, auxiliary_tableau, min_ratio_index, i);
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
        if (auxiliary_tableau[0][num_cols - 1] < -EPSILON) {
          *is_viable = false;
          result_unviable(n, auxiliary_tableau);
        }
        loop = false;
      }
    }
  }
  return auxiliary_tableau;
}

int main(int argc, char *argv[]) {
  int n; // Número de Restrições
  int m; // Número de Variáveis

  if (argc == 0) {
    return 1;
  }

  FILE *input_file_handler = fopen(argv[1], "r");

  if (input_file_handler == NULL) {
    return 1;
  }

  fscanf(input_file_handler, "%d %d", &n, &m);

  double *c = (double *)malloc(sizeof(double) * m);
  double *b = (double *)malloc(sizeof(double) * n);

  for (int i = 0; i < m; ++i) {
    fscanf(input_file_handler, "%lf", &c[i]);
  }

  double **a = (double **)malloc(sizeof(double *) * n);

  bool run_auxiliary = false;
  for (int i = 0; i < n; ++i) {
    a[i] = (double *)malloc(sizeof(double) * m);
    for (int j = 0; j < m; ++j) {
      fscanf(input_file_handler, "%lf", &a[i][j]);
    }
    fscanf(input_file_handler, "%lf", &b[i]);
    if (b[i] < 0) {
      run_auxiliary = true;
    }
  }

  fclose(input_file_handler);

  if (run_auxiliary) {
    bool is_viable = true;
    double **tableau = auxiliary(n, m, a, b, &is_viable);

    for (int i = 0; i < m; ++i) {
      tableau[0][i + n] = -c[i];
    }

    for (int i = 0; i < n; ++i) {
      tableau[0][i] = 0;
    }

    if (is_viable) {
      simplex(n, m, tableau, true);
    }
  } else {
    double **tableau = init_tableau(n, m, c, a, b);

    simplex(n, m, tableau, false);
  }

  free(a);
  free(c);
  free(b);

  return EXIT_SUCCESS;
}
