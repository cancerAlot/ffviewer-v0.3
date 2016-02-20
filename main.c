/* 
 * File:   main.c
 * Author: protodev
 * License: see LICENSE
 *
 * Created on February 20, 2016
 */

#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <X11/X.h>
#include <gtk-3.0/gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "main.h"
#include "ff2png.h"

/* EVENTS */
void close_window(GtkWidget*, gpointer);
gboolean on_key_press(GtkWidget*, GdkEventKey*, gpointer);

void addIfFarbfeld(char*);
void updateImage(void);
int isDirectory(const char*);
int isRegularFile(const char *);
void loadFiles(int, char**);
int main(int, char**);


GtkWidget *image;
GtkWidget *window;
GtkWidget *titlebar;

char **fileList;
int fileIndex = 0;
int filelistLength = 0;
char* prevTempFile;

void close_window(GtkWidget *widget, gpointer win)
{
	(void) widget;
	if (prevTempFile != NULL && strlen(prevTempFile) != 0) {
		unlink(prevTempFile);
	}
	gtk_widget_destroy(win);
	exit(0);
}

gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	(void) widget;
	(void) user_data;
	switch (event->keyval) {
	case GDK_KEY_Left:
		fileIndex--;
		if (fileIndex < 0) fileIndex = filelistLength - 1;
		updateImage();
		break;
	case GDK_KEY_Right:
		fileIndex++;
		if (fileIndex >= filelistLength) fileIndex--;
		updateImage();
		break;
	case GDK_KEY_q:
		/* fall through */
	case GDK_KEY_Q:
		close_window(NULL, window);
		break;
	}
	return FALSE;
}

void addIfFarbfeld(char *file)
{
	static int index = 0;
	char *fileExtension = strrchr(file, '.');
	char *magic = calloc(8, 1);
	FILE *f;

	if (!fileList) {
		fileList = calloc(DEF_BUF_LEN, sizeof(char*));
	}
	if (strlen(file) == 0) {
		return;
	}

	if (!(f = fopen(file, "r"))) {
		printf("empty file\n");
		return;
	}

	/* read the farbfeld magic */
	if (fread(magic, 1, 8, f) != 8) {
		printf("invalid magic\n");
		free(magic);
		fclose(f);
		return;
	}

	if (fileExtension && strcmp(fileExtension + 1, "ff") == 0 &&
		strncmp(magic, "farbfeld", strlen("farbfeld") - 1) == 0) {
		free(magic);
		fclose(f);
		fileList[index] = calloc(PATH_MAX, sizeof(char));
		strncpy(fileList[index], file, PATH_MAX);
		index++;
		filelistLength = index;
		if (!(index % DEF_BUF_LEN)) {
			printf("expanding...\n");
			char** new;
			if (!(new = realloc(fileList, sizeof(char**)*(index / DEF_BUF_LEN)))) {
				printf("fatal error: could not realloc enough memory!!\n");
				free(fileList);
				exit(1);
			}
			fileList = new;
			free(new);
		}
	} else {
		printf("invalid file - skipping\n");
		free(magic);
		fclose(f);
		return;
	}
}

void updateImage()
{
	char *pngFile;
	GdkPixbuf *buf;
	char* title;
	char* currentFile;

	if (fileList == NULL || filelistLength == 0) {
		exit(0);
	}
	currentFile = fileList[fileIndex];
	title = malloc(sizeof(char)*(PATH_MAX + 30));
	snprintf(title, PATH_MAX + 30, "%s%s", "ffviewer - ", currentFile);
	
	gtk_window_set_title(GTK_WINDOW(window), title);

	/* convert image an return the path to the png-file */
	pngFile = ff2png(currentFile);


	printf("image path: %s\n", pngFile);

	/*
	 * from the man-page of "gdk_pixbuf_new_from_file_at_size"
	 * The image will be scaled to fit in the requested size, 
	 * preserving the image's aspect ratio. Note that the returned pixbuf 
	 * may be smaller than width x height , if the aspect ratio requires it. 
	 * 
	 * Scale down the image to fit into the current size of the image-widget
	 */
	buf = gdk_pixbuf_new_from_file_at_size(pngFile, gtk_widget_get_allocated_width(image), gtk_widget_get_allocated_height(image), NULL);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image), buf);

	if (prevTempFile != NULL && strlen(prevTempFile) != 0) {

		unlink(prevTempFile);
	}
	prevTempFile = pngFile;
}

int isDirectory(const char *path)
{
	struct stat statbuf;
	if (stat(path, &statbuf) != 0) return 0;

	return S_ISDIR(statbuf.st_mode);
}

int isRegularFile(const char *path)
{
	struct stat path_stat;
	stat(path, &path_stat);

	return S_ISREG(path_stat.st_mode);
}

void loadFiles(int argc, char** argv)
{
	int i = 1;
	int len = 0;
	DIR *d;
	struct dirent* in_file;
	char absoluteFile[PATH_MAX];

	for (; i < argc; i++) {
		if (isDirectory(argv[i])) {
			d = opendir(argv[i]);
			len = strlen(argv[i]) - 1;
			while ((in_file = readdir(d))) {
				// ensure that the is no difference between 'dir/' and 'dir'
				if (argv[i][len] == '/') {
					argv[i][len] = '\0';
				}
				snprintf(absoluteFile, PATH_MAX, "%s/%s", argv[i], in_file->d_name);
				if (isRegularFile(absoluteFile)) {
					addIfFarbfeld(absoluteFile);
				}
			}
		} else {

			addIfFarbfeld(argv[i]);
		}
	}
}

int main(int argc, char** argv)
{
	if(argc<=1){
		printf("Usage: ffviewer file1 file2...\n");
		exit(0);
	}
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
	gtk_window_set_wmclass(GTK_WINDOW(window), "ffviewer", "ffviewer");
	gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
	g_signal_connect(window, "delete_event", G_CALLBACK(close_window), NULL);
	g_signal_connect(window, "key_press_event", G_CALLBACK(on_key_press), NULL);

	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	image = gtk_image_new();
	g_signal_connect(image, "size-allocate", G_CALLBACK(updateImage), NULL);
	gtk_box_pack_start(GTK_BOX(vbox), image, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	loadFiles(argc, argv);

	gtk_widget_show_all(window);
	gtk_widget_grab_focus(window);
	updateImage();
	gtk_main();

	return(EXIT_SUCCESS);
}
