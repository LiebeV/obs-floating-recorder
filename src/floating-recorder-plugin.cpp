#include <obs-module.h>
#include <obs-frontend-api.h>
#include <QMainWindow>
#include <QAction>
#include <QTimer>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QMenu>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-floating-recorder", "en-US")

class SimpleFloatingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SimpleFloatingWindow(QWidget *parent = nullptr) : QWidget(parent)
    {
        // 设置窗口属性
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
        setAttribute(Qt::WA_TranslucentBackground);
        
        // 创建简单UI
        auto *layout = new QVBoxLayout(this);
        statusLabel = new QLabel("OBS控制面板", this);
        recordButton = new QPushButton("开始录制", this);
        pauseButton = new QPushButton("暂停", this);
        stopButton = new QPushButton("停止", this);
        
        layout->addWidget(statusLabel);
        layout->addWidget(recordButton);
        layout->addWidget(pauseButton);
        layout->addWidget(stopButton);
        
        setLayout(layout);
        resize(150, 120);
        
        // 设置样式
        setStyleSheet(
            "QWidget {"
            "    background-color: rgba(40, 40, 40, 200);"
            "    border: 1px solid #555;"
            "    border-radius: 8px;"
            "    color: white;"
            "}"
            "QLabel { "
            "    color: white; "
            "    padding: 5px; "
            "    background-color: transparent;"
            "    font-size: 11px;"
            "}"
            "QPushButton {"
            "    background-color: #555;"
            "    border: none;"
            "    border-radius: 4px;"
            "    color: white;"
            "    padding: 5px;"
            "    margin: 1px;"
            "    font-size: 10px;"
            "}"
            "QPushButton:hover { background-color: #666; }"
            "QPushButton:pressed { background-color: #777; }"
            "QPushButton:disabled { background-color: #333; color: #777; }"
        );
        
        // 连接信号
        connect(recordButton, &QPushButton::clicked, this, &SimpleFloatingWindow::toggleRecording);
        connect(pauseButton, &QPushButton::clicked, this, &SimpleFloatingWindow::togglePause);
        connect(stopButton, &QPushButton::clicked, this, &SimpleFloatingWindow::stopRecording);
        
        // 更新状态定时器
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &SimpleFloatingWindow::updateStatus);
        timer->start(1000);
        
        // 添加上下文菜单
        setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, &QWidget::customContextMenuRequested, [this](const QPoint &pos) {
            QMenu menu(this);
            QAction *closeAction = menu.addAction("关闭");
            connect(closeAction, &QAction::triggered, this, &QWidget::close);
            menu.exec(mapToGlobal(pos));
        });
        
        updateStatus();
    }

private slots:
    void toggleRecording()
    {
        if (obs_frontend_recording_active()) {
            obs_frontend_recording_stop();
        } else {
            obs_frontend_recording_start();
        }
        updateStatus();
    }
    
    void togglePause()
    {
        if (obs_frontend_recording_active()) {
            bool paused = obs_frontend_recording_paused();
            obs_frontend_recording_pause(!paused);
        }
        updateStatus();
    }
    
    void stopRecording()
    {
        if (obs_frontend_recording_active()) {
            obs_frontend_recording_stop();
        }
        updateStatus();
    }
    
    void updateStatus()
    {
        bool isRecording = obs_frontend_recording_active();
        bool isPaused = obs_frontend_recording_paused();
        
        if (!isRecording) {
            statusLabel->setText("未录制");
            statusLabel->setStyleSheet("color: gray;");
            recordButton->setText("开始录制");
            pauseButton->setEnabled(false);
            stopButton->setEnabled(false);
        } else if (isPaused) {
            statusLabel->setText("已暂停");
            statusLabel->setStyleSheet("color: orange;");
            recordButton->setText("继续录制");
            pauseButton->setText("继续");
            pauseButton->setEnabled(true);
            stopButton->setEnabled(true);
        } else {
            statusLabel->setText("录制中");
            statusLabel->setStyleSheet("color: #ff4444;");
            recordButton->setText("停止录制");
            pauseButton->setText("暂停");
            pauseButton->setEnabled(true);
            stopButton->setEnabled(true);
        }
    }

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            dragPosition = event->globalPos() - frameGeometry().topLeft();
            event->accept();
        }
    }
    
    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (event->buttons() & Qt::LeftButton) {
            move(event->globalPos() - dragPosition);
            event->accept();
        }
    }

private:
    QLabel *statusLabel;
    QPushButton *recordButton;
    QPushButton *pauseButton;
    QPushButton *stopButton;
    QPoint dragPosition;
};

SimpleFloatingWindow *floatingWindow = nullptr;

bool obs_module_load(void)
{
    blog(LOG_INFO, "Loading Floating Recorder Plugin");
    
    QMainWindow *mainWindow = (QMainWindow*)obs_frontend_get_main_window();
    
    // 创建悬浮窗
    floatingWindow = new SimpleFloatingWindow(mainWindow);
    floatingWindow->move(100, 100);
    floatingWindow->show();
    
    // 添加到OBS菜单
    QAction *toggleAction = (QAction*)obs_frontend_add_tools_menu_qaction("浮动录制控制器");
    
    QObject::connect(toggleAction, &QAction::triggered, []() {
        if (floatingWindow) {
            bool visible = floatingWindow->isVisible();
            if (visible) {
                floatingWindow->hide();
            } else {
                floatingWindow->show();
                floatingWindow->raise();
            }
        }
    });
    
    return true;
}

void obs_module_unload(void)
{
    blog(LOG_INFO, "Unloading Floating Recorder Plugin");
    if (floatingWindow) {
        delete floatingWindow;
        floatingWindow = nullptr;
    }
}

const char *obs_module_name(void)
{
    return "OBS Floating Recorder Controller";
}

const char *obs_module_description(void)
{
    return "全局置顶的录制控制悬浮窗";
}

// 添加moc包含
#include "floating-recorder-plugin.moc"
