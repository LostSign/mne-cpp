//=============================================================================================================
/**
* @file     interpreter.cpp
* @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Jens Haueisen <jens.haueisen@tu-ilmenau.de>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     January, 2013
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
* @brief    MNEViewer class implementation
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "mne_viewer.h"
#include "listener.h"


//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QSet>
#include <QTextStream>


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

MNEViewer::MNEViewer(QObject *parent)
: QObject(parent)
, m_pListener(new Listener(this))
, m_qFileFwd("./MNE-sample-data/MEG/sample/sample_audvis-meg-eeg-oct-6-fwd.fif")
, m_Fwd(m_qFileFwd)
, m_annotSet("./MNE-sample-data/subjects/sample/label/lh.aparc.a2009s.annot","./MNE-sample-data/subjects/sample/label/rh.aparc.a2009s.annot")
, m_surfSet("./MNE-sample-data/subjects/sample/surf/lh.white", "./MNE-sample-data/subjects/sample/surf/rh.white")
{
    qRegisterMetaType<QSharedPointer<MNELIB::MNESourceEstimate> >("QSharedPointer<MNELIB::MNESourceEstimate>");

    //ToDo overload toLabels using instead of t_surfSet rr of MNESourceSpace
    m_annotSet.toLabels(m_surfSet, m_qListLabels, m_qListRGBAs);

    //
    // Cluster forward solution;
    //
    MNEForwardSolution t_clusteredFwd = m_Fwd.cluster_forward_solution(m_annotSet, 20);//40);

    m_pView = QSharedPointer<InverseView>(new InverseView(t_clusteredFwd.src, m_qListLabels, m_qListRGBAs, 24, false));

    if (m_pView->stereoType() != QGLView::RedCyanAnaglyph)
        m_pView->camera()->setEyeSeparation(0.3f);

    m_pView->resize(800, 600);
    m_pView->setTitle(QString("Online Brain Monitoring"));
    m_pView->show();

    connect(m_pListener.data(), &Listener::sourceEstimateAvailable, m_pView.data(), &InverseView::pushSharedSourceEstimate);

//    connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
//    connect(thread, SIGNAL(started()), worker, SLOT(process()));
//    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
//    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
//    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    m_pListener->start();

    std::cout << "mne_viewer initialized\n";
    fflush(stdout);
}

//*************************************************************************************************************

MNEViewer::~MNEViewer()
{
    if(m_pListener->isRunning())
        m_pListener->stop();

}
