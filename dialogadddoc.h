#ifndef DIALOGADDDOC_H
#define DIALOGADDDOC_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>

#include "WidgetPopulator.h"
#include "search-server/search_server.h"
#include "search-server/string_processing.h"

namespace Ui {
class DialogAddDoc;
}

class DialogAddDoc : public QDialog
{
    Q_OBJECT

private:
    struct DocInfo
    {
        DocInfo (){}
        int id;
        std::string text;
        std::string status;
        std::vector<int> rating;
    };
public:
    explicit DialogAddDoc(QWidget *parent = nullptr);
    ~DialogAddDoc();
    DocInfo& GetData();
private:
    Ui::DialogAddDoc *ui;
private:
    QLineEdit* id;
    QComboBox* status;
    QTextEdit* text;
    QLineEdit* rating;


private slots:
    void on_buttonBox_accepted();

private:
    void RecData();

private:
    DocInfo data;
};

#endif // DIALOGADDDOC_H
