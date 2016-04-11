//=============================================================================================================
/**
* @file     brainhemispheretreeitem.cpp
* @author   Lorenz Esch <Lorenz.Esch@tu-ilmenau.de>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     November, 2015
*
* @section  LICENSE
*
* Copyright (C) 2015, Lorenz Esch and Matti Hamalainen. All rights reserved.
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
* @brief    BrainHemisphereTreeItem class definition.
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "brainhemispheretreeitem.h"


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace Eigen;
using namespace FSLIB;
using namespace MNELIB;
using namespace DISP3DLIB;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

BrainHemisphereTreeItem::BrainHemisphereTreeItem(int iType, const QString& text)
: AbstractTreeItem(iType, text)
{
    this->setEditable(false);    
    this->setCheckable(true);
    this->setCheckState(Qt::Checked);
    this->setToolTip("Brain hemisphere");
}


//*************************************************************************************************************

BrainHemisphereTreeItem::~BrainHemisphereTreeItem()
{
    delete m_pSurfaceItem;
    delete m_pAnnotItem;
    delete m_pBrainRTSourceLocDataTreeItem;
}


//*************************************************************************************************************

QVariant BrainHemisphereTreeItem::data(int role) const
{
    return AbstractTreeItem::data(role);
}


//*************************************************************************************************************

void  BrainHemisphereTreeItem::setData(const QVariant& value, int role)
{
    AbstractTreeItem::setData(value, role);
}


//*************************************************************************************************************

bool BrainHemisphereTreeItem::addData(const Surface& tSurface, const Annotation& tAnnotation, Qt3DCore::QEntity* p3DEntityParent)
{
    //Set name of BrainHemisphereTreeItem based on the hemisphere information
    switch (tSurface.hemi()) {
    case 0:
        this->setText("Left");
        break;
    case 1:
        this->setText("Right");
        break;
    default:
        this->setText("Unknown");
        break;
    }

    QVariant data;
    data.setValue(tSurface.hemi());

    this->setData(data, BrainHemisphereTreeItemRoles::SurfaceHemi);

    //Add childs
    bool state = false;

    //Add surface child
    m_pSurfaceItem = new BrainSurfaceTreeItem(BrainTreeModelItemTypes::SurfaceItem);
    *this<<m_pSurfaceItem;
    state = m_pSurfaceItem->addData(tSurface, p3DEntityParent);

    //Add annotation child
    if(!tAnnotation.isEmpty()) {
        m_pAnnotItem = new BrainAnnotationTreeItem(BrainTreeModelItemTypes::AnnotationItem);
        connect(m_pAnnotItem, &BrainAnnotationTreeItem::annotationVisibiltyChanged,
                m_pSurfaceItem, &BrainSurfaceTreeItem::onAnnotationVisibilityChanged);

        *this<<m_pAnnotItem;
        state = m_pAnnotItem->addData(tSurface, tAnnotation);
    }

    return state;
}


//*************************************************************************************************************

bool BrainHemisphereTreeItem::addData(const MNEHemisphere& tHemisphere, Qt3DCore::QEntity* p3DEntityParent)
{
    //Set name of BrainHemisphereTreeItem based on the hemisphere information
    QVariant data;

    switch (tHemisphere.id) {
    case 101:
        this->setText("Left");
        data.setValue(0);
        break;
    case 102:
        this->setText("Right");
        data.setValue(1);
        break;
    default:
        this->setText("Unknown");
        data.setValue(-1);
        break;
    }

    this->setData(data, BrainHemisphereTreeItemRoles::SurfaceHemi);

    //Add childs
    bool state = false;

    //Add surface child
    BrainSourceSpaceTreeItem* pSourceSpaceItem = new BrainSourceSpaceTreeItem(BrainTreeModelItemTypes::SourceSpaceItem);
    *this<<pSourceSpaceItem;
    state = pSourceSpaceItem->addData(tHemisphere, p3DEntityParent);

    return state;
}


//*************************************************************************************************************

BrainRTSourceLocDataTreeItem* BrainHemisphereTreeItem::addData(const MNESourceEstimate& tSourceEstimate, const MNEForwardSolution& tForwardSolution)
{
    if(!tSourceEstimate.isEmpty()) {
        //Add source estimation data as child
        if(this->findChildren(BrainTreeModelItemTypes::RTSourceLocDataItem).size() == 0) {
            //If rt data item does not exists yet, create it here!
            if(!tForwardSolution.isEmpty()) {
                m_pBrainRTSourceLocDataTreeItem = new BrainRTSourceLocDataTreeItem();
                *this<<m_pBrainRTSourceLocDataTreeItem;

                connect(m_pBrainRTSourceLocDataTreeItem, &BrainRTSourceLocDataTreeItem::rtVertColorChanged,
                        m_pSurfaceItem, &BrainSurfaceTreeItem::onRtVertColorChanged);
                connect(m_pSurfaceItem, &BrainSurfaceTreeItem::colorInfoOriginChanged,
                        m_pBrainRTSourceLocDataTreeItem, &BrainRTSourceLocDataTreeItem::onColorInfoOriginChanged);

                m_pBrainRTSourceLocDataTreeItem->init(tForwardSolution,
                                                m_pSurfaceItem->data(BrainSurfaceTreeItemRoles::SurfaceCurrentColorVert).value<QByteArray>(),
                                                this->data(BrainHemisphereTreeItemRoles::SurfaceHemi).toInt(),
                                                m_pAnnotItem->data(BrainAnnotationTreeItemRoles::LabeIds).value<VectorXi>(),
                                                m_pAnnotItem->data(BrainAnnotationTreeItemRoles::LabeList).value<QList<FSLIB::Label>>());

                m_pBrainRTSourceLocDataTreeItem->addData(tSourceEstimate);
            } else {
                qDebug()<<"BrainHemisphereTreeItem::addData - Cannot add real time data since the forwad solution was not provided and therefore the rt source localization data item has not been initilaized yet. Returning...";
            }
        } else {
            m_pBrainRTSourceLocDataTreeItem->addData(tSourceEstimate);
        }

        return m_pBrainRTSourceLocDataTreeItem;
    } else {
        qDebug()<<"BrainHemisphereTreeItem::addData - tSourceEstimate is empty";
    }

    return new BrainRTSourceLocDataTreeItem();
}


//*************************************************************************************************************

void BrainHemisphereTreeItem::onCheckStateChanged(const Qt::CheckState& checkState)
{
    for(int i = 0; i < this->rowCount(); i++) {
        if(this->child(i)->isCheckable()) {
            this->child(i)->setCheckState(checkState);
        }
    }
}

