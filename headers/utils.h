#ifndef UTILS_H
# define UTILS_H
# include <sys/types.h>

/*
 * Thoses functions memory with a garbage collector for each, used for each tests involving a
 * filedescriptor, or a test returning heap memory (which will be move in shared memory for analyze).
 */

typedef struct s_mem t_mem;
struct s_mem {
    void    *pointer;
    t_mem   *next;
};

typedef struct s_memmap t_memmap;
struct s_memmap {
    void    *pointer;
    off_t   length;
    t_memmap   *next;
};

typedef struct s_memory_collector t_memory_collector;
struct s_memory_collector {
    t_mem       memfd;
    t_memmap    mmap;
    unsigned    latest_fd;
    char        fd_name[6];
};

/*create a shared mapped memory and back-up informations*/
void *collect_mmap(t_memory_collector *collector, size_t length);

/*
 * create file descriptor on memory, collect basic details to be able to release 
 */
ssize_t collect_memfd(t_memory_collector *collector, off_t length);

/*
 * same as collect_memfd, but write some content inside the fresh fd.
 */
ssize_t collect_memfd_write(t_memory_collector *collector, char *content, off_t content_size);

#include <stdio.h>
#define LOG(format, ...) \
    dprintf(2, "[DEBUG][%s:%d]:" format, __FILE__, __LINE__, ##__VA_ARGS__)
#endif
