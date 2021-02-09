#ifndef UNIT_TEST_H
# define UNIT_TEST_H
//# include <unistd.h>
# include <stdio.h>

#define PROTOTYPE(return_type, symbol_name) \
	return_type symbol_name; \
	return_type ft_##symbol_name

#define LOG(format, ...) dprintf(2, "[%s:%d]:" format, __FILE__, __LINE__, __VA_ARGS__)

#define TEST(function) \
do { \
	printf("\n" #function ":"); \
	if (ft_##function != function) \
		printf("FAILURE:\n"); \
	else \
		printf("SUCCESS:\n"); \
} while (0)

#include <errno.h>

#define DEFAULT_ERRNO -42

#define TEST_SYSCALL(syscall) \
do { \
	int errno_result; \
\
	errno = DEFAULT_ERRNO; \
	printf(">>>=  \e[31m" #syscall "\e[0m\n" \
	"--------------------------------------------------------------------------------\n"); \
	\
	ssize_t result = syscall; \
	errno_result = errno; \
	\
	printf("\nRETURN VALUE: %ld\nERNO: %s (%d)\n" \
			"--------------------------------------------------------------------------------\n" \
			">>>=  \e[32mft_" #syscall "\e[0m\n" \
			"--------------------------------------------------------------------------------\n" \
			,result, errno_to_string(errno_result), errno_result); \
	\
	errno = DEFAULT_ERRNO; \
	ssize_t ft_result = ft_##syscall; \
	printf("\nRETURN VALUE: %ld\nERNO: %s (%d)\n" \
			"--------------------------------------------------------------------------------\n" \
			, ft_result, errno_to_string(errno), errno); \
	if (errno != errno_result) \
		printf("Errno value of syscall's differ\n"); \
	else \
		printf("Errno: success\n"); \
	if (result != ft_result) \
	{ \
		printf("Return value of syscall's differ\n" \
				"Standard syscall: %ld\n" \
				"ft_syscall: %ld\n", \
				result, ft_result); \
	} \
	else \
		printf("Return value: success\n"); \
	printf("\n"); \
} while (0)

void test_string();
void test_io_syscalls();

#endif
