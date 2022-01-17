#include "commandlineinterface.h"

#include <iostream>

CommandLineInterface::CommandLineInterface(QObject *parent)
    : QObject{parent} {}

void CommandLineInterface::Start() {
  const char *line;

  linenoiseSetCompletionCallback(Completion);
  linenoiseSetHintsCallback(Hints);
  linenoiseHistorySetMaxLen(10);

  QTextStream out(stdout);
  while ((line = linenoise("hinotori> ")) != nullptr) {
    if (!strncmp(line, "quit", 5) || !strncmp(line, "exit", 5)) {
      out << "See you later.\n";
      linenoiseHistoryAdd(line);
      break;
    } else if (!strncmp(line, "questionnaires", 9)) {
      if (!k_data_directory_.exists()) {
        out << "No data found in " << k_data_directory_.path() << "\n";
      } else {
        DisplayQuestionnaires(out);
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

void CommandLineInterface::Completion(const char *buf,
                                      linenoiseCompletions *lc) {
  if (buf[0] == 'q') {
    linenoiseAddCompletion(lc, "question");
    linenoiseAddCompletion(lc, "questionnaires");
  }
}

char *CommandLineInterface::Hints(const char *buf, int *color, int *bold) {
  if (!strcasecmp(buf, "question")) {
    *color = 35;
    *bold = 0;
    return "naires";
  }
  return nullptr;
}
void CommandLineInterface::DisplayQuestionnaires(QTextStream &out) const {
  auto files_in_data_directory{
      k_data_directory_.entryList(QDir::Files | QDir::Readable)};
  out << "\n";
  for (const auto &file : files_in_data_directory) {
    out << "Filename: " << file << "\n";
    if (Questionnaire questionnaire;Questionnaire::LoadQuestionnaire(questionnaire,
                                                                     k_data_directory_.filePath(file))) {
      out << "Title: " << questionnaire.Title() << "\n";
      out << "Author: " << questionnaire.Author() << "\n";
      out << "Number of questions: " << questionnaire.Questions().length()
          << "\n";
    }
    out << "\n";
  }
}
