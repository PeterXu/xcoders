#ifndef _LOGTRACE_H_
#define _LOGTRACE_H_

#ifdef ANDROID
#include <android/log.h>

#define IVYTAG "IvyMirageNative"

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, IVYTAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, IVYTAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, IVYTAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, IVYTAG, __VA_ARGS__))
#else
#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#endif

#endif
