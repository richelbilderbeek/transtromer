#include "editwindow.h"
#include "ui_editwindow.h"

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


  ui->rhyme_edit->setPlainText(
    cursor.selectedText() + " " +
    QString::fromStdString(
      std::to_string(std::rand() % 10000)
    )
  );
}
