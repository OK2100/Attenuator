#ifndef DOUBLESPINBOX_H
#define DOUBLESPINBOX_H

#include <QDoubleSpinBox>

class DoubleSpinBox : public QDoubleSpinBox
{
public:
    DoubleSpinBox(QWidget* parent=nullptr);

    QString textFromValue(double value) const override;
    double valueFromText(const QString &text) const override;

};

#endif // DOUBLESPINBOX_H
