#include <stddef.h>
#include "test.h"
#include "utils.h"

void dump_allocated_string(const t_args *args, void *output, size_t output_size, void *return_value)
{
    (void)args;
    size_t len = strnlen((char *)return_value, output_size);
    memmove(output, return_value, len);
}
