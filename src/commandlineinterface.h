#ifndef COMMANDLINEINTERFACE_H
#define COMMANDLINEINTERFACE_H

#include <hinotori-core/questionnaire.h>
#include <hinotori-core/question.h>

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
  static void completion(const char *buf, linenoiseCompletions *lc);

  static char *hints(const char *buf, int *color, int *bold);

  const QDir kDataDirectory;

 signals:
  void Quit(int returnCode = 0);
};

#endif  // COMMANDLINEINTERFACE_H
