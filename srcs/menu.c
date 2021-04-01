#ifdef DISPLAY
#include <menu.h>
#include <panel.h>
#include <string.h>
#include "utils.h"
#include "test.h"
#include "display.h"

static size_t  get_nb_test_with_status(const t_function *const unit, int status)
{
    size_t nb_test = 0;
    t_test *test;
    for (size_t i = 0; i < unit->nb_test; ++i) {
        test = &unit->test[i];
        if (test->valid == status)
            ++nb_test;
    }
    return nb_test;
}

static void set_menu_title(WINDOW *win, int starty, int startx, int width, char *string, chtype color)
{
    int length, x, y;
	float temp;

	if(win == NULL)
		win = stdscr;
	getyx(win, y, x);
	if(startx != 0)
		x = startx;
	if(starty != 0)
		y = starty;
	if(width == 0)
		width = 80;

	length = strlen(string);
	temp = (width - length)/ 2;
	x = startx + (int)temp;
	wattron(win, color);
	mvwprintw(win, y, x, "%s", string);
	wattroff(win, color);
}

int define_menu_win(t_display *display, const t_function ft_unit[NB_FUNCTION])
{
    t_menu *menu = &display->menu;

    if (!menu->window) {
        /*define menu*/
        int width = display->terminal_width;

        int width_box, height_box;
        scale_menu(menu->menu, &height_box, &width_box);
        height_box += BOX_BORDER + TITLE_SPACE + CHOICES_MARGIN - 1;
        width_box += BOX_BORDER;

        {
            int width_box_is_odd = width_box % 2;
            if (width % 2 && !width_box_is_odd)
                ++width_box;
            else if (width_box_is_odd)
                ++width_box;
        }

        int begin_x = 0;
        int begin_y = 0;

        menu->window = newwin(height_box, width_box, begin_y, begin_x);
        if (!menu->window) {
            LOG("Menu allocation failure !\n");
            return -1;
        }
        set_menu_win(menu->menu, menu->window);
        set_menu_title(menu->window, 1, 0, width_box, "Functions", COLOR_PAIR(1));

        mvwaddch(menu->window, 2, 0, ACS_LTEE);
        mvwhline(menu->window, 2, 1, ACS_HLINE, width_box - 2);
        mvwaddch(menu->window, 2, width_box - 1, ACS_RTEE);
        box(menu->window, 0, 0);

        menu->panel = new_panel(menu->window);

        /*Allocation sub-window handling choices*/
        menu->choices_win = derwin(menu->window, height_box - TITLE_SPACE - BOX_BORDER, width_box - BOX_BORDER, 3, 1);
        if (!menu->choices_win) {
            LOG("Menu items allocation failure !\n");
            return -1;
        }
        menu->sub_panel = new_panel(menu->choices_win);
        set_menu_sub(menu->menu, menu->choices_win);

        /*backup dimensions*/
        menu->width = width_box;
        menu->height = height_box;
        menu->begin_x = begin_x;
        menu->begin_y = begin_y;
        post_menu(menu->menu);

        show_panel(menu->sub_panel);
        show_panel(menu->panel);
    }
    else
    {
        int begin_y = 0;
        unpost_menu(menu->menu);
        if (begin_y != menu->begin_y)
        {
            menu->begin_y = begin_y;
            move_panel(menu->panel, begin_y, 0);
        }
    }

    post_menu(menu->menu);
    for (size_t i = 0; i < NB_FUNCTION; ++i) {
        if (!ft_unit[i].nb_test)
            mvwprintw(menu->choices_win, (i * CHOICES_MARGIN) + 1, 4, "not found");
        else {
            size_t nb_ok = get_nb_test_with_status(&ft_unit[i], OK);
            size_t nb_ko = get_nb_test_with_status(&ft_unit[i], KO);
            mvwprintw(menu->choices_win, (i * CHOICES_MARGIN) + 1, 4,
                    "[OK=%zu]%.*s[KO=%zu]", nb_ok, (output_size(nb_ok, 10) > 1 ? 1 : 2), "       ", nb_ko);
        }
    }

    doupdate();
    return 0;
}
#endif
