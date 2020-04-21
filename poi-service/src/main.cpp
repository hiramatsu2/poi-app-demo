#include "poiservice.h"

#include <poi.h>

#include <QUrl>
#include <QUrlQuery>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (argc < 2)
        return 1;

    bool isOk;
    quint16 port = QString(argv[1]).toInt(&isOk);
    if (!isOk)
        return 1;
    QString secret = QString(argv[2]);

    QUrl bindingAddress;
    bindingAddress.setScheme(QStringLiteral("ws"));
    bindingAddress.setHost(QStringLiteral("localhost"));
    bindingAddress.setPort(port);
    bindingAddress.setPath(QStringLiteral("/api"));
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("token"), secret);
    bindingAddress.setQuery(query);

    PoiService poi(new POI(bindingAddress, POI::ProcessType::service), &a);

    return a.exec();
}
