#include "explorer.h"
#include <QtWidgets/QApplication>

/*
 * Qt 5.10.1 msvc 2015 x86
 * std: c++17
 * Visual Studio 2019
 */

int main(int argc, char *argv[])
{
	const QScopedPointer app( new QApplication(argc, argv));
	const QScopedPointer w(new Explorer);

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
