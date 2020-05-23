#include "doublespinbox.h"

DoubleSpinBox::DoubleSpinBox(QWidget* parent)
    :
      QDoubleSpinBox(parent)
{
}

QString DoubleSpinBox::textFromValue(double value) const
{
    QString text = QDoubleSpinBox::textFromValue(value);
    return text.replace(QLocale().decimalPoint(), QLatin1Char('.'));
}

double DoubleSpinBox::valueFromText(const QString& text) const
{
    return QDoubleSpinBox::valueFromText(QString(text).replace(QLatin1Char('.'), QLocale().decimalPoint()));
}
