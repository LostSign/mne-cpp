#include "realtimesourceestimatewidgethelper.h"
#include "realtimesourceestimatewidget.h"


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace XDISPLIB;


RealTimeSourceEstimateWidgetHelper::RealTimeSourceEstimateWidgetHelper(QObject *parent)
: QThread(parent)
{
    m_pRTSEW = qobject_cast<RealTimeSourceEstimateWidget*>(this->parent());

}


//*************************************************************************************************************

RealTimeSourceEstimateWidgetHelper::~RealTimeSourceEstimateWidgetHelper()
{
    if(this->isRunning())
        this->stop();
}


//*************************************************************************************************************

bool RealTimeSourceEstimateWidgetHelper::start()
{
    QThread::start();
    m_bIsRunning = true;
    return true;
}


//*************************************************************************************************************

bool RealTimeSourceEstimateWidgetHelper::stop()
{
    m_bIsRunning = false;
    return true;
}

//*************************************************************************************************************

void RealTimeSourceEstimateWidgetHelper::run()
{
    while(m_bIsRunning)
    {
        if(m_pRTSEW->m_qVectorSTC.size() > 0)
        {

            MNESourceEstimate stc = m_pRTSEW->m_qVectorSTC[0];

//            m_pRTSEW->m_qVectorSTC.pop_front();

            QByteArray qByteArray;

            QTextStream io(&qByteArray, QIODevice::WriteOnly);

            io << "--stcstream\n";
            stc.writeToTxtStream(io);
            io.flush();

            m_pRTSEW->m_qMutexRTSEW.lock();
            m_pRTSEW->m_qProcess.write(qByteArray);
            m_pRTSEW->m_qProcess.waitForBytesWritten();
            m_pRTSEW->m_qMutexRTSEW.unlock();

            std::cout << "popped source" << std::endl;
        }
    }

    printf("RealTimeSourceEstimateWidgetHelper ended\n");
}
