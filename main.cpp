#include "editwindow.h"

#include <QApplication>

#include "text.h"
#include "website_reader.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  test_text();
  test_website_reader();

  std::string word_list_filename{"word_list.txt"};
  if (!file_exists(word_list_filename))
  {
    read_and_save_all_words(word_list_filename);
    assert(file_exists(word_list_filename));
  }

  EditWindow w;
  w.show();
  return a.exec();
}
