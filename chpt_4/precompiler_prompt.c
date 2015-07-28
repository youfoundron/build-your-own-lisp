#include <stdio.h>
#include <stdlib.h>

/* if we are compiling on Windows compile these functions */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/* Fake readline function */
char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

/* Fake add history function */
void add_history(char* unused) {}

/* Otherwise include the editless headers */
#else
#include <editline/readline.h>
// #include <editline/history.h>
#endif

int main(int argc, char** argv) {

  /* Print Version and Exit Information */
  puts("Nihilisp Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit\n");

  /* In a never ending loop */
  while (1) {

    /* Now in either case readline will be correctly defined */
    /* Output our prompt and get input */
    char* input = readline("nilisp> ");
    /* Add input to history */
    add_history(input);
    /* Echo input back to user */
    printf("No you're a %s\n", input);
    /* Free retrieved input */
    free(input);
  }
  return 0;
}
