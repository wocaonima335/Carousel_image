#include "Elliptical_background.h"

EllipseCornerCard::EllipseCornerCard(QWidget *parent)
{
    this->resize(1200, 606);
    boundaryRect = QRect(0, 0, 1200, 606);

    QPalette pal(this->palette());
    pal.setColor(QPalette::Window, QColor(243, 246, 253));
    this->setPalette(pal);

    leftControlButton = new LatticeCircleButton(this);
    rightControlButton = new LatticeCircleButton(this);

    leftControlButton->isColorStateActive = true;

    connect(leftControlButton,
            &LatticeCircleButton::latticeCircleClicked,
            this,
            &EllipseCornerCard::moveLeft);

    connect(rightControlButton,
            &LatticeCircleButton::latticeCircleClicked,
            this,
            &EllipseCornerCard::moveRight);

    this->loadImages();
    this->buildBaseData();
    this->arrangeImages();

    connect(&autoMoveTimer, &QTimer::timeout, this, &EllipseCornerCard::updateView);
    autoMoveTimer.setInterval(16);

    connect(&progressTimer, &QTimer::timeout, this, &EllipseCornerCard::updateProgressBarData);
    progressTimer.setInterval(16);
    progressTimer.start();

    this->setupAnimations();
}

void EllipseCornerCard::loadImages()
{
    qreal x = 0;
    for (int i = 1; i < 99; i++)
    {
        QPixmap pixmap(QString("://img/card_image%1.jpg").arg(i));
        if (pixmap.isNull())
            return;
        pixmapManager.append(pixmap);
    }
}

void EllipseCornerCard::buildBaseData()
{
    leftBoundary = boundaryRect.width() * 0.3750;
    upperEllipseCenter = QPointF(boundaryRect.width() / 2.0, 0);
    upperEllipseXRadius = boundaryRect.width() * 0.56;
    upperEllipseYRadius = boundaryRect.height() * 0.2215;

    lowerEllipseCenter = QPointF(boundaryRect.width() / 2.0, boundaryRect.height());
    lowerEllipseXRadius = boundaryRect.width() * 0.656;
    lowerEllipseYRadius = boundaryRect.height() * 0.3261;

    int buttonSize = boundaryRect.width() / 30;
    leftControlButton->resize(buttonSize, buttonSize);
    rightControlButton->resize(buttonSize, buttonSize);

    leftControlButton->move(boundaryRect.width() * 0.45, boundaryRect.height() * 0.8);
    rightControlButton->move(boundaryRect.width() * 0.50, boundaryRect.height() * 0.8);

    progressBarStep = qreal(boundaryRect.width()) / qreal(boundaryRect.height()) * 1.6;
    expandCollapseHeight = boundaryRect.height() * 0.98;

    boundaryCoordinates.clear();
    this->arrangeImages();
    this->setupAnimations();
}

void EllipseCornerCard::arrangeImages()
{
    qreal x = positionRatio * boundaryRect.width();

    for (int i = 0; i < pixmapManager.size(); i++)
    {
        if (i == 0)
            leftLimitMargin = x + -leftBoundary * 0.4444;
        else if (i == boundaryCoordinates.size() - 1)
            rightLimitMargin = x + leftBoundary * 0.3333;

        boundaryCoordinates.append(qMakePair(x, leftBoundary * 0.6666 + x));
        x += leftBoundary * 0.6888;
    }
    referenceSpacing = boundaryCoordinates.at(3).second - boundaryCoordinates.at(3).first;
}

void EllipseCornerCard::calculateDrawingArea(qreal &imageFrontX, qreal &imageBackX, QPixmap &picture)
{
    QPainter painter(this);

    painter.setRenderHints(QPainter::Antialiasing | QPainter::LosslessImageRendering);

    if (imageFrontX > boundaryRect.width() || imageBackX < 0)
    {
        return;
    }

    qreal width = imageBackX - imageFrontX;
    QRectF tempRect(imageFrontX, 0, width, boundaryRect.height());

    QPainterPath lowerEllipsePath;
    lowerEllipsePath.addEllipse(lowerEllipseCenter, lowerEllipseXRadius, lowerEllipseYRadius);

    QPainterPath upperEllipsePath;
    upperEllipsePath.addEllipse(upperEllipseCenter, upperEllipseXRadius, upperEllipseYRadius);

    // 合并椭圆区域并计算裁剪路径

    QPainterPath mergedEllipsePath = lowerEllipsePath.intersected(upperEllipsePath);
    QPainterPath entireAreaPath;
    entireAreaPath.addRect(boundaryRect);

    QPainterPath clipPath = entireAreaPath.subtracted(mergedEllipsePath);

    // 5. 计算实际绘制区域
    QPainterPath drawingAreaPath;
    drawingAreaPath.addRect(tempRect);
    QPainterPath finalClipPath = clipPath.intersected(drawingAreaPath);

    // 6. 设置裁剪路径并绘制图片
    painter.setClipPath(finalClipPath);

    qreal targetHeight = tempRect.height() * calculateContractionHeight(imageFrontX);
    qreal aspectRatio = picture.width() / qreal(picture.height());
    qreal targetWidth = targetHeight * aspectRatio;

    // 设置图像绘制矩形
    QRectF targetRect;
    targetRect.setSize(QSizeF(targetWidth, targetHeight));
    // 垂直位置在45%高度处
    targetRect.moveCenter(QPointF(tempRect.center().x(), tempRect.height() * 0.45));

    // 创建3D旋转变换
    QTransform transform;
    QPointF center = targetRect.center();
    transform.translate(center.x(), center.y());
    transform.rotate(calculateRectRotation(imageFrontX), Qt::YAxis, 2048);
    transform.translate(-center.x(), -center.y());

    // 应用变换
    painter.setWorldTransform(transform);
    painter.drawPixmap(targetRect, picture, picture.rect());
}

void EllipseCornerCard::drawText()
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setViewport(boundaryRect);
    painter.setWindow(0, 0, boundaryRect.width(), boundaryRect.height());

    QRect textRect(0, 0, boundaryRect.width(), boundaryRect.height());
    QFont font;
    font.setPixelSize(boundaryRect.width() / 35);
    font.setBold(true);
    font.setWordSpacing(4);
    font.setStyleHint(QFont::Decorative);
    painter.setFont(font);

    QColor semiTransparent(0, 0, 0, 255);
    painter.setPen(semiTransparent);

    QRect actualRect = painter.boundingRect(textRect, Qt::AlignCenter | Qt::TextWordWrap, "Hello Blibli");
    textRect.setHeight(actualRect.height());
    textRect.setWidth(actualRect.width());
    textRect.moveCenter(QPoint(width() / 2, height() / 10));
    painter.drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, "Hello Blibli");
}

void EllipseCornerCard::drawProgressBar()
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setViewport(boundaryRect);
    painter.setWindow(0, 0, boundaryRect.width(), boundaryRect.height());
    painter.setPen(Qt::NoPen);
    QBrush brush;
    brush.setColor(QColor(129, 249, 255, expandCollapseOpacity));
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    painter.drawRect(QRectF(progressBarX, expandCollapseHeight, progressBarWidth, boundaryRect.height() * 0.04));
}

qreal EllipseCornerCard::calculateRectRotation(qreal &xLeft)
{
    if (xLeft < leftBoundary)
    {
        qreal overflow = leftBoundary - xLeft;
        return -maxSecondaryRadians * (overflow / leftBoundary);
    }
    else if (xLeft > leftBoundary)
    {
        qreal overflow = xLeft - leftBoundary;
        return maxSecondaryRadians * (overflow / leftBoundary);
    }
    else
    {
        return 0;
    }
}

qreal EllipseCornerCard::calculateContractionHeight(qreal &xLeft)
{
    if (xLeft < leftBoundary)
    {
        qreal overflow = leftBoundary - xLeft;
        return overflow / leftBoundary / 4 + maxRotationRadians;
    }
    else if (xLeft > leftBoundary)
    {
        qreal overflow = xLeft - leftBoundary;
        return overflow / leftBoundary / 4 + maxRotationRadians;
    }
    else
    {
        return maxRotationRadians;
    }
}

void EllipseCornerCard::setupAnimations()
{
    buttonleftAnimation = new QPropertyAnimation(this->buttonleftAnimation, "geometry");
    buttonleftAnimation->setDuration(300);
    buttonleftAnimation->setStartValue(this->leftControlButton->geometry());
    buttonleftAnimation->setEndValue(QRect(this->leftControlButton->pos().x() - zoomRate / 2, this->leftControlButton->pos().y() - zoomRate / 2, leftControlButton->width() + zoomRate, leftControlButton->height() + zoomRate));
    buttonleftAnimation->setEasingCurve(QEasingCurve::Linear);

    buttonrightAnimation = new QPropertyAnimation(this->buttonrightAnimation, "geometry");
    buttonrightAnimation->setDuration(300);
    buttonrightAnimation->setStartValue(this->rightControlButton->geometry());
    buttonrightAnimation->setEndValue(QRect(this->rightControlButton->pos().x() - zoomRate / 2, this->rightControlButton->pos().y() - zoomRate / 2, rightControlButton->width() + zoomRate, rightControlButton->height() + zoomRate));
    buttonrightAnimation->setEasingCurve(QEasingCurve::Linear);

    connect(leftControlButton, &LatticeCircleButton::animationStateChanged, this, [this](LatticeCircleButton::AnimationState state)
            {
        if(state == LatticeCircleButton::ExecutingAnimation){
            buttonleftAnimation->setDirection(QAbstractAnimation::Forward);
            buttonleftAnimation->start(); 
        }else if(state == LatticeCircleButton::RestoringState){
            buttonleftAnimation->setDirection(QAbstractAnimation::Backward);
            buttonleftAnimation->start();
        } });

    connect(rightControlButton, &LatticeCircleButton::animationStateChanged, this, [this](LatticeCircleButton::AnimationState state)
            {
        if(state == LatticeCircleButton::ExecutingAnimation){
            buttonrightAnimation->setDirection(QAbstractAnimation::Forward);
            buttonrightAnimation->start(); 
        }else if(state == LatticeCircleButton::RestoringState){
            buttonrightAnimation->setDirection(QAbstractAnimation::Backward);
            buttonrightAnimation->start();
        } });

    expansionAnimation = new QPropertyAnimation(this, "expandCollapseHeight");
    expansionAnimation->setDuration(360);
    expansionAnimation->setStartValue(expandCollapseHeight);
    expansionAnimation->setEndValue(boundaryRect.height());
    expansionAnimation->setEasingCurve(QEasingCurve::Linear);

    expansionAnimation = new QPropertyAnimation(this, "expandCollapseOpacity");
    opacityAnimation->setDuration(360);
    opacityAnimation->setStartValue(expandCollapseOpacity);
    opacityAnimation->setEndValue(0);
}

void EllipseCornerCard::moveCardLeft()
{
    isActiveState = false;
    autoMoveTimer.start();
    dynamicMovementControl = 0;
    isActiveState = false;
}

void EllipseCornerCard::moveCardRight()
{
    isActiveState = false;
    autoMoveTimer.start();
    dynamicMovementControl = 0;
    isActiveState = false;
}

qreal EllipseCornerCard::progress_BarX() const
{
    return progressBarX;
}

void EllipseCornerCard::setProgressBarX(qreal newProgressBarX)
{
    progressBarX = newProgressBarX;
}

qreal EllipseCornerCard::progress_BarWidth() const
{
    return progressBarWidth;
}

void EllipseCornerCard::setProgressBarWidth(qreal newProgressBarWidth)
{
    progressBarWidth = newProgressBarWidth;
}

qreal EllipseCornerCard::expand_CollapseHeight() const
{
    return expandCollapseHeight;
}

void EllipseCornerCard::setExpandCollapseHeight(qreal newExpandCollapseHeight)
{
    expandCollapseHeight = newExpandCollapseHeight;
    update();
}

int EllipseCornerCard::expand_CollapseOpacity() const
{
    return expandCollapseOpacity;
}

void EllipseCornerCard::setExpandCollapseOpacity(int newExpandCollapseOpacity)
{
    expandCollapseOpacity = newExpandCollapseOpacity;
    update();
}

void EllipseCornerCard::updateView()
{
    for (auto &pair : boundaryCoordinates)
    {
        if (isReverseDirection == false)
        {
            pair.first += timeStep;
            pair.second += timeStep;
        }
        else
        {
            pair.first -= timeStep;
            pair.second -= timeStep;
        }
    }

    if (boundaryCoordinates[0].first < leftLimitMargin)
    {
        boundaryCoordinates.append(boundaryCoordinates.takeFirst());
        pixmapManager.append(pixmapManager.takeFirst());
        boundaryCoordinates[boundaryCoordinates.size() - 1].first = boundaryCoordinates[boundaryCoordinates.size() - 2].first + leftBoundary * 0.6888;
        boundaryCoordinates[boundaryCoordinates.size() - 1].second = boundaryCoordinates[boundaryCoordinates.size() - 2].second + leftBoundary * 0.6888;
    }
    else if (boundaryCoordinates[boundaryCoordinates.size() - 1].first > rightLimitMargin)
    {
        boundaryCoordinates.insert(boundaryCoordinates.begin(), boundaryCoordinates.takeLast());
        pixmapManager.insert(pixmapManager.begin(), pixmapManager.takeLast());
        boundaryCoordinates[0].first = boundaryCoordinates[1].first - leftBoundary * 0.6888;
        boundaryCoordinates[0].second = boundaryCoordinates[1].second - leftBoundary * 0.6888;
    }

    if (isMousePressed == true)
    {
        autoMoveTimer.stop();
        positionRatio = boundaryCoordinates[0].first / boundaryRect.width();
        update();
        return;
    }

    dynamicMovementControl += timeStep;
    if (dynamicMovementControl >= leftBoundary * 0.6888)
    {
        autoMoveTimer.stop();
        isActiveState = true;
        positionRatio = boundaryCoordinates[0].first / boundaryRect.width();
    }
    update();
}

void EllipseCornerCard::updateProgressBarData()
{

    if (isWidthAdjustmentActive)
    {
        progressBarWidth += progressBarStep;
        progressBarX = 0;
        if (progressBarWidth >= boundaryRect.width())
        {
            isWidthAdjustmentActive = false;
            isPositionAdjustmentActive = true;
            this->moveCardRight();
        }
        else if (progressBarX > 0)
        {
            progressBarX -= progressBarStep;
            if (progressBarX <= 0)
            {
                progressBarX = 0;
                this -> moveCardRight();
            }
        }
        else{
            
        }
    }
}
