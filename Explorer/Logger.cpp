#include "Logger.h"

#include "FileManager.h"

#include <QDebug>

Logger::LogType Logger::s_type = LogType::File;

inline Logger* Logger::instance()
{
    ILogger* type_ = nullptr;
    if (s_type == LogType::File)
        type_ = new FileLogger;
    else if (s_type == LogType::Console)
        type_ = new ConsoleLogger;

    static Logger *logger = new Logger(type_);
    return logger;
}

inline void Logger::log(const QString& message, const LogLevel level)
{
    QMutexLocker locker(&m_mutex);
    m_logger->log(formatMessage(message, level));
}

QString Logger::formatMessage(const QString& message, const LogLevel level)
{
    QString level_msg;
    switch (level)
    {
    case LogLevel::Debug:
        level_msg = "[DGB]";
        break;
    case LogLevel::Info:
        level_msg = "[INF]";
        break;
    case LogLevel::Warning:
        level_msg = "[WRG]";
        break;
    case LogLevel::Error:
        level_msg = "[ERR]";
        break;
    default:
        break;
    }
    return QDateTime::currentDateTime().toString("[yyyy.MM.dd hh:mm:ss.zzz] ") + level_msg + " " + message;
}

inline void FileLogger::log(const QString& message)
{
    FileManager::writeStringToFile(m_fileName, message);
}

inline void ConsoleLogger::log(const QString& message)
{
    qDebug() << message;
}
