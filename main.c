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
}

void init_tableau(double **tableau, int n, int m, const double *c, double **a,
                  const double *b) {
  int num_rows = n + 1;
  int num_cols = n + m + n + 1;

  for (int i = 0; i < num_rows; ++i) {
    tableau[i] = (double *)malloc(sizeof(double) * num_cols);
    memset(tableau[i], 0, sizeof(int) * num_cols);
  }

  // ??
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
    tableau[i + 1][num_cols - 1] = b[i];
  }
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
    if (ratio > 0 && ratio < min_ratio) {
      min_ratio = ratio;
      min_ratio_index = j;
    }
  }

  return min_ratio_index;
}

void pivoting(int num_cols, int num_rows, double **tableau, int min_ratio_index,
              int col) {
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

void result_optimal(double **tableau, int num_cols, const int *basis, int n) {
  // TODO ver questão de usar variáveis de folga na saída
  printf("otima\n");
  printf("%05.2lf\n", tableau[0][num_cols - 1]);
  for (int j = 0; j < n; ++j) {
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

void simplex(int n, int m, const double *c, double **a, const double *b) {
  int num_rows = n + 1;
  int num_cols = n + m + n + 1;
  double **tableau = (double **)malloc(sizeof(double *) * num_rows);
  int basis[n];

  for (int i = 0; i < n; ++i) {
    basis[i] = m + n + i;
  }

  init_tableau(tableau, n, m, c, a, b);

  bool loop = true;
  while (loop) {
    for (int i = n; i < n + m + n; ++i) {
      if (tableau[0][i] < 0) {
        int min_ratio_index = get_min_ratio_index(num_cols, i, tableau, n);
        if (min_ratio_index != -1) {
          basis[min_ratio_index - 1] = i;
          pivoting(num_cols, num_rows, tableau, min_ratio_index, i);
        }
        // A solução é ilimitada quando nenhuma das razões é maior que 0
        else {
          printf("ilimitada\n");
          print_tableau(tableau, num_rows, num_cols);
          for (int j = 0; j < n + m; ++j) {
            if (j + n == i) {
              printf("%05.2f ", 1.);
              continue;
            }
            for (int k = 0; k < n; ++k) {
              if (basis[k] == j + n) {
                printf("%05.2lf ", -tableau[k][i]);
                break;
              }
              // if (k == n - 1) {
              //   printf("%05.2f ", 0.);
              // }
            }
          }
          putchar('\n');
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
        result_optimal(tableau, num_cols, basis, n);
        loop = false;
      }
    }
  }
  for (int i = 0; i < num_rows; i++) {
    free(tableau[i]);
  }
  free(tableau);
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

  for (int i = 0; i < n; ++i) {
    a[i] = (double *)malloc(sizeof(double) * m);
    for (int j = 0; j < m; ++j) {
      scanf("%lf", &a[i][j]);
    }
    scanf("%lf", &b[i]);
  }

  int first_lines = n + 1;
  int first_columns = n + m + n + 1;
  double **first_phase = (double **)malloc(sizeof(double *) * first_lines);

  for (int i = 0; i < first_lines; ++i) {
    first_phase[i] = (double *)malloc(sizeof(double) * first_columns);
    memset(first_phase[i], 0, sizeof(int) * first_columns);
  }

  // Criando a linha do topo
  for (int i = n + m; i < first_columns - 1; ++i) {
    first_phase[0][i] = 1;
  }

  // Colocando a matriz 'a'
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      first_phase[i + 1][n + j] = a[i][j];
    }
  }

  // Colocando a identidade
  for (int i = 0; i < n; ++i) {
    first_phase[i + 1][i] = 1;
  }

  // Colocando o vetor de b's
  for (int i = 0; i < n; ++i) {
    if (b[i] < 0) {
      first_phase[i + 1][first_columns - 1] = -b[i];
      for (int j = 0; j < first_columns - 1; ++j) {
        first_phase[i + 1][j] *= -1;
      }
    } else {
      first_phase[i + 1][first_columns - 1] = b[i];
    }
  }

  // Colocando a identidade (das variáveis de folga)
  // Não deve ser invertida com o 'b'
  for (int i = 0; i < n; ++i) {
    first_phase[i + 1][n + m + i] = 1;
  }

  // for (int j = 0; j < first_lines; j++) {
  //   for (int k = 0; k < first_columns; ++k) {
  //     printf("%+06.2lf ", first_phase[j][k]);
  //   }
  //   putchar('\n');
  // }
  // putchar('\n');

  // Colocando na base canônica
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < first_columns; ++j) {
      first_phase[0][j] -= first_phase[i + 1][j];
    }
  }

  // for (int j = 0; j < first_lines; j++) {
  //   for (int k = 0; k < first_columns; ++k) {
  //     printf("%+06.2lf ", first_phase[j][k]);
  //   }
  //   putchar('\n');
  // }

  bool loop_first_phase = true;
  bool is_viable = true;
  while (loop_first_phase) {
    for (int i = n; i < first_columns - 1; ++i) {
      if (first_phase[0][i] < 0) {

        // for (int j = 0; j < first_lines; j++) {
        //   for (int k = 0; k < first_columns; ++k) {
        //     printf("%+06.2lf ", first_phase[j][k]);
        //   }
        //   putchar('\n');
        // }
        // printf("----------------------------------\n");

        double min_ratio = INFINITY;
        int min_ratio_index = -1;

        // Encontrando o índice para entrar na base
        for (int j = 1; j <= n; ++j) {
          if (first_phase[j][i] == 0) {
            continue;
          }
          double ratio = first_phase[j][first_columns - 1] / first_phase[j][i];

          // Regra de Bland: nós sempre pegamos a variável de menor índice
          // dentre aquelas que podem entrar na base.
          //
          // Para fazer isso, basta não trocar a razão quando quando há empate
          if (ratio > 0 && ratio < min_ratio) {
            min_ratio = ratio;
            min_ratio_index = j;
          }
        }

        // Pivoteamento
        if (min_ratio != INFINITY) {
          // Fazendo a elemento da linha do índice ser 1
          double multiplier = first_phase[min_ratio_index][i];
          for (int j = 0; j < first_columns; ++j) {
            first_phase[min_ratio_index][j] =
                first_phase[min_ratio_index][j] / multiplier;
          }
          // Zerando outras linhas
          for (int j = 0; j < first_lines; ++j) {
            // Estamos na coluna que realmente deve ser igual a 1
            if (j == min_ratio_index) {
              continue;
            }
            double ratio = first_phase[j][i] / first_phase[min_ratio_index][i];
            for (int k = 0; k < first_columns; ++k) {
              first_phase[j][k] -= ratio * first_phase[min_ratio_index][k];
            }
          }
          // for (int j = 0; j < first_lines; ++j) {
          //   if (j == min_ratio_index) {
          //     // Fazendo a elmento da linha do índice ser 1
          //     double multiplier = first_phase[min_ratio_index][i];
          //
          //     for (int k = 0; k < first_columns; ++k) {
          //       first_phase[j][k] = first_phase[j][k] / multiplier;
          //     }
          //     continue;
          //   }
          //
          //   // Zerando outras linhas
          //   double ratio = first_phase[j][i] /
          //   first_phase[min_ratio_index][i];
          //
          //   for (int k = 0; k < first_columns; ++k) {
          //     first_phase[j][k] -= ratio * first_phase[min_ratio_index][k];
          //   }
          // }
        }

        // Regra de Bland: para escolher a variável que vai sair da base, nós
        // escolhemos a de menor índice
        //
        // Um jeito de fazer isso é breakar o loop
        break;
      }
      // Se ninguém pode sair da base, sair do loop
      if (i == first_columns - 2) {
        // Problema é inviável
        if (first_phase[0][first_columns - 1] < 0) {
          is_viable = false;

          // for (int j = 0; j < first_lines; j++) {
          //   for (int k = 0; k < first_columns; ++k) {
          //     printf("%+06.2lf ", first_phase[j][k]);
          //   }
          //   putchar('\n');
          // }

          printf("inviavel\n");
          for (int j = 0; j < n; ++j) {
            printf("%04.2f ", first_phase[0][j]);
          }
          putchar('\n');
        }
        loop_first_phase = false;
      }
    }
  }

  if (is_viable) {
    // TODO(): usar isso aqui pra não ter que ficar caçando quem é a base na
    // mão
    // int second_basis[n];
    // for (int i = 0; i < n; ++i) {
    //   second_basis[i] = i;
    // }
    simplex(n, m, c, a, b);
  }

  for (int i = 0; i < first_lines; i++) {
    free(first_phase[i]);
  }
  for (int i = 0; i < n; i++) {
    free(a[i]);
  }
  free(first_phase);
  free(a);
  free(c);
  free(b);

  return EXIT_SUCCESS;
}
