#ifdef DISPLAY
#include <menu.h>
#include <panel.h>
#include <string.h>
#include "utils.h"
#include "test.h"
#include "display.h"

static void show_result_panels(t_display *display)
{
    t_menu *menu = &display->menu;

    /*flush previous panels*/
    t_result *result = &display->result[display->menu.cur_index];
    for (size_t i = 0; i < result->nb_test; ++i)
        hide_panel(result->panel[i]);
    update_panels();

    ITEM *cur_choice = current_item(menu->menu);
    display->menu.cur_index = item_index(cur_choice);

    result = &display->result[display->menu.cur_index];
    int page = display->result[display->menu.cur_index].pages.cur;
    for (size_t i = 0; i < result->nb_test && page > -1; ++i) {
        int y = getbegy(result->window[i]);
        if (!y)
            --page;
        if (!page)
            show_panel(result->panel[i]);
    }
    update_panels();
}

static void pages_handler(t_display *display, int dir, const t_function ft_unit[NB_FUNCTION])
{
    t_menu *menu = &display->menu;
    ITEM *cur_choice = current_item(menu->menu);
    int index = item_index(cur_choice);
    t_result *result = &display->result[index];

    if (dir == REQ_PREV_PAGE && result->pages.cur > 1) {
        --result->pages.cur;
        set_menu_pages(display);
        define_menu_win(display, ft_unit);
    }

    if (dir == REQ_NEXT_PAGE && result->pages.cur < result->pages.number) {
        ++result->pages.cur;
        set_menu_pages(display);
        define_menu_win(display, ft_unit);
    }
}

int event_loop(const t_function ft_unit[NB_FUNCTION], t_display *display)
{
    t_menu *menu = &display->menu;

    define_menu_win(display, ft_unit);
    update_results(ft_unit, display);
    show_result_panels(display);
    doupdate();
    int c;
    while((c = getch()) != 27) {
        switch(c) {
            case KEY_UP:
            case 'k':
            {
                int ret_value = menu_driver(menu->menu, REQ_UP_ITEM);
                if (ret_value != E_OK)
                    menu_driver(menu->menu, REQ_LAST_ITEM);
                define_menu_win(display, ft_unit);
                break;
            }
            case KEY_DOWN:
            case '\t':
            case 'j':
            {
                int ret_value = menu_driver(menu->menu, REQ_DOWN_ITEM);
                if (ret_value != E_OK)
                    menu_driver(menu->menu, REQ_FIRST_ITEM);
                define_menu_win(display, ft_unit);
                break;
            }
            case KEY_LEFT:
            case 'h':
            {
                pages_handler(display, REQ_PREV_PAGE, ft_unit);
                break;
            }
            case KEY_RIGHT:
            case 'l':
            {
                pages_handler(display, REQ_NEXT_PAGE, ft_unit);
                break;
            }
            case KEY_RESIZE:
                erase();
                update_results(ft_unit, display);
                define_menu_win(display, ft_unit);
                break;
        }
        show_result_panels(display);
        doupdate();
    }
    return 0;
}

static int print_func_call(t_display *display, WINDOW *win, const t_function *function, t_test *const test)
{
    t_test_area *test_area = &display->test_area;

    int middle = test_area->middle;

    int begin_x = (sizeof("|") - 1) + MARGIN_OFFSET + 1;
    int std_begin_x = middle + 5;

    /*print function call*/
    mvwprintw(win, 1, begin_x, "%s(", function->name);
    mvwprintw(win, 1, std_begin_x, "%s(", function->name + 3);

    size_t format = function->format;
    size_t nb_args;
    if (format & (ARG_4))
        nb_args = 4;
    else if (format & (ARG_3))
        nb_args = 3;
    else if (format & (ARG_2))
        nb_args = 2;
    else if (format & (ARG_1))
        nb_args = 1;
    else
        nb_args = 0;

    begin_x += strlen(function->name) + 1;
    std_begin_x += strlen(function->name + 3) + 1;
    int old_pos = 0;
    for (size_t i = 0; i < nb_args; ++i) {
        int last = i == (nb_args - 1);

        size_t arg = (size_t)0b11111111111 << (POS_ARG_1 + i * (BITS_PER_ARG + 1));
        arg &= format;
        arg >>= (POS_ARG_1 + (i * (BITS_PER_ARG + 1)));

        const char *fmt = convert_format(arg, last);
        void *argument = *((void**)&test->std_args + i);
        if ((arg == STR || arg > VOID) && (ssize_t)argument < 1) {
            mvwprintw(win, 1, begin_x, (last ? "\"invalid adress\"" : "\"invalid adress\", "));
            mvwprintw(win, 1, std_begin_x, (last ? "\"invalid adress\"" : "\"invalid adress\", "));
            old_pos += 16 + (last ? 0 : 2);
        }
        else if (arg == BUFFER_TO_FILL) {
            mvwprintw(win, 1, begin_x + old_pos, (last ? "A buffer to fill" : "A buffer to fill, "));
            mvwprintw(win, 1, std_begin_x + old_pos, (last ? "A buffer to fill" : "A buffer to fill, "));
            old_pos += 16 + (last ? 0 : 2);
        }
        else if (arg == STR) {
            print_string_erase_nl(win, argument, begin_x + old_pos, last);
            old_pos += print_string_erase_nl(win, argument, std_begin_x + old_pos, last);
        }
        else {
            mvwprintw(win, 1, begin_x + old_pos, fmt, argument);
            mvwprintw(win, 1, std_begin_x + old_pos, fmt, argument);
            int offset = convert_format_to_size(arg, argument, last);
            old_pos += offset;
        }
    }
    mvwprintw(win, 1, begin_x + old_pos, ");");
    mvwprintw(win, 1, std_begin_x + old_pos, ");");
    return 0;
}

static int print_comparison_template(t_display *display, WINDOW *win, t_test *const test)
{
    t_test_area *test_area = &display->test_area;

    int middle = test_area->middle;
    int height = getmaxy(win);

    wmove(win, 0, MARGIN_OFFSET);
    wvline(win, ACS_TTEE, 1);

    wmove(win, 0, middle);
    wvline(win, ACS_TTEE, 1);

    int posy = 2;
	mvwprintw(win, posy, 1, "Return value");
    ++posy;

	mvwprintw(win, posy, 1, "Errno value");
    ++posy;

	mvwprintw(win, posy, 1, "Termination status");
    ++posy;

    if (test->std_output)
	    mvwprintw(win, posy, 1, "Output");

    wmove(win, 1, MARGIN_OFFSET);
    wvline(win, ACS_VLINE, height - 1);

    wmove(win, height - 1, MARGIN_OFFSET);
    wvline(win, ACS_BTEE, 1);
    wmove(win, height - 1, middle);
    wvline(win, ACS_BTEE, 1);

    /*Print unit-test status zone*/
    wmove(win, 2, middle - 2);
    wvline(win, ACS_VLINE, 1);
    wmove(win, 2, middle + 3);
    wvline(win, ACS_VLINE, 1);

    wmove(win, 1, middle + 3);
    wvline(win, ACS_URCORNER, 1);
    wmove(win, 3, middle + 3);
    wvline(win, ACS_LRCORNER, 1);

    wmove(win, 1, middle - 2);
    wvline(win, ACS_ULCORNER, 1);
    wmove(win, 3, middle - 2);
    wvline(win, ACS_LLCORNER, 1);
    mvwhline(win, 3, middle - 1, ACS_HLINE, 1);
    mvwhline(win, 1, middle - 1, ACS_HLINE, 1);
    mvwhline(win, 3, middle + 1, ACS_HLINE, 2);
    mvwhline(win, 1, middle + 1, ACS_HLINE, 2);

    wmove(win, 4, middle);
    wvline(win, ACS_VLINE, height - 5);
    wmove(win, 1, middle);
    wvline(win, ACS_BTEE, 1);
    wmove(win, 3, middle);
    wvline(win, ACS_TTEE, 1);
    /*end of unit status*/

    return 0;
}

static int print_comparison_result(t_display *display, WINDOW *win, const t_function *function, t_test *const test)
{
    #define TITLE_POS 3
    t_test_area *test_area = &display->test_area;

    int line_space = test_area->line_space;
    int middle = test_area->middle;

    int posy = 2;
    int ft_middle = (sizeof("|") - 1) + MARGIN_OFFSET + 1;
    size_t return_value = function->format & 0b111111111111;
    const char *fmt = convert_format(return_value, 1);
    assert(fmt);

    if (test->valid == 0) {
        wattron(win, COLOR_PAIR(2));
        mvwprintw(win, 2, middle, "OK");
        wattroff(win, COLOR_PAIR(2));
    }
    else if (test->valid == 1) {
        wattron(win, COLOR_PAIR(1));
        mvwprintw(win, 2, middle, "KO");
        wattroff(win, COLOR_PAIR(1));
    }

    if (test->valid == 1)
	    wattron(win, COLOR_PAIR(1));
    else if (!test->valid)
	    wattron(win, COLOR_PAIR(2));
    if (return_value == STR)
	    mvwprintw(win, posy, ft_middle, fmt, test->std_output_size, *((void **)test->ft_result.return_value));
    else
	    mvwprintw(win, posy, ft_middle, fmt, *((void **)test->ft_result.return_value));

	wattron(win, COLOR_PAIR(2));
    int std_middle = middle + 5;
    if (return_value == STR)
	    mvwprintw(win, posy, std_middle, fmt, test->std_output_size, *((void **)test->std_result.return_value));
    else
	    mvwprintw(win, posy, std_middle, fmt, *((void **)test->std_result.return_value));
	wattroff(win, COLOR_PAIR(2));
    ++posy;

    {
        int ft_errno = *((int *)test->ft_result.errno_value);
        int std_errno = *((int *)test->std_result.errno_value);

        if (test->valid == 1 && ft_errno != std_errno)
            wattron(win, COLOR_PAIR(1));
        else if (!test->valid)
            wattron(win, COLOR_PAIR(2));
        mvwprintw(win, posy, ft_middle, "%s", strerror(ft_errno));
        mvwprintw(win, posy, std_middle, "%s", strerror(std_errno));
        if (test->valid == 1 && ft_errno != std_errno)
            wattroff(win, COLOR_PAIR(1));
        else if (!test->valid)
            wattroff(win, COLOR_PAIR(2));
        ++posy;
    }

    {
        int ft_terminate_code = test->ft_result.terminate_code;
        int std_terminate_code = test->std_result.terminate_code;

        if (test->valid == 1 && ft_terminate_code != std_terminate_code)
            wattron(win, COLOR_PAIR(1));
        else if (!test->valid)
            wattron(win, COLOR_PAIR(2));
        mvwprintw(win, posy, ft_middle, "%s", strsignal(test->ft_result.terminate_code));
        mvwprintw(win, posy, std_middle, "%s", strsignal(test->std_result.terminate_code));
        if (test->valid == 1 && ft_terminate_code != std_terminate_code)
            wattroff(win, COLOR_PAIR(1));
        else if (!test->valid)
            wattroff(win, COLOR_PAIR(2));

        ++posy;
    }

    {
        int line_limit = line_space / 2;
        if (test->ft_output) {
            size_t output_size = strnlen(test->ft_output, test->ft_output_size);
            print_content_erase_nl(win, test->ft_output, posy, MARGIN_OFFSET + 1, output_size, line_limit);
        }

        if (test->std_output) {
            size_t output_size = strnlen(test->std_output, test->std_output_size);
            print_content_erase_nl(win, test->std_output, posy, (middle + 1), output_size, line_limit);
        }
    }

    return 0;
}

static int fill_result_win(t_display *display,
        const t_function ft_unit[NB_FUNCTION],
        size_t current_function, size_t cur_test)
{
    const t_function *const function = &ft_unit[current_function];
    t_test *test = &function->test[cur_test];

    t_result *result = &display->result[current_function];
    WINDOW *win = result->window[cur_test];

    print_comparison_template(display, win, test);
    print_func_call(display, win, function, test);
    if (test->valid != -1)
        print_comparison_result(display, win, function, test);
    update_panels();
    return 0;
}

static int terminal_resized(t_display *display)
{
    int width = getmaxx(stdscr);
    int height = getmaxy(stdscr);

    if (width != display->terminal_width || height != display->terminal_height) {
        display->terminal_width = width;
        display->terminal_height = height;
        return 1;
    }
    return 0;
}

static int refresh_test_result_area(const t_function ft_unit[NB_FUNCTION],
                                    t_display *display, size_t current_function,
                                    size_t cur_test)
{
    t_menu *menu = &display->menu;
    t_test_area *test_area = &display->test_area;

    t_result *result = &display->result[current_function];
    const t_function *function = &ft_unit[current_function];
    t_test *test = &function->test[cur_test];

    int win_height = TEST_WINDOW_DEFAULT_HEIGHT;
    WINDOW *win = result->window[cur_test];

    int win_width = getmaxx(win);
    if (test->std_output)
        win_height = get_nb_lines(display, &ft_unit[current_function].test[cur_test]);

    if (win_width != test_area->width || win_height != getmaxy(win))
    {
        PANEL *panel = result->panel[cur_test];
        werase(win);

        int err_value;
        err_value = wresize(win, win_height, test_area->width);
        assert(err_value == OK);
        box(win, 0, 0);

        err_value = replace_panel(panel, win);
        assert(err_value == OK);
        update_panels();
    }

    if (cur_test > 0) {
        int top_pos_prev_test = getbegy(result->window[cur_test - 1]);
        top_pos_prev_test += getmaxy(result->window[cur_test - 1]);

        int y;
        if ((top_pos_prev_test + win_height) >= display->terminal_height)
            y = 0;
        else
            y = top_pos_prev_test;

        PANEL *panel = result->panel[cur_test];
        move_panel(panel, y, menu->begin_x + menu->width);
        update_panels();
    }

    if (fill_result_win(display, ft_unit, current_function, cur_test)) {
        LOG("Error occured while filling unit-test results\n");
        return -1;
    }

    return 0;
}

static int define_test_area_cords(t_display *display)
{
    t_menu *menu = &display->menu;
    t_test_area *test_area = &display->test_area;

    test_area->width = display->terminal_width - menu->width;

    test_area->line_space = test_area->width - MARGIN_OFFSET - (sizeof("||") - 1);
    if (test_area->line_space < 33)
        return -1;

    test_area->middle = test_area->width - 1 - (test_area->line_space / 2) - 1;

    return 0;
}

int update_results(const t_function ft_unit[NB_FUNCTION], t_display *display)
{
    t_result *result = display->result;

    /*In case of terminal resize we recreate all windows associated with tests*/
    if (terminal_resized(display)) {
        if (define_test_area_cords(display))
            return -1;

        for (size_t i = 0; i < NB_FUNCTION; ++i)
        {
            result[i].pages.cur = 1;
            result[i].pages.number = 0;
            for (size_t cur_test = 0; cur_test < result[i].nb_test; ++cur_test)
            {
                if (refresh_test_result_area(ft_unit, display, i, cur_test))
                    return -1;
                WINDOW *win = result[i].window[cur_test];
                int y = getbegy(win);
                if (!y)
                    ++result[i].pages.number;
            }
        }
        set_menu_pages(display);
        return 0;
    }

    /*In case of a unit-test changing his status with an output greater of
     * container limit => rebuild his window*/
    for (size_t i = 0; i < NB_FUNCTION; ++i) {
        for (size_t cur_test = 0; cur_test < result[i].nb_test; ++cur_test) {
            int test_status = result[i].validity[cur_test];
            if (test_status != ft_unit[i].test[cur_test].valid)
                result[i].validity[cur_test] = ft_unit[i].test[cur_test].valid;
            if (refresh_test_result_area(ft_unit, display, i, cur_test))
                return -1;
        }
    }
    return 0;
}

static int create_menu(const t_function ft_unit[NB_FUNCTION], t_display *display)
{
    t_result *result = display->result;
    t_menu *menu = &display->menu;

    for (size_t i = 0; i < NB_FUNCTION; ++i) {
        memcpy(&menu->pages[i * 3], "N/A", 3);
        menu->choices[i] = new_item(ft_unit[i].name, &menu->pages[i * 3]);
        if (!menu->choices[i])
            return -1;

        result[i].nb_test = ft_unit[i].nb_test;
        if (!ft_unit[i].nb_test)
            continue ;

        result[i].window = malloc(sizeof(WINDOW *) * result[i].nb_test);
        if (!result[i].window)
            return -1;

        result[i].panel = malloc(sizeof(PANEL *) * result[i].nb_test);
        if (!result[i].panel)
            return -1;

        result[i].validity = malloc(sizeof(int *) * result[i].nb_test);
        if (!result[i].validity)
            return -1;
        memset(result[i].validity, -1, result[i].nb_test);
    }
    menu->choices[NB_FUNCTION] = new_item(NULL, NULL);
    if (!menu->choices)
        return -1;

    menu->menu = new_menu((ITEM **)menu->choices);
    if (!menu->menu)
        return -1;

    set_menu_fore(menu->menu, COLOR_PAIR(3));
    set_menu_spacing(menu->menu, 1, CHOICES_MARGIN, 0);
    set_menu_mark(menu->menu, MENU_CURSOR);

    if (define_menu_win(display, ft_unit)) {
        LOG("Creating menu subwindow failed !\n");
        return -1;
    }
    return 0;
}

void set_menu_pages(t_display *display)
{
    t_menu *menu = &display->menu;
    t_result *result = display->result;

    for (size_t i = 0; i < NB_FUNCTION; ++i) {
        menu->pages[(i * 3)] = result[i].pages.cur + '0';
        menu->pages[(i * 3) + 2] = result[i].pages.number + '0';
    }
}

int init_display(const t_function ft_unit[NB_FUNCTION], t_display *display)
{
    display->terminal_width = getmaxx(stdscr);
    display->terminal_height = getmaxy(stdscr);

    if (create_menu(ft_unit, display)) {
        LOG("Menu allocation failure !\n");
        return -1;
    }

    t_menu *menu = &display->menu;
    t_result *result = display->result;

    if (define_test_area_cords(display)) {
        LOG("Not enough space to display basic informations !\n");
        return -1;
    }
    t_test_area *test_area = &display->test_area;

    int win_width = test_area->width;
    int result_begin_x = menu->begin_x + menu->width;
    int prev_win_height;
    for (size_t i = 0; i < NB_FUNCTION; ++i) {
        prev_win_height = 0;
        result[i].pages.cur = 1;
        for (size_t nb_test = 0; nb_test < result[i].nb_test; ++nb_test) {
            if (prev_win_height >= display->terminal_height)
                prev_win_height = 0;

            if (!prev_win_height)
                ++result[i].pages.number;

            int win_height = TEST_WINDOW_DEFAULT_HEIGHT;
            if (ft_unit[i].test[nb_test].std_output || ft_unit[i].test[nb_test].ft_output)
                ++win_height;

            result[i].window[nb_test] = newwin(win_height, win_width,
                                                prev_win_height, result_begin_x);
            assert(result[i].window[nb_test]);
            box(result[i].window[nb_test], 0, 0);

            result[i].panel[nb_test] = new_panel(result[i].window[nb_test]);
            assert(result[i].panel[nb_test]);
            prev_win_height += win_height;

            if (fill_result_win(display, ft_unit, i, nb_test)) {
                LOG("Error occured while filling unit-test results\n");
                return -1;
            }
            hide_panel(result[i].panel[nb_test]);
        }
    }
    set_menu_pages(display);
    update_panels();
    return 0;
}
#endif
