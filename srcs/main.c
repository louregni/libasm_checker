#include <dlfcn.h>

#include "unit-test.h"

typedef ssize_t (ft_read)(int fd, void *buf, size_t count);

#define CREATE_TEST(ft_name) \
{ \
	void *k = dlsym(); \
}

int	main()
{
	void *libasm = dlopen("libasm.a", RTLD_LAZY);
	if (!libasm)
		printf("could not load libasm library !\n");
	else
		printf("loading successful\n");

	t_function ft_unit[] = {
		{
			.ft_function = "ft_read",
			.test = {
			}
		}
	};
//	test_string();
	test_io_syscalls();
	return 0;
}
