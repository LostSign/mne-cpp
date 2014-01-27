#ifndef REALTIMESOURCEESTIMATEWIDGETHELPER_H
#define REALTIMESOURCEESTIMATEWIDGETHELPER_H

#include <QThread>


//*************************************************************************************************************
//=============================================================================================================
// DEFINE NAMESPACE XDISPLIB
//=============================================================================================================

namespace XDISPLIB
{

class RealTimeSourceEstimateWidget;



class RealTimeSourceEstimateWidgetHelper : public QThread
{
    Q_OBJECT
public:
    explicit RealTimeSourceEstimateWidgetHelper(QObject *parent = 0);


    ~RealTimeSourceEstimateWidgetHelper();

    //=========================================================================================================
    /**
    * Starts the Listener.
    *
    * @return true if success, false otherwise
    */
    bool start();

    //=========================================================================================================
    /**
    * Stops the Listener.
    *
    * @return true if success, false otherwise
    */
    bool stop();

signals:

protected:
    //=========================================================================================================
    /**
    * The starting point for the thread. After calling start(), the newly created thread calls this function.
    * Returning from this method will end the execution of the thread.
    * Pure virtual method inherited by QThread
    */
    virtual void run();

private:
    bool m_bIsRunning;

    RealTimeSourceEstimateWidget* m_pRTSEW;

};

} // NAMESPACE

#endif // REALTIMESOURCEESTIMATEWIDGETHELPER_H
