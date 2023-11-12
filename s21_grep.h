#ifndef SRC_GREP_S21_GREP_H_
#define SRC_GREP_S21_GREP_H_

#define size 4096

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
	int match;
	int line;
	int count;
	int file_count;

	char str[size];
} Counters;

typedef struct flags {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
} opt;

void flag_priority(opt struct_flags);

#endif  // SRC_GREP_S21_GREP_H_
