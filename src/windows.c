Window** create_windows(Directory* dir) {
	Window** windows = malloc(sizeof(Window*) * dir -> file_count);

	int width = MAX_X / dir -> file_count,
		height = MAX_Y / dir -> file_count;

	for (int i = 0; i < dir -> file_count; i++) {
		Window* window = malloc(sizeof(Window));
		window -> origin.x = rand() % (MAX_X - width);
		window -> origin.y = rand() % (MAX_Y - height);
		window -> width = width;
		window -> height = height;

		char* name = dir -> filenames[i];
		int name_len = strlen(name);
		window -> name = name;
		// if the name length is longer than the window width, trucate its length
		window -> name_len = name_len > width ? width : name_len;

		windows[i] = window;
	}
	return windows;
}

void free_windows(Window** windows, int file_count) {
	for (int i = 0; i < file_count; i++)
		free(windows[i]);
	free(windows);
}

void draw_window(Window* window) {
	attron(COLOR_PAIR(COLOR_WINDOW));
	Point origin = window -> origin;
	for (int y = origin.y; y < origin.y + window -> height; y++) {
		for (int x = origin.x; x < origin.x + window -> width; x++)
			mvprintw(y, x, " ");
	}
	attroff(COLOR_PAIR(COLOR_WINDOW));

	attron(COLOR_PAIR(COLOR_TEXT_AND_BORDER));
	mvprintw(origin.y, origin.x, "%.*s", window -> name_len, window -> name);
	attroff(COLOR_PAIR(COLOR_TEXT_AND_BORDER));
}

void trace_drag_path(MouseState mstate, Window* window,
	char* path, char* command_buffer, int command_mode) {

	int win_w_mid = window -> width / 2, win_h_mid = window -> height / 2;

	int x0 = mstate.path.src.x, y0 = mstate.path.src.y;
	int x1 = mstate.path.dest.x, y1 = mstate.path.dest.y;

	int dy = abs(y1 - y0), dx = abs(x1 - x0);
	int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
	int grad_error_1 = (dx > dy ? dx : -dy) / 2;
	int grad_error_2;

	while (1) {
		attron(COLOR_PAIR(COLOR_DRAG));
		mvprintw(y0 + win_h_mid, x0 + win_w_mid , " ");
		attroff(COLOR_PAIR(COLOR_DRAG));

		attron(COLOR_PAIR(COLOR_TEXT_AND_BORDER));
		draw_info(path, command_buffer, command_mode);
		draw_border();
		attroff(COLOR_PAIR(COLOR_TEXT_AND_BORDER));

		napms(3);
		refresh();

		if (x0 == x1 && y0 == y1)
			break;

		grad_error_2 = grad_error_1;

		if (grad_error_2 > -dx) 
			grad_error_1 -= dy, x0 += sx;

		if (grad_error_2 < dy)
			grad_error_1 += dx, y0 += sy;
	}
}

int mouse_in_window(Window* window, int click_y, int click_x) {
	Point origin = window -> origin;
	for (int y = origin.y; y < origin.y + window -> height; y++) {
		if (y == click_y) {
			for (int x = origin.x + 1; x < origin.x + window -> width; x++) {
				if (x == click_x) return 1;
			}
		}
	}
	return 0;
}

Window* get_clicked_window(Window** windows, int win_count, int y, int x) {
	for (int i = 0; i < win_count; i++) {
		Window* w = windows[i];
		if (mouse_in_window(w, y, x))
			return w;
	}
	return NULL;
}