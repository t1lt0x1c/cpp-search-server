#include "dialogadddoc.h"
#include "ui_dialogadddoc.h"

DialogAddDoc::DialogAddDoc(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogAddDoc)
{
    ui->setupUi(this);

    id = findChild<QLineEdit*>("id");
    status = findChild<QComboBox*>("status");
    text = findChild<QTextEdit*>("text");
    rating = findChild<QLineEdit*>("rating");

    StatusDoc(status);
}

void DialogAddDoc::RecData() {
    data.id = id->text().toInt();
    data.status = status->currentText().toStdString();
    data.text = text->toPlainText().toStdString();
    data.rating = SplitRating(rating->text());
}

DialogAddDoc::~DialogAddDoc()
{
    delete ui;
}

DialogAddDoc::DocInfo& DialogAddDoc::GetData() {
    return data;
}

void DialogAddDoc::on_buttonBox_accepted()
{
    RecData();
    accept();
}

