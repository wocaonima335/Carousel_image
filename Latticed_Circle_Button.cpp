#include "Latticed_Circle_Button.h"

LatticeCircleButton::LatticeCircleButton(QWidget *parent)
{
    this->resize(50, 50);
    this->setCursor(Qt::PointingHandCursor);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 13);
    shadow->setBlurRadius(30);
    shadow->setColor(QColor(0, 0, 0, 100));
    this->setGraphicsEffect(shadow);

    opacityAnimation = new QPropertyAnimation(this, "opacity");
    opacityAnimation->setDuration(300);
    opacityAnimation->setStartValue(m_opacity);
    opacityAnimation->setEndValue(255);
    opacityAnimation->setEasingCurve(QEasingCurve::Linear);

    colorChangeAnimation = new QPropertyAnimation(this, "currentColor");
    colorChangeAnimation->setDuration(300);
    colorChangeAnimation->setStartValue(m_currentColor);
    colorChangeAnimation->setEndValue(QColor(255, 255, 255, 255));
    colorChangeAnimation->setEasingCurve(QEasingCurve::Linear);
}

int LatticeCircleButton::opacity() const
{
    return m_opacity;
}

void LatticeCircleButton::setOpacity(int newOpacity)
{
    if (m_opacity == newOpacity)
        return;
    m_opacity = newOpacity;
    update();
}

QColor LatticeCircleButton::currentColor() const
{
    return m_currentColor;
}

void LatticeCircleButton::setCurrentColor(const QColor &newCurrentColor)
{
    if (m_currentColor == newCurrentColor)
        return;
    m_currentColor = newCurrentColor;
    update();
}

void LatticeCircleButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setViewport(0, 0, width(), width());
    painter.setWindow(0, 0, 50, 50);

    QPen pen;
    pen.setWidth(2.00);
    pen.setColor(Qt::black);
    painter.setPen(pen);

    QBrush brush;
    brush.setColor(QColor(0, 0, 0, m_opacity));
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);

    painter.drawEllipse(3, 3, 44, 44);

    QPen pen1;
    pen1.setWidth(4);
    pen1.setColor(m_currentColor);
    pen1.setStyle(Qt::SolidLine);
    pen1.setCapStyle(Qt::FlatCap);
    pen1.setJoinStyle(Qt::MiterJoin);
    painter.setPen(pen1);

    if (isActiveWindow() == true) {
        QPoint points[] = {
            QPoint(28, 15),
            QPoint(18, 25),
            QPoint(28, 35),
        };
        painter.drawPolyline(points, 3);
    } else {
        QPoint points[] = {
            QPoint(23, 15),
            QPoint(33, 25),
            QPoint(23, 35),
        };
        painter.drawPolyline(points, 3);
    }
}

void LatticeCircleButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit latticeCircleClicked();
}

bool LatticeCircleButton::event(QEvent *e)
{
    if (e->type() == QEvent::Enter) {
        opacityAnimation->setDirection(QPropertyAnimation::Forward);
        opacityAnimation->start();
        colorChangeAnimation->setDirection(QPropertyAnimation::Forward);
        colorChangeAnimation->start();
        emit animationStateChanged(AnimationState::ExecutingAnimation);
        update();
    } else if (e->type() == QEvent::Leave) {
        opacityAnimation->setDirection(QPropertyAnimation::Backward);
        opacityAnimation->start();
        colorChangeAnimation->setDirection(QPropertyAnimation::Backward);
        colorChangeAnimation->start();
        emit animationStateChanged(AnimationState::RestoringState);
        update();
    }
    return QPushButton::event(e);
}
