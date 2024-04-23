see [app.c](app.c), [utils.c](utils.c), [str-utils.c](str-utils.c) and [common-utils.h](common-utils.h)

compile with:
```bash
gcc $(pkg-config --cflags gtk4) -o app app.c $(pkg-config --libs gtk4)
```