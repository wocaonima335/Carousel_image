#include "Elliptical_background.h"

EllipseCornerCard::EllipseCornerCard(QWidget *parent)
{
    this->resize(1200, 606);
    boundaryRect = QRect(0, 0, 1200, 606);

    QPalette pal(this->palette());
    pal.setColor(QPalette::Window, QColor(243, 246, 253));
    this->setPalette(pal);

    this->loadImages();
    this->buildBaseData();

    connect(&autoMoveTimer, &QTimer::timeout, this, &EllipseCornerCard::updateView);
    autoMoveTimer.setInterval(16);

    connect(&progressTimer, &QTimer::timeout, this, &EllipseCornerCard::updateProgressBarData);
    progressTimer.setInterval(16);
    progressTimer.start();

    this->setupAnimations();
}

void EllipseCornerCard::loadImages()
{
    QString basePath = "E:/AnimationImage/card_image%1.jpg";
    QDir dir("E:/AnimationImage");

    if (!dir.exists()) {
        qCritical() << "错误：图片目录不存在 - " << dir.path();
        return;
    }

    for (int i = 1; i < 7; i++) {
        QString filePath = basePath.arg(i);

        if (!QFile::exists(filePath)) {
            qWarning() << "图片文件不存在:" << filePath;
            continue; // 跳过缺失文件继续加载
            // 或者 return; 根据需求选择
        }

        QPixmap pixmap(filePath);

        if (pixmap.isNull()) {
            qWarning() << "加载失败（可能损坏）:" << filePath;
            continue;
        }

        pixmapManager.append(pixmap);
    }

    qDebug() << "成功加载图片数量:" << pixmapManager.size();
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

    progressBarStep = qreal(boundaryRect.width()) / qreal(boundaryRect.height()) * 1.6;
    expandCollapseHeight = boundaryRect.height() * 0.98;

    boundaryCoordinates.clear();
    this->arrangeImages();
    this->setupAnimations();
}

void EllipseCornerCard::arrangeImages()
{
    qreal x = positionRatio * boundaryRect.width();

    for (int i = 0; i < pixmapManager.size(); i++) {
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

    if (imageFrontX > boundaryRect.width() || imageBackX < 0) {
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
    transform.rotate(calculateRectRotation(imageFrontX), Qt::YAxis, 2048.0);
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

    QRect actualRect = painter.boundingRect(textRect,
                                            Qt::AlignCenter | Qt::TextWordWrap,
                                            "Hello Blibli");
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
    painter.drawRect(
        QRectF(progressBarX, expandCollapseHeight, progressBarWidth, boundaryRect.height() * 0.04));
}

qreal EllipseCornerCard::calculateRectRotation(qreal &xLeft)
{
    if (xLeft < leftBoundary) {
        qreal overflow = leftBoundary - xLeft;
        return -maxSecondaryRadians * (overflow / leftBoundary);
    } else if (xLeft > leftBoundary) {
        qreal overflow = xLeft - leftBoundary;
        return maxSecondaryRadians * (overflow / leftBoundary);
    } else {
        return 0;
    }
}

qreal EllipseCornerCard::calculateContractionHeight(qreal &xLeft)
{
    if (xLeft < leftBoundary) {
        qreal overflow = leftBoundary - xLeft;
        return overflow / leftBoundary / 4 + maxRotationRadians;
    } else if (xLeft > leftBoundary) {
        qreal overflow = xLeft - leftBoundary;
        return overflow / leftBoundary / 4 + maxRotationRadians;
    } else {
        return maxRotationRadians;
    }
}

void EllipseCornerCard::setupAnimations() {}

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

void EllipseCornerCard::moveLeft()
{
    if (isActiveState != true)
        return;
    this->moveCardLeft();
}

void EllipseCornerCard::moveRight()
{
    if (isActiveState != true)
        return;
    this->moveCardRight();
}

void EllipseCornerCard::updateView()
{
    for (auto &pair : boundaryCoordinates) {
        if (isReverseDirection == false) {
            pair.first += timeStep;
            pair.second += timeStep;
        } else {
            pair.first -= timeStep;
            pair.second -= timeStep;
        }
    }

    if (boundaryCoordinates[0].first < leftLimitMargin) {
        boundaryCoordinates.append(boundaryCoordinates.takeFirst());
        pixmapManager.append(pixmapManager.takeFirst());
        boundaryCoordinates[boundaryCoordinates.size() - 1].first
            = boundaryCoordinates[boundaryCoordinates.size() - 2].first + leftBoundary * 0.6888;
        boundaryCoordinates[boundaryCoordinates.size() - 1].second
            = boundaryCoordinates[boundaryCoordinates.size() - 2].second + leftBoundary * 0.6888;

    } else if (boundaryCoordinates[boundaryCoordinates.size() - 1].first > rightLimitMargin) {
        boundaryCoordinates.prepend(boundaryCoordinates.takeLast());
        pixmapManager.prepend(pixmapManager.takeLast());
        boundaryCoordinates[0].first = boundaryCoordinates[1].first - leftBoundary * 0.6888;
        boundaryCoordinates[0].second = boundaryCoordinates[1].second - leftBoundary * 0.6888;
    }

    if (isMousePressed == true) {
        autoMoveTimer.stop();
        positionRatio = boundaryCoordinates[0].first / boundaryRect.width();
        update();
        return;
    }

    dynamicMovementControl += timeStep;
    if (dynamicMovementControl >= leftBoundary * 0.6888) {
        autoMoveTimer.stop();
        isActiveState = true;
        positionRatio = boundaryCoordinates[0].first / boundaryRect.width();
    }
    update();
}

void EllipseCornerCard::updateProgressBarData()
{
    if (isWidthAdjustmentActive) {
        progressBarWidth += progressBarStep;
        progressBarX = 0;
        if (progressBarWidth >= boundaryRect.width()) {
            isWidthAdjustmentActive = false;
            isPositionAdjustmentActive = true;
            this->moveCardRight();
        }
    } else if (isPositionAdjustmentActive) {
        progressBarX += progressBarStep;
        progressBarWidth -= progressBarStep;

        if (progressBarX >= boundaryRect.width()) {
            progressBarWidth = boundaryRect.width();
            isPositionAdjustmentActive = false;
            this->moveCardRight();
        }
    } else if (progressBarX > 0) {
        progressBarX -= progressBarStep;
        if (progressBarX <= 0) {
            progressBarX = 0;
            this->moveCardRight();
        }
    } else {
        progressBarWidth -= progressBarStep;
        if (progressBarWidth <= 0) {
            progressBarWidth = 0;
            isWidthAdjustmentActive = true;
            this->moveCardRight();
        }
    }

    update();
}

void EllipseCornerCard::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::LosslessImageRendering);

    for (int i = 0; i < boundaryCoordinates.size(); i++) {
        calculateDrawingArea(boundaryCoordinates[i].first,
                             boundaryCoordinates[i].second,
                             pixmapManager[i]);
    }

    this->drawText();
    this->drawProgressBar();
}
