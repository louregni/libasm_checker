#ifdef DISPLAY
#include <menu.h>
#include <panel.h>
#include <string.h>
#include "utils.h"
#include "test.h"
#include "display.h"

int output_size(size_t number, size_t base)
{
    int size = 1;
    while (number /= base)
        ++size;
    return size;
}

int output_ssize(ssize_t number)
{
    int size = (number < 0 ? 2 : 1);
    while (number /= 10)
        ++size;
    return size;
}

int convert_format_to_size(size_t type, void *parameter, int last)
{
    switch (type) {
        case INT:
            return output_ssize((int)(ssize_t)parameter) + (last ? 0 : 2);
        case SIZE_T:
            return output_size((size_t)parameter, 10) + (last ? 0 : 2);
        case SSIZE_T:
            return output_ssize((ssize_t)parameter) + (last ? 0 : 2);
        case STR:
            return (int)strlen((char *)parameter) + 2 + (last ? 0 : 2);
        case PTR:
        case LIST:
        case STAR_STAR_LIST:
        case STAR_VOID:
        case FUNC_PTR:
            return output_size((size_t)parameter, 16) + 2 + (last ? 0 : 2);
        default:
            LOG("Unexecepted : %zu\n", type);
            endwin();
            exit(EXIT_FAILURE);
    }
}

const char *convert_format(size_t type, int last)
{
    switch (type) {
        case INT:
            return (last ? "%d" : "%d, ");
        case SIZE_T:
            return (last ? "%zu" : "%zu, ");
        case SSIZE_T:
            return (last ? "%zd" : "%zd, ");
        case STR:
            return (last ? "\"%.*s\"" : "\"%.*s\", ");
        case VOID:
            return (last ? "void" : "void, ");
        case PTR:
        case LIST:
        case STAR_STAR_LIST:
        case STAR_VOID:
        case FUNC_PTR:
            return (last ? "%p" : "%p, ");
    }
    return NULL;
}

void print_content_erase_nl(WINDOW *win, char *content, int begin_y, int begin_x, size_t content_size, size_t line_limit)
{
    char *str;
    while (content_size && *content) {
        str = strchr(content, '\n');
        if (str) {
            mvwprintw(win, begin_y, begin_x, "%.*s\\n", (str - content), content);
            content_size -= (str - content);
            content = str + 1;
        }
        else if (content_size > line_limit) {
            mvwprintw(win, begin_y, begin_x, "%.*s", line_limit, content);
            content += line_limit;
            content_size -= line_limit;
        }
        else {
            mvwprintw(win, begin_y, begin_x, "%.*s", content_size, content);
            return ;
        }
        ++begin_y;
    }
}

int print_string_erase_nl(WINDOW *win, char *str, int begin_x, int last)
{
#define MAX_CHARS_PER_ARG 8
    size_t len = strlen(str);
    size_t remaining_chars = len;
    int bytes_writtens = begin_x;
    if (len > MAX_CHARS_PER_ARG)
        remaining_chars = MAX_CHARS_PER_ARG;

    mvwprintw(win, 1, begin_x, "\"");
    ++begin_x;
    char *newline;
    while (remaining_chars && *str) {
        newline = strchr(str, '\n');
        if (newline) {
            int tmp = (newline - str);
            mvwprintw(win, 1, begin_x, "%.*s\\n", tmp, str);
            remaining_chars -= tmp;
            str = newline + 1;
            begin_x += tmp;
        }
        else {
            mvwprintw(win, 1, begin_x, "%.*s", remaining_chars, str);
            str += remaining_chars;
            begin_x += remaining_chars;
            remaining_chars = 0;
        }
    }

    if (len > MAX_CHARS_PER_ARG) {
        mvwprintw(win, 1, begin_x, (last ? "..." : "..., "));
        begin_x += (last ? 3 : 5);
    }
    else {
        mvwprintw(win, 1, begin_x, (last ? "\"" : "\", "));
        begin_x += (last ? 1 : 3);
    }

    return begin_x - bytes_writtens;
#undef MAX_CHARS_PER_ARG
}

int get_nb_lines(t_display *display, t_test *const test)
{
    int result = 0;
    if (test->std_output_size) {
        int output_line_width = display->test_area.line_space;
        output_line_width /= 2;

        result = test->std_output_size / output_line_width;
        if (test->std_output_size % output_line_width)
            ++result;
    }

    return result + TEST_WINDOW_DEFAULT_HEIGHT;
}
#endif
