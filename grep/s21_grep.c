//-vo не должен ничего выводить

#include "s21_grep.h"

#include <getopt.h>
#include <regex.h>
#include <stdio.h>

void flags(regex_t patt_compiled, opt *struct_flags, Counters *counters,
           FILE *file, char *file_name);
void flag_priority(opt *struct_flags);
void parser(int argc, char **argv, opt *struct_flags, char *pattern,
            Counters *counters);
void files(int argc, char **argv, opt *struct_flags, Counters *counters,
           char *pattern);
void if_match(opt *struct_flags, Counters *counters, char *file_name);
void output_filename(opt *struct_flags, char *file_name, Counters *counters);
void flag_o(opt *struct_flags, Counters *counters, char *begin_str,
            regmatch_t *pmatch, char *file_name);
void other_flags(opt *struct_flags, Counters *counters, char *file_name);
void flag_e(char *pattern, char *str_e, Counters *counters);
// void flag_f(Counters *counters, char *patterns);
void flag_f(char *pattern, char **argv, Counters *counters);

int main(int argc, char **argv) {
  char pattern[1024] = {0};
  opt struct_flags = {0};
  Counters counters = {0};
  parser(argc, argv, &struct_flags, pattern, &counters);
  files(argc, argv, &struct_flags, &counters, pattern);

  return 0;
}

void parser(int argc, char **argv, opt *struct_flags, char *pattern,
            Counters *counters) {
  int flag;
  while ((flag = getopt_long(argc, argv, "ncove:lsf:hi", 0, 0)) != -1) {
    switch (flag) {
    case 'n':
      struct_flags->n = 1;
      break;
    case 'c':
      struct_flags->c = 1;
      break;
    case 'o':
      struct_flags->o = 1;
      break;
    case 'v':
      struct_flags->v = 1;
      break;
    case 'l':
      struct_flags->l = 1;
      break;
    case 's':
      struct_flags->s = 1;
      break;
    case 'i':
      struct_flags->i = 1;
      break;
    case 'h':
      struct_flags->h = 1;
      break;
    case 'f':
      struct_flags->f = 1;
      strcpy(&counters->filename, optarg);
      flag_f(pattern, argv, counters);
      break;
    case 'e':
      struct_flags->e = 1;
      flag_e(pattern, optarg, counters);
      break;
    default:
      fprintf(stderr, "Parser file(delete): invalid option\n");
      exit(1);
      break;
    }
    flag_priority(struct_flags);
  }
}

void files(int argc, char **argv, opt *struct_flags, Counters *counters,
           char *pattern) {
  FILE *file;
  counters->str[size] = 0;
  regex_t patt_compiled;
  int cflags = REG_EXTENDED;
  counters->file_count = 1;
  if (!(struct_flags->e || struct_flags->f) && argv[optind]) {
    strcpy(pattern, argv[optind]);
    optind++;
  }

  if (struct_flags->i) {
    cflags = REG_ICASE;
  }

  if ((argc - optind) > 1) {
    counters->file_count++;
  }

  for (int i = optind; i < argc; i++) {
    if (*argv[i] != '-') {
      if ((file = fopen(argv[i], "r")) == NULL) {
        if (!struct_flags->s) {
          fprintf(stderr, "grep: %s: No such file or derectory\n", argv[i]);
          continue;
        }
      }
      if (file != NULL) {
        regcomp(&patt_compiled, pattern, cflags);
        flags(patt_compiled, struct_flags, counters, file, argv[i]);
      } else {
        if (!struct_flags->s) {
          fprintf(stderr, "grep: %s: No such file or derectory\n", argv[i]);
        }
      }
    }
    if (file != NULL) {
      regfree(&patt_compiled);
      fclose(file);
    }
  }
}

void flags(regex_t patt_compiled, opt *struct_flags, Counters *counters,
           FILE *file, char *file_name) {
  regmatch_t pmatch[1];
  counters->line = 0;
  counters->count = 0;
  while (fgets(counters->str, size - 1, file)) {
    counters->match = 0;
    counters->count++;
    int res_regexec = 0;
    char *begin_str = counters->str;
    while ((res_regexec = regexec(&patt_compiled, begin_str, 1, pmatch, 0)) ==
           0) {
      if (counters->match == 0 && !struct_flags->n &&
          (counters->file_count > 1) && !struct_flags->l && !struct_flags->o &&
          !struct_flags->c && !struct_flags->h && !struct_flags->i) {
        printf("%s:", file_name);
      }

      if (struct_flags->o) {
        flag_o(struct_flags, counters, begin_str, pmatch, file_name);
      }

      counters->match = 1;
      begin_str += pmatch[0].rm_eo;
    }
    if (struct_flags->v) {
      counters->match = !counters->match;
    }
    if (!struct_flags->o) {
      if (counters->match) {
        if_match(struct_flags, counters, file_name);
      }
    }

    if (counters->match && struct_flags->n && !struct_flags->o) {
      printf("%d:", counters->count);
    }
    if (counters->match && !struct_flags->c && !struct_flags->l &&
        !struct_flags->o) {
      if (strchr(counters->str, '\n') == NULL) {
        strcat(counters->str, "\n");
      }
      printf("%s", counters->str);
    }
    counters->line += !!counters->match;
  }
  other_flags(struct_flags, counters, file_name);
}

void output_filename(opt *struct_flags, char *file_name, Counters *counters) {
  if (counters->match == 0 && !struct_flags->h && !struct_flags->l &&
      !struct_flags->c && !struct_flags->o) {
    printf("%s:", file_name);
  }
}

void if_match(opt *struct_flags, Counters *counters, char *file_name) {
  if (!struct_flags->c && !struct_flags->l) {
    if (counters->file_count > 1 && !struct_flags->l && !struct_flags->h &&
        !struct_flags->c) {
      printf("%s:", file_name);
    }
  }
}

void other_flags(opt *struct_flags, Counters *counters, char *file_name) {
  if (struct_flags->l && counters->line > 0 && !struct_flags->c) {
    printf("%s\n", file_name);
  }
  if (struct_flags->c && !struct_flags->l && !struct_flags->h) {
    if (counters->file_count > 1) {
      printf("%s:%d\n", file_name, counters->line);
    } else {
      printf("%d\n", counters->line);
    }
  } else if (struct_flags->c && !struct_flags->l && struct_flags->h) {
    printf("%d\n", counters->line);
  }
  if (struct_flags->c && struct_flags->l) {
    if (counters->file_count < 2) {
      if (!struct_flags->n && !struct_flags->o) {
        printf("%d\n%s\n", counters->file_count, file_name);
      } else {
        printf("%s:%d\n", file_name, counters->line);
      }
    } else {
      if (counters->line && !struct_flags->h) {
        printf("%s:%d\n", file_name, counters->file_count - 1);
        printf("%s\n", file_name);
      } else if (!counters->line && !struct_flags->h) {
        printf("%s:0\n", file_name);
      } else if (counters->line && struct_flags->h) {
        printf("%d\n%s\n", counters->file_count - 1, file_name);
      } else if (!counters->line && struct_flags->h) {
        printf("0\n");
      }
    }
  }
}

void flag_o(opt *struct_flags, Counters *counters, char *begin_str,
            regmatch_t *pmatch, char *file_name) {
  if (counters->file_count > 1) {
    printf("%s:", file_name);
  }

  if (struct_flags->n && struct_flags->o) {
    printf("%d:", counters->count);
  }
  if (struct_flags->o) {
    for (int i = pmatch[0].rm_so; i < pmatch[0].rm_eo; i++) {
      printf("%c", begin_str[i]);
    }
    printf("\n");
  }
}

void flag_e(char *pattern, char *str_e, Counters *counters) {
  strcpy(counters->str, str_e);
  if (*pattern != '\0') {
    strcat(pattern, "|");
  }
  strcat(pattern, counters->str);
}

/*
void flag_f(Counters *counters, char *patterns) {
  FILE *f;
  f = fopen(&counters->filename, "rb");
  if (f != NULL) {
    while (!feof(f)) {
      if (fgets(counters->str, 1000, f) != NULL) {
        if (counters->str[strlen(counters->str) - 1] == '\n' &&
            strlen(counters->str) - 1 != 0)
          counters->str[strlen(counters->str) - 1] = '\0';
        strcat(patterns, counters->str);
        strcat(patterns, "|");
      }
    }
    fclose(f);
  }
}
*/

void flag_f(char *pattern, char **argv, Counters *counters) {
  FILE *fp;
  char strtwo[size] = {0};
  fp = fopen(argv[optind - 1], "r");
  if (fp != NULL) {
    while (fgets(strtwo, size - 1, fp) != NULL) {
      int p = strlen(strtwo);
      if (strtwo[p - 1] == '\n') {
        strtwo[p - 1] = '\0';
      }
      if (strtwo[0] == '\0') {
        strcpy(strtwo, ".");
      }
      flag_e(pattern, strtwo, counters);
    }
  } else {
    fprintf(stderr, "grep: %s: No such file or directory\n", argv[optind]);
  }
  fclose(fp);
}

void flag_priority(opt *struct_flags) {
  if (struct_flags->l) {
    struct_flags->n = 0;
    struct_flags->o = 0;
  }
  if (struct_flags->c) {
    struct_flags->o = 0;
    struct_flags->n = 0;
  }
  if (struct_flags->v) {
    struct_flags->o = 0;
  }
}
