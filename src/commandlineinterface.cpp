#include "commandlineinterface.h"

#include <cli/cli.h>
#include <cli/clifilesession.h>

CommandLineInterface::CommandLineInterface(QObject* parent) : QObject{parent} {}

void CommandLineInterface::Start() {
  auto rootMenu = std::make_unique<cli::Menu>("hinotori");
  rootMenu->Insert(
      "questionnaires", [](std::ostream& out) { out << "Displaying questionnaires\n"; },
      "Display questionnaires");
  cli::Cli cli(std::move(rootMenu));
  cli.ExitAction([this](auto& out) {
    out << "Goodbye.\n";
    emit ExitAction();
  });
  cli::CliFileSession input(cli);
  input.Start();
}
