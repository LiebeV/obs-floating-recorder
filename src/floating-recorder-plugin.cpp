#include <obs-module.h>
#include <obs-frontend-api.h>
#include <QMainWindow>
#include <QAction>
#include <QTimer>

// 简化版本，不包含完整的UI
OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-floating-recorder", "en-US")

bool obs_module_load(void)
{
    blog(LOG_INFO, "Floating Recorder Plugin loaded!");
    return true;
}

void obs_module_unload(void)
{
    blog(LOG_INFO, "Floating Recorder Plugin unloaded!");
}

const char *obs_module_name(void)
{
    return "OBS Floating Recorder Controller";
}

const char *obs_module_description(void)
{
    return "Global floating window for recording control";
}