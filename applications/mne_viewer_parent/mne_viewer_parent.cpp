#include "mne_viewer_parent.h"
#include <iostream>

MNEViewerParent::MNEViewerParent(QObject *parent)
: QObject(parent)
, qFileSTC("./stc_test.fif")
, stc(qFileSTC)
, timer(new QTimer)
, m_bMNEViewerInitialized(false)
{

        myProcess.setProcessChannelMode(QProcess::SeparateChannels);
        myProcess.start("mne_viewer", QProcess::Unbuffered | QProcess::ReadWrite);
        if(!myProcess.waitForStarted(3000))
            std::cout << "Subprocess couldn't be started" << std::endl;

        std::cout << "Subprocess started" << std::endl;

        myProcess.setReadChannel(QProcess::StandardOutput);

        connect(&myProcess,&QProcess::readyReadStandardOutput, this, &MNEViewerParent::processStandardOutput);


//        myProcess.setReadChannel(QProcess::StandardOutput);

//        QString line;
//        while(myProcess.waitForReadyRead(-1)) {
//            line = myProcess.readAllStandardOutput();
//            printf("%s\n", line.toLatin1().data());
//        }



        connect(timer, &QTimer::timeout, this, &MNEViewerParent::update);
        timer->start(5000);
}

void MNEViewerParent::processStandardOutput()
{

    while(myProcess.canReadLine())
    {
        QString line = myProcess.readLine();

        if(line.indexOf("mne_viewer initialized") > -1)
        {
            m_bMNEViewerInitialized = true;
            printf("%s", line.toLatin1().data());
        }
    }

}

void MNEViewerParent::update()
{
    if(m_bMNEViewerInitialized)
    {
        std::cout << "update" << std::endl;

        QByteArray qByteArray;

        QTextStream io(&qByteArray, QIODevice::WriteOnly);

        io << "--stcstream\n";
        stc.writeToTxtStream(io);
        io.flush();

        myProcess.write(qByteArray);
        myProcess.waitForBytesWritten();
    }
}
