//=============================================================================================================
/**
* @file     mne_viewer.h
* @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Jens Haueisen <jens.haueisen@tu-ilmenau.de>;
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
* @brief    MNEViewer class declaration
*
*/

#ifndef MNEVIEWER_H
#define MNEVIEWER_H

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include <QObject>
#include <QSharedPointer>
#include <QFile>
#include <QList>

#include <fs/label.h>
#include <fs/annotationset.h>
#include <fs/surface.h>

#include <fiff/fiff_evoked.h>

#include <mne/mne_forwardsolution.h>
#include <disp3D/inverseview.h>


//*************************************************************************************************************
//=============================================================================================================
// FORWARD DECLARATIONS
//=============================================================================================================

class Listener;


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace MNELIB;
using namespace FSLIB;
using namespace DISP3DLIB;


//=============================================================================================================
/**
* MNEViewer
*
* @brief MNEViewer
*/
class MNEViewer : public QObject
{
    Q_OBJECT
public:
    explicit MNEViewer(QObject *parent = 0);

    ~MNEViewer();

public slots:

private:
    QSharedPointer<Listener>    m_pListener;

    QFile                       m_qFileFwd;
    MNEForwardSolution          m_Fwd;
//    MNEForwardSolution          m_clusteredFwd;

    AnnotationSet               m_annotSet;
    SurfaceSet                  m_surfSet;

    QList<Label>                m_qListLabels;
    QList<RowVector4i>          m_qListRGBAs;

    QSharedPointer<InverseView> m_pView;

};

#endif // MNEVIEWER_H
