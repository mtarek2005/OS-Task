see [app.c](app.c) and [utils.c](utils.c)

compile with:
```bash
gcc $(pkg-config --cflags gtk4) -o app app.c $(pkg-config --libs gtk4)
```