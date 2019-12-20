#include "lcdcounter.hh"

#include "images.hh"
#include <QLayout>

LcdCounter::LcdCounter(int value, QWidget* parent) :
    QWidget(parent),
    m_value(value),
    m_max_value(999)
{
    for (int i = 0; i < 3; ++i)
    {
        m_digit_labels[i] = new QLabel(this);
        m_digit_labels[i]->move(i * 13, 0);
    }
    this->refresh_digits();
    this->resize(3 * 13, 23);
}

bool LcdCounter::set_value(int value)
{
    // TODO: Negative numbers.
    // Enforce 0 <= m_value <= m_max_value.
    m_value = std::max(0, std::min(value, m_max_value));

    m_value = value;
    this->refresh_digits();
    return true;
}

bool LcdCounter::increment()
{
    this->set_value(m_value + 1);
    return true;
}

bool LcdCounter::decrement()
{
    this->set_value(m_value - 1);
    return true;
}

void LcdCounter::refresh_digits()
{
    int value_temp = m_value;

    int hundreds = m_value / 100;
    value_temp -= hundreds * 100;

    int tens = value_temp / 10;
    value_temp -= tens * 10;

    int units = value_temp;

    this->set_digit_graphic(m_digit_labels[0], hundreds);
    this->set_digit_graphic(m_digit_labels[1], tens);
    this->set_digit_graphic(m_digit_labels[2], units);
}

void LcdCounter::set_digit_graphic(QLabel* digit_label, int digit_value)
{
    switch (digit_value)
    {
        case 0:
            digit_label->setPixmap(QPixmap(IMG_LCD_0)); return;
        case 1:
            digit_label->setPixmap(QPixmap(IMG_LCD_1)); return;
        case 2:
            digit_label->setPixmap(QPixmap(IMG_LCD_2)); return;
        case 3:
            digit_label->setPixmap(QPixmap(IMG_LCD_3)); return;
        case 4:
            digit_label->setPixmap(QPixmap(IMG_LCD_4)); return;
        case 5:
            digit_label->setPixmap(QPixmap(IMG_LCD_5)); return;
        case 6:
            digit_label->setPixmap(QPixmap(IMG_LCD_6)); return;
        case 7:
            digit_label->setPixmap(QPixmap(IMG_LCD_7)); return;
        case 8:
            digit_label->setPixmap(QPixmap(IMG_LCD_8)); return;
        case 9:
            digit_label->setPixmap(QPixmap(IMG_LCD_9)); return;
    }
}
