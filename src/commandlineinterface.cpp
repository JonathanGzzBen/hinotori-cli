#include "commandlineinterface.h"

#include <iostream>

CommandLineInterface::CommandLineInterface(QObject *parent)
    : QObject{parent},
      kDataDirectory{QStandardPaths::writableLocation(
                         QStandardPaths::GenericDataLocation) +
                     QDir::separator() + "hinotori"} {}

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
    } else if (!strncmp(line, "questionnaires", 9)) {
      if (!kDataDirectory.exists()) {
        out << "No data found in " << kDataDirectory.path() << "\n";
      } else {
        auto filesInDataDirectory{
            kDataDirectory.entryList(QDir::Files | QDir::Readable)};
        out << "\n";
        for (const auto &file : filesInDataDirectory) {
          out << "Filename: " << file << "\n";
          Questionnaire questionnaire;
          if (Questionnaire::LoadQuestionnaire(questionnaire,
                                               kDataDirectory.filePath(file))) {
            out << "Title: " << questionnaire.Title() << "\n";
            out << "Author: " << questionnaire.Author() << "\n";
            out << "Number of questions: " << questionnaire.Questions().length()
                << "\n";
          }
          out << "\n";
        }
      }
      linenoiseHistoryAdd(line);
    } else if (!strncmp(line, "clear", 6)) {
      linenoiseClearScreen();
      linenoiseHistoryAdd(line);
    }
    out.flush();
  }
  emit Quit();
}

void CommandLineInterface::completion(const char *buf,
                                      linenoiseCompletions *lc) {
  if (buf[0] == 'q') {
    linenoiseAddCompletion(lc, "question");
    linenoiseAddCompletion(lc, "questionnaires");
  }
}

char *CommandLineInterface::hints(const char *buf, int *color, int *bold) {
  if (!strcasecmp(buf, "question")) {
    *color = 35;
    *bold = 0;
    return "naires";
  }
  return nullptr;
}
