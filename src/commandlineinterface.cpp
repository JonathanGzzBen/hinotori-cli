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
      quint8 questionnaire_number{0};
      try {
        questionnaire_number = std::stoi(line + 9);
      } catch (...) {
        out << "Expected numeric argument <questionnaire_number>\n\n";
        out.flush();
        continue;
      }
      DisplayQuestions(out, questionnaire_number);
      linenoiseHistoryAdd(line);
    } else if (strncmp(line, "answer", 6) == 0) {
      quint8 questionnaire_number{0};
      try {
        questionnaire_number = std::stoi(line + 6);
      } catch (...) {
        out << "Expected numeric argument <questionnaire_number>\n\n";
        out.flush();
        continue;
      }
      out << "Answering questionnaire " << questionnaire_number << "\n";
      AnswerQuestionnaire(out, questionnaire_number);
      linenoiseHistoryAdd(line);
    } else if (strncmp(line, "create", 6) == 0) {
      QString input_filename{line + 6};
      if (input_filename.isEmpty()) {
        out << "Expected argument <questionnary_name>\n\n";
        out.flush();
        continue;
      }
      input_filename = input_filename.trimmed();
      CreateQuestionnaire(out, input_filename.endsWith(".json")
                                   ? input_filename
                                   : input_filename + ".json");
      linenoiseHistoryAdd(line);
    } else if (strncmp(line, "help", 4) == 0) {
      DisplayHelp(out, line + 4);
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
  constexpr char *hints[] = {"",
                             "answer",
                             "questions",
                             "questionnaires",
                             "create",
                             "help",
                             "help answer",
                             "help questions",
                             "help questionnaires",
                             "help create",
                             "exit"};
  for (const auto &hint : hints) {
    if (strncmp(buf, hint, strlen(buf)) == 0) {
      linenoiseAddCompletion(lc, hint);
    }
  }
}

char *CommandLineInterface::Hints(const char *buf, int *color, int *bold) {
  *color = 35;
  *bold = 0;
  constexpr char *hints[] = {"",
                             "answer",
                             "questions",
                             "questionnaires",
                             "create",
                             "help",
                             "help answer",
                             "help questions",
                             "help questionnaires",
                             "help create",
                             "exit"};
  for (const auto &hint : hints) {
    if (strncmp(buf, hint, strlen(buf)) == 0) {
      return hint + strlen(buf);
    }
  }
  return nullptr;
}

QList<QSharedPointer<Questionnaire>> CommandLineInterface::LoadQuestionnaires()
    const {
  QList<QSharedPointer<Questionnaire>> questionnaires;
  for (auto files_in_data_directory{
           k_data_directory_.entryList(QDir::Files | QDir::Readable)};
       const auto &file : files_in_data_directory) {
    if (Questionnaire questionnaire; Questionnaire::LoadQuestionnaire(
            questionnaire, k_data_directory_.filePath(file))) {
      questionnaires.append(QSharedPointer<Questionnaire>::create(
          questionnaire.Title(), questionnaire.Author(),
          questionnaire.Questions()));
    }
  }
  return questionnaires;
}

QSharedPointer<Questionnaire> CommandLineInterface::LoadQuestionnaire(
    quint8 questionnaire_number) const {
  const auto questionnaires{LoadQuestionnaires()};
  if (questionnaires.empty()) {
    return nullptr;
  }
  return questionnaires.at(questionnaire_number - 1);
}

void CommandLineInterface::DisplayQuestionnaires(QTextStream &out) const {
  const auto questionnaires{LoadQuestionnaires()};
  if (questionnaires.empty()) {
    out << "No data found in " << k_data_directory_.path() << "\n";
    out.flush();
    return;
  }
  for (quint8 questionnaire_number{1};
       const auto &questionnaire : questionnaires) {
    out << "\nQuestionnaire " << questionnaire_number++ << "\n";
    out << "Title: " << questionnaire->Title() << "\n";
    out << "Author: " << questionnaire->Author() << "\n";
    out << "Number of questions: " << questionnaire->Questions().length()
        << "\n";
  }
  out << "\n";
  out.flush();
}

void CommandLineInterface::DisplayQuestions(QTextStream &out,
                                            quint8 questionnaire_number) const {
  const auto questionnaire{LoadQuestionnaire(questionnaire_number)};
  if (!questionnaire) {
    out << "No questionnaire with that number found\n";
    out.flush();
    return;
  }
  out << "\n" << questionnaire->Title() << "\n\n";
  for (quint8 question_number{1};
       const auto &question : questionnaire->Questions()) {
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
}

void CommandLineInterface::AnswerQuestionnaire(
    QTextStream &out, quint8 questionnaire_number) const {
  auto questionnaire{LoadQuestionnaire(questionnaire_number)};
  if (!questionnaire) {
    out << "Cannot answer, no questionnaire with that number found\n";
    out.flush();
    return;
  }
  out << "\n" << questionnaire->Title() << "\n";
  quint8 correct_answers_count{0};
  QList<quint8> incorrect_questions_numbers;
  for (quint8 question_number{1};
       const auto &question : questionnaire->Questions()) {
    out.setFieldWidth(3);
    out << "\n" << question_number++ << ".- " << question->Message() << "\n";
    out.setFieldWidth(0);
    for (quint8 possible_answer_index{0};
         const auto &possible_answer : question->PossibleAnswers()) {
      out << ++possible_answer_index << " " << possible_answer << "\n";
    }
    out << "\n";
    out.flush();
    bool exit{false};
    if (AnswerQuestion(out, question, exit)) {
      correct_answers_count++;
    } else {
      incorrect_questions_numbers.append(question_number - 1);
    }
    if (exit) {
      out << "Stopped answering questionnaire.\n\n";
      out.flush();
      return;
    }
  }
  out << "\nAnswered correctly " << correct_answers_count << " questions\n";
  if (!incorrect_questions_numbers.empty()) {
    out << "Wrong questions: [ ";
    for (const auto incorrect_answer_number : incorrect_questions_numbers) {
      out << incorrect_answer_number << " ";
    }
    out << "]\n";
  }
  out.flush();
}

void CommandLineInterface::CreateQuestionnaire(QTextStream &out,
                                               QString filename) const {
  if (!k_data_directory_.exists()) {
    const QDir generic_data_location{
        QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)};
    generic_data_location.mkdir("hinotori");
  }
  QFile file{k_data_directory_.filePath(filename)};
  if (file.exists()) {
    out << "There is already a questionnaire with that filename\n";
    return;
  }
  QList<QSharedPointer<Question>> questions;
  questions.append(QSharedPointer<Question>{new Question(
      "Sample question 1", 1,
      {"Incorrect Answer 1", "Correct Answer", "Incorrect Answer"})});
  questions.append(QSharedPointer<Question>{new Question(
      "Sample question 2", 0, {"Correct Answer", "Incorrect Answer"})});
  Questionnaire questionnaire{filename.trimmed(), QDir::home().dirName(),
                              questions};
  if (Questionnaire::SaveQuestionnaire(questionnaire, file.fileName())) {
    out << "Created questionnaire in: " << file.fileName() << "\n";
  }
}

void CommandLineInterface::DisplayHelp(QTextStream &out, QString command) {
  command = command.trimmed();
  if (command == "questionnaires") {
    out << "questionnaires\nDisplay all stored questionnaires\n\n";
  } else if (command == "questions") {
    out << "questions <questionnaire_number>\nDisplay all questions in a "
           "questionnaire\n\n";
  } else if (command == "answer") {
    out << "answer <questionnaire_number>\nStart answering questionnaire\n\n";
  } else if (command == "create") {
    out << "create <questionnaire_name>\nCreate a questionnaire\n\n";
  } else if (command == "exit") {
    out << "exit\nExit hinotori\n\n";
  } else {
    out << "help <command>\nAvailable commands:"
           "\nquestionnaires\nquestions\nanswer\ncreate\nexit\n\n";
  }
}

bool CommandLineInterface::AnswerQuestion(QTextStream &out,
                                          QSharedPointer<Question> question,
                                          bool &exit) {
  exit = false;
  char *line;
  while ((line = linenoise("answer> ")) != nullptr) {
    if (strncmp(line, "quit", 4) == 0 || strncmp(line, "exit", 4) == 0) {
      exit = true;
      return false;
    }
    try {
      auto answer_number{std::stoi(line)};
      return question->CorrrectAnswerIndex() == answer_number - 1;
    } catch (...) {
      out << "Specify answer number\n";
    }
    out.flush();
  }
  return false;
}
