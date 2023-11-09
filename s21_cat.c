#include "s21_cat.h"

int main(int argc, char **argv) {
	StructFlags struct_flags = {0}; 
	char flags_array[12] = "EenbsvtT";
	FILE *file = NULL;
	int quantity_files = 0;

	for(int i = 1; i < argc; i++) { 
		if(*argv[i] != '-') {
			file = fopen(argv[i], "r");
			quantity_files = 1;
		}
	}

	if(argc - optind == 0) {
		fprintf(stderr, "Введите параметры!\n", file);
		return -1;
	} else if(quantity_files == 0) {
		fprintf(stderr, "Введите файл!\n", file);
		return -1;
	} else if(file == NULL) {
		fprintf(stderr, "Такого файла не существует!\n", file);
		return -1;
	}

	int index_getopt = 0;
	char getopt_result = getopt_long(argc, argv, flags_array, long_options, &index_getopt);
	while(getopt_result != -1) {
		switch(getopt_result) {
			case's':
				struct_flags.s = 1;
				break;
			case 'n':
				struct_flags.n = 1;
				break;
			case 'e':
				struct_flags.e = 1;
				break;
			case 'b':
				struct_flags.b = 1;
				break;
			case 't':
				struct_flags.t = 1;
				break;
			case 'v':
				struct_flags.v = 1;
				break;
			case 'E':
				struct_flags.E = 1;
				break;
			case 'T':
				struct_flags.T = 1;
				break;
			case '?':
			default:
				printf("ERROR default");
		}
		getopt_result = getopt_long(argc, argv, flags_array, long_options, &index_getopt);
	}
	if(struct_flags.b == 1) 
		struct_flags.n = 0;


	int count = 1, count_flag_b = 1, count_flag_s = 0;
	char previous = '\n';
	char previous_flag_s = '\n';
	char ch = fgetc(file);
		while(!feof(file)) {
			if(struct_flags.s == 1) 
				flag_s(ch, &previous_flag_s, &count_flag_s);		
			if(count_flag_s < 2) {
				if(struct_flags.n == 1 || struct_flags.b == 1)
					flag_n_b(ch, &struct_flags, &count, &previous);
				if(struct_flags.e == 1 || struct_flags.E)
					flag_e(ch, &struct_flags, &previous);
				if(struct_flags.t == 1 && ch == '\t' || struct_flags.T == 1 && ch == '\t') {
					flag_t(ch);
					ch = fgetc(file);
					continue;
				}
			print_char(ch, &struct_flags);
			}
				
			ch = fgetc(file);
		}
	fclose(file);
	return 0;
}

void flag_n_b(char ch, StructFlags *struct_flags, int *count, char *previous) {
	if(struct_flags->b) {
		if(ch != '\n') { 
			if(*previous == '\n')
			fprintf(stdout, "%6d\t", (*count)++);
		}  
	}

	if(struct_flags->n) {
		if(*previous == '\n')
		fprintf(stdout, "%6d\t", (*count)++);
	}
	*previous = ch;
}

void flag_e(char ch, StructFlags *struct_flags, char *previous) {
	if(ch == '\n') 
		fputc('$', stdout);

	*previous = ch;

}

void flag_s(char ch, char *previous_flag_s, int *count_flag_s) {
	if(ch == '\n' && *previous_flag_s == '\n') 
		(*count_flag_s)++;
	 else 
		(*count_flag_s) = 0;
	
	*previous_flag_s = ch;
}	

void flag_t(char ch) {
	fputc('^', stdout);
	fputc('I', stdout);
}

void print_char(char ch, StructFlags *struct_flags) {
	if(struct_flags->e || struct_flags->t || struct_flags->v) {
		if(ch != '\t' && ch != '\n' && iscntrl(ch)) {
			fputc('^', stdout);
			if(ch == 127) 
				fputc(ch - 64, stdout);
			else
				fputc(ch + 64, stdout);
		} else
			fputc(ch, stdout);
	} else
		fputc(ch, stdout);
}
