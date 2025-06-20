#ifndef ELLIPTICAL_BACKGROUND_H
#define ELLIPTICAL_BACKGROUND_H

#include "Latticed_Circle_Button.h"

#include <QDir>
#include <QFile>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QTimer>
#include <QVector>
#include <QWidget>

class EllipseCornerCard : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal expandCollapseHeight READ expand_CollapseHeight WRITE setExpandCollapseHeight)
    Q_PROPERTY(int expandCollapseOpacity READ expand_CollapseOpacity WRITE setExpandCollapseOpacity)

public:
    explicit EllipseCornerCard(QWidget *parent = nullptr); // 椭圆角卡片控件
    void loadImages();                                     // 加载图像资源
    void buildBaseData();                                  // 构建基础数据
    void arrangeImages();                                  // 排列图像布局
    void calculateDrawingArea(qreal &imageFrontX,
                              qreal &imageBackX,
                              QPixmap &picture);    // 计算绘制区域
    void drawText();                                // 绘制文本内容
    void drawProgressBar();                         // 绘制进度条
    qreal calculateRectRotation(qreal &xLeft);      // 计算矩形旋转角度
    qreal calculateContractionHeight(qreal &xLeft); // 计算收缩高度
    void setupAnimations();                         // 初始化动画系统

    // 动画控制对象
    QPropertyAnimation *buttonleftAnimation; 
    QPropertyAnimation *opacityAnimation;     // 透明度动画
    QPropertyAnimation *expansionAnimation;   // 展开动画
    QPropertyAnimation *buttonrightAnimation; 
    int zoomRate = 10;                        // 缩放比例

    // 交互按钮
    LatticeCircleButton *leftControlButton;  // 左侧控制按钮
    LatticeCircleButton *rightControlButton; // 右侧控制按钮

    // 定时控制器
    QTimer autoMoveTimer; // 自动移动定时器
    QTimer progressTimer; // 动画控制定时器

    // 卡片行为控制
    void moveCardLeft();  // 卡片左移行为
    void moveCardRight(); // 卡片右移行为

public:
    qreal progress_BarX() const;                 // 获取进度条X位置
    void setProgressBarX(qreal newProgressBarX); // 设置进度条X位置

    qreal progress_BarWidth() const;                     // 获取进度条宽度
    void setProgressBarWidth(qreal newProgressBarWidth); // 设置进度条宽度

    qreal expand_CollapseHeight() const;                         // 获取展开/折叠高度
    void setExpandCollapseHeight(qreal newExpandCollapseHeight); // 设置展开/折叠高度

    int expand_CollapseOpacity() const;                          // 获取展开/折叠透明度
    void setExpandCollapseOpacity(int newExpandCollapseOpacity); // 设置展开/折叠透明度

public slots:
    // 导航控制
    void moveLeft();
    void moveRight();

    // 视图更新
    void updateView();

    // 进度条控制
    void updateProgressBarData();

protected:
    void paintEvent(QPaintEvent *event) override;
    // void resizeEvent(QResizeEvent *event);
    // void mousePressEvent(QMouseEvent *event);
    // void mouseMoveEvent(QMouseEvent *event);
    // void mouseReleaseEvent(QMouseEvent *event);
    // bool event(QEvent *e) override;
    // void wheelEvent(QWheelEvent *event);

private:
    // 图像资源管理器 (存储多个像素图资源)
    QVector<QPixmap> pixmapManager; // 修正拼写: pximapManager → pixmapManager

    // 边界坐标集合 (存储成对的x,y边界坐标)
    QVector<QPair<qreal, qreal>> boundaryCoordinates; // 蛇形命名 → 驼峰命名

    // 布局参数
    qreal referenceSpacing;          // 静态基准间距值（不可变参考）
    qreal dynamicMovementControl;    // 动态运动控制参数（运行时调整）
    qreal leftLimitMargin;           // 左侧边界容限 (蛇形命名 → 驼峰命名)
    qreal rightLimitMargin;          // 右侧边界容限 (蛇形命名 → 驼峰命名)
    qreal substituteRequiredSpacing; // 替代所需间距 (修正拼写: subtitue → substitute)

    // 状态控制标志
    bool isActiveState = true;       // 当前激活状态 (更清晰的命名)
    bool isReverseDirection = false; // 执行方向标志 (false=正向, true=反向)

    // 几何参数
    QRect boundaryRect;               // 边界矩形区域 (更清晰的命名)
    qreal maxRotationRadians = 0.5;   // 最大旋转弧度值 (添加单位说明)
    qreal maxSecondaryRadians = 28.0; // 辅助系统最大弧度值 (更明确的命名)

    // 位置参数
    qreal leftBoundary = 0.0;     // 左边界位置 (更清晰的命名)
    qreal positionRatio = -0.375; // 位置比例系数 (更精确的命名)

    // 时间参数
    qreal timeStep = 15.000; // 时间步长(毫秒) (添加单位说明)

    // 椭圆参数 (上椭圆)
    QPointF upperEllipseCenter; // 上椭圆中心点
    double upperEllipseYRadius; // 上椭圆Y轴半径
    double upperEllipseXRadius; // 上椭圆X轴半径

    // 椭圆参数 (下椭圆)
    QPointF lowerEllipseCenter; // 下椭圆中心点
    double lowerEllipseYRadius; // 下椭圆Y轴半径
    double lowerEllipseXRadius; // 下椭圆X轴半径

    // 鼠标交互状态
    bool isMousePressed = false;     // 鼠标按下状态标志
    int mousePressStartX = 0;        // 鼠标按下起始X坐标 (更清晰的命名)
    int dragAccumulatedDistance = 0; // 鼠标拖动累计距离 (更精确的命名)

    // 进度条控制参数
    qreal progressBarX = 0;                  // 进度条X位置 (移除冗余前缀)
    qreal progressBarWidth = 0;              // 进度条宽度 (移除冗余前缀)
    qreal progressBarStep = 1;               // 进度条步进值
    bool isWidthAdjustmentActive = true;     // 宽度调整激活标志 (更清晰的命名)
    bool isPositionAdjustmentActive = false; // 位置调整激活标志 (更清晰的命名)

    // 展开/折叠动画参数
    qreal expandCollapseHeight;      // 展开/折叠高度值
    int expandCollapseOpacity = 255; // 展开/折叠透明度(0-255) (添加范围说明)
};

#endif // ELLIPTICAL_BACKGROUND_H
