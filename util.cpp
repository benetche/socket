#include "util.hpp"
#include "interface.hpp"

char *readLine(FILE *stream) {
  char *string = NULL;
  int size = 0;
  char c;

  fscanf(stream, " ");

  do {
    fscanf(stream, "%c", &c);

    if ((c == '\n' && size == 0) || c == 13) {
      c = 0;
    } else {
      if (size % READLINE_BUFFER == 0) {
        string = (char *)realloc(string, (size / READLINE_BUFFER + 1) *
                                             READLINE_BUFFER * sizeof(char));
      }
      string[size++] = c;
    }

  } while (c != '\n' && !feof(stream));

  string[--size] = 0;

  string = (char *)realloc(string, (size + 1) * sizeof(char));

  return string;
}

std::string readLineCpp(FILE *stream) {
  char *tmp = readLine(stream);
  std::string line = std::string(tmp);
  free(tmp);
  return line;
}

void exitFailure(std::string message, int code) {
  std::cerr << message << std::endl;
  exit(code);
}

void safeExitFailure(std::string message, int code) {
  GUI *gui = GUI::GetInstance("<IRC> ");
  gui->exitFailing(message, code);
}