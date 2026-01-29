/*
 * exe2gui
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

#include <stdio.h>
int main(int argc, char ** argv) {
	if (argc != 2) {
		puts("exe2gui FILE");
		return 1;
	}
	FILE * exe = fopen(argv[1], "r+b");
	int peSigOfs = 0;
	short val = 2;
	if (!exe) {
		puts("FILE must exist and be writable");
		return 1;
	}
	fseek(exe, 0x3C, SEEK_SET);
	fread(&peSigOfs, 4, 1, exe);
	fseek(exe, peSigOfs + 4 + 20 + 68, SEEK_SET);
	fwrite(&val, 2, 1, exe);
	fclose(exe);
	return 0;
}
