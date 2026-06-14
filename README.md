# Password Strength Checker (C)

A small command-line program written in C that analyzes a password and
tells you how strong it is. It checks the password's **length**,
**character complexity** (lowercase, uppercase, digits, symbols), and
common **weaknesses** such as being a well-known leaked password,
containing repeated characters (`"aaa"`), or containing sequential runs
(`"abc"`, `"321"`).

This project was built as a learning exercise for beginner C programmers
and is intentionally kept simple, readable, and well-commented.

## Features

- Reads a password from the terminal
- Reports its length and which character types it contains
- Flags common weaknesses:
  - Matches against a small built-in list of frequently used/leaked passwords
  - 3+ repeated characters in a row (e.g. `"aaa"`, `"111"`)
  - 3-character sequential runs (e.g. `"abc"`, `"789"`, `"cba"`)
- Produces a numeric score (0-7) and a final rating: `VERY WEAK`, `WEAK`,
  `MODERATE`, `STRONG`, or `VERY STRONG`

## Getting Started

### Prerequisites

You need a C compiler. On most Linux systems and macOS, `gcc` or `clang`
is available. On Windows, you can use [MinGW](https://www.mingw-w64.org/)
or the compiler bundled with WSL.

### Build

```bash
gcc -Wall -Wextra -o password_checker password_checker.c
```

### Run

```bash
./password_checker
```

You'll be prompted to type a password, then the program prints its analysis.

### Example

```
=== Password Strength Checker ===
Enter a password to analyze: MyP@ssw0rd2026!

----- Analysis -----
Length: 15 characters
Contains lowercase letters : yes
Contains uppercase letters : yes
Contains digits            : yes
Contains special characters: yes

----- Weaknesses -----
- No major weaknesses detected.

----- Verdict -----
Score: 7 / 7
Rating: VERY STRONG
```

## How the Scoring Works

The score starts at 0 and is built up as follows:

| Check                | Points |
|-----------------------|--------|
| Length >= 12          | +3     |
| Length 8-11           | +2     |
| Length 6-7            | +1     |
| Length < 6            | +0     |
| Each character type present (lowercase, uppercase, digit, special) | +1 each (max +4) |

That gives a maximum possible score of **7**. Then penalties are applied:

| Weakness                          | Penalty |
|------------------------------------|---------|
| Password is in the common-password list | -4 |
| 3+ repeated characters in a row    | -1      |
| 3-character sequential run         | -1      |

The score is clamped so it never goes below 0. Final ratings:

- `0-2` -> **WEAK**
- `3-4` -> **MODERATE**
- `5-6` -> **STRONG**
- `7`   -> **VERY STRONG**
- Any match in the common-password list is always reported as **VERY WEAK**,
  regardless of score, because such passwords are trivial to guess.

## Code Walkthrough (for beginners)

The program is split into small functions, each doing one job. This is a
good habit to build early - it makes code easier to read, test, and reuse.

- **`to_lowercase()`** - copies a string into another buffer, converting
  every character to lowercase using `tolower()` from `<ctype.h>`. This
  lets us compare passwords to the common-password list without worrying
  about capitalization (e.g. `"Password"` vs `"password"`).

- **`is_common_password()`** - loops over a hardcoded array of strings
  (`COMMON_PASSWORDS`) and uses `strcmp()` to see if the lowercase version
  of the input matches any of them.

- **`check_character_types()`** - loops over every character of the
  password and uses `islower()`, `isupper()`, `isdigit()` to classify it.
  Anything that isn't a letter or digit is treated as a "special"
  character. Notice this function doesn't `return` a value - instead it
  takes **pointers** (`int *has_lower`, etc.) so it can update four
  variables in the caller at once. This is a common C pattern for
  "returning" multiple values.

- **`has_repeated_chars()`** - walks through the password keeping a
  running count (`run`) of how many identical characters appear in a row.
  If that count reaches 3, it returns `1` immediately.

- **`has_sequential_chars()`** - looks at every group of 3 consecutive
  characters and checks if their character codes increase by 1 each time
  (like `a`, `b`, `c`) or decrease by 1 each time (like `c`, `b`, `a`).

- **`main()`** - ties everything together: reads input with `fgets()`,
  strips the trailing newline, runs all the checks, computes the score,
  and prints a readable report.

## Why These Checks Matter

- **Length** is one of the biggest factors in how long a password takes to
  crack by brute force - each extra character multiplies the number of
  possible combinations.
- **Character variety** (mixing cases, digits, and symbols) increases the
  size of the "alphabet" an attacker has to search through.
- **Common passwords** are the first thing attackers try, because lists of
  millions of real leaked passwords are freely available online.
- **Repeated and sequential characters** are easy for humans to type and
  remember, but they're also easy for cracking tools to guess, since they
  follow predictable patterns.

## Ideas for Extending This Project

These are good next steps if you want to keep practicing:

1. **Load the common password list from a file** instead of hardcoding it,
   so it can be updated without recompiling.
2. **Hide password input** so it doesn't echo to the screen (look into
   `termios` on Linux/macOS or `_getch()` on Windows).
3. **Add an entropy calculation** to estimate how many guesses an attacker
   would need.
4. **Detect keyboard-walk patterns** like `"qwerty"` or `"asdf"` as their
   own category of weakness.
5. **Write unit tests** for each function using a simple test framework or
   plain `assert()` calls.
6. **Accept a password as a command-line argument** in addition to
   interactive input (note: command-line arguments can be visible to other
   processes, so this is mainly for learning).

## License

This project is released under the MIT License - feel free to use it for
learning, modify it, and share it.
