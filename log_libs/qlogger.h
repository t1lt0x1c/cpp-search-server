#include <QTextEdit>
#include <QString>
#include <QTime>

class QLogger {
public:
    void CreateLog(QString text) {
        QTime time = QTime::currentTime();
        QString log = time.toString() + ": ";
        outlog->append(log + text);
    }
    void SetTextEdit(QTextEdit* qtextedit_) {
        outlog = qtextedit_;
    }
    void CLearWindow() {
        outlog->clear();
    }
private:
    QTextEdit* outlog;
};
