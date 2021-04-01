#ifdef DISPLAY
#ifndef DISPLAY_H
# define DISPLAY_H
# include <ncurses.h>
# include <menu.h>
# include <panel.h>

# define NB_TEST_PER_WINDOW 8
# define MARGIN_OFFSET 19
# define TEST_WINDOW_DEFAULT_HEIGHT 6

/*Menu informations*/
# define MENU_CURSOR ">"
# define BOX_BORDER 2
# define TITLE_SPACE 2
# define CHOICES_MARGIN 3
# define PAGE_INDICATOR 5
# define LEFT_BORDER_SUB 3
# define REQ_PREV_PAGE 1
# define REQ_NEXT_PAGE 0

typedef struct  s_menu {
    int width;
    int height;
    int begin_x;
    int begin_y;

    PANEL *panel;
    PANEL *sub_panel;
    WINDOW *window;
    WINDOW *choices_win;
    MENU *menu;
    ITEM *choices[NB_FUNCTION + 1];
    char pages[NB_FUNCTION * 3];
    int cur_index;
    int max_length_choices;
}               t_menu;

typedef struct  s_test_area {
    int width;
    int middle;
    int line_space;
}               t_test_area;

typedef struct  s_result {
    size_t nb_test;
    WINDOW **window;
    PANEL **panel;
    int *validity;
    struct {
        size_t cur;
        size_t number;
    } pages;
}               t_result;

typedef struct  s_display {
    int terminal_width;
    int terminal_height;

    t_menu menu;
    t_test_area test_area;
    t_result result[NB_FUNCTION];
}               t_display;

void set_menu_pages(t_display *display);
int define_menu_win(t_display *display, const t_function ft_unit[NB_FUNCTION]);
void get_choices_max_length(t_menu *menu);
const char *convert_format(size_t type, int last);
int convert_format_to_size(size_t type, void *parameter, int last);
int update_results(const t_function ft_unit[NB_FUNCTION], t_display *display);
int init_display(const t_function ft_unit[NB_FUNCTION], t_display *display);
int get_nb_lines(t_display *display, t_test *const test);
int event_loop(const t_function ft_unit[NB_FUNCTION], t_display *display);
int print_string_erase_nl(WINDOW *win, char *str, int begin_x, int last);
void print_content_erase_nl(WINDOW *win, char *content, int begin_y, int begin_x, size_t content_size, size_t line_limit);
int output_size(size_t number, size_t base);

#endif
#endif
