#include <stdlib.h> /*malloc*/
#include <sys/mman.h> /*mmap*/
#include <assert.h> /*Huh huh*/
#include <unistd.h> /*ftruncate*/
#include <sys/types.h> /*ftruncate*/
#include "utils.h"

static t_mem *create_address(t_mem *base, t_mem *value)
{
    t_mem *mem = malloc(sizeof(*value));
    if (!mem)
    {
        LOG("Shouldn't appen :/ (malloc failed)");
        return NULL;
    }

    *mem = *value;
    base->next = mem;
    return mem;
}

void *collect_mmap(t_memory_collector *collector, size_t length)
{
    /* TODO
    length += 1; This byte will be analyzed to check if the function overflow
    It is eventually possible to place one offset byte at the beginning / end
    of memory, and analyze them to determine if tested function over or underflow
    */
    void *adr = mmap(NULL, length, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    assert(adr != MAP_FAILED);

    t_memmap *mem = malloc(sizeof(t_memmap));
    if (!mem)
    {
        LOG("Shouldn't happen :/ (malloc failed)");
        return NULL;
    }

    mem->pointer = adr;
    mem->length = length;
    mem->next = collector->mmap.next;
    collector->mmap.next = mem;
    return adr;
}

ssize_t collect_memfd(t_memory_collector *collector, off_t length)
{
    if (length < 0)
        length = 1;
    else
        ++length;

    if (collector->latest_fd > 8192)
    {
        LOG("Number of fd overloaded !\n");
        return -1;
    }

    if (sprintf(collector->fd_name, "%u\n", collector->latest_fd) < 0)
    {
        LOG("Failed to create memfd ! exiting\n");
        return -1;
    }

    ssize_t fd = memfd_create(collector->fd_name, 0);
    if (fd == -1)
        return -1;

    {
        int return_value = ftruncate(fd, length);
        if (return_value == -1)
        {
            LOG("ftruncate failure !\n");
            return -1;
        }
    }

    {
        t_mem *tmp = create_address(&collector->memfd, &(t_mem)
                { .pointer = (void *)fd, .next = collector->memfd.next } );
        if (!tmp)
        {
            LOG("Garbage collector allocation failure !\n");
            return -1;
        }
    }
    ++collector->latest_fd;
    return fd;
}

