#include <cli/cli.h>
#include <cli/clifilesession.h>
#include <hinotori-core/question.h>

#include <QObject>
#include <QCoreApplication>

#include "commandlineinterface.h"

int main(int argc, char* argv[]) {
  QCoreApplication a(argc, argv);

  Question question{
      "Which is the best operating system?", 0, {"Linux", "Windows", "MacOS"}};
  qInfo() << question.Message();

  auto rootMenu = std::make_unique<cli::Menu>("hinotori");
  CommandLineInterface commandLineInterface{};
  QObject::connect(&commandLineInterface, &CommandLineInterface::ExitAction, &a, &QCoreApplication::exit, Qt::QueuedConnection);
  commandLineInterface.Start();
  return a.exec();
}
