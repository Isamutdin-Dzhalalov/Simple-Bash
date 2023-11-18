#ifndef S21_CAT
#define S21_CAT

#include <ctype.h>
#include <getopt.h>
#include <stdio.h>

typedef struct {
  int n;
  int b;
  int s;
  int e;
  int t;
  int v;
  int E;
  int T;
} StructFlags;

struct option long_options[] = {{"number_nonblank", no_argument, NULL, 'b'},
                                {"number", no_argument, NULL, 'n'},
                                {"squeeze-blank", no_argument, NULL, 's'},
                                {0, 0, 0, 0}};

void flag_n_b(char ch, StructFlags *struct_flags, int *count, char *previous);
void flag_e(char ch, char *previous);
void flag_s(char ch, char *previous_flag_s, int *count_flag_s);
void flag_t();
void print_char(char ch, StructFlags *structflags);

int my_strcmp(char *str1, char *str2);

#endif
