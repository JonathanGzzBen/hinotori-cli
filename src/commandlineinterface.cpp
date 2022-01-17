#include "commandlineinterface.h"

#include <iostream>
CommandLineInterface::CommandLineInterface(QObject *parent) : QObject{parent} {}

void CommandLineInterface::Start() {
  const char *line;

  linenoiseSetCompletionCallback(Completion);
  linenoiseSetHintsCallback(Hints);
  linenoiseHistorySetMaxLen(10);

  QTextStream out(stdout);
  while ((line = linenoise("hinotori> ")) != nullptr) {
    if (!strncmp(line, "quit", 4) || !strncmp(line, "exit", 4)) {
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
    } else if (!strncmp(line, "questions", 9)) {
      quint8 questionnary_number{0};
      try {
        questionnary_number = std::stoi(line + 9);
      } catch (...) {
        out << "Expected numeric argument <questionnarie_number>\n\n";
        out.flush();
        continue;
      }
      DisplayQuestions(out, questionnary_number);
      linenoiseHistoryAdd(line);
    } else if (!strncmp(line, "clear", 5)) {
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
    linenoiseAddCompletion(lc, "questions");
    linenoiseAddCompletion(lc, "questionnaires");
  }
}

char *CommandLineInterface::Hints(const char *buf, int *color, int *bold) {
  *color = 35;
  *bold = 0;
  if (strcasecmp(buf, "q") == 0) {
    return "uestions";
  }
  if (strcasecmp(buf, "question") == 0) {
    return "naires";
  }
  return nullptr;
}

void CommandLineInterface::DisplayQuestionnaires(QTextStream &out) const {
  auto files_in_data_directory{
      k_data_directory_.entryList(QDir::Files | QDir::Readable)};
  if (files_in_data_directory.empty()) {
    out << "\nNo data found in " << k_data_directory_.path() << "\n";
  }
  quint8 questionnaire_number{1};
  for (const auto &file : files_in_data_directory) {
    out << "\nQuestionnaire " << questionnaire_number++ << "\n";
    out << "Filename: " << file << "\n";
    if (Questionnaire questionnaire; Questionnaire::LoadQuestionnaire(
            questionnaire, k_data_directory_.filePath(file))) {
      out << "Title: " << questionnaire.Title() << "\n";
      out << "Author: " << questionnaire.Author() << "\n";
      out << "Number of questions: " << questionnaire.Questions().length()
          << "\n";
    }
  }
  out << "\n";
}

void CommandLineInterface::DisplayQuestions(QTextStream &out,
                                            quint8 questionnaire_number) const {
  auto files_in_data_directory{
      k_data_directory_.entryList(QDir::Files | QDir::Readable)};
  for (quint8 current_questionnaire_number{1};
       const auto &file : files_in_data_directory) {
    if (Questionnaire questionnaire; Questionnaire::LoadQuestionnaire(
            questionnaire, k_data_directory_.filePath(file))) {
      if (current_questionnaire_number != questionnaire_number) {
        current_questionnaire_number++;
        continue;
      }
      out << "\n" << questionnaire.Title() << "\n\n";
      for (quint8 question_number{1};
           const auto &question : questionnaire.Questions()) {
        out.setFieldWidth(3);
        out << question_number++ << ".- " << question->Message() << "\n";
        out.setFieldWidth(0);
        QString correct_answer;
        for (quint8 possible_answer_index{0};
             const auto &possible_answer : question->PossibleAnswers()) {
          if (possible_answer_index == question->CorrrectAnswerIndex()) {
            correct_answer = possible_answer;
          }
          out << ++possible_answer_index << " " << possible_answer << "\n";
        }
        out << "Correct answer: " << correct_answer << "\n\n";
      }
      return;
    }
  }
  out << "No questionnarie with that number\n\n";
}
