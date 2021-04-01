#include <ctype.h>
#include <stdlib.h>
#include "test.h"
#include "utils.h"
#include "list.h"

int compare(void *a, void *b)
{
    if ((size_t)a > (size_t)b)
        return 1;
    return 0;
}

void change_data(void *data)
{
    (void)data;
}

int		atoi_base(char *str, char *base)
{
    if (!base || !*base || !*(base + 1))
        return 0;

    for (size_t i = 0; base[i]; ++i)
    {
        if (base[i] == '+' || base[i] == '-')
            return 0;
        for (size_t c = i + 1; base[c]; ++c)
            if (base[c] == base[i])
                return 0;
    }

    while (isspace(*str))
        ++str;

    int sign = 1;
    while (*str == '-' || *str == '+')
    {
        if (*str == '-')
            sign = 1 - sign;
        ++str;
    }

    unsigned int base_size = strlen(base);
    int nb = 0;
    while (*str)
    {
        size_t i = 0;
        while(base[i] && *str != base[i])
            ++i;
        if (base[i] != *str)
            break;

        nb *= base_size;
        nb += base[*str % base_size];
        ++str;
    }
    if (!sign)
        return nb * -1;
    return nb;
}

void    list_push_front(t_list **begin_list, void *data)
{
    t_list *elem = malloc(sizeof(*elem));
    if (elem)
    {
        elem->data = data;
        elem->next = *begin_list;
        *begin_list = elem;
    }
}

int     list_size(t_list *begin_list)
{
    int size = 0;
    while (begin_list)
    {
        begin_list = begin_list->next;
        ++size;
    }
    return size;
}

void    list_sort(t_list **begin_list, int (*cmp)())
{
    t_list *list = *begin_list;
    void *data;

    while (list)
    {
        if (list->next)
        {
            if (cmp(list->data, list->next->data) > 0)
            {
                data = list->data;
                list->data = list->next->data;
                list->next->data = data;
                list = *begin_list;
                continue ;
            }
        }
        list = list->next;
    }
}

void list_remove_if(t_list **begin_list, void *data_ref, int (*cmp)(), void (*free_fct)(void *))
{
    t_list *list = *begin_list;

    while (list)
    {
        if(!cmp(list->data, data_ref))
            free_fct(list->data);
        list = list->next;
    }
}
