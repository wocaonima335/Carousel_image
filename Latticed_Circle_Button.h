#ifndef LATTICED_CIRCLE_BUTTON_H
#define LATTICED_CIRCLE_BUTTON_H
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QPushButton>

class LatticeCircleButton : public QPushButton
{
    Q_OBJECT
    // 属性声明使用驼峰命名法
    Q_PROPERTY(int opacity READ opacity WRITE setOpacity FINAL)
    Q_PROPERTY(QColor currentColor READ currentColor WRITE setCurrentColor FINAL)

public:
    explicit LatticeCircleButton(QWidget *parent = nullptr);

    // 使用更具描述性的枚举值
    enum AnimationState
    {
        ExecutingAnimation, // 动画执行中
        RestoringState      // 状态恢复中
    };

    // 动画指针使用功能描述命名
    QPropertyAnimation *expandAnimation;      // 展开动画
    QPropertyAnimation *colorChangeAnimation; // 颜色变换动画

    // 布尔状态变量添加 is/has 前缀
    bool isColorStateActive = false; // 颜色状态激活标志

    // 属性访问器统一命名规范
    int opacity() const;             // 获取透明度
    void setOpacity(int newOpacity); // 设置透明度

    QColor currentColor() const;                         // 获取当前颜色
    void setCurrentColor(const QColor &newCurrentColor); // 设置当前颜色

signals:
    // 信号命名使用动词描述动作
    void latticeCircleClicked(); // 格子圆按钮点击信号

    // 信号参数明确状态变更
    void animationStateChanged(LatticeCircleButton::AnimationState state);

protected:
    // 事件处理函数使用标准命名
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    bool event(QEvent *e) override;

private:
    // 私有成员变量保持 m_ 前缀
    int m_opacity = 0;                            // 透明度值(0-255)
    QColor m_currentColor = QColor(0, 0, 0, 255); // 当前显示颜色
};

#endif // LATTICED_CIRCLE_BUTTON_H
