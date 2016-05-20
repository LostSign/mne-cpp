//=============================================================================================================
/**
* @file     brainrtconnectivitydatatreeitem.h
* @author   Lorenz Esch <Lorenz.Esch@tu-ilmenau.de>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     January, 2016
*
* @section  LICENSE
*
* Copyright (C) 2016, Lorenz Esch and Matti Hamalainen. All rights reserved.
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
* @brief     BrainRTConnectivityDataTreeItem class declaration.
*
*/

#ifndef BRAINRTCONNECTIVITYDATATREEITEM_H
#define BRAINRTCONNECTIVITYDATATREEITEM_H

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "../../disp3D_global.h"

#include "../../helpers/abstracttreeitem.h"
#include "../../helpers/types.h"
#include "../../rt/rtSourceLoc/rtsourcelocdataworker.h"

#include "braintreemetaitem.h"

#include "fiff/fiff_types.h"

#include "mne/mne_sourceestimate.h"
#include "mne/mne_forwardsolution.h"


//*************************************************************************************************************
//=============================================================================================================
// Qt INCLUDES
//=============================================================================================================

#include <QList>
#include <QVariant>
#include <QStringList>
#include <QColor>
#include <QStandardItem>
#include <QStandardItemModel>


//*************************************************************************************************************
//=============================================================================================================
// Eigen INCLUDES
//=============================================================================================================

#include <Eigen/Core>


//*************************************************************************************************************
//=============================================================================================================
// DEFINE NAMESPACE DISP3DLIB
//=============================================================================================================

namespace DISP3DLIB
{


//*************************************************************************************************************
//=============================================================================================================
// FORWARD DECLARATIONS
//=============================================================================================================


//=============================================================================================================
/**
* BrainRTConnectivityDataTreeItem provides a generic item to hold information about real time connectivity data to plot onto the brain surface.
*
* @brief Provides a generic brain tree item to hold real time data.
*/
class DISP3DNEWSHARED_EXPORT BrainRTConnectivityDataTreeItem : public AbstractTreeItem
{
    Q_OBJECT

public:
    typedef QSharedPointer<BrainRTConnectivityDataTreeItem> SPtr;             /**< Shared pointer type for BrainRTConnectivityDataTreeItem class. */
    typedef QSharedPointer<const BrainRTConnectivityDataTreeItem> ConstSPtr;  /**< Const shared pointer type for BrainRTConnectivityDataTreeItem class. */

    //=========================================================================================================
    /**
    * Default constructor.
    *
    * @param[in] iType      The type of the item. See types.h for declaration and definition.
    * @param[in] text       The text of this item. This is also by default the displayed name of the item in a view.
    */
    explicit BrainRTConnectivityDataTreeItem(int iType = BrainTreeModelItemTypes::RTConnectivityDataItem, const QString& text = "RT Connectivity Data");

    //=========================================================================================================
    /**
    * Default destructor
    */
    ~BrainRTConnectivityDataTreeItem();

    //=========================================================================================================
    /**
    * AbstractTreeItem functions
    */
    QVariant data(int role = Qt::UserRole + 1) const;
    void setData(const QVariant& value, int role = Qt::UserRole + 1);

    //=========================================================================================================
    /**
    * Initializes the rt connectivity data item with neccessary information for visualization computations.
    *
    * @param[in] tForwardSolution       The MNEForwardSolution.
    * @param[in] hemi                   The hemispehre of this brain rt connectivity data item.
    *
    * @return                           Returns true if successful.
    */
    bool init(const MNELIB::MNEForwardSolution& tForwardSolution, int iHemi);

    //=========================================================================================================
    /**
    * Adds actual rt connectivity data which is streamed by this item's worker thread item. In order for this function to worker, you must call init(...) beforehand.
    *
    * @param[in] matNewConnection    The new connectivity data.
    *
    * @return                       Returns true if successful.
    */
    bool addData(const Eigen::MatrixXd& matNewConnection);

    //=========================================================================================================
    /**
    * Updates the rt connectivity data which is streamed by this item's worker thread item.
    *
    * @return                       Returns true if this item is initialized.
    */
    inline bool isInit() const;

private:
    bool                        m_bIsInit;                      /**< The init flag. */

signals:

};

//*************************************************************************************************************
//=============================================================================================================
// INLINE DEFINITIONS
//=============================================================================================================

inline bool BrainRTConnectivityDataTreeItem::isInit() const
{
    return m_bIsInit;
}

} //NAMESPACE DISP3DLIB

#endif // BRAINRTCONNECTIVITYDATATREEITEM_H
