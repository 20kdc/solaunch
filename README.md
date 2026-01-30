# 'solaunch' : stub 'run this .so/.dll/.dylib file' program built for various targets

Basically, it's annoying to try and build large projects as static monoliths for... well, all the reasons.

Folding dependencies into your build system can be extremely difficult, and using static libraries has a habit of backfiring, especially when needing to use slightly hacky build systems to overcome platform challenges (read: Mac existing).

This is to say nothing of the build-time effects on larger projects.

But building as a mixture of dynamic libraries and regular executables tends to also be messy, because it becomes a build matrix between 'compiler' and 'build type'.

Therefore, this project provides a set of Actions-built (and thus hopefully verifiable) executables that can simply be treated as a pre-built third-party dependency.

The actual application built using them can therefore be reduced to a process of building dynamic libraries based on source code and other dynamic libraries.

For C/C++ code, this may as well amount to some build context (`-I`/`-L` flags), and a set of library-specific `-l` and source-file flags.

Since dynamic libraries can be basically treated as 'finished products', this massively isolates their effects on the build process.

From the perspective of custom configure/build script design, this changes the executable from having a massive effect on link flags to simply requiring the addition of an extra file.

## The Actual Function Of The Executable

* The program gets its own filename.
* On Unixes other than Mac, it then canonicalizes it fully and appends `.so`; on Mac it instead appends `.dylib`.
* On Windows, the filename must be more than 3 characters; it replaces the last three with `dll` (the assumption is from `exe`, `com`, etc.).
* The library is then loaded and the function `main` is called appropriately.
	* On Windows, `WinMain@16` is tried first, followed by `wmain`, and then `main`.

## License

Everything's public-domain/Unlicense (see `COPYING`).

The hopefully minimal Windows CRT code comes from MinGW; everything else should be reasonably 'pure' unless code's being embedded that I'm not aware of.

## Status

* Not tested at all on Mac
* Wine seems happy for 32-bit and 64-bit, XP VM seems happy for 32-bit (64-bit untested), ARM64 is handled as 'least-effort'
* `readelf --dyn-syms run.lgx64 -W`
* musl build support would be nice, but might require pulling in even more stuff.
	* _With how the mingw stuff seems to be working out, I am reasonably certain this can actually be done._
