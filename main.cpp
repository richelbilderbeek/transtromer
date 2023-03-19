#include "editwindow.h"

#include <QApplication>

#include "text.h"
#include "website_reader.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  test_text();
  test_website_reader();
  EditWindow w;
  w.show();
  return a.exec();
}
