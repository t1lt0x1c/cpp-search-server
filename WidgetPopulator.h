#ifndef WIDGETPOPULATOR_H
#define WIDGETPOPULATOR_H

#include <QComboBox>

static void StatusDoc(QComboBox *comboBox) {
    QStringList statuses;
    statuses << "ACTUAL" << "IRRELEVANT" << "BANNED" << "REMOVED";
    comboBox->addItems(statuses);
}
#endif // WIDGETPOPULATOR_H
