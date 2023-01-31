#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#include <editline/readline.h>

static char input[2048];

typedef struct lval {
  int type;
  long num;
  /* Error and Symbol types have some string data */
  char* err;
  char* sym;
  /* Count and Pointer to a list of "lval*" */
  int count;
  struct lval** cell;
} lval;

int lval_read_sym(lval* v, char* s, int i) {
    char* part = calloc(1,1);

    while (strchr(
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789_+-*\\/=<>!&", s[i]) && s[i] != '\0') 
    {
        part = realloc(part, strlen(part) + 2);
        part[strlen(part) + 1] = '\0';
        part[strlen(part) + 0] = s[i];
        i++
    }

    int is_num = strchr("-0123456789", part[0]);
    for (int i=1; i<strlen(part); i++) {
        if (!strchr("0123456789", part[i])) { is_num = 0; break; }
    }

    if (is_num) {
        errno = 0;
        long x = strtol(part, NULL, 10);
        lval_add(v, errno != ERANGE ? lval_num(x) : lval_err("Invalid Number %s", part));
    }
    else {
        lval_add(v, lval_sym(part));
    }

    free(part);

    return i;
}

char* lval_str_unescapable = "abfnrtv\\\'\"";
char* lval_str_escapable = "\a\b\f\n\r\t\v\\\'\"";

char lval_str_unescape(char x) {
    switch (x) {
        case 'a': return '\a';
        case 'b': return '\b';
        case 'f': return '\f';
        case 'n': return '\n';
        case 'r': return '\r';
        case 't': return '\t';
        case 'v': return '\v';
        case '\\': return '\\';
        case '\'': return '\'';
        case '\"': return '\"';
    }
    return '\0';
}

char* lval_str_escape(char x) {
    switch (x) {
        case '\a': return "\\a";
        case '\b': return "\\b";
        case '\f': return "\\f";
        case '\n': return "\\n";
        case '\r': return "\\r";
        case '\t': return "\\t";
        case '\v': return "\\v";
        case '\\': return "\\\\";
        case '\'': return "\\\'";
        case '\"': return "\\\"";
    }
    return "";
}

int lval_read_str(lval* v, char* s, int i) {
    char* part = calloc(1,1);

    while (s[i] != '"') {
        char c = s[i];
        if (c == '\0') {
            lval_add(v, lval_err("Unexpected end of input at string literal"));
            free(part);
            return strlen(s);
        }

        if (c== '\\') {
            i++;
            if (strchr(lval_str_unescapable, s[i])) {
                c = lval_str_unescape(s[i]);
            }
            else {
                lval_add(v, lval_err("Invalid escape character %c", c));
                free(part);
                return strlen(s);
            }
        }

        part = realloc(part, strlen(part) + 2);
        part[strlen(part) + 1] = '\0';
        part[strlen(part) + 0] = c;
        i++;
    }

    lval_add(v, lval_str(part));
    free(part);

    return i+1;
}

lval_print_str(lval* v) {
    putchar('"');

    for (int i=0; i<strlen(v->str); i++) {
        if (strchr(lval_str_escapable, v->str[i])) {
            printf("%s", lval_str_escape(v->str[i]));
        }
        else {
            putchar(v->str[i]);
        }
    }

    putchar('"');
}

lval* lval_read_expr(char* s, int* i, char end) {
    while(s[i] != end) {
        if (s[i] == '\0') {
            lval_add(v, lval_err("Missing %c at the end of input", end));
            return strlen(s) + 1;
        }

        if (strchr(" \t\v\r\n", s[i])) {
            i++;
            continue;
        }

        if (s[i] == ';') {
            while (s[i] != '\n' && s[i] != '\0') { i++; }
            i++;
            continue;
        }

        if (s[i] == '(') {
            lval* x = lval_sexpr();
            lval_add(v, x);
            i = lval_read_expr(x, s, i+1, ')');
            continue;
        }

        if (s[i] == '"') {
            i = lval_read_str(v, s, i+1);
            continue;
        }

        if (strchr(
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "0123456789_+-*\\/=<>!&", s[i])) 
        {
            i = lval_read_sym(v, s, i);
            continue;
        }

        lval_add(v, lval_err("Unknown Character %c", s[i]));
        return strlen(s) + 1;
    }

    return i+1;
}

/*lval* lval_eval(e, expr) {*/
/*}*/

/*lval* builtin_load([>lenv* e,<]lval* a) {*/
    /*//TODO asserts*/
    /*FILE* f = fopen(a->cell[0]->str, "rb");*/

    /*if (f==NULL) {*/
        /*[>lval* err = lval_err("")<]*/
    /*}*/
/*}*/


int main(int argc, char** argv) {
    puts("Lisp Version 0.1");
    puts("Press Ctrl+c to exit \n");

    while (1) {
        char* input = readline("lisp> ");
        add_history(input);

        int pos=0;
        lval* expr = lval_read_expr(input, &pos, '\0');
        /*lval* x = lval_eval(e, expr);*/

        free(input);
    }

    return 0;
}
