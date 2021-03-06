#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#if EMSCRIPTEN
#include <emscripten.h>
#endif

int line = 0;

void main_loop()
{
  char str[10] = {0};
  int ret;

  errno = 0;
  while (errno != EAGAIN) {
    if (line == 0) {
      ret = fgetc(stdin);
      if (ret != EOF) putc(ret, stdout);
      if (ret == '\n') line++;
    } else if (line > 0) {
      ret = scanf("%10s", str);
      if (ret > 0) puts(str);
    }

    if (ferror(stdin) && errno != EAGAIN) {
      puts("error");
      exit(EXIT_FAILURE);
    }

    if (feof(stdin)) {
      puts("eof");
      exit(EXIT_SUCCESS);
    }

    clearerr(stdin);
  }
}

int main(int argc, char const *argv[])
{
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

  // SM shell doesn't implement an event loop and therefor doesn't support
  // emscripten_set_main_loop. However, its stdin reads are sync so it
  // should exit out after calling main_loop once.
  main_loop();

#if EMSCRIPTEN
  emscripten_set_main_loop(main_loop, 60, 0);
#else
  while (1) main_loop(); sleep(1);
#endif
  return 0;
}
