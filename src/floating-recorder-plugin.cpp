#include <obs-module.h>
#include <obs-frontend-api.h>
#include <QMainWindow>
#include <QAction>
#include <QTimer>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

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
        
        layout->addWidget(statusLabel);
        layout->addWidget(recordButton);
        
        setLayout(layout);
        resize(150, 80);
        
        // 设置样式
        setStyleSheet(
            "SimpleFloatingWindow {"
            "    background-color: rgba(40, 40, 40, 200);"
            "    border: 1px solid #555;"
            "    border-radius: 8px;"
            "    color: white;"
            "}"
            "QLabel { color: white; padding: 5px; }"
            "QPushButton {"
            "    background-color: #555;"
            "    border: none;"
            "    border-radius: 4px;"
            "    color: white;"
            "    padding: 5px;"
            "}"
            "QPushButton:hover { background-color: #666; }"
        );
        
        // 连接信号
        connect(recordButton, &QPushButton::clicked, this, &SimpleFloatingWindow::toggleRecording);
        
        // 更新状态定时器
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &SimpleFloatingWindow::updateStatus);
        timer->start(1000);
        
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
    
    void updateStatus()
    {
        if (obs_frontend_recording_active()) {
            statusLabel->setText("录制中...");
            recordButton->setText("停止录制");
        } else {
            statusLabel->setText("未录制");
            recordButton->setText("开始录制");
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
            floatingWindow->setVisible(!floatingWindow->isVisible());
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

#include "floating-recorder-plugin.moc"
