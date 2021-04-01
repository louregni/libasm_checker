#include <dlfcn.h> /*dlopen & dlclose*/
#include <stdlib.h> /*free*/
#include <sys/mman.h> /*mmap*/
#include <sys/types.h> /*waitpid*/
#include <sys/wait.h>
#include "test.h"
#include "display.h"
#include "unit-test.h"
#include "utils.h"
#include "list.h"

int	main(void)
{
#ifdef DISPLAY
    initscr();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    raw();
    keypad(stdscr, TRUE);
    noecho();
#endif

    t_memory_collector collector = { };

/*  size_t strlen(const char *s);   */
	t_test test_strlen[] = {
        DEFINE_TEST("1"),
        DEFINE_TEST("12"),
        DEFINE_TEST("123"),
        DEFINE_TEST("1234"),
        DEFINE_TEST("12345"),
        DEFINE_TEST("123456"),
        DEFINE_TEST("1234567"),
        DEFINE_TEST("12345678"),
		DEFINE_TEST("Hello World !"),
		DEFINE_TEST(""),
        DEFINE_TEST(NULL),
        DEFINE_TEST(-1)
	};

/*  char *strcpy(char *dest, const char *src);  */
	t_test test_strcpy[] = {
        SET_MMAP(sizeof("Hello World !"), collect_mmap(&collector, sizeof("Hello World !")), "Hello World !")
	};

/*  int strcmp(const char *s1, const char *s2);   */
	t_test test_strcmp[] = {
		DEFINE_TEST("1", "1"),
		DEFINE_TEST("12", "12"),
		DEFINE_TEST("123", "123"),
		DEFINE_TEST("1234", "1234"),
		DEFINE_TEST("12345", "12345"),
		DEFINE_TEST("123456", "123456"),
		DEFINE_TEST("1234567", "1234567"),
		DEFINE_TEST("12345678", "12345678"),
		DEFINE_TEST("", ""),
		DEFINE_TEST("a", "b"),
		DEFINE_TEST("21", "12"),
		DEFINE_TEST("12345677", "12345678")
	};

/*  ssize_t write(int fd, const void *buf, size_t count);   */
	t_test test_write[] = {
		DEFINE_TEST(VALID_FD(15), "Hello World !\n", 15),
		DEFINE_TEST(VALID_FD(5), "This is a test :)", 5),
		DEFINE_TEST(VALID_FD(5), "An other one", 5),
		DEFINE_TEST(VALID_FD(445), "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.", 445),
		DEFINE_TEST(-1, "", 1),
		DEFINE_TEST(VALID_FD(1), NULL, 1),
		DEFINE_TEST(VALID_FD(1), "", 0)
	};

/*  ssize_t read(int fd, void *buf, size_t count);  */
	t_test test_read[] = {
        DEFINE_READ_TEST("Hi, this is a test actually :)", 25),
        DEFINE_READ_TEST("", 1),
		DEFINE_TEST(-1, "", 1)
	};

/*  char *strdup(const char *s);    */
	t_test test_strdup[] = {
        SET_MMAP(23, "Can You Duplicate It ?")
	};

/*  int		atoi_base(char *str, char *base)    */
	t_test test_atoi_base[] = {
        DEFINE_TEST("Well Well", "Well Well")
	};

/*  void    list_push_front(t_list **begin_list, void *data)    */
    t_test test_list_push_front[] = {
        SET_MMAP(sizeof(void *) * 13, GET_LIST(12), 42)
    };

/*  int     list_size(t_list *begin_list)   */
    t_test test_list_size[] = {
        SET_MMAP(sizeof(void *) * 12, CREATE_LIST(12))
    };

/*  void    list_sort(t_list **begin_list, int (*cmp)())    */
    t_test test_list_sort[] = {
        SET_MMAP(sizeof(void *) * 12, GET_LIST(12), compare)
    };

/*  void list_remove_if(t_list **begin_list, void *data_ref, int (*cmp)(), void (*free_fct)(void *))    */
    t_test test_list_remove_if[] = {
        SET_MMAP(sizeof(void *) * 12, GET_LIST(12), compare, change_data)
    };

    t_core core = {
        .ft_unit = {
            BIND_TEST(strlen, NO_CB, SIZE_T | STR << ARG_1),
            BIND_TEST(strcpy, dump_allocated_string, STR | BUFFER_TO_FILL << ARG_1 | STR << ARG_2),
            BIND_TEST(strcmp, NO_CB, INT | STR << ARG_1 | STR << ARG_2),
            BIND_TEST(write, NO_CB, SSIZE_T | INT << ARG_1 | STR << ARG_2 | SIZE_T << ARG_3),
            BIND_TEST(read, NO_CB, SSIZE_T | INT << ARG_1 | BUFFER_TO_FILL << ARG_2 | SIZE_T << ARG_3),
            BIND_TEST(strdup, dump_allocated_string, PTR | STR << ARG_1),
            BIND_TEST(atoi_base, NO_CB, INT | STR << ARG_1 | STR << ARG_2),
            BIND_TEST(list_push_front, dump_list_with_head, VOID | STAR_STAR_LIST << ARG_1 | STAR_VOID << ARG_2),
            BIND_TEST(list_size, dump_list, INT | LIST << ARG_1),
            BIND_TEST(list_sort, dump_list_with_head, VOID | STAR_STAR_LIST << ARG_1 | FUNC_PTR << ARG_2),
            BIND_TEST(list_remove_if, dump_list_with_head, VOID | STAR_STAR_LIST << ARG_1 | FUNC_PTR << ARG_2)
        },
        .mmap_buffer = mmap(NULL, MMAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0),
        .handle = dlopen(NULL, RTLD_NOW),
    };

    if (core.mmap_buffer == MAP_FAILED || !core.handle)
    {
        LOG("init failed !\n");
        goto free_and_leave;
    }
    core.sandbox_return = core.mmap_buffer;

#ifdef DISPLAY
    t_display display = { 0 };
    if (init_display(core.ft_unit, &display))
        goto free_and_leave;
#endif

/*Start unit-tests*/
    execute_units(&core);

    collect_results(&core);

#ifdef DISPLAY
    event_loop(core.ft_unit, &display);
#endif

free_and_leave:;
#ifdef DISPLAY
    t_menu *menu = &display.menu;
    t_result *result = display.result;

    unpost_menu(menu->menu);
    for (size_t i = 0; i < NB_FUNCTION; ++i)
    {
        free(result[i].window);
        free(result[i].panel);
        free(result[i].validity);
        free_item(menu->choices[i]);
    }
    free_item(menu->choices[NB_FUNCTION]);
    free_menu(menu->menu);
    endwin();
#endif

    t_mem *mem = collector.memfd.next;
    while (collector.memfd.next)
    {
        mem = mem->next;
        free(collector.memfd.next);
        collector.memfd.next = mem;
    }

    t_memmap *memmap = collector.mmap.next;
    while (collector.mmap.next)
    {
        memmap = memmap->next;
        if (munmap(collector.mmap.next->pointer, collector.mmap.next->length))
            LOG("munmap failed\n");
        free(collector.mmap.next);
        collector.mmap.next = memmap;
    }
    munmap(core.mmap_buffer, MMAP_SIZE);

	dlclose(core.handle);
	return 0;
}
