#ifndef COUNTER_HH
#define COUNTER_HH

#include <QWidget>
#include <QLabel>

class LcdCounter : public QWidget
{
public:
    LcdCounter(int value = 0, QWidget* parent = nullptr);
    bool set_value(int value);
    bool increment();
    bool decrement();
private:
    void refresh_digits();
    void set_digit_graphic(QLabel* digit_label, int digit_value);
    int m_value;
    int m_max_value;
    QLabel* m_digit_labels[3];
};

#endif // COUNTER_HH
