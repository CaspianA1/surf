Directory* make_dir() {
	char* cwd = malloc(PATH_MAX * sizeof(char));	
	getcwd(cwd, PATH_MAX);

	char** filenames = malloc(sizeof(char*));
	int file_count = 0;

	DIR *dir_obj = opendir(".");
	struct dirent* entry;
	while ((entry = readdir(dir_obj)) != NULL) {
		char* name = entry -> d_name;
		if (strcmp(name, ".") != 0) {
			filenames[file_count++] = name;
			filenames = realloc(filenames, sizeof(char*) * file_count + 1);
		}
	}
	closedir(dir_obj);

	Directory* dir = malloc(sizeof(Directory));
	dir -> path = cwd;
	dir -> filenames = filenames;
	dir -> file_count = file_count;
	return dir;
}

void free_dir(Directory* dir) {
	free(dir -> path);
	free(dir -> filenames);
	free(dir);
}

int key_works_for_shell(char key) {
	return key > 31 && key < 127;
}