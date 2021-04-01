#ifndef LIST_H
# define LIST_H

typedef struct  s_list
{
    void            *data;
    struct s_list   *next;
}               t_list;

t_list **create_list_ptr(t_memory_collector *collector, size_t nb);
t_list *create_list(t_memory_collector *collector, size_t nb);

void dump_list_with_head(const t_args *args, void *output, size_t output_size, void *ret);
void dump_list(const t_args *args, void *output, size_t output_size, void *ret);

# define CREATE_LIST(size) create_list(&collector, size)
# define GET_LIST(size) create_list_ptr(&collector, size)

#endif
