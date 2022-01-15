#ifndef COMMANDLINEINTERFACE_H
#define COMMANDLINEINTERFACE_H

#include <QObject>
#include <QDebug>

#include "linenoise.h"

class CommandLineInterface : public QObject {
  Q_OBJECT
 public:
  explicit CommandLineInterface(QObject *parent = nullptr);
  void Start();

 private:
  static void completion(const char *buf, linenoiseCompletions *lc);

  static char *hints(const char *buf, int *color, int *bold);

 signals:
  void Quit(int returnCode = 0);
};

#endif  // COMMANDLINEINTERFACE_H
