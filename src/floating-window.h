#ifndef FLOATING_WINDOW_H
#define FLOATING_WINDOW_H

#include <QWidget>
#include <QTimer>
#include <QMouseEvent>

namespace Ui {
class FloatingWindow;
}

class FloatingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FloatingWindow(QWidget *parent = nullptr);
    ~FloatingWindow();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void updateRecordingStatus();
    void onRecordButtonClicked();
    void onPauseButtonClicked();
    void onStopButtonClicked();

private:
    Ui::FloatingWindow *ui;
    QTimer *statusTimer;
    QPoint dragPosition;
    bool isDragging;
    
    void updateUI();
    QString formatTime(int seconds);
};

#endif // FLOATING_WINDOW_H