#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QInputDialog>
#include <QFileDialog>
#include <QComboBox>

#include <memory>
#include <fstream>

#include "search-server/search_server.h"
#include "search-server/string_processing.h"
#include <search-server/read_input_functions.h>
#include <json_libs/json_reader.h>
#include "log_libs/log_duration.h"
#include "log_libs/qlogger.h"
#include "WidgetPopulator.h"
#include "dialogadddoc.h"



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
    void on_add_doc_triggered();

private:
    std::shared_ptr<SearchServer> server;
    QLogger logger;
private: //ui
    Ui::MainWindow *ui;
private: //widgets
    QTextEdit* outlog;
    QWidget* menu_add;
    QAction* remove_doc;
    QTextEdit* search_result;
    QLineEdit* line_query;
    QComboBox* status;
};
#endif // MAINWINDOW_H
