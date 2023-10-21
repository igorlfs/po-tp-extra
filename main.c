#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  int n; // Número de Restrições
  int m; // Número de Variáveis

  scanf("%d %d", &n, &m);

  double *c = (double *)malloc(sizeof(double *) * m);
  double *b = (double *)malloc(sizeof(double *) * n);

  for (int i = 0; i < m; ++i) {
    scanf("%lf", &c[i]);
  }

  double **a = (double **)malloc(sizeof(double *) * n);

  for (int i = 0; i < n; ++i) {
    a[i] = (double *)malloc(sizeof(double) * m);
    for (int j = 0; j < m + 1; ++j) {
      if (j == m) {
        scanf("%lf", &b[i]);
      } else {
        scanf("%lf", &a[i][j]);
      }
    }
  }

  // for (int i = 0; i < n; ++i) {
  //   for (int j = 0; j < m; ++j) {
  //     printf(" %d", a[i][j]);
  //   }
  //   putchar('\n');
  // }
  // putchar('\n');

  int simplex_lines = n + 1;
  int simplex_columns = m + n + 1;
  double **first_phase = (double **)malloc(sizeof(double *) * simplex_lines);

  for (int i = 0; i < simplex_lines; ++i) {
    first_phase[i] = (double *)malloc(sizeof(double) * simplex_columns);
    memset(first_phase[i], 0, sizeof(int) * simplex_columns);
  }

  // Criando a linha do topo
  for (int i = 0; i < simplex_columns; ++i) {
    // TODO(dev): Ui = 1 se bi >= e -1 cc
    if (i >= m && i != simplex_columns - 1) {
      first_phase[0][i] = 1;
    } else {
      first_phase[0][i] = 0;
    }
  }

  // Botando a matriz 'a' dentro
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      if (b[i] < 0) {
        first_phase[i + 1][j] = -a[i][j];
      } else {
        first_phase[i + 1][j] = a[i][j];
      }
    }
  }

  // Colocando o vetor de b's
  for (int i = 0; i < n; ++i) {
    if (b[i] <= 0) {
      first_phase[i + 1][simplex_columns - 1] = -b[i];
    } else {
      first_phase[i + 1][simplex_columns - 1] = b[i];
    }
  }

  // Colocando a identidade
  for (int i = 0; i < n; ++i) {
    first_phase[i + 1][m + i] = 1;
  }

  // Colocando na base canônica
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < simplex_columns; ++j) {
      first_phase[0][j] -= first_phase[i + 1][j];
    }
  }

  for (int i = 0; i < simplex_columns; ++i) {
    // TODO(dev): regra de Bland (aula 11)
    if (first_phase[0][i] < 0) {
      double max_ratio = -INFINITY;
      int max_ratio_index = -1;

      // Encontrando o índice para entrar na base
      for (int j = 0; j < n; ++j) {
        if (first_phase[j + 1][simplex_columns - 1] == 0) {
          // TODO(dev): se ninguém puder entrar então o problema é esquisitinho
          continue;
        }
        double ratio =
            first_phase[j + 1][i] / first_phase[j + 1][simplex_columns - 1];
        if (ratio >= max_ratio) {
          max_ratio = ratio;
          max_ratio_index = j + 1;
        }
      }

      // TODO(dev): fazer o estendido e já matar a inviabilidade
      // Pivoteamento
      if (max_ratio != -INFINITY) {
        // Parte 1: Dividindo linha pelo maior razão (para o elemento ser 1)
        double multiplier = first_phase[max_ratio_index][i];
        for (int j = 0; j < simplex_columns; ++j) {
          first_phase[max_ratio_index][j] =
              first_phase[max_ratio_index][j] / multiplier;
        }
        // Parte 2: Zerando outras colunas
        for (int j = 0; j < simplex_lines; ++j) {
          // Estamos na coluna que realmente deve ser igual a 1
          if (j == max_ratio_index) {
            continue;
          }
          double ratio = first_phase[j][i] / first_phase[max_ratio_index][i];
          for (int k = 0; k < simplex_columns; ++k) {
            first_phase[j][k] -= ratio * first_phase[max_ratio_index][k];
          }
        }
      }
      for (int j = 0; j < simplex_lines; ++j) {
        for (int k = 0; k < simplex_columns; ++k) {
          printf(" %f", first_phase[j][k]);
        }
        putchar('\n');
      }
    }
  }

  for (int i = 0; i < simplex_lines; ++i) {
    for (int j = 0; j < simplex_columns; ++j) {
      printf(" %f", first_phase[i][j]);
    }
    putchar('\n');
  }

  return EXIT_SUCCESS;
}
