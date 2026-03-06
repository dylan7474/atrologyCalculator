# AGENTS.md

## Scope
These instructions apply to the entire repository.

## Project summary
NASA Astrology Calculator has two implementations of the same user-facing concept:

- `main.c` (CLI)
- `index.html` (single-file web app)

Keep behavior aligned across both versions whenever practical.

## Working agreements for AI coders

1. Prefer small, incremental patches over broad rewrites.
2. Preserve naming consistency across C and JavaScript concepts:
   - sun sign
   - house
   - biorhythm
   - aspect summary
3. Do not introduce a new web build system unless explicitly requested.
4. Favor defensive parsing and clear failure messaging for NASA API/network errors.
5. Keep user-facing wording simple and actionable.

## Documentation requirements
When behavior changes in `index.html` or `main.c`:

- Update `README.md` with:
  - run/build instructions (copy/paste ready), and
  - any user-visible behavior changes.

If new files or scripts are added, document their purpose briefly in README.

## Validation checklist
For non-trivial changes:

- Run `make`
- Run `python3 -m py_compile` for any Python files that were added/edited.

For front-end-only changes:

- Ensure no obvious HTML syntax issues.
- Verify key flows still behave correctly:
  - valid date path,
  - invalid date path,
  - NASA/network error path.

## Suggested PR checklist

- [ ] Scope is narrow and intentional.
- [ ] README updated (if behavior changed).
- [ ] Validation commands run and reported.
- [ ] No unrelated formatting churn.
