#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QMainWindow>

namespace Ui {
  class EditWindow;
}

class EditWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit EditWindow(QWidget *parent = nullptr);
  ~EditWindow();

private:
  Ui::EditWindow *ui;

public slots:
  void on_mouse_move_over_text();

};

#endif // EDITWINDOW_H
