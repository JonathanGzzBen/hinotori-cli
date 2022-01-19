#ifndef COMMANDLINEINTERFACE_H
#define COMMANDLINEINTERFACE_H

#include <hinotori-core/question.h>
#include <hinotori-core/questionnaire.h>

#include <QDebug>
#include <QDir>
#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QStandardPaths>

#include "linenoise.h"

class CommandLineInterface : public QObject {
  Q_OBJECT
 public:
  explicit CommandLineInterface(QObject *parent = nullptr);
  void Start();

 private:
  static void Completion(const char *buf, linenoiseCompletions *lc);

  static char *Hints(const char *buf, int *color, int *bold);

  QList<QSharedPointer<Questionnaire>> LoadQuestionnaires() const;
  QSharedPointer<Questionnaire> LoadQuestionnaire(quint8 questionnaire_number) const;

  void DisplayQuestionnaires(QTextStream &out) const;
  void DisplayQuestions(QTextStream &out, quint8 questionnaire_number) const;
  void AnswerQuestionnaire(QTextStream &out, quint8 questionnaire_number) const;

  static bool AnswerQuestion(QTextStream &out,
                             QSharedPointer<Question> question, bool &exit);

  const QDir k_data_directory_{
      QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) +
      QDir::separator() + "hinotori"};

 signals:
  void Quit(int return_code = 0);
};

#endif  // COMMANDLINEINTERFACE_H
