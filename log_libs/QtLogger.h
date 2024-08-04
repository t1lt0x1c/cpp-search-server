#ifndef QTLOGGER_H
#define QTLOGGER_H

#include <plog/Log.h>
#include <QTextEdit>
#include <QString>

class QtLogger : public plog::IFormatter {
public:
    QtLogger(QTextEdit* textEdit);
    void log(const plog::Record& record) override;

private:
    QTextEdit* m_textEdit;
};

#endif // QTLOGGER_H
