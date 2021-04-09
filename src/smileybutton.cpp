#include "smileybutton.hh"

#include "images.hh"

SmileyButton::SmileyButton(QWidget* parent) :
    QLabel(parent),
    m_status(SmileyButton::Status::HAPPY)
{
    this->setPixmap(QPixmap(IMG_SMILEY_HAPPY));
    //this->resize(this->pixmap()->width(), this->pixmap()->height());

    QPixmap pixmap = this->pixmap(Qt::ReturnByValueConstant::ReturnByValue);
    this->resize(pixmap.width(), pixmap.height());
}

void SmileyButton::set_status(SmileyButton::Status status)
{
    if (status == Status::HAPPY)
        this->setPixmap(QPixmap(IMG_SMILEY_HAPPY));
    else if (status == Status::WORRIED)
        this->setPixmap(QPixmap(IMG_SMILEY_WORRIED));
    else if (status == Status::COOL)
        this->setPixmap(QPixmap(IMG_SMILEY_COOL));
    else if (status == Status::DEAD)
        this->setPixmap(QPixmap(IMG_SMILEY_DEAD));

    m_status = status;
}

void SmileyButton::mousePressEvent(QMouseEvent* event)
{
    if (m_status == Status::HAPPY)
        this->setPixmap(QPixmap(IMG_SMILEY_HAPPY_PRESSED));
    else if (m_status == Status::WORRIED)
        this->setPixmap(QPixmap(IMG_SMILEY_WORRIED_PRESSED));
    else if (m_status == Status::COOL)
        this->setPixmap(QPixmap(IMG_SMILEY_COOL_PRESSED));
    else if (m_status == Status::DEAD)
        this->setPixmap(QPixmap(IMG_SMILEY_DEAD_PRESSED));

    this->QLabel::mousePressEvent(event);
}

void SmileyButton::mouseReleaseEvent(QMouseEvent* event)
{
    this->set_status(Status::HAPPY);
    emit(clicked());

    this->QLabel::mouseReleaseEvent(event);
}

void SmileyButton::mouseMoveEvent(QMouseEvent* event)
{
    this->QLabel::mouseMoveEvent(event);
}
