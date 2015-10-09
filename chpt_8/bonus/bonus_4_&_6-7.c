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

/* Create enumeration of possible error types */
enum { ERR_DIV_ZERO, ERR_INVALID_OP, ERR_INVALID_NUM };

/* Create enumeration of possibal val types */
enum { VAL_NUM, VAL_ERR };

/* Declare new val struct*/
typedef struct {
  int type;
  union {
    long num;
    int err;
  };
} val;

/* Create a new number type val */
val val_num(long x) {
  val v;
  v.num = x;
  v.type = VAL_NUM;
  return v;
}

/* Create a new error type val */
val val_err(int x) {
  val v;
  v.err = x;
  v.type = VAL_ERR;
  return v;
}

/* Print a val */
void val_print(val v) {
  switch (v.type) {
    /* In the case the type is a number print it */
    /* Then 'break' out of the switch statement. */
    case VAL_NUM: printf("%li", v.num); break;

    /* In the case the type is an error */
    case VAL_ERR:
      /* Check what type of error it is and print it */
      if (v.err == ERR_DIV_ZERO) {
        printf("error: Division by zero");
      }
      if (v.err == ERR_INVALID_OP) {
        printf("Error: Invalid operator");
      }
      if (v.err == ERR_INVALID_NUM) {
        printf("Errror: Invalid number");
      }
    break;
  }
}

/* Print a val followed by a new line */
void val_println(val v) {
  val_print(v);
  putchar('\n');
}

/* User operator string to see which operation to perform */
val eval_op(val x, char* op, val y) {

  /* If either value is an error return it */
  if (x.type == VAL_ERR) { return x; }
  if (y.type == VAL_ERR) { return y; }

  /* Otherwise do math on the number values */
  if (strcmp(op, "+") == 0) { return val_num(x.num + y.num); }
  if (strcmp(op, "-") == 0) { return val_num(x.num - y.num); }
  if (strcmp(op, "*") == 0) { return val_num(x.num * y.num); }
  if (strcmp(op, "/") == 0) {
    return y.num == 0 ? val_err(ERR_DIV_ZERO) : val_num(x.num / y.num);
  }
  // › Extend parsing and evaluation to support the remainder operator %.
  if (strcmp(op, "%") == 0) { return val_num(x.num % y.num); }
  if (strcmp(op, "^") == 0) {
    long result = 1;
    for (int i = 0; i < y.num; i++) { result = result * x.num; }
    return val_num(result);
  }
  if (strcmp(op, "min") == 0) { return (x.num <= y.num) ? val_num(x.num) : val_num(y.num); }
  if (strcmp(op, "max") == 0) { return (x.num >= y.num) ? val_num(x.num) : val_num(y.num); }
  return val_err(ERR_INVALID_OP);
}

/* evaluate function */
val eval(mpc_ast_t* t) {
  /* If tagged as number return it directly. */
  if (strstr(t->tag, "number")) {
    /* Check if there is some error in conversion */
    errno  = 0;
    /* see if a float is given by checking if contents have a decimal point */
  if (strchr(t->contents, '.') != NULL) {
      float x_float = strtof(t->contents, NULL);
      // printf("%f - float\n", x_float);
      return errno != ERANGE ? val_num(x_float) : val_err(ERR_INVALID_NUM);
    /* otherwise treat it as a long */
    } else {
      long x_long = strtol(t->contents, NULL, 10);
      // printf("%li - long\n", x_long);
      return errno != ERANGE ? val_num(x_long) : val_err(ERR_INVALID_NUM);
    }
  }

  /* The operator is always second child. */
  char* op = t->children[1]->contents;

  /* We store the third child in 'x' */
  val x = eval(t->children[2]);

  /* If '-' operator receives one argument, negate it */
  if (strcmp(op, "-") == 0 && t->children_num < 5) {
    x = eval_op(val_num(0), op, x);
  }

  /* Iterate the remaining children and combining. */
  int i = 3;
  while (strstr(t->children[i]->tag, "expr"))  {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }
  return x;
}

int main(int argc, char** argv) {
  /* Create Some Parsers */
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Nilisp   = mpc_new("nilisp");

  /* Define them with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT,
    // (-?([0-9]+)?)(\.?[0-9]+)
    "                                                    \
      number   : /-?[0-9](\\.[0-9]+)?/;                 \
      operator : '+' | '-' | '*' | '/' | '%' | '^' |     \
                \"min\" | \"max\" ;                      \
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
      /* On Success Evaluate the AST and Print the response */
      val result = eval(r.output);
      val_println(result);
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
