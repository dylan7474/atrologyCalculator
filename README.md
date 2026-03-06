# NASA Astrology Calculator

A small project with two implementations of the same idea:

- **CLI app (`main.c`)**: a terminal-based forecast generator.
- **Web app (`index.html`)**: a browser-based UI that mirrors the C app logic.

Both versions:
- Query NASA JPL Horizons data.
- Estimate the "true" Sun sign from solar coordinates at birth.
- Compute simple biorhythm cycles (physical/emotional/intellectual).
- Build a daily summary from planetary transit placement.

## Repository layout

- `main.c` - C implementation.
- `index.html` - web implementation (single-file HTML/CSS/JS).
- `configure.sh` - local setup helper.
- `Makefile` - build targets for the C app.

## Build and run the C app

```bash
make
./nasa_astro
```

Or directly:

```bash
gcc main.c -o nasa_astro -lcurl -ljansson -lm
./nasa_astro
```

## Run the web app

Because the web version calls external APIs, run it from a local web server instead of opening the file directly:

```bash
python3 -m http.server 8000
```

Then open:

- `http://localhost:8000/index.html`

## Notes on the web version

- The app first tries direct access to NASA Horizons and falls back to a CORS proxy when needed.
- Input validation checks for real calendar dates before requests are made.
- If NASA data cannot be parsed, the app now returns a clearer, actionable error.

## Dependencies

### C app
- `gcc`
- `libcurl`
- `jansson`
- `libm`

### Web app
- Modern browser with JavaScript enabled.
- Network access to NASA Horizons API (or fallback proxy availability).
