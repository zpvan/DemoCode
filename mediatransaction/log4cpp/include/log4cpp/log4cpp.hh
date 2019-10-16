#ifndef LOG4CPP_HH_
#define LOG4CPP_HH_

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/Priority.hh>

#define LL_EMERG        log4cpp::Priority::EMERG
#define LL_FATAL        log4cpp::Priority::FATAL
#define LL_ALERT        log4cpp::Priority::ALERT
#define LL_CRIT         log4cpp::Priority::CRIT
#define LL_ERROR        log4cpp::Priority::ERROR
#define LL_WARN         log4cpp::Priority::WARN
#define LL_NOTICE       log4cpp::Priority::NOTICE
#define LL_INFO         log4cpp::Priority::INFO
#define LL_DEBUG        log4cpp::Priority::DEBUG
#define LL_NOTSET       log4cpp::Priority::NOTSET

#define CATEGORY                "CATEGORY"

#define MYLog_Init(/*const char* */filename) do \
{ \
    log4cpp::PropertyConfigurator::configure(std::string(filename)); \
} while (0)

#define MYLog_Log(priority, fmt, ...) do \
{ \
    log4cpp::Category& refCategory = log4cpp::Category::getInstance(std::string(CATEGORY)); \
    refCategory.log(priority, fmt" --[%s:%u]", ##__VA_ARGS__, __FILE__, __LINE__); \
} while (0)

#define MYLog_Shutdown() do \
{ \
    log4cpp::Category::shutdown(); \
} while (0)

#endif /* LOG4CPP_HH_ */
