#include <sys/mman.h>
#include <linux/memfd.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "unit-test.h"

int result_is_valid(char *a, char *b, ssize_t size)
{
	return memcmp(a, b, size);
}

void test_io_syscalls()
{
	PROTOTYPE(ssize_t, write(int fd, const void *buf, size_t count));

	/*Valid case*/
	TEST_SYSCALL(write(1, "Bonjour à tous", sizeof("Bonjour à tous")));
	/*Invalid fd*/
	TEST_SYSCALL(write(-1, "Bonjour à tous", sizeof("Bonjour à tous")));
	/*invalid buf*/
	TEST_SYSCALL(write(1, 0, 12));
	/*invalid nbytes*/
	TEST_SYSCALL(write(1, "", 0));

// int memfd_create(const char *name, unsigned int flags);
	PROTOTYPE(ssize_t, read(int fd, void *buf, size_t count));
	int fd = memfd_create("read", 0);

	if (fd != -1)
	{
		const char *test = "Bonjour à tous" ;
		const size_t test_len = sizeof("Bonjour à tous");
		printf("test_len : %d\n", test_len);

		if (ftruncate(fd, 4096))
			LOG("READ: ftruncate failed to allocate place - %s\n", strerror(errno));
		char result_read[test_len];
		char result_ft_read[test_len];
		memset(result_read, 0, test_len);
		memset(result_ft_read, 0, test_len);

		write(1, test, test_len);
		if (write(fd, test, test_len) > 0)
		{
			lseek(fd, 0, SEEK_SET);
			ssize_t return_value_read = read(fd, result_read, test_len);
			lseek(fd, 0, SEEK_SET);
			ssize_t return_value_ft_read = ft_read(fd, result_ft_read, test_len);

			if (!result_is_valid(result_read, result_ft_read, test_len))
				printf("read: success\n");
			else
				printf("read: failure\n");
			printf("return value : %d - read text : |%*s|\n", return_value_read, test_len, result_read);

			if (return_value_read == return_value_ft_read)
				printf("read: success\n");
			else
				printf("read: failure - %ld - %ld\n",
					   return_value_read, return_value_ft_read);
		}
		else
		{
			LOG("READ: write failed - %s\n", strerror(errno));
		}
	}
	else
	{
		LOG("READ: Unable to create file-descriptor - %s\n", strerror(errno));
	}
}
