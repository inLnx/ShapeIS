#ifndef LIBC_GETOPT_H
#define LIBC_GETOPT_H 

struct option {
	const char *name;
	int has_arg;
	int *flag;
	int val;
};

/**
 * @brief Get the opt long object
 * 
 * @param argc 
 * @param argv 
 * @param optstring 
 * @param longopts 
 * @param longindex 
 * @return int 
 */
int getopt_long(int argc, char *const argv[], const char *optstring, const struct option *longopts, int *longindex);

#define no_argument 0
#define required_argument 1
#define optional_argument 2 

#endif