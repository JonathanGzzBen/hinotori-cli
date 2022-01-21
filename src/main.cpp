#include <hinotori-core/question.h>

#include <QCoreApplication>

#include "commandlineinterface.h"

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);
  QCoreApplication::setOrganizationName("hinotori");
  QCoreApplication::setOrganizationDomain("hinotori.com");
  QCoreApplication::setApplicationName("hinotori-cli");

  CommandLineInterface cli{};
  QObject::connect(&cli, &CommandLineInterface::Quit, &a,
                   &QCoreApplication::quit,
                   Qt::ConnectionType::QueuedConnection);

  if (argc > 1 && strncmp(argv[1], "--version", 9) == 0) {
    QTextStream out{stdout};
    out << "hinotori v1.0.0\n";
    return 0;
  } else {
    cli.Start();
  }

  return a.exec();
}
