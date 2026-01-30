/*
 * solaunch
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <https://unlicense.org/>
 */

#include <limits.h>
#include <stddef.h>

#ifndef TRICKERY
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#else
typedef struct _IO_FILE FILE;
extern FILE * stderr;
int fprintf(FILE *, const char *, ...);
void * memset(void *, int, size_t);
size_t strlen(const char *);
void exit(int);
void * realloc(void *, size_t);
void free(void *);
void * dlopen(const char *, int);
void * dlsym(void *, const char *);
char * strcat(char *, const char *);
char * strdup(const char *);
#define RTLD_NOW 2
#endif

#define BRAND "solaunch"

#ifdef MAC
#include <mach-o/dyld.h>
static const char suffix[] = ".dylib";
static char * executable_path(const char * whatami) {
	uint32_t bufsize = 0;
	_NSGetExecutablePath(NULL, &bufsize);
	char * res = malloc(bufsize);
	if (!res) {
		fprintf(stderr, "%s: " BRAND ": unable to allocate memory to find self\n", whatami);
		exit(1);
	}
	if (_NSGetExecutablePath(res, &bufsize)) {
		fprintf(stderr, "%s: " BRAND ": unable to find self (late-early)\n", whatami);
		exit(1);
	}
	return res;
}
#else
static const char suffix[] = ".so";
static char * executable_path(const char * whatami) {
	return strdup("/proc/self/exe");
}
#endif
static const char symbol[] = "main";

static void * find_and_load_library(const char * whatami) {
	char * initpath, * tmppath, * tmppath2;
	void * lib;
	initpath = executable_path(whatami);
	if (!initpath) {
		fprintf(stderr, "%s: " BRAND ": unable to find self (early)\n", whatami);
		exit(1);
	}
#ifndef TRICKERY
	tmppath = realpath(initpath, NULL);
#else
	/*
	 * This is to get glibc to actually do the right thing.
	 * It returns NULL in the realpath call if you don't have the right symbol version.
	 * What we really want is POSIX 2008 behaviour, but portable to very old Linux distros.
	 */
	tmppath = ((char * (*)(const char *, char *)) dlsym(NULL, "realpath"))(initpath, NULL);
#endif
	if (!tmppath) {
		fprintf(stderr, "%s: " BRAND ": unable to find self from %s\n", whatami, initpath);
		exit(1);
	}
	free(initpath);
	tmppath2 = realloc(tmppath, strlen(tmppath) + strlen(suffix) + 1);
	if (!tmppath2) {
		free(tmppath);
		fprintf(stderr, "%s: " BRAND ": unable to ensure room for extra characters\n", whatami);
		exit(1);
	}
	strcat(tmppath2, suffix);
	lib = dlopen(tmppath2, RTLD_NOW);
	if (!lib) {
		fprintf(stderr, "%s: " BRAND ": unable to open library: %s\n", whatami, tmppath2);
		exit(1);
	}
	lib = dlsym(lib, symbol);
	if (!lib) {
		fprintf(stderr, "%s: " BRAND ": no '%s' symbol in library: %s\n", whatami, symbol, tmppath2);
		exit(1);
	}
	free(tmppath2);
	return lib;
}

int main(int argc, char ** argv) {
	return ((int (*)(int, char **)) find_and_load_library(argc ? argv[0] : "?"))(argc, argv);
}
