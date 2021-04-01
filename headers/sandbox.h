#ifndef SANDBOX_H
# define SANDBOX_H
#include <sys/types.h>
#include <unistd.h> /*fork*/
#include <dlfcn.h> /*dlsym*/
#include <string.h> /*strlen*/
#include <assert.h>
#include <stdlib.h>
#include <errno.h>

typedef struct s_sandbox
{
    pid_t       pid;
    int         wstatus;
}               t_sandbox;

#define SANDBOX_FUNCTION(dest, function) \
do { \
    t_sandbox *sandbox = &test->dest##_sandbox; \
\
	sandbox->pid = fork(); \
	assert(sandbox->pid != -1); \
\
    const t_args *args = (const t_args *)&test->dest##_args; \
    /*Child*/ \
	if (!sandbox->pid) \
	{ \
        close(2); \
		errno = -1; \
		void *return_value = function(args->first, args->second, args->third, args->fourth); \
        if (unit->callback) \
            unit->callback(args, test->dest##_output, test->dest##_output_size, return_value); \
        memcpy((int *)core->sandbox_return + 1, &return_value, sizeof(void *)); \
        memcpy(core->sandbox_return, &errno, sizeof(int)); \
		exit(EXIT_SUCCESS); \
	} \
    /*Parent*/ \
} while (0)

#define TEST(function, ...) \
do { \
    t_function *unit = (t_function *)&core->ft_unit[test_id]; \
\
    void *(*ft_function)() = dlsym(handle, "ft_" #function); \
    void *(*std_function)() = dlsym(handle, #function); \
	if (ft_function && std_function) \
	{ \
	    t_test *test; \
\
		for (unsigned int i = 0; i < unit->nb_test; ++i) \
		{ \
			test = (t_test *)&unit->test[i]; \
\
			SANDBOX_FUNCTION(std, std_function); \
            test->std_result.errno_value = core->sandbox_return; \
            test->std_result.return_value = (int *)core->sandbox_return + 1; \
            core->sandbox_return = (char *)core->sandbox_return + 12; \
\
			SANDBOX_FUNCTION(ft, ft_function); \
            test->ft_result.errno_value = core->sandbox_return; \
            test->ft_result.return_value = (int *)core->sandbox_return + 1; \
            core->sandbox_return = (char *)core->sandbox_return + 12; \
\
		} \
	} \
	else \
    { \
		unit->nb_test = 0; \
    } \
	++test_id; \
} while (0)

#endif
