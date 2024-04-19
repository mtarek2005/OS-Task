open [app.c](app.c)

compile with:
```bash
gcc $(pkg-config --cflags gtk4) -o app app.c $(pkg-config --libs gtk4)
```