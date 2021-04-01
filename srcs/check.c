#include <sys/mman.h>
#include "utils.h"
#include "test.h"
#include "unit-test.h"

static int check_terminate(t_test *const test)
{
    return test->ft_result.terminate_code != test->std_result.terminate_code;
}

void check_strlen(t_test *const test)
{
    if (check_terminate(test)) {
        test->valid = KO;
        return ;
    }

    ssize_t ft_ret = *((ssize_t *)test->ft_result.return_value);
    ssize_t std_ret = *((ssize_t *)test->std_result.return_value);

    test->valid = ft_ret != std_ret;
}

void check_strcmp(t_test *const test)
{
    if (check_terminate(test)) {
        test->valid = KO;
        return ;
    }

    int ft_ret = *((int *)test->ft_result.return_value);
    int std_ret = *((int *)test->std_result.return_value);

    test->valid = (!std_ret == !ft_ret) || (std_ret < 0 && ft_ret < 0) || (std_ret > 0 && ft_ret > 0);
    test->valid = KO - test->valid;
}

void check_write(t_test *const test)
{
    if (check_terminate(test)) {
        test->valid = KO;
        return ;
    }

    const t_args *const std_args = &test->std_args;
    const t_args *const ft_args = &test->ft_args;
    int std_errno = *((int *)test->std_result.errno_value);
    int ft_errno = *((int *)test->ft_result.errno_value);
    ssize_t std_ret = *((ssize_t *)test->std_result.return_value);
    ssize_t ft_ret = *((ssize_t *)test->ft_result.return_value);

    int std_fd = (ssize_t)std_args->first;
    if (std_fd > 0) {
        test->std_output_size = lseek(std_fd, 0, SEEK_END);
        lseek(std_fd, 0, SEEK_CUR);
        test->std_output = mmap(NULL, test->std_output_size, PROT_READ | PROT_WRITE, MAP_SHARED, std_fd, 0);
        if (test->std_output == MAP_FAILED)
            LOG("check write tests result: failed to mmap memfd\n");
    }

    int ft_fd = (ssize_t)ft_args->first;
    if (ft_fd > 0) {
        test->ft_output_size = lseek(ft_fd, 0, SEEK_END);
        lseek(ft_fd, 0, SEEK_CUR);
        test->ft_output = mmap(NULL, test->ft_output_size, PROT_READ, MAP_SHARED, ft_fd, 0);
        if (test->ft_output == MAP_FAILED)
            LOG("check write tests result: failed to mmap memfd\n");
    }

    if (std_errno != ft_errno) {
        test->valid = KO;
        return ;
    }

    if (std_ret < 1 && std_ret == ft_ret) {
        test->valid = OK;
        return ;
    }

    test->valid = !((std_ret == ft_ret) && !memcmp(test->std_output, test->ft_output, std_ret));
}

void check_read(t_test *const test)
{
    if (check_terminate(test)) {
        test->valid = KO;
        return ;
    }

    const t_args *const std_args = &test->std_args;
    const t_args *const ft_args = &test->ft_args;
    int std_errno = *((int *)test->std_result.errno_value);
    int ft_errno = *((int *)test->ft_result.errno_value);
    ssize_t std_ret = *((ssize_t *)test->std_result.return_value);
    ssize_t ft_ret = *((ssize_t *)test->ft_result.return_value);

    if (std_errno != ft_errno) {
        test->valid = KO;
        return ;
    }

    if (std_ret < 1 && std_ret == ft_ret) {
        test->valid = OK;
        return ;
    }

    if ((size_t)std_args->third > 0) {
        test->std_output = (char *)std_args->second;
        test->std_output_size = (size_t)std_args->third;
        test->ft_output = (char *)ft_args->second;
        test->ft_output_size = (size_t)std_args->third;
    }

    test->valid = !((std_ret == ft_ret) && !memcmp(std_args->second, ft_args->second, std_ret));
}

void check_strcpy(t_test *const test)
{
    if (check_terminate(test)) {
        test->valid = KO;
        return ;
    }
    test->valid = !!memcmp(test->std_output, test->ft_output, test->std_output_size);
}

void check_strdup(t_test *const test)
{
    if (check_terminate(test)) {
        test->valid = KO;
        return ;
    }
    test->valid = !((test->std_output_size == test->ft_output_size) && \
            !memcmp(test->std_output, test->ft_output, test->std_output_size));
}

void check_atoi_base(t_test *const test)
{
    ssize_t std_ret = *((ssize_t *)test->std_result.return_value);
    ssize_t ft_ret = *((ssize_t *)test->ft_result.return_value);

    if (check_terminate(test)) {
        test->valid = KO;
        return ;
    }
    test->valid = std_ret != ft_ret;
}

#include "list.h"

void check_list_push_front(t_test *const test)
{
    if (check_terminate(test)) {
        test->valid = KO;
        return ;
    }
    test->valid = !!memcmp(test->std_output, test->ft_output, test->std_output_size);
}

void check_list_size(t_test *const test)
{
    if (check_terminate(test)) {
        test->valid = KO;
        return ;
    }

    ssize_t std_ret = *((ssize_t *)test->std_result.return_value);
    ssize_t ft_ret = *((ssize_t *)test->ft_result.return_value);

    test->valid = std_ret != ft_ret;
    if (!test->valid)
        test->valid = !!memcmp(test->std_output, test->ft_output, test->std_output_size);
}

void check_list_sort(t_test *const test)
{
    if (check_terminate(test)) {
        test->valid = KO;
        return ;
    }

    test->valid = !!memcmp(test->std_output, test->ft_output, test->std_output_size);
}

void check_list_remove_if(t_test *const test)
{
    if (check_terminate(test)) {
        test->valid = KO;
        return ;
    }

    test->valid = !!memcmp(test->std_output, test->ft_output, test->std_output_size);
}
