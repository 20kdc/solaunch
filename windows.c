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

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BRAND "solaunch"

#define MY_MAX_PATH 0x10000

static void stderr_write(const char * text) {
	WriteFile(GetStdHandle(STD_ERROR_HANDLE), text, strlen(text), NULL, NULL);
}

int __getmainargs(int *, char ***, char ***, int);
int __wgetmainargs(int *, wchar_t ***, wchar_t ***, int, int *);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	/* We aren't allowed to know the real size of the buffer we need. Grr. */
	wchar_t * text;
	void * lib, * proc;
	DWORD filename_len;
	text = calloc(MY_MAX_PATH + 1, sizeof(wchar_t));
	if (!text) {
		stderr_write(BRAND ": Filename alloc failed.\n");
		exit(1);
	}
	filename_len = GetModuleFileNameW(NULL, text, MY_MAX_PATH);
	if (!filename_len) {
		stderr_write(BRAND ": GetModuleFileNameW failed.\n");
		exit(1);
	} else if (filename_len < 3) {
		stderr_write(BRAND ": GetModuleFileNameW too small to rewrite suffix.\n");
		exit(1);
	}
	/* rewrite suffix */
	text[filename_len - 3] = 'd';
	text[filename_len - 2] = 'l';
	text[filename_len - 1] = 'l';
	/* get the library? */
	lib = LoadLibraryW(text);
	free(text);
	if (!lib) {
		stderr_write(BRAND ": Unable to find DLL.\n");
		exit(1);
	}
	/* Try this first as it's the 'Windowsy' way. */
	proc = GetProcAddress(lib, "WinMain@16");
	if (proc)
		return ((int WINAPI (*)(HINSTANCE, HINSTANCE, LPSTR, int)) proc)(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
	/* Try this second as it's internationalization-friendly. */
	proc = GetProcAddress(lib, "wmain");
	if (proc) {
		int argc = 0;
		wchar_t ** argv = NULL;
		wchar_t ** env = NULL;
		int nm = 0;
		__wgetmainargs(&argc, &argv, &env, 0, &nm);
		return ((int (*)(int, wchar_t *[], wchar_t *[])) proc)(argc, argv, env);
	}
	proc = GetProcAddress(lib, "main");
	if (proc) {
		int argc = 0;
		char ** argv = NULL;
		char ** env = NULL;
		__getmainargs(&argc, &argv, &env, 0);
		return ((int (*)(int, char *[], char *[])) proc)(__argc, __argv, _environ);
	}
	stderr_write(BRAND ": Unable to find WinMain@16, wmain, or main.\n");
	exit(1);
}
