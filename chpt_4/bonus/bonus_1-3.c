// › Change the prompt from lispy> to something of your choice.
// › Change what is echoed back to the user.
// › Add an extra message to the Version and Exit Information.

// preproc wrappers: http://stackoverflow.com/questions/5919996/how-to-detect-reliably-mac-os-x-ios-linux-windows-in-c-preprocessor

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
#include <editline/history.h>
#elif __unix // Unix - all unices not caught above
#elif __posix // POSIX
#endif


int main(int argc, char** argv) {
  /* Print Version and Exit Information */
  puts("Nihilisp Version 0.0.0.0.1");
  puts("Created by @youfoundron");
  puts("Press Ctrl+c to Exit\n");

  /* In a never ending loop */
  while (1) {

    /* Now in either case readline will be correctly defined */
    /* Output our prompt and get input */
    char* input = readline("nilisp> ");
    /* Add input to history */
    add_history(input);
    /* Echo input back to user */
    printf("%s has no meaning.\n", input);
    /* Free retrieved input */
    free(input);
  }
  return 0;
}
