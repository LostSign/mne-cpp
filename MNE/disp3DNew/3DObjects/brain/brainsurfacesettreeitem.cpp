//=============================================================================================================
/**
* @file     brainsurfacesettreeitem.cpp
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
* @brief    BrainSurfaceSetTreeItem class definition.
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "brainsurfacesettreeitem.h"


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace DISP3DNEWLIB;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

BrainSurfaceSetTreeItem::BrainSurfaceSetTreeItem(const int& iType, const QString& text)
: AbstractTreeItem(iType, text)
{
}


//*************************************************************************************************************

BrainSurfaceSetTreeItem::~BrainSurfaceSetTreeItem()
{
}


//*************************************************************************************************************

QVariant BrainSurfaceSetTreeItem::data(int role) const
{
    switch(role) {
        case BrainTreeModelRoles::GetSurfSetName:
            return QVariant();
    }

    return QStandardItem::data(role);
}


//*************************************************************************************************************

void  BrainSurfaceSetTreeItem::setData(const QVariant& value, int role)
{
    QStandardItem::setData(value, role);
}


//*************************************************************************************************************

bool BrainSurfaceSetTreeItem::addFsData(const SurfaceSet& tSurfaceSet, const AnnotationSet& tAnnotationSet, Qt3DCore::QEntity* p3DEntityParent)
{
    //Set name for this item based on surface data
    this->setText("Set");

    //Generate child items based on surface set input parameters
    bool state = false;

    for(int i = 0; i < tSurfaceSet.size(); i++) {
        BrainSurfaceTreeItem* pSurfaceItem = new BrainSurfaceTreeItem(BrainTreeItemTypes::SurfaceItem, "", p3DEntityParent);

        if(i < tAnnotationSet.size()) {
            if(tAnnotationSet[i].hemi() == tSurfaceSet[i].hemi()) {
                state = pSurfaceItem->addFsData(tSurfaceSet[i], tAnnotationSet[i]);
            } else {
                state = pSurfaceItem->addFsData(tSurfaceSet[i], Annotation());
            }
        } else {
            state = pSurfaceItem->addFsData(tSurfaceSet[i], Annotation());
        }

        *this<<pSurfaceItem; //same as this->appendRow(pSurfaceItem)
    }

    return state;
}
