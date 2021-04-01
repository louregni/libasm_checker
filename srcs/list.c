#include "test.h"
#include "utils.h"
#include "list.h"

t_list *create_list(t_memory_collector *collector, size_t nb)
{
    t_list *list = collect_mmap(collector, (nb * sizeof(*list)));
    for (size_t i = 0; i < nb; ++i)
    {
        list[i].data = (void*)(nb - i);
        list[i].next = (list + i + 1);
    }
    if (nb)
        list[nb - 1].next = 0;

    return list;
}

t_list **create_list_ptr(t_memory_collector *collector, size_t nb)
{
    t_list **begin = collect_mmap(collector, sizeof(begin));
    *begin = create_list(collector, nb);
    return begin;
}

void print_list(t_list *a)
{
    for (size_t i = 0; a; ++i)
    {
        LOG("%zu] data : %zu | next : %p\n", i, (size_t)a->data, a->next);
        a = a->next;
    }
}

void dump_list(const t_args *args, void *output, size_t output_size, void *ret)
{
    (void)ret;
    t_list *list = (t_list *)args->first;
    while (list && output_size)
    {
        memcpy(output, &list->data, sizeof(void *));
        output_size -= sizeof(void *);
        output += sizeof(void *);
        list = list->next;
    }
}

void dump_list_with_head(const t_args *args, void *output, size_t output_size, void *ret)
{
    (void)ret;
    t_list *list = *((t_list **)args->first);
    while (list && output_size)
    {
        memcpy(output, &list->data, sizeof(void *));
        output_size -= sizeof(void *);
        output += sizeof(void *);
        list = list->next;
    }
}
