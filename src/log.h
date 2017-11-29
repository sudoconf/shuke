//
// Created by Yu Yang <yyangplus@NOSPAM.gmail.com> on 2017-05-11
//

#ifndef _LOG_H_
#define _LOG_H_ 1

#include <time.h>
#include <sys/time.h>

#include <rte_log.h>

#ifndef SK_LOG_DP_LEVEL
#define SK_LOG_DP_LEVEL RTE_LOG_DEBUG
#endif

struct logModuleCfg{
    uint32_t logtype;
    char* ty_name;
};

#define DEF_LOG_MODULE(ty, ty_name)                       \
    static struct logModuleCfg  __log_cfg = {ty, ty_name}

#define MODULE_LOGTYPE  __log_cfg.logtype

static inline int __rte_log(uint32_t level, uint32_t logtype, const char *tstr, const char *lstr, const char *fmt, ...) {
    if ((level > rte_logs.level) || !(logtype & rte_logs.type))
        return 0;

    char format[1024];
    int ret;
    size_t off;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    off = strftime(format, 1024, "%Y/%m/%d %H:%M:%S.",localtime(&tv.tv_sec));
    snprintf(format+off, 1024-off,"%03d %s %s%s\n", (int)tv.tv_usec/1000, tstr, lstr, fmt);

    va_list ap;
    va_start(ap, fmt);

    ret = rte_vlog(level, logtype, format, ap);

    va_end(ap);
    return ret;
}

#define LOG_DEBUG(...)                                                  \
    (void)((RTE_LOG_DEBUG <= SK_LOG_DP_LEVEL)?                          \
           __rte_log(RTE_LOG_DEBUG, __log_cfg.logtype, __log_cfg.ty_name, "[debug]: ", ##__VA_ARGS__): \
           0)

#define LOG_INFO(...)                                                   \
    __rte_log(RTE_LOG_INFO, __log_cfg.logtype, __log_cfg.ty_name, "[info]: ", ##__VA_ARGS__)

#define LOG_NOTICE(...)                                                 \
    __rte_log(RTE_LOG_NOTICE, __log_cfg.logtype, __log_cfg.ty_name, "[notice]: ", ##__VA_ARGS__)


#define LOG_WARN(...)                                                   \
    __rte_log(RTE_LOG_WARNING, __log_cfg.logtype, __log_cfg.ty_name, "[warn]: ", ##__VA_ARGS__)

#define LOG_WARNING  LOG_WARN

#define LOG_ERR(...)                                                    \
    __rte_log(RTE_LOG_ERR, __log_cfg.logtype, __log_cfg.ty_name, "[err]: ", ##__VA_ARGS__)

#define LOG_ERROR LOG_ERR

#define LOG_FATAL(...)                                                  \
    __rte_log(RTE_LOG_ERR, __log_cfg.logtype, __log_cfg.ty_name, "[err]: ", ##__VA_ARGS__); abort()

#define LOG_EXIT(...)                                                   \
    __rte_log(RTE_LOG_ERR, __log_cfg.logtype, __log_cfg.ty_name, "[err]: ", ##__VA_ARGS__); exit(EXIT_FAILURE)

#define LOG_RAW(l, ...)                                     \
    rte_log(RTE_LOG_ ## l, __log_cfg.logtype, __VA_ARGS__)

static inline uint32_t
str2loglevel(char *ss) {
    uint32_t level;
    if (strcasecmp(ss, "debug") == 0) {
        level = RTE_LOG_DEBUG;
    } else if (strcasecmp(ss, "info") == 0) {
        level = RTE_LOG_INFO;
    } else if (strcasecmp(ss, "notice") == 0) {
        level = RTE_LOG_NOTICE;
    } else if (strcasecmp(ss, "warn") == 0) {
        level = RTE_LOG_WARNING;
    } else if (strcasecmp(ss, "error") == 0) {
        level = RTE_LOG_ERR;
    } else if (strcasecmp(ss, "critical") == 0) {
        level = RTE_LOG_CRIT;
    } else {
        rte_exit(EXIT_FAILURE, "unkown log level %s\n", ss);
    }
    return level;
}

#endif /* _LOG_H_ */
