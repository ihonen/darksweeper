#ifndef SMILEYBUTTON_HH
#define SMILEYBUTTON_HH

#include <QLabel>
#include <QObject>

class SmileyButton : public QLabel
{
    Q_OBJECT

signals:
    void clicked();
public:
    enum class Status {HAPPY, WORRIED, COOL, DEAD};

    SmileyButton(QWidget* parent = nullptr);
    virtual ~SmileyButton() override = default;
    void set_status(Status status);
protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
private:
    Status m_status;
};

#endif // SMILEYBUTTON_HH
