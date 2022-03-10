#ifndef LOGGER_H
#define LOGGER_H
void logger(const char* msg, const char* level);
void logger(String msg, const char* level);
#endif
#ifndef LOG(msg, level)
#define LOG(msg, level)(logger(msg, level))
#endif
