#include "s21_grep.h"
#include <stdio.h>
#include <getopt.h>
#include <regex.h>

void flags(regex_t patt_compiled, opt *struct_flags, Counters *counters, FILE *file, char *file_name);
void flag_priority(opt struct_flags);
void parser(int argc, char **argv, opt *struct_flags, char *pattern);
void files(int argc, char **argv, opt *struct_flags, Counters *counters, char *pattern);
void if_match(opt *struct_flags, Counters* counters);
void output_filename(opt *struct_flags, char *file_name, Counters *counters);
void flag_o(opt *struct_flags, Counters *counters, char *begin_str, regmatch_t *pmatch, char *file_name);

int main(int argc, char **argv) {
	char pattern[1024] = {0};
	opt struct_flags = {0};
	Counters counters = {0};
	parser(argc, argv, &struct_flags, pattern);
	files(argc, argv, &struct_flags, &counters, pattern);

	return 0;
}

void parser(int argc, char **argv, opt *struct_flags, char *pattern) {
	int flag;
	while((flag = getopt_long(argc, argv, "nco", 0, 0)) != -1) {
		switch(flag) {
			case 'n':
				struct_flags->n = 1;
				break;
			case 'c':
				struct_flags->c = 1;
				break;
			case 'o':
				struct_flags->o = 1;
				break;
			default:
				fprintf(stderr, "Parser file(delete): invalid option\n");
				exit(1);
				break;
		}
		flag_priority(*struct_flags);
	}
}

void files(int argc, char **argv, opt *struct_flags, Counters *counters, char *pattern) {
	FILE *file;
	counters->str[size] = '\0';
	regex_t patt_compiled;
	int cflags = REG_EXTENDED;
	counters->file_count = 1;
	if(!(struct_flags->e || struct_flags->f) && argv[optind]) {
		strcpy(pattern, argv[optind]);
		optind++;
	}

	if((argc - optind) > 1) {
		counters->file_count++;
	}

	for(int i = optind; i < argc; i++) {
		if(*argv[i] != '-') {
			if((file = fopen(argv[i], "r")) == NULL) {
				if(!struct_flags->s) {
					fprintf(stderr, "grep: %s: No such file or derectory\n", argv[i]);
					continue;
				}
			}
			if(file != NULL) {
				regcomp(&patt_compiled, pattern, cflags);
				flags(patt_compiled, struct_flags, counters, file, argv[i]);
			} else {
				if(!struct_flags->s) {
					fprintf(stderr, "grep: %s: No such file or derectory\n", argv[i]);
				}
			}
		}
		if(file != NULL) {
			regfree(&patt_compiled);
			fclose(file);
		}
	}
}

void flags(regex_t patt_compiled, opt *struct_flags, Counters *counters, FILE *file, char *file_name) {
	regmatch_t pmatch[1];
	while(fgets(counters->str, size - 1, file)) {
		counters-> match = 0;
		counters->count++;
		int res_regexec = 0;
		char *begin_str = counters->str;
		while((res_regexec = regexec(&patt_compiled, begin_str, 1, pmatch, 0)) == 0) {
			if(counters->match == 0 && (counters->file_count > 1) && !struct_flags->l && !struct_flags->o && !struct_flags->c && !struct_flags->h) {
				printf("%s:", file_name);
			}
			if(counters->file_count > 1) {
				output_filename(struct_flags, file_name, counters);
			}
			if(struct_flags->o) {
				flag_o(struct_flags, counters, begin_str, pmatch, file_name);
			}

			counters->match = 1;
			begin_str += pmatch[0].rm_eo;
		}
		if(!struct_flags->o) {
			if(counters->match) {
				if_match(struct_flags, counters);
			}
		}
	}
}

void output_filename(opt *struct_flags, char *file_name, Counters* counters) {
			if(counters->match == 0 && !struct_flags->h && !struct_flags->l && !struct_flags->c && !struct_flags->o) {
				printf("%s:", file_name);
			}
}

void if_match(opt *struct_flags, Counters* counters) {
	if(struct_flags->n) {
		printf("%d:", counters->count);
	}
	
	if(!struct_flags->c && !struct_flags->l && !struct_flags->o) {
		if(strchr(counters->str, '\n') == NULL) {
			strcat(counters->str, "\n");
		}
		printf("%s", counters->str);
	}
	counters->line += counters->match; //В оригинале += !!match
}

void flag_o(opt *struct_flags, Counters *counters, char *begin_str, regmatch_t *pmatch, char *file_name) {
	if(counters->file_count > 1){ 
		printf("%s:", file_name);
	}
	if(struct_flags->n && struct_flags->o) {
		printf("%d:", counters->count);
	} 
	if(struct_flags->o) {
		for(int i = pmatch[0].rm_so; i < pmatch[0].rm_eo; i++) {
			printf("%c", begin_str[i]);
		}
		printf("\n");
	}
}


void flag_priority(opt struct_flags) {
	if(struct_flags.l) {
		struct_flags.n = 0;
		struct_flags.o = 0;
	}
	if(struct_flags.c) {
		struct_flags.o = 0;
		struct_flags.n = 0;
	}
	if(struct_flags.v) {
		struct_flags.o = 0;
	}
}
