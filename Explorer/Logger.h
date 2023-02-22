#pragma once

#include <QString>
#include <QMutex>

class ILogger
{
public:
    virtual ~ILogger() {}
    virtual void log(const QString& message) = 0;
};

class ConsoleLogger : public ILogger
{
public:
    virtual void log(const QString& message) override;
};

class FileLogger : public ILogger
{
public:
    FileLogger(const QString& fileName = QStringLiteral("log.log"))
        : m_fileName(fileName)
    {
    }

    virtual void log(const QString& message) override;

private:
    QString m_fileName;
};

class Logger
{
public:
    enum class LogType {
        File,
        Console
    };

    enum class LogLevel {
        Debug, 
        Info, 
        Warning, 
        Error
    };

    static Logger* instance();

    void log(const QString& message, const LogLevel level = LogLevel::Debug);

    static void SetLogType(const LogType type) { s_type = type; }

private:
    Logger(ILogger* logger)
        : m_logger(logger ? logger : new FileLogger)
    {
    }
    ~Logger() { delete m_logger; }

    ILogger* m_logger;
    QMutex m_mutex;
    static LogType s_type;

    static QString formatMessage(const QString& message, const LogLevel level);
};