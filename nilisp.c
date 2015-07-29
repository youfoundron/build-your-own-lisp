// To compile on mac & linux:

// On Linux you will also have to link to the maths library with -lm flag
// cc -std=c99 -Wall nilisp.c mpc.c -ledit -lm -o nilisp

// On linux you may need to install editline beforehand
// sudo apt-get install libedit-dev

// On Fedora you can use
// su -c "yum install libedit-dev*"

#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32 //define something for Windows (32-bit and 64-bit, this part is common)
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

   #ifdef _WIN64 //define something for Windows (64-bit only)
   #endif
#elif __APPLE__ //define something for Mac OS
#include <editline/readline.h>
  #include "TargetConditionals.h"
  #if TARGET_IPHONE_SIMULATOR // iOS Simulator
  #elif TARGET_OS_IPHONE // iOS device
  #elif TARGET_OS_MAC // Other kinds of Mac OS
  #else // Unsupported platform
  #endif
#elif __linux // linux
#include <editline/readline.h>
#include <editline/history.h> // histedit.h on Arch Linux
#elif __unix // Unix - all unices not caught above
#elif __posix // POSIX
#endif


int main(int argc, char** argv) {
  /* Create Some Parsers */
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Nilisp   = mpc_new("nilisp");

  /* Define them with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                    \
      number   : /-?[0-9]+(\\.[0-9]+)?/;                 \
      operator : '+' | '-' | '*' | '/' | '%' ;           \
      expr     : <number> | '(' <operator> <expr>+ ')';  \
      nilisp   : /^/ <operator> <expr>+ /$/ |            \
                 /^\\(/ <operator> <expr>+ /\\)$/;       \
    ",
    Number, Operator, Expr, Nilisp);

  /* Print Version and Exit Information */
  puts("Nihilisp Version 0.0.0.0.1");
  puts("Created by @youfoundron");
  puts("Press Ctrl+c to Exit\n");

  while (1) {                           /* In a never ending loop */
    char* input = readline("nilisp> "); /* Output our prompt and get input */
    add_history(input);                 /* Add input to history */

    /* Atempt to Parse the user Input */
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Nilisp, &r)) {
      /* On Success Print the AST */
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      /* Otherwise Print the Error */
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);                        /* Free retrieved input */
  }

  /* Undefine and Delete our Parsers */
  mpc_cleanup(4, Number, Operator, Expr, Nilisp);

  return 0;
}
