// #define DEBUG
#include "surf.h"

void draw_border() {
	border(BORDER_VERT, BORDER_VERT, BORDER_HORI, BORDER_HORI,
		BORDER_CORNER, BORDER_CORNER, BORDER_CORNER, BORDER_CORNER);
}

void draw_info(char* path, char* command_buffer, int command_mode) {
	mvprintw(MAX_Y - 2, 1, path);
	mvprintw(MAX_Y - 3, 1, "Surf mode: %s", command_mode ? "command" : "nagivation");
	if (command_mode) mvprintw(MAX_Y - 4, 1, command_buffer);
}

int main() {
	initscr();
	cbreak();
	noecho();
	nodelay(stdscr, 1);
	start_color();
	use_default_colors();	
	init_pair(COLOR_WINDOW, COLOR_BLACK, COLOR_BLUE);
	init_pair(COLOR_DRAG, COLOR_BLUE, COLOR_WHITE);
	init_pair(COLOR_TEXT_AND_BORDER, COLOR_CYAN, COLOR_WHITE);
	keypad(stdscr, 1);
	mousemask(ALL_MOUSE_EVENTS, NULL);
	mouseinterval(0);
	srand(time(NULL));

	MouseState mstate = {0, 0};
	debug_file = fopen("other/debug.txt", "w");
	getmaxyx(stdscr, MAX_Y, MAX_X);
	Directory* dir = make_dir();
	Window** windows = create_windows(dir);
	Window* window = NULL;

	int command_mode = 0;

	char* command_buffer = malloc(MAX_CMD_LEN);
	memset(command_buffer, '\0', MAX_CMD_LEN);
	int command_buffer_ind = 0;

	while (1) {
		clear();
		napms(1000 / FPS);
		MEVENT event;
		mstate.placed_window = 0;
		int input = getch();
		// this allows 'q' to be part of commands made
		if (input == 'q' && !command_mode) goto finder_end;
		switch (input) {
			case '\n': case KEY_ENTER:
				command_mode = !command_mode;
				break;

			case '\b': case KEY_BACKSPACE: case 127:
				if (command_buffer_ind > 0)
					command_buffer[--command_buffer_ind] = '\0';
				break;

			case KEY_MOUSE:
				if (getmouse(&event) == OK) {
					if (MOUSE(BUTTON1_CLICKED)) {
						Window* clicked = get_clicked_window(windows,
							dir -> file_count, event.y, event.x);

						if (clicked != NULL) {
							char* path_addenum = clicked -> name;

							int error_state = chdir(path_addenum);
							if (error_state == -1) {
								attron(COLOR_PAIR(COLOR_TEXT_AND_BORDER));
								mvprintw(MAX_Y - 1, 0, "Could not move into %s.\n", path_addenum);
								attroff(COLOR_PAIR(COLOR_TEXT_AND_BORDER));
								WAIT;
							}
							else {
								REINIT_DIR_AND_WINDOWS;
							}
						}
					}

					else if (MOUSE(BUTTON1_PRESSED)) {
						Window* clicked = get_clicked_window(windows,
							dir -> file_count, event.y, event.x);

						if (clicked == NULL)
							mstate.selected_window = 0;
						else {
							mstate.selected_window = 1;
							mstate.path.src.y = event.y;
							mstate.path.src.x = event.x;
							window = clicked;
						}

					}
					else if (MOUSE(BUTTON1_RELEASED) && mstate.selected_window) {
						mstate.selected_window = 0;
						mstate.placed_window = 1;
						mstate.path.dest.y = event.y;
						mstate.path.dest.x = event.x;
					}
				}
				break;

			default:
				if (command_mode && key_works_for_shell(input)) {
					if (command_buffer_ind + 1 < MAX_CMD_LEN)
						command_buffer[command_buffer_ind++] = (char) input;
				}
				break;
		}

		for (int i = 0; i < dir -> file_count; i++)
			draw_window(windows[i]);

		if (!mstate.selected_window && mstate.placed_window) {
			int y1 = mstate.path.dest.y, x1 = mstate.path.dest.x;
			if (y1 > MAX_Y - window -> height) y1 = MAX_Y - window -> height;
			if (x1 > MAX_X - window -> width) x1 = MAX_X - window -> width - 1;
			window -> origin.y = y1;
			window -> origin.x = x1;
			trace_drag_path(mstate, window, dir -> path, command_buffer, command_mode);
			mstate.placed_window = 0;
		}

		attron(COLOR_PAIR(COLOR_TEXT_AND_BORDER));
		if (!command_mode && command_buffer_ind != 0) {
			FILE* cmd_file = popen(command_buffer, "r");
			char response[MAX_RESPONSE_LEN + 1];
			int newlines = 0;
			for (int i = 0; i < MAX_RESPONSE_LEN; i++) {
				char next = getc(cmd_file);

				if (i == MAX_RESPONSE_LEN - 1 || next == EOF) {
					response[i] = '\0';
					if (strcmp(response, "") != 0) {
						clear();
						mvprintw(MAX_Y - newlines, 0, response);
						WAIT;
					}
					pclose(cmd_file);
					memset(command_buffer, '\0', MAX_CMD_LEN);
					command_buffer_ind = 0;
					REINIT_DIR_AND_WINDOWS;
					break;
				}

				if (next == '\n') newlines++;
				response[i] = next;
			}
		}
		draw_info(dir -> path, command_buffer, command_mode);
		draw_border();
		attroff(COLOR_PAIR(COLOR_TEXT_AND_BORDER));

		refresh();
	}
	finder_end:
		endwin();
		fclose(debug_file);
		free(command_buffer);
		free_windows(windows, dir -> file_count);
		free_dir(dir);
}