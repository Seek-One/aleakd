#include <QApplication>

#include "QMainEventFilter.h"
#include "QApplicationWindow.h"
#include "QApplicationWindowController.h"

#include "MemOpRcptServer.h"

int main( int argc, char **argv )
{
	bool bGoOn = true;

    QApplication a( argc, argv );

	qCritical("[aleakd-server] Starting server application");

	QApplicationWindow windowApp;
	QApplicationWindowController windowController;

	QMainEventFilter eventFilter(&windowController);
	eventFilter.setApplicationWindowController(&windowController);
	a.installEventFilter(&eventFilter);

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

    // Init controller
	if(bGoOn) {
		bGoOn = windowController.init(&windowApp);
		if(!bGoOn){
			qCritical("[aleakd-server] Unable to init main window");
		}
	}

	// Display window
	if(bGoOn) {
		windowApp.resize(1280, 800);
		windowApp.show();

		a.exec();
	}

	return 0;
}
