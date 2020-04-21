#include <QWidget>
#include <QFont>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QPixmap>
#include <QImageReader>
#include "InfoPanel.h"
#include "traces.h"

#define BTN_STYLE "background-color: #333333; color: white;"

#define FONT_SIZE_BOLD      22
#define FONT_SIZE           16
#define STARS_IMG_OFFSET    1520
#define STARS_IMG_HEIGHT    69
#define STARS_IMG_WIDTH     324

#define LABEL_NAME_HEIGHT       37
#define LABEL_ADDR_HEIGHT       30
#define LABEL_PHONE_HEIGHT      30
#define LABEL_IMG_HEIGHT        220
#define LABEL_REVIEWS_HEIGHT    24

InfoPanel::InfoPanel(QWidget *parent, QRect rect):
            QWidget(parent),
            nameLabel(this, rect),
            imageLabel(this, rect),
            addressLabel(this, rect),
            phoneLabel(this, rect),
            imgRatingLabel(this, rect),
            nbReviewsLabel(this, rect),
            btnsBackground(this, rect),
            cancelButton("Cancel", this),
            goButton("Go !", this),
            networkManager(parent)
{
    setGeometry(rect);
    setVisible(false);
}

void InfoPanel::populateInfo(Business & business)
{
    int y = 0;
    QPixmap pixmap;
    bool isImageDownloaded = false;
    QFont font, fontBold;
    QRect rect = this->geometry();

    font = nameLabel.font();
    font.setPointSize(FONT_SIZE);

    fontBold = nameLabel.font();
    fontBold.setPointSize(FONT_SIZE_BOLD);
    fontBold.setBold(true);

    /* Preload image Url: */
    TRACE_INFO("Image URL: %s", qPrintable(business.ImageUrl));
    QEventLoop eventLoop;
    QObject::connect(&networkManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    QNetworkRequest req(QUrl(business.ImageUrl));
    QNetworkReply* reply = networkManager.get(req);

    eventLoop.exec(); // wait for answer
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray jpegData = reply->readAll();
        pixmap.loadFromData(jpegData);
        isImageDownloaded = true;
    }
    networkManager.disconnect();
    delete reply;

    /* Display Name: */
    nameLabel.Init(y, LABEL_NAME_HEIGHT, business.Name, &fontBold);
    y += LABEL_NAME_HEIGHT;

    /* Display Address: */
    addressLabel.Init(y, LABEL_ADDR_HEIGHT,
        business.Address+", "+business.City+", "+business.State+" "+business.ZipCode+", "+business.Country,
        &font);
    y += LABEL_ADDR_HEIGHT;

    /* Display phone number: */
    phoneLabel.Init(y, LABEL_PHONE_HEIGHT, business.Phone, &font);
    y += LABEL_PHONE_HEIGHT;

    /* Image Url: */
    imageLabel.Init(y, LABEL_IMG_HEIGHT, QString(""));
    y += LABEL_IMG_HEIGHT;
    if (isImageDownloaded)
    {
        imageLabel.setPixmap(pixmap.scaled(QSize(rect.width(), LABEL_IMG_HEIGHT-6), Qt::KeepAspectRatio));
    }

    /* Display number of reviews: */
    nbReviewsLabel.Init(y, LABEL_REVIEWS_HEIGHT, QString("Number of reviews : %1").arg(business.ReviewCount), &font);
    y += LABEL_REVIEWS_HEIGHT;

    /* Rating image: */
    QImageReader reader(QString(":/images/stars_map_www.png"));
    int RatingImgIndex = (int)((double)business.Rating*2)-1;
    if (RatingImgIndex < 0)
    {
        RatingImgIndex = 0;
    }
    reader.setClipRect(QRect(0,
        STARS_IMG_OFFSET + RatingImgIndex*STARS_IMG_HEIGHT, STARS_IMG_WIDTH, STARS_IMG_HEIGHT));
    const QImage image = reader.read();
    imgRatingLabel.Init(y, STARS_IMG_HEIGHT, QString(""));
    y += STARS_IMG_HEIGHT;
    imgRatingLabel.setPixmap(QPixmap::fromImage(image).scaled(QSize(rect.width() / 3, STARS_IMG_HEIGHT), Qt::KeepAspectRatio));

    /* Buttons: */
    btnsBackground.Init(y, 70, QString(""));
    y += 70;

    cancelButton.setStyleSheet(BTN_STYLE);
    cancelButton.setFont(font);
    cancelButton.setMinimumSize(QSize(rect.width()/4, 50));
    //cancelButton.setGeometry(QRect(rect.x()+rect.width()/8, rect.y()+y-60, rect.width()/4, 50));
    cancelButton.setGeometry(QRect(rect.width()/8, y-60, rect.width()/4, 50));

    goButton.setStyleSheet(BTN_STYLE);
    goButton.setFont(font);
    goButton.setMinimumSize(QSize(rect.width()/4, 50));
    goButton.setGeometry(QRect(rect.width()*5/8, y-60, rect.width()/4, 50));
}
