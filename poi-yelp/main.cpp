#include <QApplication>
#include <iostream>
#include "MainApp.h"
#include <getopt.h>

#include <libhomescreen.hpp>
#include <qlibwindowmanager.h>

#include <navigation.h>

#define DEFAULT_CREDENTIALS_FILE "/etc/poikey"

using namespace std;

static QLibWindowmanager* qwm;
static LibHomeScreen* hs;
static QString graphic_role;
static MainApp *mainapp;

static void SyncDrawHandler(json_object *object)
{
    qwm->endDraw(graphic_role);
}

static void ShowWindowHandler(json_object *object)
{
    qwm->activateWindow(graphic_role);
}

// Callback to drive raising navigation app
static void NavWindowRaiseHandler(void)
{
    if (hs) {
        hs->showWindow("navigation", nullptr);
    }
}

int main(int argc, char *argv[], char *env[])
{
    int opt;
    QApplication a(argc, argv);
    QString credentialsFile(DEFAULT_CREDENTIALS_FILE);
    qwm = new QLibWindowmanager();
    hs = new LibHomeScreen();
    graphic_role = QString("poi");

    QString pt = QString(argv[1]);
    int port = pt.toInt();
    QString secret = QString(argv[2]);
    std::string token = secret.toStdString();

    QUrl bindingAddress;
    bindingAddress.setScheme(QStringLiteral("ws"));
    bindingAddress.setHost(QStringLiteral("localhost"));
    bindingAddress.setPort(port);
    bindingAddress.setPath(QStringLiteral("/api"));
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("token"), secret);
    bindingAddress.setQuery(query);

    if (qwm->init(port, secret) != 0) {
        exit(EXIT_FAILURE);
    }

    if (qwm->requestSurface(graphic_role) != 0) {
        cerr << "Error: wm check failed" << endl;
        exit(EXIT_FAILURE);
    }

    qwm->set_event_handler(QLibWindowmanager::Event_SyncDraw, SyncDrawHandler);

    mainapp = new MainApp(new Navigation(bindingAddress));

    hs->init(port, token.c_str());

    hs->set_event_handler(LibHomeScreen::Event_ShowWindow, ShowWindowHandler);

    // force setting
    mainapp->setInfoScreen(true);
    mainapp->setKeyboard(true);

    // hook up callback to start/raise navigation app
    mainapp->setNavWindowRaiseCallback(NavWindowRaiseHandler);

    /* then, authenticate connexion to POI service: */
    if (mainapp->AuthenticatePOI(credentialsFile) < 0)
    {
        cerr << "Error: POI server authentication failed" << endl;
        return -1;
    }

    cerr << "authentication succes !" << endl;

    /* now, let's start monitor user inut (register callbacks): */
    if (mainapp->StartMonitoringUserInput() < 0)
        return -1;

    qwm->activateWindow(graphic_role);

    /* main loop: */
    return a.exec();
}
