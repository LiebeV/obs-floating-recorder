#include "floating-window.h"
#include "ui_floating-window.h"
#include <obs-frontend-api.h>
#include <obs.h>
#include <QMenu>
#include <QAction>

FloatingWindow::FloatingWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FloatingWindow),
    isDragging(false)
{
    ui->setupUi(this);
    
    // 设置窗口属性
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    
    // 创建状态更新定时器
    statusTimer = new QTimer(this);
    connect(statusTimer, &QTimer::timeout, this, &FloatingWindow::updateRecordingStatus);
    statusTimer->start(1000); // 每秒更新一次
    
    // 连接按钮信号
    connect(ui->recordButton, &QPushButton::clicked, this, &FloatingWindow::onRecordButtonClicked);
    connect(ui->pauseButton, &QPushButton::clicked, this, &FloatingWindow::onPauseButtonClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &FloatingWindow::onStopButtonClicked);
    
    // 添加上下文菜单
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, [this](const QPoint &pos) {
        QMenu menu(this);
        QAction *closeAction = menu.addAction("关闭");
        connect(closeAction, &QAction::triggered, this, &QWidget::close);
        menu.exec(mapToGlobal(pos));
    });
    
    updateRecordingStatus();
}

FloatingWindow::~FloatingWindow()
{
    delete ui;
}

void FloatingWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void FloatingWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void FloatingWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        event->accept();
    }
}

void FloatingWindow::updateRecordingStatus()
{
    updateUI();
}

void FloatingWindow::onRecordButtonClicked()
{
    if (obs_frontend_recording_active()) {
        obs_frontend_recording_pause(true);
    } else {
        obs_frontend_recording_start();
    }
}

void FloatingWindow::onPauseButtonClicked()
{
    if (obs_frontend_recording_active()) {
        bool paused = obs_frontend_recording_paused();
        obs_frontend_recording_pause(!paused);
    }
}

void FloatingWindow::onStopButtonClicked()
{
    if (obs_frontend_recording_active()) {
        obs_frontend_recording_stop();
    }
}

void FloatingWindow::updateUI()
{
    bool isRecording = obs_frontend_recording_active();
    bool isPaused = obs_frontend_recording_paused();
    
    // 更新状态标签
    if (!isRecording) {
        ui->statusLabel->setText("未录制");
        ui->statusLabel->setStyleSheet("color: gray;");
    } else if (isPaused) {
        ui->statusLabel->setText("已暂停");
        ui->statusLabel->setStyleSheet("color: orange;");
    } else {
        ui->statusLabel->setText("录制中");
        ui->statusLabel->setStyleSheet("color: red;");
    }
    
    // 更新按钮状态
    ui->recordButton->setEnabled(true);
    ui->pauseButton->setEnabled(isRecording);
    ui->stopButton->setEnabled(isRecording);
    
    // 更新录制/暂停按钮文本
    if (isRecording) {
        ui->recordButton->setText("暂停录制");
        ui->pauseButton->setText(isPaused ? "继续" : "暂停");
    } else {
        ui->recordButton->setText("开始录制");
        ui->pauseButton->setText("暂停");
    }
    
    // 更新录制时间
    if (isRecording) {
        // 这里需要获取录制时间，可能需要额外的处理
        ui->timeLabel->setText("录制中...");
    } else {
        ui->timeLabel->setText("00:00:00");
    }
}

QString FloatingWindow::formatTime(int seconds)
{
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;
    return QString("%1:%2:%3")
        .arg(hours, 2, 10, QLatin1Char('0'))
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(secs, 2, 10, QLatin1Char('0'));
}