#include "youtubeinforeader.h"
#include "networkaccess.h"

#include <qregexp.h>

namespace The {
    NetworkAccess* http();
}

void YouTubeInfoReader::readInfoAbout(Video *video) {
    qDebug() << "reading info";
    QRegExp re("^http://www\\.youtube\\.com/watch\\?v=([0-9A-Za-z_-]+).*");
    bool match = re.exactMatch(video->webpage().toString());
    if (!match || re.numCaptures() < 1) {
        //emit errorStreamUrl(QString("Cannot get video id for %1").arg(m_webpage.toString()));
        qDebug() << "errorStreamUrl()";
        return;
    }
    QString videoId = re.cap(1);
    QString urlString = QString("http://gdata.youtube.com/feeds/api/videos/%1?v=2").arg(videoId);
    QUrl url(urlString);
    this->video = video;
    QObject *reply = The::http()->get(url);
    connect(reply, SIGNAL(data(QByteArray)), SLOT(parseResults(QByteArray)));
}

void YouTubeInfoReader::parseResults(QByteArray data) {
    QString strData = QString::fromUtf8(data);

    QRegExp reTitle("<title>(.*)</title>");
    reTitle.setMinimal(true); // don't be greedy
    int pos = reTitle.indexIn(strData);
    if (pos == -1) {
      qDebug() << "reTitle failed";
      return;
    }
    video->setTitle(reTitle.cap(1));

    // author
    QRegExp reAuthor("<author><name>(.*)</name>");
    reAuthor.setMinimal(true); 
    pos = reAuthor.indexIn(strData);
    if (pos == -1) {
      qDebug() << "reAuthor failed";
      return;
    }
    video->setAuthor(reAuthor.cap(1));

    // thumbnail
    QRegExp reThumb("<media:thumbnail url='(.*)' height=");
    reThumb.setMinimal(true); 
    pos = reThumb.indexIn(strData);
    if (pos == -1) {
      qDebug() << "reThumb failed";
      return;
    }
    video->addThumbnailUrl(QUrl(reThumb.cap(1)));

    // duration
    QRegExp reDuration("duration='(\\d*)'");
    reDuration.setMinimal(true); 
    pos = reDuration.indexIn(strData);
    if (pos == -1) {
      qDebug() << "reDuration failed";
      return;
    }
    video->setDuration(reDuration.cap(1).toInt());

    // <media:description type='plain'>
    QRegExp reDesc("<media:description type='plain'>(.*)</");
    reDesc.setMinimal(true); 
    pos = reDesc.indexIn(strData);
    if (pos == -1) {
      qDebug() << "reDesc failed";
      return;
    }
    video->setDescription(reDesc.cap(1));

    // view count
    QRegExp reView("viewCount='(\\d*)'");
    reView.setMinimal(true); 
    pos = reView.indexIn(strData);
    if (pos == -1) {
      qDebug() << "reView failed";
      return;
    }
    video->setViewCount(reView.cap(1).toInt());

    // published
    QRegExp rePublished("<published>(.*)</");
    rePublished.setMinimal(true); 
    pos = rePublished.indexIn(strData);
    if (pos == -1) {
      qDebug() << "rePublished failed";
      return;
    }
    video->setPublished(QDateTime::fromString(rePublished.cap(1), Qt::ISODate));
  
    emit infoRead(video);
}
