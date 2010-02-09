#include <menu.h>
#include <cstdlib>
#include <cstring>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4

char *choices[] = {
                        "Choice 1",
                        "Choice 2",
                        "Choice 3",
                        "Choice 4",
                        "Exit",
                        (char *)NULL,
                  };
void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);

int main()
{	ITEM **my_items;
	int c;				
	MENU *my_menu;
        WINDOW *my_menu_win;
        int n_choices, i;
	int size_of_menu = sizeof(ITEM *);
	int x_loc = 0;
	int y_loc = 0;

	
	/* Initialize curses */
	initscr();
	start_color();
        cbreak();
        noecho();
	keypad(stdscr, TRUE);
	assume_default_colors(COLOR_BLACK, COLOR_BLUE);
	init_pair(1, COLOR_YELLOW, COLOR_BLUE);
	init_pair(2, COLOR_BLUE, COLOR_WHITE);
	init_pair(3, COLOR_RED, COLOR_BLUE);

	/* Create items */
        n_choices = ARRAY_SIZE(choices);
        my_items = (ITEM **)calloc(n_choices, sizeof(ITEM *));
        for(i = 0; i < n_choices; ++i)
                my_items[i] = new_item(choices[i], choices[i]);

	/* Crate menu */
	my_menu = new_menu((ITEM **)my_items);

	/* Create the window to be associated with the menu */
	x_loc = getmaxx(stdscr) / 2 - 20;
	y_loc = getmaxy(stdscr) / 2 - 10;
        my_menu_win = newwin(size_of_menu + 4, getmaxx(stdscr) / 3 + 5, y_loc, x_loc);
        keypad(my_menu_win, TRUE);
     
	/* Set main window and sub window */
        set_menu_win(my_menu, my_menu_win);
	x_loc = getmaxx(stdscr) / 2 - 20;
	y_loc = getmaxy(stdscr) / 2 - 7;
        set_menu_sub(my_menu, derwin(my_menu_win, 3, getmaxx(stdscr) / 3 + 5, y_loc, x_loc));
	set_menu_fore(my_menu, COLOR_PAIR(1) | A_REVERSE);
	set_menu_back(my_menu, COLOR_PAIR(2) | A_REVERSE);
	set_menu_grey(my_menu, COLOR_PAIR(3) | A_REVERSE);
	

	/* Set menu mark to the string " * " */
        set_menu_mark(my_menu, " * ");

	/* Print a border around the main window and print a title */
        box(my_menu_win, 0, 0);
	print_in_middle(my_menu_win, 1, 0, 40, "Main Menu", COLOR_PAIR(1));
	mvwaddch(my_menu_win, 2, 0, ACS_LTEE);
	mvwhline(my_menu_win, 2, 1, ACS_HLINE, 38);
	mvwaddch(my_menu_win, 2, 39, ACS_RTEE);
	mvprintw(LINES - 2, 0, "F1 to exit");
	refresh();
        
	/* Post the menu */
	post_menu(my_menu);
	wrefresh(my_menu_win);

	while((c = wgetch(my_menu_win)) != KEY_F(1))
	{       switch(c)
	        {	case KEY_DOWN:
				menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				break;

			case 10: //enter key

				hello();
		}
                wrefresh(my_menu_win);
	}	

	/* Unpost and free all the memory taken up */
        unpost_menu(my_menu);
        free_menu(my_menu);
        for(i = 0; i < n_choices; ++i)
                free_item(my_items[i]);
	endwin();
}

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color)
{	int length, x, y;
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
	refresh();
}
