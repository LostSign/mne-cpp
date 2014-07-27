//=============================================================================================================
/**
* @file     covariance.cpp
* @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     February, 2013
*
* @section  LICENSE
*
* Copyright (C) 2013, Christoph Dinh and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of the Massachusetts General Hospital nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MASSACHUSETTS GENERAL HOSPITAL BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    Contains the implementation of the Covariance class.
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "covariance.h"
#include "FormFiles/covariancesetupwidget.h"


//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QtCore/QtPlugin>
#include <QDebug>


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace CovariancePlugin;
using namespace MNEX;
using namespace XMEASLIB;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

Covariance::Covariance()
: m_bIsRunning(false)
, m_bProcessData(false)
, m_pCovarianceInput(NULL)
//, m_pCovarianceOutput(NULL)
, m_pCovarianceBuffer(CircularMatrixBuffer<double>::SPtr())
{
    m_pActionShowAdjustment = new QAction(QIcon(":/images/covariance.png"), tr("Covariance Adjustments"),this);
//    m_pActionSetupProject->setShortcut(tr("F12"));
    m_pActionShowAdjustment->setStatusTip(tr("Covariance Adjustments"));
    connect(m_pActionShowAdjustment, &QAction::triggered, this, &Covariance::showCovarianceWidget);
    addPluginAction(m_pActionShowAdjustment);
}


//*************************************************************************************************************

Covariance::~Covariance()
{
    if(this->isRunning())
        stop();
}


//*************************************************************************************************************

QSharedPointer<IPlugin> Covariance::clone() const
{
    QSharedPointer<Covariance> pCovarianceClone(new Covariance);
    return pCovarianceClone;
}


//*************************************************************************************************************
//=============================================================================================================
// Creating required display instances and set configurations
//=============================================================================================================

void Covariance::init()
{
    // Input
    m_pCovarianceInput = PluginInputData<NewRealTimeMultiSampleArray>::create(this, "CovarianceIn", "Covariance input data");
    connect(m_pCovarianceInput.data(), &PluginInputConnector::notify, this, &Covariance::update, Qt::DirectConnection);
    m_inputConnectors.append(m_pCovarianceInput);

    // Output
    m_pCovarianceOutput = PluginOutputData<RealTimeCov>::create(this, "CovarianceOut", "Covariance output data");
    m_outputConnectors.append(m_pCovarianceOutput);

    //init channels when fiff info is available
    connect(this, &Covariance::fiffInfoAvailable, this, &Covariance::initConnector);

    //Delete Buffer - will be initailzed with first incoming data
    if(!m_pCovarianceBuffer.isNull())
        m_pCovarianceBuffer = CircularMatrixBuffer<double>::SPtr();
}


//*************************************************************************************************************

void Covariance::initConnector()
{
    qDebug() << "void Covariance::initConnector()";
//    if(m_pFiffInfo)
//        m_pRTMSAOutput->data()->initFromFiffInfo(m_pFiffInfo);
}


//*************************************************************************************************************

bool Covariance::start()
{
    //Check if the thread is already or still running. This can happen if the start button is pressed immediately after the stop button was pressed. In this case the stopping process is not finished yet but the start process is initiated.
    if(this->isRunning())
        QThread::wait();

    m_bIsRunning = true;

    // Start threads
    QThread::start();

    return true;
}


//*************************************************************************************************************

bool Covariance::stop()
{
    //Wait until this thread is stopped
    m_bIsRunning = false;

    //In case the semaphore blocks the thread -> Release the QSemaphore and let it exit from the pop function (acquire statement)
    m_pCovarianceBuffer->releaseFromPop();

    m_pCovarianceBuffer->clear();

    return true;
}


//*************************************************************************************************************

IPlugin::PluginType Covariance::getType() const
{
    return _IAlgorithm;
}


//*************************************************************************************************************

QString Covariance::getName() const
{
    return "Covariance";
}


//*************************************************************************************************************

void Covariance::showCovarianceWidget()
{

}


//*************************************************************************************************************

QWidget* Covariance::setupWidget()
{
    CovarianceSetupWidget* setupWidget = new CovarianceSetupWidget(this);//widget is later distroyed by CentralWidget - so it has to be created everytime new
    return setupWidget;
}


//*************************************************************************************************************

void Covariance::update(XMEASLIB::NewMeasurement::SPtr pMeasurement)
{
    QSharedPointer<NewRealTimeMultiSampleArray> pRTMSA = pMeasurement.dynamicCast<NewRealTimeMultiSampleArray>();

    if(pRTMSA)
    {
        //Check if buffer initialized
        if(!m_pCovarianceBuffer)
            m_pCovarianceBuffer = CircularMatrixBuffer<double>::SPtr(new CircularMatrixBuffer<double>(64, pRTMSA->getNumChannels(), pRTMSA->getMultiArraySize()));

        //Fiff information
        if(!m_pFiffInfo)
        {
            m_pFiffInfo = pRTMSA->getFiffInfo();
            emit fiffInfoAvailable();
        }


        if(m_bProcessData)
        {
            MatrixXd t_mat(pRTMSA->getNumChannels(), pRTMSA->getMultiArraySize());

            for(qint32 i = 0; i < pRTMSA->getMultiArraySize(); ++i)
                t_mat.col(i) = pRTMSA->getMultiSampleArray()[i];

            m_pCovarianceBuffer->push(&t_mat);
        }
    }
}


//*************************************************************************************************************

void Covariance::appendCovariance(FiffCov::SPtr p_pCovariance)
{
    mutex.lock();
    m_qVecCovData.push_back(p_pCovariance);
    mutex.unlock();
}


//*************************************************************************************************************

void Covariance::run()
{
    //
    // Read Fiff Info
    //
    while(!m_pFiffInfo)
        msleep(10);// Wait for fiff Info

    //
    // Init Real-Time Covariance estimator
    //
    m_pRtCov = RtCov::SPtr(new RtCov(5000, m_pFiffInfo));
    connect(m_pRtCov.data(), &RtCov::covCalculated, this, &Covariance::appendCovariance);

    //
    // Start the rt helpers
    //
    m_pRtCov->start();

    //
    // start processing data
    //
    m_bProcessData = true;

    while (m_bIsRunning)
    {
        if(m_bProcessData)
        {
            /* Dispatch the inputs */
            MatrixXd t_mat = m_pCovarianceBuffer->pop();

            //Add to covariance estimation
            m_pRtCov->append(t_mat);

            if(m_qVecCovData.size() > 0)
            {
                mutex.lock();
                m_pCovarianceOutput->data()->setValue(*m_qVecCovData[0]);

                m_qVecCovData.pop_front();
                mutex.unlock();
            }
        }
    }
}

