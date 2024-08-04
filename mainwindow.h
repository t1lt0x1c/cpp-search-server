#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QInputDialog>
#include <QFileDialog>
#include <QComboBox>

#include "QLogger.h"

#include <memory>
#include <fstream>

#include <search_server.h>
#include <read_input_functions.h>
#include <json_reader.h>
#include "log_duration.h"



QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_create_empty_triggered();

    void on_create_stop_words_triggered();

    void on_import_json_triggered();

    void on_search_button_clicked();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<SearchServer> server;
    QLogger logger;
    QTextEdit* outlog;
    QWidget* menu_add;
    QAction* remove_doc;
    QTextEdit* search_result;
    QLineEdit* line_query;
    QComboBox* status;
};
#endif // MAINWINDOW_H
