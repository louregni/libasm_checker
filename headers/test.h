#ifndef TEST_H
# define TEST_H
# include "sandbox.h"

typedef struct	s_statement
{
	int     *errno_value;
	void    *return_value;

	int     terminate_code;
}				t_statement;

typedef struct	s_args {
	const void *const first;
	const void *const second;
	const void *const third;
	const void *const fourth;
}				t_args;

typedef struct	s_test{
	/*input*/
	const t_args std_args;
	const t_args ft_args;

	/*output*/
	t_statement std_result;
	t_statement ft_result;

    /*sandbox relative*/
    t_sandbox   std_sandbox;
    t_sandbox   ft_sandbox;

    void *std_output;
    size_t std_output_size;
    void *ft_output;
    size_t ft_output_size;

    int valid;
}				t_test;

typedef struct s_function{
	const char *name;

	t_test *const test;
	size_t nb_test;
	void (*check)(t_test *const test);
    void (*callback)(const t_args *args, void *output, size_t output_size, void *ret);

	size_t format;
}				t_function;

#define NB_FUNCTION 11
#define MMAP_SIZE 4096
typedef struct  s_core
{
    const t_function ft_unit[NB_FUNCTION];
    void *mmap_buffer;
    void *sandbox_return;

    void *handle;
}               t_core;

void    execute_units(t_core *core);
void    collect_results(t_core *core);

#ifndef OK
#define OK 0
#endif
#define KO 1

/*
 *   ARGUMENT 4     ARGUMENT 3     ARGUMENT 2     ARGUMENT 1   RETURN VALUE
 * [100000000000]  [100000000000]  [100000000000]  [100000000000] [000000000000]
 *  63        51    50        38    37        25    24        12   11         0
 *
 *  12 bits / argument
 *  12 bits / return value
 */

#define INT 0ULL
#define SIZE_T 1ULL
#define SSIZE_T 2ULL
#define STR 3ULL
#define VOID 4ULL
#define LIST 5ULL
#define STAR_STAR_LIST 6ULL
#define STAR_VOID 7ULL
#define FUNC_PTR 8ULL
#define PTR 9ULL
#define BUFFER_TO_FILL 10ULL

#define POS_ARG_1 12ULL
#define POS_ARG_2 24ULL
#define POS_ARG_3 36ULL
#define POS_ARG_4 48ULL
#define BITS_PER_ARG 11ULL

#define ARG_1 POS_ARG_1 | 1ULL << (POS_ARG_1 + BITS_PER_ARG)
#define ARG_2 POS_ARG_2 | 1ULL << (POS_ARG_2 + BITS_PER_ARG)
#define ARG_3 POS_ARG_3 | 1ULL << (POS_ARG_3 + BITS_PER_ARG)
#define ARG_4 POS_ARG_4 | 1ULL << (POS_ARG_4 + BITS_PER_ARG)

#endif
