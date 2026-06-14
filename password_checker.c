/*
 * Password Strength Checker
 * ==========================
 * A beginner-friendly C program that analyzes a password typed at the
 * keyboard and reports on:
 *
 *   1. Length        - how many characters long it is
 *   2. Complexity    - whether it mixes lowercase, uppercase, digits,
 *                       and special characters
 *   3. Weaknesses    - whether it is a commonly used/leaked password,
 *                       has 3+ repeated characters in a row (e.g. "aaa"),
 *                       or has a sequential run (e.g. "abc", "321")
 *
 * Based on these checks it produces a score and a final rating
 * (VERY WEAK, WEAK, MODERATE, STRONG, VERY STRONG).
 *
 * Concepts demonstrated (useful for C beginners):
 *   - Reading a line of text safely with fgets()
 *   - Working with arrays of strings (a list of common passwords)
 *   - Character classification using <ctype.h> (isupper, islower, ...)
 *   - Passing variables by pointer so a function can return multiple values
 *   - Simple loops for pattern detection
 *   - Building a basic scoring/rating system
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN 128

/*
 * A small sample list of extremely common / frequently leaked passwords.
 * Real-world checkers use much larger lists (often loaded from a file or
 * database), but this is enough to demonstrate the idea.
 */
static const char *COMMON_PASSWORDS[] = {
    "password", "123456", "123456789", "12345678", "12345",
    "qwerty", "abc123", "password1", "111111", "123123",
    "letmein", "iloveyou", "admin", "welcome", "monkey",
    "dragon", "football", "1234567890", "qwerty123", "000000"
};
#define COMMON_COUNT (int)(sizeof(COMMON_PASSWORDS) / sizeof(COMMON_PASSWORDS[0]))

/* Copies "src" into "dest" converting every character to lowercase.
 * Used so we can compare passwords to the common list regardless of case. */
void to_lowercase(char *dest, const char *src) {
    int i = 0;
    while (src[i] != '\0' && i < MAX_LEN - 1) {
        dest[i] = (char)tolower((unsigned char)src[i]);
        i++;
    }
    dest[i] = '\0';
}

/* Returns 1 if the password (case-insensitive) matches an entry in the
 * common password list, otherwise returns 0. */
int is_common_password(const char *password) {
    char lower[MAX_LEN];
    to_lowercase(lower, password);

    for (int i = 0; i < COMMON_COUNT; i++) {
        if (strcmp(lower, COMMON_PASSWORDS[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Walks through every character of the password and sets each flag to
 * 1 if that character type is present, otherwise leaves it at 0.
 * The flags are passed in as pointers so this function can "return"
 * four results at once. */
void check_character_types(const char *password, int *has_lower, int *has_upper,
                            int *has_digit, int *has_special) {
    *has_lower = *has_upper = *has_digit = *has_special = 0;

    for (int i = 0; password[i] != '\0'; i++) {
        unsigned char c = (unsigned char)password[i];

        if (islower(c))      *has_lower   = 1;
        else if (isupper(c)) *has_upper   = 1;
        else if (isdigit(c)) *has_digit   = 1;
        else                 *has_special = 1; /* anything else: !@#$ etc. */
    }
}

/* Returns 1 if the password contains 3 or more identical characters
 * in a row, e.g. "aaa" in "paaassword". */
int has_repeated_chars(const char *password, int len) {
    int run = 1; /* length of the current run of identical characters */

    for (int i = 1; i < len; i++) {
        if (password[i] == password[i - 1]) {
            run++;
            if (run >= 3) return 1;
        } else {
            run = 1;
        }
    }
    return 0;
}

/* Returns 1 if the password contains 3 characters in a row that form an
 * ascending or descending sequence, e.g. "abc", "789", "cba", "321". */
int has_sequential_chars(const char *password, int len) {
    for (int i = 0; i + 2 < len; i++) {
        int a = password[i];
        int b = password[i + 1];
        int c = password[i + 2];

        if ((b - a == 1 && c - b == 1) ||    /* ascending,  e.g. a,b,c */
            (b - a == -1 && c - b == -1)) {  /* descending, e.g. c,b,a */
            return 1;
        }
    }
    return 0;
}

int main(void) {
    char password[MAX_LEN];

    printf("=== Password Strength Checker ===\n");
    printf("Enter a password to analyze: ");

    if (fgets(password, sizeof(password), stdin) == NULL) {
        printf("Error reading input.\n");
        return 1;
    }

    /* fgets() keeps the trailing newline character - strip it off */
    int len = (int)strlen(password);
    if (len > 0 && password[len - 1] == '\n') {
        password[len - 1] = '\0';
        len--;
    }

    if (len == 0) {
        printf("\nNo password entered. Please try again.\n");
        return 1;
    }

    /* ---- Run all checks ---- */
    int has_lower, has_upper, has_digit, has_special;
    check_character_types(password, &has_lower, &has_upper, &has_digit, &has_special);

    int common     = is_common_password(password);
    int repeated   = has_repeated_chars(password, len);
    int sequential = has_sequential_chars(password, len);

    int variety = has_lower + has_upper + has_digit + has_special; /* 0..4 */

    /* ---- Scoring ----
     * Length contributes 0-3 points, character variety contributes 0-4
     * points, for a maximum possible score of 7. Weaknesses subtract
     * points from that total. */
    int score = 0;

    if (len >= 12)     score += 3;
    else if (len >= 8) score += 2;
    else if (len >= 6) score += 1;
    /* len < 6 contributes 0 points */

    score += variety;

    if (common)     score -= 4;
    if (repeated)   score -= 1;
    if (sequential) score -= 1;

    if (score < 0) score = 0;

    /* ---- Report: character analysis ---- */
    printf("\n----- Analysis -----\n");
    printf("Length: %d characters\n", len);
    printf("Contains lowercase letters : %s\n", has_lower ? "yes" : "no");
    printf("Contains uppercase letters : %s\n", has_upper ? "yes" : "no");
    printf("Contains digits            : %s\n", has_digit ? "yes" : "no");
    printf("Contains special characters: %s\n", has_special ? "yes" : "no");

    /* ---- Report: weaknesses ---- */
    printf("\n----- Weaknesses -----\n");
    if (common)
        printf("- This is one of the most commonly used/leaked passwords. Avoid it!\n");
    if (repeated)
        printf("- Contains 3 or more repeated characters in a row (e.g. 'aaa').\n");
    if (sequential)
        printf("- Contains a sequential pattern (e.g. 'abc' or '321').\n");
    if (len < 8)
        printf("- Shorter than the recommended minimum of 8 characters.\n");
    if (variety < 3)
        printf("- Uses too few character types. Try mixing upper/lowercase letters, digits, and symbols.\n");
    if (!common && !repeated && !sequential && len >= 8 && variety >= 3)
        printf("- No major weaknesses detected.\n");

    /* ---- Final verdict ---- */
    printf("\n----- Verdict -----\n");
    printf("Score: %d / 7\n", score);

    if (common) {
        printf("Rating: VERY WEAK (this password is publicly known)\n");
    } else if (score <= 2) {
        printf("Rating: WEAK\n");
    } else if (score <= 4) {
        printf("Rating: MODERATE\n");
    } else if (score <= 6) {
        printf("Rating: STRONG\n");
    } else {
        printf("Rating: VERY STRONG\n");
    }

    return 0;
}
