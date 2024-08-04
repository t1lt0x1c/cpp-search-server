#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    outlog = findChild<QTextEdit*>("outlog");
    logger.SetTextEdit(outlog);
    logger.CreateLog("Логгер инициализирован");
    menu_add = findChild<QWidget*>("menu_add");
    remove_doc = findChild<QAction*>("remove_doc");
    search_result = findChild<QTextEdit*>("search_result");
    line_query = findChild<QLineEdit*>("line_query");
    status = findChild<QComboBox*>("status_box");
    QStringList statuses;
    statuses << "ACTUAL" << "IRRELEVANT" << "BANNED" << "REMOVED";
    status->addItems(statuses);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_create_empty_triggered()
{
    server = std::make_shared<SearchServer>(SearchServer());
    logger.CreateLog("Создан новый сервер");
    menu_add->setEnabled(true);
    remove_doc->setEnabled(true);
}

void MainWindow::on_create_stop_words_triggered()
{
    QInputDialog* r = new QInputDialog(this);
    int result = r->exec();
    QString stop_words;
    if(result == QDialog::Accepted){
        stop_words = r->textValue();
    }
    server = std::make_shared<SearchServer>(SearchServer(stop_words.toStdString()));
    logger.CreateLog("Создан новый сервер со стоп-словами");
    menu_add->setEnabled(true);
    //remove_doc->setEnabled(true);
}

void MainWindow::on_import_json_triggered()
{
    auto json_file = QFileDialog::getOpenFileName(this);
    if(!json_file.isEmpty()) {
        std::ifstream in;       // поток для чтения
        in.open(json_file.toStdString());
        Json_reader* reader = new Json_reader(server, in);
        reader->FillServer();
        auto last_slash = json_file.lastIndexOf('/');
        logger.CreateLog("Добавлены документы из JSON. Файл: " + json_file.sliced(++last_slash));
    }
}

void MainWindow::on_search_button_clicked()
{
    search_result->clear();
    QString* query = new QString(line_query->text());
    if(!query->isEmpty()) {
        std::vector<Document> result = server.get()->FindTopDocuments(query->toStdString(), server.get()->StrToStatus(status->currentText().toStdString()));
        if (!result.empty()) {
            for(const auto& doc : result) {
                search_result->append(ToQStr(doc));
            }
        } else {
            search_result->append("Ничего не найдено по запросу: " + *query);
        }

        logger.CreateLog("Поиск по запросу: " + *query);
    } else {
        search_result->append("Пустой запрос");
    }
}

