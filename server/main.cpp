#include <QApplication>
#include <QPushButton>

#include "MemOpRcptServer.h"

int main( int argc, char **argv )
{
	bool bGoOn = true;

    QApplication a( argc, argv );

	qCritical("[aleakd-server] Starting server application");

	// Initialize tcp server
	int iPort = 19999;
	MemOpRcptServer leakServer;
    if(bGoOn) {
		qCritical("[aleakd-server] Creating server on port: %d", iPort);
		bGoOn = leakServer.listen(QHostAddress::Any, iPort);
		if(!bGoOn){
			qCritical("[aleakd-server] Unable to create server");
		}
	}

	if(bGoOn) {
		QPushButton hello("Hello world!", 0);
		hello.resize(100, 30);
		hello.show();

		a.exec();
	}

	return 0;
}
