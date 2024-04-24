see [app.c](app.c), [utils-sh.c](utils-sh.c), [str-utils.c](str-utils.c) and [common-utils.h](common-utils.h)

compile with:
```bash
gcc $(pkg-config --cflags gtk4) -o app app.c $(pkg-config --libs gtk4)
```
for shell commands backend.  
for native backend, compile with:
```bash
gcc $(pkg-config --cflags gtk4) -o app app.c $(pkg-config --libs gtk4) -D__NATIVE
```