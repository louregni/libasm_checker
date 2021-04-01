#ifndef UNIT_TEST_H
# define UNIT_TEST_H

/*function validating results*/
void check_write(t_test *const test);
void check_strcmp(t_test *const test);
void check_strcpy(t_test *const test);
void check_strlen(t_test *const test);
void check_read(t_test *const test);
void check_strdup(t_test *const test);
void check_atoi_base(t_test *const test);
void check_list_push_front(t_test *const test);
void check_list_size(t_test *const test);
void check_list_sort(t_test *const test);
void check_list_remove_if(t_test *const test);
int compare(void *a, void *b);
void change_data(void *data);

void dump_allocated_string(const t_args *args, void *output, size_t output_size, void *ret);

#define NARGS_SEQ(_1,_2,_3,_4,_5,_6,_7,_8,N,...) N
#define NARGS(...) NARGS_SEQ(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)

#define PRIMITIVE_CAT(x, y) x ## y
#define CAT(x, y) PRIMITIVE_CAT(x, y)

#define APPLY(macro, ...) CAT(APPLY_, NARGS(__VA_ARGS__))(macro, __VA_ARGS__)
#define APPLY_1(m, x1) m(x1)
#define APPLY_2(m, x1, x2) m(x1), m(x2)
#define APPLY_3(m, x1, x2, x3) m(x1), m(x2), m(x3)
#define APPLY_4(m, x1, x2, x3, x4) m(x1), m(x2), m(x3), m(x4)
#define APPLY_5(m, x1, x2, x3, x4, x5) m(x1), m(x2), m(x3), m(x4), m(x5)
#define APPLY_6(m, x1, x2, x3, x4, x5, x6) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6)
#define APPLY_7(m, x1, x2, x3, x4, x5, x6, x7) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7)
#define APPLY_8(m, x1, x2, x3, x4, x5, x6, x7, x8) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8)

#define DEFINE_TEST(...) \
{ \
	.std_args = { APPLY((const void *const ), __VA_ARGS__ ) }, \
	.ft_args = { APPLY((const void *const), __VA_ARGS__ ) }, \
    .valid = -1 \
}

#define ARGUMENT_1(first) first
#define ARGUMENT_2(first, second) first

#define GET_MACRO(_1,_2,NAME,...) NAME
#define FIRST_ARG(...) GET_MACRO(__VA_ARGS__, ARGUMENT_2, ARGUMENT_1)(__VA_ARGS__)

#define SET_MMAP(size, ...) \
{ \
	.std_args = { APPLY((const void *const ), __VA_ARGS__ ) }, \
	.ft_args = { APPLY((const void *const), __VA_ARGS__ ) }, \
    .valid = -1, \
    .std_output = collect_mmap(&collector, size), \
    .std_output_size = size, \
    .ft_output = collect_mmap(&collector, size), \
    .ft_output_size = size \
} \

#define DEFINE_TEST_STD(...) \
{ \
	.std_args = { APPLY((const void *const ), __VA_ARGS__ ) }, \

#define DEFINE_TEST_FT(...) \
	.ft_args = { APPLY((const void *const), __VA_ARGS__ ) }, \
	.valid = -1 \
}

#define VALID_MMAP(size) collect_mmap(&collector, size)
#define VALID_FD(size) collect_memfd(&collector, size)

#define DEFINE_READ_TEST(a, b) SET_MMAP(b, collect_memfd_write(&collector, a, b), \
         collect_mmap(&collector, b < 0 ? 1 : b), b)

#define NO_CB NULL

#define BIND_TEST(function, call_back, printf_format) \
{ \
	.name = "ft_" #function, \
	.test = test_##function, \
	.nb_test = sizeof(test_##function)/sizeof(*test_##function), \
    .check = &check_##function, \
    .callback = call_back, \
	.format = printf_format, \
}

#endif
