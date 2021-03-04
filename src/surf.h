typedef struct {
	char* path;
	char** filenames;
	int file_count;
} Directory;

typedef struct {int x, y;} Point;
typedef struct {Point src, dest;} Line;

typedef struct {
	int selected_window, placed_window;
	Line path;
} MouseState;

typedef struct {
	Point origin;
	int width, height;
	char* name;
	int name_len;
} Window;

// filesystem.c
Directory* make_dir();
void free_dir(Directory* dir);
int key_works_for_shell(char key);

// windows.c
Window** create_windows(Directory* dir);
void free_windows(Window** windows, int file_count);
void draw_window(Window* window);
void trace_drag_path(MouseState mstate, Window* window,
	char* path, char* command_buffer, int command_mode);
int mouse_in_window(Window* window, int click_y, int click_x);
Window* get_clicked_window(Window** windows, int win_count, int y, int x);

// surf.c
void draw_border();
void draw_info(char* path, char* command_buffer, int command_mode);

#ifdef DEBUG
#define LOG(...) fprintf(debug_file, __VA_ARGS__)
#else
#define LOG(...)
#endif

#define FPS 15

#define COLOR_WINDOW 1
#define COLOR_DRAG 2
#define COLOR_TEXT_AND_BORDER 3

#define MAX_CMD_LEN 50
#define MAX_RESPONSE_LEN 400

#define BORDER_HORI '_'
#define BORDER_VERT '|'
#define BORDER_CORNER 'o'

#define MOUSE(event_type) (event.bstate & event_type)

#define REINIT_DIR_AND_WINDOWS\
	free_windows(windows, dir -> file_count);\
	free_dir(dir);\
	dir = make_dir();\
	windows = create_windows(dir);

#define WAIT while (getch() == -1)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <curses.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>

FILE* debug_file;
int MAX_Y, MAX_X;

#include "windows.c"
#include "filesystem.c"