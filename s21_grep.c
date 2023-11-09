#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <regex.h>
#include <string.h>

#define SIZE 10000

typedef struct {
    int e;
    int i;
    int l;
    int n;
    int c;
    int v;
    int f;
    int o;

    char str_flag_f[SIZE];
    char str_flag_pattern[SIZE];
    char char_flag_e[SIZE];
    char char_flag_f[SIZE];
}option;

void parser(int argc, char *argv[], option *opt);
void reader_files(char *file_name, option *opt, char *pattern, int *count);
void output(char *current, int *count, char *file_name);
void flag_o(char *current, char *pattern, regex_t reg, regmatch_t *regmatch);
void flag_f(option *opt);


void flag_n(char *current, int count_str);
void flag_c(char *current, int *count_str);
void flag_i(char *current, regex_t *reg, char *pattern, option *opt);

int main(int argc, char *argv[]) {
  option opt = {0};
  int count = 0;
  int temp = optind + 1;

  parser(argc, argv, &opt);  // Вызов функции parser для обработки опций

  for ( int i = temp; i < argc; i++) {
    reader_files(argv[i], &opt, argv[optind], &count);
    ++count;
  }

  return 0;
}

void reader_files(char *file_name, option *opt, char *pattern, int *count) {

  FILE *f = fopen(file_name, "r");
  if (f) {
    size_t len = 0;
    ssize_t read = 0;
    regex_t reg;
    regmatch_t regmatch[2];
    char *current = NULL;
    int count_str = 0;
    int flag = 0;
    
    int IFLAG = REG_EXTENDED;
    if(opt->i){
        IFLAG = REG_EXTENDED | REG_ICASE;
    }
    int result = regcomp(&reg, pattern, IFLAG);

    while ((read = getline(&current, &len, f)) != -1) {
        //for(int i = 0; i < read; i++) {
            //if ((current[i] >= 0 && current[i] <= 31) || current[i] == 127) {
                //printf("Binary file matches");
                //return;
            //}
            int re = regexec(&reg, current, 2, regmatch, 0);
            if(opt->e) {
                if(!re)
                    output(current, count, file_name);
                    continue;
            }
            if (opt->l) {
              if (!re) {
                flag = 1;
              }
            }
            if (opt->i) {
              flag_i(current, &reg, pattern, opt);
            }
            if (opt->n) {
              ++count_str;
              if (!re) {
                flag_n(current, count_str);
              }
            }
            if (opt->v) {
              if (re) {
                output(current, count, file_name);
              }
            }
            if(opt->o){
                flag_o(current, pattern, reg, regmatch);    
                continue;
            }
            if(opt->f) {
                opt->f = 1;
                strcpy(opt->char_flag_f, optarg);
                flag_f(opt);
                break;
            }
            if (opt->c) {
              if (!re) {
                flag_c(current, &count_str);
              }
            } else { 
              if (!re && opt->v == 0 && opt->l == 0) {
                output(current, count, file_name);
              }
            }
            //output(current);
        //}
    }
    if (flag == 1) {
      printf("%s", file_name);
    } 
    if (opt->c == 1) {
      printf("%d", count_str);
    }
    fclose(f);

    if (current) {
      free(current);
    }
  } else {
    fprintf(stderr, "Cannot open file: %s\n", file_name);
  }
  fclose(f);
}

void parser(int argc, char *argv[], option *opt) {
  int optt;
  while ((optt = getopt(argc, argv, "e:incvlfo:")) != -1) {
    switch (optt) {
      case 'e':
        opt->e = 1;
        strcat(opt->char_flag_e, optarg);
        strcat(opt->char_flag_e, "|");
        break;
      case 'i':
        opt->i = 1;
        break;
      case 'l':
        opt->l = 1;
        break;
      case 'n':
        opt->n = 1;
        break;
      case 'c':
        opt->c = 1;
        break;
      case 'v':
        opt->v = 1;
        break;
      case 'o':
        opt->o = 1;
      break;
      default:
        fprintf(stderr, "Usage: %s [-ncv] file1 file2 ...\n", argv[0]);
        break;
    }
  }
}

void flag_n(char *current, int count_str) {
    if(current[0] != '\n') {
        printf("%d:", count_str);
    }
}

void flag_c(char *current, int *count_str) {
    ++*count_str;
}

void flag_o(char *current, char *pattern, regex_t reg, regmatch_t *regmatch) {

        char *pointer = current;
        while(regexec(&reg, pointer, 1, regmatch, 0) == 0) {
            printf("%.*s\n", (int)(regmatch->rm_eo - regmatch->rm_so), pointer + regmatch->rm_so);
        pointer += regmatch->rm_eo;
        }
}

void output(char *current, int *count, char *file_name) {
  if (*count == 1){
    printf("%s", current);
  } else {
    printf("%s:%s", file_name, current);
  }
}

void flag_i(char *current, regex_t *reg, char *pattern ,option *opt) {
  for (int i = 0; pattern[i]; i++) {
    regex_t regex;
    if (opt->i) {
        regcomp(&regex, &pattern[i], REG_ICASE);
    }
  }
}

void flag_f(option *opt) {
    FILE *file = fopen(opt->char_flag_f, "rb");
    if(file != NULL) {
        while(!feof(file)) {
            if(fgets(opt->str_flag_f, SIZE, file) != NULL) {
                if(opt->str_flag_f[strlen(opt->str_flag_f) - 1] == '\n' && strlen(opt->str_flag_f) - 1 != 0)
                    opt->str_flag_f[strlen(opt->str_flag_f) - 1] = '\0';
                strcat(opt->str_flag_pattern, opt->str_flag_f);
                strcat(opt->str_flag_pattern, "|");
            }
        }
    }
}
