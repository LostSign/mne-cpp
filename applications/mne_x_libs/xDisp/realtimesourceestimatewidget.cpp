//=============================================================================================================
/**
* @file     realtimesourceestimatewidget.cpp
* @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     July, 2012
*
* @section  LICENSE
*
* Copyright (C) 2012, Christoph Dinh and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of MNE-CPP authors nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    Implementation of the RealTimeSourceEstimateWidget Class.
*
*/

//ToDo Paint to render area

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "realtimesourceestimatewidget.h"
#include <xMeas/realtimesourceestimate.h>

#include <mne/mne_forwardsolution.h>
#include <mne/mne_inverse_operator.h>

#include <inverse/minimumNorm/minimumnorm.h>

#include <Eigen/Core>


//*************************************************************************************************************
//=============================================================================================================
// STL INCLUDES
//=============================================================================================================

#include <math.h>


//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QLabel>
#include <QGridLayout>
#include <QSettings>

#include <QDebug>


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace XDISPLIB;
using namespace DISP3DLIB;
using namespace MNELIB;
using namespace XMEASLIB;


using namespace INVERSELIB;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

RealTimeSourceEstimateWidget::RealTimeSourceEstimateWidget(QSharedPointer<RealTimeSourceEstimate> &pRTSE, QWidget* parent)
: NewMeasurementWidget(parent)
, m_pRTSE(pRTSE)
, m_bInitialized(false)
{
    m_pAction3DControl = new QAction(QIcon(":/images/3DControl.png"), tr("Shows the 3D control widget (F9)"),this);
    m_pAction3DControl->setShortcut(tr("F9"));
    m_pAction3DControl->setToolTip(tr("Shows the 3D control widget (F9)"));
    connect(m_pAction3DControl, &QAction::triggered,
            this, &RealTimeSourceEstimateWidget::show3DControlWidget);
    addDisplayAction(m_pAction3DControl);
    m_pAction3DControl->setVisible(true);

    m_p3DView = View3D::SPtr(new View3D());

    m_pControl3DView = Control3DWidget::SPtr(new Control3DWidget(this));
    m_pControl3DView->setView3D(m_p3DView);

    QGridLayout *mainLayoutView = new QGridLayout;
    QWidget *pWidgetContainer = QWidget::createWindowContainer(m_p3DView.data());
    mainLayoutView->addWidget(pWidgetContainer);

    this->setLayout(mainLayoutView);

    getData();
}


//*************************************************************************************************************

RealTimeSourceEstimateWidget::~RealTimeSourceEstimateWidget()
{
    //
    // Store Settings
    //
    if(!m_pRTSE->getName().isEmpty())
    {
    }
}


//*************************************************************************************************************

void RealTimeSourceEstimateWidget::update(XMEASLIB::NewMeasurement::SPtr)
{
    getData();
}


//*************************************************************************************************************

void RealTimeSourceEstimateWidget::getData()
{
    if(m_bInitialized)
    {
        //
        // Add rt brain data
        //
        if(m_lRtItem.isEmpty()) {
            qDebug()<<"RealTimeSourceEstimateWidget::getData - Creating m_lRtItem list";
            m_lRtItem = m_p3DView->addRtBrainData("HemiLRSet", *m_pRTSE->getValue(), *m_pRTSE->getFwdSolution());

            for(int i = 0; i<m_lRtItem.size(); i++) {
                m_lRtItem.at(i)->setLoopState(false);
                m_lRtItem.at(i)->setTimeInterval(10);
                //m_lRtItem.at(i)->onTimeIntervalChanged(m_pRTSE->getValue()->tstep*1000000);
                m_lRtItem.at(i)->setNumberAverages(1);
                m_lRtItem.at(i)->setStreamingActive(true);
            }
        } else {
            qDebug()<<"RealTimeSourceEstimateWidget::getData - Working with m_lRtItem list";

            for(int i = 0; i<m_lRtItem.size(); i++) {
                m_lRtItem.at(i)->addData(*m_pRTSE->getValue());
            }
        }
    }
    else
    {
        if(m_pRTSE->getAnnotSet() && m_pRTSE->getSurfSet())
        {
            m_pRTSE->m_bStcSend = false;
            init();

            //
            // Add brain data
            //
            m_p3DView->addBrainData("HemiLRSet", *m_pRTSE->getSurfSet(), *m_pRTSE->getAnnotSet());
        }
    }
}


//*************************************************************************************************************

void RealTimeSourceEstimateWidget::init()
{
    m_bInitialized = true;
    m_pRTSE->m_bStcSend = true;
}


//*************************************************************************************************************

void RealTimeSourceEstimateWidget::show3DControlWidget()
{
    qDebug()<<"RealTimeSourceEstimateWidget::show3DControlWidget()";

    if(m_pControl3DView->isActiveWindow())
        m_pControl3DView->hide();
    else {
        m_pControl3DView->activateWindow();
        m_pControl3DView->show();
    }
}


//*************************************************************************************************************
//=============================================================================================================
// STATIC DEFINITIONS
//=============================================================================================================
