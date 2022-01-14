#include <hinotori-core/question.h>

#include <QCoreApplication>

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  Question question{
      "Which is the best operating system?", 0, {"Linux", "Windows", "MacOS"}};
  qInfo() << question.Message();

  return a.exec();
}
