# NASA Astrology Calculator

NASA Astrology Calculator is a dual-interface hobby project that generates a light, astronomy-inspired daily astrology report from your birth date.

It includes:

- **CLI app (`main.c`)** for terminal use.
- **Web app (`index.html`)** for browser use.

Both implementations follow the same core ideas:

- Query NASA JPL Horizons data.
- Estimate a "true" Sun sign from solar coordinates at birth.
- Compute simple biorhythm cycles (physical, emotional, intellectual).
- Build an aspect/transit summary for the current day.

## Repository layout

- `main.c` - C implementation.
- `index.html` - single-file HTML/CSS/JS implementation.
- `configure.sh` - local setup helper.
- `Makefile` - build targets for the C app.
- `nasa_astro` - compiled CLI binary (after build).

## Build instructions (CLI)

### Option 1: via Makefile

```bash
make
./nasa_astro
```

### Option 2: direct compile

```bash
gcc main.c -o nasa_astro -lcurl -ljansson -lm
./nasa_astro
```

## Run instructions (Web)

Serve the repository through a local HTTP server (recommended for API/CORS behavior):

```bash
python3 -m http.server 8000
```

Then open:

- `http://localhost:8000/index.html`

## Basic controls

### CLI (`main.c`)

- Run `./nasa_astro`.
- Enter your **date of birth** when prompted.
- Review:
  - derived Sun sign,
  - current biorhythm percentages,
  - daily transit/aspect summary.

### Web (`index.html`)

- Enter your **date of birth** in the date input.
- Click the calculate/generate action button.
- The page displays:
  - validated birth-date feedback,
  - Sun sign estimation,
  - biorhythm values,
  - daily forecast summary.

## Notes

- The web app attempts direct access to NASA Horizons and can fall back to a CORS proxy path.
- Input validation checks for real calendar dates before API requests.
- If NASA data parsing fails, the app returns a user-facing error message.

## Dependencies

### C app

- `gcc`
- `libcurl`
- `jansson`
- `libm`

### Web app

- A modern browser with JavaScript enabled.
- Network access to NASA Horizons API (or fallback proxy availability).

## Short roadmap

- Improve retry/timeout handling and surface clearer network diagnostics.
- Expand forecast details while keeping `main.c` and `index.html` behavior aligned.
- Add lightweight automated checks for date-validation and API-error paths.
