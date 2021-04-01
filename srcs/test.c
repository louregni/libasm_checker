#include "test.h"
#include "display.h"

void execute_units(t_core *core)
{
    unsigned int test_id = 0;
    void *handle = core->handle;

    TEST(strlen);
    TEST(strcpy);
    TEST(strcmp);
    TEST(write);
    TEST(read);
    TEST(strdup);
    TEST(atoi_base);
    TEST(list_push_front);
    TEST(list_size);
    TEST(list_sort);
    TEST(list_remove_if);
}
