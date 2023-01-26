#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>

static char input[2048];

int main(int argc, char** argv) {
    puts("Lisp Version 0.1");
    puts("Press Ctrl+c to exit \n");

    while (1) {
        char* input = readline("lisp> ");
        add_history(input);

        printf("woo: %s\n", input);

        free(input);
    }

    return 0;
}
