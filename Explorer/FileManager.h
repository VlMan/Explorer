#pragma once

#include <QDir>
#include <QPixmap>
#include <QDateTime>

class FileManager {
    static QString getFileType(const QString& path) {
        if (isDirectory(path)) {
            return "directory";
        }
        else if (isExecutable(path)) {
            return "executable";
        }
        else if (isImage(path)) {
            return "image";
        }
        else if (isMusic(path)) {
            return "music";
        }
        else {
            return "unknown";
        }
    }

public:
    static QString getFormatFile(const QString& path) {
        return QFileInfo(path).suffix().toLower();
    }

    static QStringList getDirectoryContents(const QString& path) {
        QDir dir(path);
        QStringList files = dir.entryList(QDir::Files);
        QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        return files + dirs;
    }

    static bool isDirectory(const QString& path) {
        QDir dir(path);
        return dir.exists() && dir.isReadable() && dir.isAbsolute() && dir.isRoot() == false;
    }

    static QDateTime getFileLastModified(const QString& path) {
        QFile file(path);
        if (file.exists()) {
            return file.fileTime(QFileDevice::FileModificationTime);
        }
        else {
            return QDateTime();
        }
    }

    static QStringList getSystemDrives() {
        QStringList drives;
        foreach(const QFileInfo & info, QDir::drives()) {
            drives.append(info.absoluteFilePath());
        }
        return drives;
    }

    static QString getCurrentWorkingDirectory() {
        return QDir::currentPath();
    }

    static qint64 getFileSize(const QString& path) {
        QFile file(path);
        if (file.exists()) {
            return file.size();
        }
        else {
            return -1;
        }
    }

    static bool remove(const QString& path) {
        QFile file(path);
        QDir dir(path);
        if (file.exists()) {
            return file.remove();
        }
        else if (dir.exists()) {
            return dir.removeRecursively();
        }
        else {
            return false;
        }
    }

    static bool createDirectory(const QString& path) {
        QDir dir(path);
        return dir.mkpath(".");
    }

    //static bool copy(const QString& source, const QString& destination) {
    //    QFile file(source);
    //    QDir dir(source);
    //    if (file.exists()) {
    //        return file.copy(destination);
    //    }
    //    else if (dir.exists()) {
    //        return dir.mkpath(destination) && QDir(destination).removeRecursively() && QDir(source).rename(destination);
    //    }
    //    else {
    //        return false;
    //    }
    //}

    //static bool move(const QString& source, const QString& destination) {
    //    QFile file(source);
    //    QDir dir(source);
    //    if (file.exists()) {
    //        return file.rename(destination);
    //    }
    //    else if (dir.exists()) {
    //        return QDir(source).rename(destination);
    //    }
    //    else {
    //        return false;
    //    }
    //}

    static QStringList getDirectoryContentsByExtension(const QString& path, const QString& extension) {
        QDir dir(path);
        QStringList filters;
        filters << QString("*.%1").arg(extension);
        dir.setNameFilters(filters);
        return dir.entryList(QDir::Files);
    }

    static bool isExecutable(const QString& path) {
        return QFileInfo(path).isExecutable();
    }

    static bool isImage(const QString& path) {
        QString extension = getFormatFile(path);
        return extension == "jpg" || extension == "jpeg" || extension == "bmp" || extension == "png" || extension == "gif";
    }

    static bool isMusic(const QString& path) {
        QString extension = getFormatFile(path);
        return extension == "mp3" || extension == "wav" || extension == "aac" || extension == "flac";
    }

    static void openFile(const QString& path) {
        if (QFile::exists(path))
            QDesktopServices::openUrl(path);
    }

    static QFileDevice::Permissions getFilePermissions(const QString& path) {
        QFileInfo fileInfo(path);
        return fileInfo.permissions();
    }

    static QPixmap getIcon(const QString& path, const QRect& default_geometry) {
        static const std::unordered_map<QString, QString> icons = {
             { "directory", ":/Explorer/folder.png" },
             { "executable", ":/Explorer/exe.png" },
             { "image", ":/Explorer/picture_png.png" },
             { "music", ":/Explorer/music.png" }
        };

        QString fileType = getFileType(path);

        if (icons.count(fileType) > 0) {
            return QPixmap(icons.at(fileType)).scaled(default_geometry.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }
        else {
            return QPixmap(":/Explorer/unknown.png").scaled(default_geometry.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }
    }
};