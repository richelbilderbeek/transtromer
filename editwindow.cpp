#include "editwindow.h"
#include "ui_editwindow.h"

//#include <QtNetwork/QNetworkAccessManager>
//#include <QtNetwork/qnetworkreply.h>

#include "website_reader.h"

EditWindow::EditWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::EditWindow)
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
  delete ui;
}



void EditWindow::on_mouse_move_over_text()
{
  QTextCursor cursor = ui->text_edit->textCursor();
  cursor.select(QTextCursor::WordUnderCursor);

  website_reader r;
  const auto v{
    r.get_rhyme_words(cursor.selectedText().toStdString())
  };
  QString qs;
  for (const auto& s: v) qs += QString::fromStdString(s) + '\n';

  ui->rhyme_edit->setPlainText(qs);
}
