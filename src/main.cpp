#include <hinotori-core/question.h>

#include <QCoreApplication>

#include "commandlineinterface.h"

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);
  QCoreApplication::setOrganizationName("hinotori");
  QCoreApplication::setOrganizationDomain("hinotori.com");
  QCoreApplication::setApplicationName("hinotori-cli");

  CommandLineInterface cli{};
  QObject::connect(&cli, &CommandLineInterface::Quit, &a, &QCoreApplication::quit, Qt::ConnectionType::QueuedConnection);
  cli.Start();

  return a.exec();
}
