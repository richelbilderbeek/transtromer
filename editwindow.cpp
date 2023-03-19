#include "editwindow.h"
#include "ui_editwindow.h"

//#include <QtNetwork/QNetworkAccessManager>
//#include <QtNetwork/qnetworkreply.h>

#include "website_reader.h"

EditWindow::EditWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::EditWindow),
  m_reader{load_website_reader("transtromer.txt")}

{
  ui->setupUi(this);
  this->setMouseTracking(true);
  ui->text_frame->setMouseTracking(true);
  ui->text_edit->setMouseTracking(true);
  connect(
    ui->text_edit, &QPlainTextEdit::cursorPositionChanged,
    this, &EditWindow::on_mouse_move_over_text
  );
  on_mouse_move_over_text();


}

EditWindow::~EditWindow()
{
  save(m_reader, "transtromer.txt");
  delete ui;
}



void EditWindow::on_mouse_move_over_text()
{
  QTextCursor cursor = ui->text_edit->textCursor();
  cursor.select(QTextCursor::WordUnderCursor);

  const auto v{
    m_reader.get_rhyme_words(cursor.selectedText().toStdString())
  };
  QString qs;
  for (const auto& s: v) qs += QString::fromStdString(s) + '\n';

  ui->rhyme_edit->setPlainText(qs);
  save(m_reader, "transtromer.txt");
}
