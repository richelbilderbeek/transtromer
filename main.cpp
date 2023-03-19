#include "editwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  EditWindow w;
  w.show();
  return a.exec();
}
