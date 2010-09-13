#ifndef YOUTUBEINFOREADER_H
#define YOUTUBEINFOREADER_H

#include "video.h"

class YouTubeInfoReader : public QObject {
  
  Q_OBJECT

public:
    YouTubeInfoReader() : QObject() {}

    void readInfoAbout(Video *video);

private slots:
    void parseResults(QByteArray data);

signals:
    void infoRead(Video*);

private:
    Video *video;
};

#endif
