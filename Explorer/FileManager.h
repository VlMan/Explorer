#pragma once

#include <QDir>
#include <QPixmap>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QImageReader>

class FileManager {
	static QString getFileTypeInText(const QString& path) {
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

	static QPixmap scaleImage(const QPixmap& pix, const QRect& default_geometry) {
		return pix.scaled(default_geometry.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
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
		return isDirExists(dir) && dir.isReadable() && dir.isAbsolute() && dir.isRoot() == false;
	}

	static QString getPath(const QString& path) {
		return QFileInfo(path).fileName();
	}

	static QDateTime getFileLastModified(const QString& path) {
		QFile file(path);
		if (isFileExists(file)) {
			return file.fileTime(QFileDevice::FileModificationTime);
		}
		else {
			return QDateTime();
		}
	}

	static QStringList getSystemDrives() {
		QStringList drives;
		for (const QFileInfo & info : QDir::drives()) {
			drives.append(info.absoluteFilePath());
		}
		return drives;
	}

	static QString getCurrentWorkingDirectory() {
		return QDir::currentPath();
	}

	static qint64 getFileSize(const QString& path) {
		QFile file(path);
		if (isFileExists(file)) {
			return file.size();
		}
		else {
			return -1;
		}
	}

	static bool remove(const QString& path) {
		QFile file(path);
		QDir dir(path);
		if (isFileExists(file)) {
			return file.remove();
		}
		else if (isDirExists(dir)) {
			return dir.removeRecursively();
		}
		else {
			return false;
		}
	}

	static bool createDirectory(const QString& path) {
		QDir dir(path);
		if (!isDirExists(dir))
			return dir.mkpath(".");
		return true;
	}

	static bool copy(const QString& source, const QString& destination) {
		QFile file(source);
		QDir dir(source);
		if (isFileExists(file)) {
			return file.copy(destination);
		}
		else if (isDirExists(dir)) {
			return dir.mkpath(destination) && QDir(destination).removeRecursively() && QDir(source).rename(source, destination);
		}
		else {
			return false;
		}
	}

	static bool move(const QString& source, const QString& destination) {
		QFile file(source);
		QDir dir(source);
		if (isFileExists(file)) {
			return file.rename(destination);
		}
		else if (isDirExists(dir)) {
			return QDir(source).rename(source, destination);
		}
		else {
			return false;
		}
	}

	static bool isDirExists(const QString& dir_name) {
		QDir dir(dir_name);
		return isDirExists(dir);
	}

	static bool isDirExists(const QDir& dir) {
		return dir.exists();
	}

	static bool isFileExists(const QString& path) {
		return QFile::exists(path);
	}

	static bool isFileExists(const QFile& file) {
		return file.exists();
	}

	static QStringList getDirectoryContentsByExtension(const QString& path, const QString& extension) {
		QDir dir(path);
		if (!isDirExists(dir))
			return QStringList();
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

	static bool writeToFile(const QString& path, const QString& text) {
		if (!isFileExists(path))
			return false;
		QFile file(path);
		if (!file.open(QIODevice::Append))
			return false;

		QTextStream stream(&file);
		stream << text << Qt::endl;
		return true;
	}

	static bool writeToFile(const QString& path, const QByteArray& data) {
		if (data.isEmpty() || !isFileExists(path))
			return false;

		QFile file(path);
		if (!file.open(QIODevice::Append))
			return false;

		file.write(data);
		file.flush();
		return true;
	}

	static QByteArray readFromFile(const QString& path) {
		if (!isFileExists(path))
			return QByteArray();
		QFile file(path);
		if (!file.open(QIODevice::ReadOnly))
			return QByteArray();
		return file.readAll();
	}

	static void openFile(const QString& path) {
		if (isFileExists(path))
			QDesktopServices::openUrl(QUrl(path));
	}

	static QFileDevice::Permissions getFilePermissions(const QString& path) {
		QFileInfo fileInfo(path);
		return fileInfo.permissions();
	}

	static QPixmap getIcon(const QString& path, const QRect& default_geometry) {
		static const std::map<const QString, const char*> icons = {
			{ "directory", ":/Explorer/folder.png" },
			{ "executable", ":/Explorer/exe.png" },
			{ "image", ":/Explorer/picture_png.png" },
			{ "music", ":/Explorer/music.png" },
			{ "unknown", ":/Explorer/unknown.png" }
		};

		auto icon_it = icons.find(getFileTypeInText(path));
		if (icon_it == icons.end()) {
			icon_it = icons.find("unknown");
		}
		const auto icon_name = icon_it->second;

		QImageReader reader(icon_name);
		if (!reader.canRead()) {
			qWarning() << "Failed to read image for" << path;
			return scaleImage(QPixmap(icon_name), default_geometry);
		}

		QPixmap pix = QPixmap::fromImageReader(&reader);
		if (pix.isNull()) {
			qWarning() << "Failed to load image for" << path;
			return scaleImage(QPixmap(icon_name), default_geometry);
		}

		return scaleImage(pix, default_geometry);
	}
};