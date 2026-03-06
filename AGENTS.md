# AGENTS.md

## Scope
These instructions apply to the entire repository.

## Project intent
- Keep `index.html` behavior aligned with `main.c` where practical.
- Prefer small, incremental changes over broad rewrites.

## Implementation guidance
- Do not add a build system for the web app; keep it as a single-file page unless explicitly asked.
- For logic updates, preserve naming consistency between C and JavaScript concepts (sun sign, house, biorhythm, aspect summary).
- Favor defensive error handling around NASA API parsing and network failures.

## Documentation expectations
When behavior changes in `index.html` or `main.c`:
- Update `README.md` with run instructions and user-visible behavior changes.
- Keep examples and command snippets copy/paste ready.

## Validation
For non-trivial changes, run:
- `make`
- `python3 -m py_compile` on any Python scripts added (if any)

For front-end-only changes, at least verify:
- the HTML file has no obvious syntax issues,
- and key user flows (valid date, invalid date, network error path) are still handled.
