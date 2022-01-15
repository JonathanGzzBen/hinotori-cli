#ifndef COMMANDLINEINTERFACE_H
#define COMMANDLINEINTERFACE_H

#include <QObject>

class CommandLineInterface : public QObject {
  Q_OBJECT
 public:
  explicit CommandLineInterface(QObject *parent = nullptr);
  void Start();

 signals:
  void ExitAction(int returnCode = 0);
};

#endif  // COMMANDLINEINTERFACE_H
