#include "commandlineinterface.h"

#include <iostream>

CommandLineInterface::CommandLineInterface(QObject *parent) : QObject{parent} {}

void CommandLineInterface::Start() {
  char *line;

  linenoiseSetCompletionCallback(completion);
  linenoiseSetHintsCallback(hints);
  linenoiseHistorySetMaxLen(10);

  QTextStream out(stdout);
  while ((line = linenoise("hinotori> ")) != nullptr) {
    if (!strncmp(line, "quit", 5) || !strncmp(line, "exit", 5)) {
      out << "See you later.\n";
      linenoiseHistoryAdd(line);
      break;
    } else if (!strncmp(line, "questionnaries", 9)) {
      out << "Displaying questionnaires:\n";
      linenoiseHistoryAdd(line);
    } else if (!strncmp(line, "clear", 6)) {
      linenoiseClearScreen();
    }
    out.flush();
  }
  emit Quit();
}

void CommandLineInterface::completion(const char *buf,
                                      linenoiseCompletions *lc) {
  if (buf[0] == 'q') {
    linenoiseAddCompletion(lc, "question");
    linenoiseAddCompletion(lc, "questionnaries");
  }
}

char *CommandLineInterface::hints(const char *buf, int *color, int *bold) {
  if (!strcasecmp(buf, "question")) {
    *color = 35;
    *bold = 0;
    return "naries";
  }
  return nullptr;
}
