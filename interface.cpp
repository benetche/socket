// Copyright (c) 2015-2019, Ulf Magnusson <ulfalizer@gmail.com>
// https://github.com/ulfalizer/readline-and-ncurses
#define COMMAND_WINDOW_HEIGHT 4
#define SUGGESTION_WINDOW_HEIGHT 3
#define CONTENT_WINDOW_HEIGHT                                                  \
  (LINES - COMMAND_WINDOW_HEIGHT - SUGGESTION_WINDOW_HEIGHT)
#define CONTENT_WINDOW_MIN_HEIGHT 1
#define SUGGESTION_WINDOW_Y_POS                                                \
  (LINES - SUGGESTION_WINDOW_HEIGHT - COMMAND_WINDOW_HEIGHT)
#define COMMAND_WINDOW_Y_POS (LINES - COMMAND_WINDOW_HEIGHT)
#define MIN_WINDOW_HEIGHT                                                      \
  (COMMAND_WINDOW_HEIGHT + SUGGESTION_WINDOW_HEIGHT + CONTENT_WINDOW_MIN_HEIGHT)

#include "interface.hpp"
#include "Socket.hpp"
#include "util.hpp"
#include <algorithm>
#include <csignal>
#include <curses.h>
#include <iostream>
#include <iterator>
#include <mutex>
#include <readline/history.h>
#include <readline/readline.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <wchar.h>
#include <wctype.h>

GUI::GUI(std::string commandString) { this->commandString = commandString; }

GUI *GUI::singleton = nullptr;
std::mutex GUI::singletonMutex;

GUI *GUI::GetInstance(std::string commandString) {

  std::lock_guard<std::mutex> lock(singletonMutex);

  if (singleton == nullptr) {
    singleton = new GUI(commandString);
  }
  return singleton;
}

void GUI::exitFailing(std::string message, int exitCode) {

  if (isInGUI) {
    endwin();
  }
  exitFailure(message, exitCode);
}

size_t GUI::strnwidth(const char *s, size_t n, size_t offset) {
  mbstate_t shift_state;
  wchar_t wc;
  size_t wc_len;
  size_t width = 0;

  memset(&shift_state, '\0', sizeof shift_state);

  for (size_t i = 0; i < n; i += wc_len) {
    wc_len = mbrtowc(&wc, s + i, MB_CUR_MAX, &shift_state);
    switch (wc_len) {
    case 0:
      return width;

    case (size_t)-1:
    case (size_t)-2:

      width += strnlen(s + i, n - i);
      return width;
    }

    if (wc == '\t') {
      width = ((width + offset + 8) & ~7) - offset;
    } else {

      width += iswcntrl(wc) ? 2 : std::max(0, wcwidth(wc));
    }
  }
  return width;
}

size_t GUI::strwidth(const char *s, size_t offset) {
  return this->strnwidth(s, SIZE_MAX, offset);
}

int GUI::readlineInputAvailable() { return singleton->isInputAvailable; }

int GUI::readlineGetc(FILE *dummy) {
  UNUSED(dummy);
  singleton->isInputAvailable = false;
  return singleton->input;
}

void GUI::forwardToReadline(char c) {
  this->input = c;
  this->isInputAvailable = true;
  rl_callback_read_char();
}

void GUI::redisplayMessage(bool isResizing) {
  CHECK_NCURSES(werase, contentWindow);
  CHECK_NCURSES(mvwaddstr, contentWindow, 0, 0, this->contentString.c_str());

  if (isResizing) {
    CHECK_NCURSES(wnoutrefresh, contentWindow);
  } else {
    CHECK_NCURSES(wrefresh, contentWindow);
  }

  windowRedisplay(isResizing);
}

void GUI::log(std::string message) {
  std::lock_guard<std::mutex> lock(singletonMutex);
  if (singleton == nullptr || !singleton->isInGUI) {
    std::cout << message << std::endl;
  } else {
    addToWindow("LOG: " + message);
  }
}

void GUI::addToWindow(std::string message) {

  if (singleton == nullptr || !singleton->isInGUI) {
    safeExitFailure("Written to GUI window without initializing it", 1);
  }

  if (singleton->contentString.size() > 0) {
    singleton->contentString += "\n";
  }
  singleton->contentString += message;
  singleton->redisplayMessage(false);
}

void GUI::setSuggestions(std::string suggestions) {

  std::string suggestionsString =
      suggestions != "" ? "Recommended commands: \n" + suggestions + "\n" : "";

  CHECK_NCURSES(werase, suggestionWindow);
  CHECK_NCURSES(mvwaddstr, suggestionWindow, 0, 0, suggestionsString.c_str());
  CHECK_NCURSES(wrefresh, suggestionWindow);
}

void GUI::handleCommand(char *line) {
  if (!line) {
    singleton->prepareClose("Exiting...");
    return;
  }

  if (*line) {
    add_history(line);
  }

  singleton->executeCommand(std::string(line));

  free(line);
}

void GUI::prepareClose(std::string message) {
  this->addToWindow(message);
  this->shouldClose = true;
}

void GUI::windowRedisplay(bool isResizing) {
  size_t prompt_width = strwidth(rl_display_prompt, 0);
  size_t cursor_col =
      prompt_width + strnwidth(rl_line_buffer, rl_point, prompt_width);
  CHECK_NCURSES(werase, commandWindow);

  mvwprintw(commandWindow, 0, 0, "%s%s", rl_display_prompt, rl_line_buffer);
  if ((int)cursor_col >= COLS) {

    curs_set(0);
  } else {
    CHECK_NCURSES(wmove, commandWindow, 0, cursor_col);
    curs_set(2);
  }
  if (isResizing) {
    CHECK_NCURSES(wnoutrefresh, commandWindow);
  } else {
    CHECK_NCURSES(wrefresh, commandWindow);
  }
}

void GUI::readlineRedisplay() { singleton->windowRedisplay(false); }

void GUI::resize() {
  if (LINES >= MIN_WINDOW_HEIGHT) {
    CHECK_NCURSES(wresize, contentWindow, CONTENT_WINDOW_HEIGHT, COLS);
    CHECK_NCURSES(wresize, suggestionWindow, SUGGESTION_WINDOW_HEIGHT, COLS);
    CHECK_NCURSES(wresize, commandWindow, COMMAND_WINDOW_HEIGHT, COLS);
    CHECK_NCURSES(mvwin, suggestionWindow, SUGGESTION_WINDOW_Y_POS, 0);
    CHECK_NCURSES(mvwin, commandWindow, COMMAND_WINDOW_Y_POS, 0);
  }

  redisplayMessage(true);
  CHECK_NCURSES_VOID(doupdate);
}

void GUI::initNCurses() {

  if (!initscr()) {
    exitFailing("Failed to initialize ncurses!", EXIT_FAILURE);
  }

  isInGUI = true;

  CHECK_NCURSES_VOID(raw);
  CHECK_NCURSES_VOID(cbreak);
  CHECK_NCURSES_VOID(noecho);
  CHECK_NCURSES_VOID(nonl);
  CHECK_NCURSES(intrflush, NULL, FALSE);
  curs_set(2);

  if (LINES >= MIN_WINDOW_HEIGHT) {
    contentWindow = newwin(CONTENT_WINDOW_HEIGHT, COLS, 0, 0);
    suggestionWindow =
        newwin(SUGGESTION_WINDOW_HEIGHT, COLS, SUGGESTION_WINDOW_Y_POS, 0);
    commandWindow =
        newwin(COMMAND_WINDOW_HEIGHT, COLS, COMMAND_WINDOW_Y_POS, 0);
  } else {

    contentWindow = newwin(1, COLS, 0, 0);
    commandWindow = newwin(1, COLS, 0, 0);
    suggestionWindow = newwin(1, COLS, 0, 0);
  }
  if (!contentWindow || !commandWindow || !suggestionWindow) {
    exitFailing("Failed to allocate windows!", EXIT_FAILURE);
  }

  CHECK_NCURSES(scrollok, contentWindow, TRUE);
}

void GUI::closeNCurses() {
  CHECK_NCURSES(delwin, contentWindow);
  CHECK_NCURSES(delwin, commandWindow);
  CHECK_NCURSES(delwin, suggestionWindow);
  CHECK_NCURSES_VOID(endwin);
  isInGUI = false;
}

void GUI::initSignalHandler() { signal(SIGINT, SIG_IGN); }

void GUI::closeSignalHandler() { signal(SIGINT, SIG_DFL); }

void GUI::initReadline() {

  rl_catch_signals = 0;
  rl_catch_sigwinch = 0;
  rl_deprep_term_function = NULL;
  rl_prep_term_function = NULL;

  rl_change_environment = 0;

  rl_getc_function = readlineGetc;
  rl_input_available_hook = readlineInputAvailable;
  rl_redisplay_function = readlineRedisplay;

  rl_attempted_completion_function = commandCompletion;

  commands["/help"] = [this](const argsT &) {
    auto commands = getCommands();
    std::string help = "Available commands: ";

    for (auto &command : commands) {
      help += command + " ";
    }

    addToWindow(help);

    return 0;
  };

  commands["/quit"] = [this](const argsT &) {
    this->prepareClose("Exiting...");
    return 0;
  };

  rl_callback_handler_install(commandString.c_str(), handleCommand);
}

void GUI::closeReadline() { rl_callback_handler_remove(); }

char GUI::readFromGUI() { return wgetch(commandWindow); }

void GUI::implementCommand(std::string command, commandFnT fn) {
  commands[command] = fn;
}

std::vector<std::string> GUI::getCommands() {
  std::vector<std::string> allCommands;
  for (auto commandPair : commands) {
    allCommands.push_back(commandPair.first);
  }

  return allCommands;
}

void GUI::updatePrompt(SocketWithInfo *user) {

  if (singleton == nullptr || !singleton->isInGUI) {
    safeExitFailure("Cannot updated prompt without initializing GUI!",
                    EXIT_FAILURE);
  }

  std::string newString = "<" + user->nickname;

  if (user->channel != "") {
    newString += "@" + user->channel;
  }

  newString += "> ";

  singleton->commandString = newString;
  rl_set_prompt(singleton->commandString.c_str());
  singleton->windowRedisplay(false);
}

char *GUI::commandIterator(const char *text, int state) {

  static GUI::commandMap::iterator it;

  auto &commands = singleton->commands;

  if (state == 0) {
    it = begin(commands);
  }

  while (it != end(commands)) {
    auto &command = it->first;
    ++it;

    if (command.find(text) != std::string::npos) {
      return strdup(command.c_str());
    }
  }
  return nullptr;
}

char **GUI::commandCompletion(const char *text, int start, int) {
  char **completionList = nullptr;
  rl_attempted_completion_over = 1;

  if (start == 0) {
    completionList = rl_completion_matches(text, GUI::commandIterator);
  }

  std::string suggestions = "";

  if (completionList) {
    for (int i = 0; completionList[i]; i++) {
      if (i != 0) {
        suggestions += std::string(completionList[i]) + " ";
        free(completionList[i]);
        completionList[i] = nullptr;
      }
    }
    if (strcmp(completionList[0], "") == 0) {
      free(completionList[0]);
      completionList[0] = nullptr;
      free(completionList);
      completionList = nullptr;
    }
  }

  singleton->setSuggestions(suggestions);

  return completionList;
}

int GUI::executeCommand(std::string command) {

  if (messageFn != nullptr && command[0] != '/') {
    return messageFn(command);
  }

  std::vector<std::string> inputs;
  {
    std::istringstream iss(command);
    std::copy(std::istream_iterator<std::string>(iss),
              std::istream_iterator<std::string>(), std::back_inserter(inputs));
  }

  if (inputs.size() == 0) {
    return 0;
  }

  GUI::commandMap::iterator it;

  if ((it = commands.find(inputs[0])) != end(commands)) {
    return static_cast<int>((it->second)(inputs));
  }

  addToWindow("Command " + inputs[0] +
              " not found! Run /help to see available commands.\n");

  return 1;
}

void GUI::enableMessaging(messageFnT fn) { messageFn = fn; }

void GUI::init() {

  if (isInGUI) {
    return;
  }

  this->initSignalHandler();
  this->initNCurses();
  this->initReadline();
}

void GUI::close() {
  if (!isInGUI) {
    return;
  }

  this->closeNCurses();
  this->closeReadline();
  this->closeSignalHandler();
}

void GUI::run() {

  if (!isInGUI) {
    exitFailing("Can't run GUI without initializing it!", EXIT_FAILURE);
  }

  if (this->isRunning) {
    return;
  }

  this->isRunning = true;

  do {

    int c = this->readFromGUI();

    switch (c) {
    case KEY_RESIZE:
      resize();
      break;

    case '\f':
      CHECK_NCURSES(clearok, curscr, true);
      resize();
      break;

    default:
      forwardToReadline(c);
    }
  } while (!shouldClose);

  this->isRunning = false;
}
