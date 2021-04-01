#include <unistd.h> /*write & lseek*/
#include <sys/types.h> /*lseek*/
#include "test.h"
#include "utils.h"

ssize_t collect_memfd_write(t_memory_collector *collector, char *content, off_t content_size)
{
    if (content_size < 0)
        content_size = 1;

    ssize_t fd = collect_memfd(collector, (content_size + 1));
    assert(fd != -1);

    if (write(fd, content, content_size) < 0)
    {
        LOG("test creation issue: write failed\n");
        exit(EXIT_SUCCESS);
    }
    lseek(fd, 0, SEEK_SET);
    return fd;
}
