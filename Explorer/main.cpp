#include "explorer.h"
#include <QtWidgets/QApplication>

/*
 * Qt5
 * std: > c++17
 * Visual Studio 2019
 */

int main(int argc, char *argv[])
{
	const QScopedPointer<QApplication> app( new QApplication(argc, argv));
	const QScopedPointer<Explorer> w(new Explorer);

	try
	{
		return app->exec();
	}
	catch (...)
	{
		qFatal("Fatal error.");
		return -1;
	}
}
