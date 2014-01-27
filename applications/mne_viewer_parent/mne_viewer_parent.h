#ifndef MNE_VIEWER_PARENT_H
#define MNE_VIEWER_PARENT_H

#include <mne/mne_sourceestimate.h>

#include <QObject>

#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QTimer>


using namespace MNELIB;

class MNEViewerParent : public QObject
{
    Q_OBJECT
public:
    explicit MNEViewerParent(QObject *parent = 0);

    void processStandardOutput();

    void update();

signals:

public slots:



private:
    QFile qFileSTC;
    MNESourceEstimate stc;

    QProcess myProcess;

    QTimer *timer;

    bool m_bMNEViewerInitialized;
};

#endif // MNE_VIEWER_PARENT_H
