#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    for (int j = 0; j < m + 1; ++j) {
      if (j == m) {
        scanf("%lf", &b[i]);
      } else {
        scanf("%lf", &a[i][j]);
      }
    }
  }

  int simplex_lines = n + 1;
  int simplex_columns = n + m + n + 1;
  double **first_phase = (double **)malloc(sizeof(double *) * simplex_lines);

  for (int i = 0; i < simplex_lines; ++i) {
    first_phase[i] = (double *)malloc(sizeof(double) * simplex_columns);
    memset(first_phase[i], 0, sizeof(int) * simplex_columns);
  }

  // Criando a linha do topo
  for (int i = n + m; i < simplex_columns - 1; ++i) {
    // TODO(dev): Ui = 1 se bi >= e -1 cc
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
      first_phase[i + 1][simplex_columns - 1] = -b[i];
      for (int j = 0; j < simplex_columns - 1; ++j) {
        first_phase[i + 1][j] *= -1;
      }
    } else {
      first_phase[i + 1][simplex_columns - 1] = b[i];
    }
  }

  // Colocando a identidade (das variáveis adicionais)
  // Essa não deve ser invertida com o 'b'
  for (int i = 0; i < n; ++i) {
    first_phase[i + 1][n + m + i] = 1;
  }

  // Colocando na base canônica
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < simplex_columns; ++j) {
      first_phase[0][j] -= first_phase[i + 1][j];
    }
  }

  bool loop_first_phase = true;
  bool is_viable = true;
  while (loop_first_phase) {
    // for (int j = 0; j < simplex_lines; ++j) {
    //   for (int k = 0; k < simplex_columns; ++k) {
    //     printf(" %+05.1f", first_phase[j][k]);
    //   }
    //   putchar('\n');
    // }
    // putchar('\n');
    for (int i = n; i < simplex_columns - 1; ++i) {
      if (first_phase[0][i] < 0) {
        // BUG Por que isso funciona com o máximo mas não com o mínimo?
        double max_ratio = -INFINITY;
        int max_ratio_index = -1;

        // Encontrando o índice para entrar na base
        for (int j = 0; j < n; ++j) {
          if (first_phase[j + 1][simplex_columns - 1] == 0) {
            continue;
          }
          double ratio =
              first_phase[j + 1][i] / first_phase[j + 1][simplex_columns - 1];
          // Regra de Bland: nós sempre pegamos a variável de menor índice
          // dentre aquelas que podem entrar na base.
          //
          // Um jeito de fazer isso é não trocar a razão quando quando há
          // empate: só trocar quando de fato for maior
          if (ratio > max_ratio) {
            max_ratio = ratio;
            max_ratio_index = j + 1;
          }
        }

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

        // Regra de Bland: para escolher a variável que vai sair da base, nós
        // escolhemos a de menor índice
        //
        // Um jeito de fazer isso é breakar o loop
        break;
      }
      // Se ninguém pode sair da base, sair do loop
      if (i == simplex_columns - 2) {
        // Problema é inviável
        if (first_phase[0][simplex_columns - 1] < 0) {
          is_viable = false;
          printf("inviavel\n");
          for (int j = 0; j < n; ++j) {
            printf("%.6f ", first_phase[0][j]);
          }
          putchar('\n');
        }
        loop_first_phase = false;
      }
    }
  }

  if (is_viable) {
    int second_lines = n + 1;
    int second_columns = n + m + 1;
    double **second_phase = (double **)malloc(sizeof(double *) * second_lines);

    for (int i = 0; i < second_lines; ++i) {
      second_phase[i] = (double *)malloc(sizeof(double) * second_columns);
      memset(second_phase[i], 0, sizeof(int) * second_columns);
    }

    // Criando a linha do topo
    for (int i = 0; i < m; ++i) {
      // TODO(dev): Ui = 1 se bi >= e -1 cc
      second_phase[0][i + n] = -c[i];
    }

    // Botando a matriz 'a' dentro
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < m; ++j) {
        second_phase[i + 1][n + j] = a[i][j];
      }
    }

    // Colocando a identidade
    for (int i = 0; i < n; ++i) {
      second_phase[i + 1][i] = 1;
    }

    // Colocando o vetor de b's
    for (int i = 0; i < n; ++i) {
      second_phase[i + 1][second_columns - 1] = b[i];
    }

    // for (int j = 0; j < second_lines; ++j) {
    //   for (int k = 0; k < second_columns; ++k) {
    //     printf(" %+.1f", second_phase[j][k]);
    //   }
    //   putchar('\n');
    // }

    bool loop_second_phase = true;
    while (loop_second_phase) {
      for (int i = n; i < second_columns - 1; ++i) {
        if (second_phase[0][i] < 0) {
          double min_ratio = INFINITY;
          int min_ratio_index = -1;

          // Encontrando o índice para entrar na base
          for (int j = 0; j < n; ++j) {
            // Isso aqui provavelmente exige mais cuidado
            if (second_phase[j + 1][second_columns - 1] == 0) {
              continue;
            }

            double ratio = second_phase[j + 1][i] /
                           second_phase[j + 1][second_columns - 1];
            // Regra de Bland: nós sempre pegamos a variável de menor índice
            // dentre aquelas que podem entrar na base.
            //
            // Um jeito de fazer isso é não trocar a razão quando quando há
            // empate: só trocar quando de fato for menor
            if (ratio > 0 && ratio < min_ratio) {
              min_ratio = ratio;
              min_ratio_index = j + 1;
            }
          }

          // Pivoteamento
          if (min_ratio != INFINITY) {
            // Parte 1: Dividindo linha pelo maior razão (para o elemento ser 1)
            double multiplier = second_phase[min_ratio_index][i];
            for (int j = 0; j < second_columns; ++j) {
              second_phase[min_ratio_index][j] =
                  second_phase[min_ratio_index][j] / multiplier;
            }
            // Parte 2: Zerando outras colunas
            for (int j = 0; j < second_lines; ++j) {
              // Estamos na coluna que realmente deve ser igual a 1
              if (j == min_ratio_index) {
                continue;
              }
              double ratio =
                  second_phase[j][i] / second_phase[min_ratio_index][i];
              for (int k = 0; k < second_columns; ++k) {
                second_phase[j][k] -= ratio * second_phase[min_ratio_index][k];
              }
            }
          }
          // A solução é ilimitada quando nenhuma das razões é menor que 0
          else {
            printf("ilimitada\n");
            // FIXME na verdade, tem que saber quem está na base
            // Dá pra checar isso "na força bruta" olhando quem é todo 0 exceto
            // na linha com o valor
            for (int j = 0; j < second_lines; ++j) {
              printf("%.6f ", second_phase[j][second_columns - 1]);
            }
            putchar('\n');
            for (int j = 0; j < second_lines; ++j) {
              printf("%.6f ", -second_phase[j][i]);
            }
            putchar('\n');
            loop_second_phase = false;
          }

          // for (int j = 0; j < second_lines; ++j) {
          //   for (int k = 0; k < second_columns; ++k) {
          //     printf(" %.2f", second_phase[j][k]);
          //   }
          //   putchar('\n');
          // }
          // Regra de Bland: para escolher a variável que vai sair da base, nós
          // escolhemos a de menor índice
          //
          // Um jeito de fazer isso é breakar o loop
          break;
        }
        // Se ninguém pode sair da base, sair do loop
        if (i == second_columns - 2 && loop_second_phase) {
          printf("otima\n");
          printf("%.6f\n", second_phase[0][second_columns - 1]);
          // FIXME na verdade, tem que saber quem está na base
          // Dá pra checar isso "na força bruta" olhando quem é todo 0 exceto na
          // linha com o valor
          for (int j = 1; j < second_lines; ++j) {
            printf("%.6f ", second_phase[j][second_columns - 1]);
          }
          putchar('\n');
          for (int j = 0; j < n; ++j) {
            printf("%.6f ", second_phase[0][j]);
          }
          putchar('\n');
          loop_second_phase = false;
        }
      }
    }
    for (int i = 0; i < second_lines; i++) {
      free(second_phase[i]);
    }
    free(second_phase);
  }

  for (int i = 0; i < simplex_lines; i++) {
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
