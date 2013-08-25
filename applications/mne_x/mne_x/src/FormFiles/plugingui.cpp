//=============================================================================================================
/**
* @file     plugingui.cpp
* @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     August, 2013
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
* @brief     PluginGui class implementation
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "arrow.h"
#include "pluginitem.h"
#include "pluginscene.h"
#include "plugingui.h"


//*************************************************************************************************************
//=============================================================================================================
// Qt INCLUDES
//=============================================================================================================

#include <QtWidgets>

//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace MNEX;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

PluginGui::PluginGui(PluginManager::SPtr pPluginManager, MNEX::PluginSceneManager::SPtr pPluginSceneManager)
: m_pPluginManager(pPluginManager)
, m_pPluginSceneManager(pPluginSceneManager)
{
    createActions();
    createMenuItem();

    m_pPluginScene = new PluginScene(m_pMenuItem, this);
    m_pPluginScene->setSceneRect(QRectF(0, 0, 200, 500));
    connect(m_pPluginScene, SIGNAL(itemInserted(PluginItem*)),
            this, SLOT(itemInserted(PluginItem*)));

    createToolbars();

    m_pGraphicsView = new QGraphicsView(m_pPluginScene);

    setCentralWidget(m_pGraphicsView);
    setWindowTitle(tr("PluginScene"));
    setUnifiedTitleAndToolBarOnMac(true);
}


//*************************************************************************************************************

void PluginGui::actionGroupTriggered(QAction* action)
{
//    m_pPluginScene->setItemType(PluginItem::DiagramType(m_qMapNameType[action->text()]));
//    m_pPluginScene->setItemName(action->text());
    m_pPluginScene->setActionPluginItem(action);
    m_pPluginScene->setMode(PluginScene::InsertPluginItem);

}


//*************************************************************************************************************

void PluginGui::deleteItem()
{
    foreach (QGraphicsItem *item, m_pPluginScene->selectedItems()) {
        if (item->type() == Arrow::Type) {
            m_pPluginScene->removeItem(item);
            Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);
            arrow->startItem()->removeArrow(arrow);
            arrow->endItem()->removeArrow(arrow);
            delete item;
        }
    }

    foreach (QGraphicsItem *item, m_pPluginScene->selectedItems()) {
         if (item->type() == PluginItem::Type)
             qgraphicsitem_cast<PluginItem *>(item)->removeArrows();
         m_pPluginScene->removeItem(item);
         delete item;
     }
}


//*************************************************************************************************************

void PluginGui::pointerGroupClicked(int)
{
    m_pPluginScene->setMode(PluginScene::Mode(m_pButtonGroupPointers->checkedId()));
}


//*************************************************************************************************************

void PluginGui::bringToFront()
{
    if (m_pPluginScene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = m_pPluginScene->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() >= zValue && item->type() == PluginItem::Type)
            zValue = item->zValue() + 0.1;
    }
    selectedItem->setZValue(zValue);
}


//*************************************************************************************************************

void PluginGui::sendToBack()
{
    if (m_pPluginScene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = m_pPluginScene->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() <= zValue && item->type() == PluginItem::Type)
            zValue = item->zValue() - 0.1;
    }
    selectedItem->setZValue(zValue);
}


//*************************************************************************************************************

void PluginGui::itemInserted(PluginItem *item)
{
    Q_UNUSED(item);
    m_pButtonGroupPointers->button(int(PluginScene::MovePluginItem))->setChecked(true);
    m_pPluginScene->setMode(PluginScene::Mode(m_pButtonGroupPointers->checkedId()));
}


//*************************************************************************************************************

void PluginGui::createActions()
{
    deleteAction = new QAction(QIcon(":/images/delete.png"), tr("&Delete"), this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(tr("Delete item from diagram (Del)"));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));

    toFrontAction = new QAction(QIcon(":/images/bringtofront.png"),
                                tr("Bring to &Front"), this);
    toFrontAction->setShortcut(tr("Ctrl+F"));
    toFrontAction->setStatusTip(tr("Bring item to front (Ctrl+F)"));
    connect(toFrontAction, SIGNAL(triggered()), this, SLOT(bringToFront()));

    sendBackAction = new QAction(QIcon(":/images/sendtoback.png"), tr("Send to &Back"), this);
    sendBackAction->setShortcut(tr("Ctrl+B"));
    sendBackAction->setStatusTip(tr("Send item to back (Ctrl+B)"));
    connect(sendBackAction, SIGNAL(triggered()), this, SLOT(sendToBack()));
}


//*************************************************************************************************************

void PluginGui::createMenuItem()
{
    m_pMenuItem = new QMenu;
    m_pMenuItem->addAction(deleteAction);
    m_pMenuItem->addSeparator();
    m_pMenuItem->addAction(toFrontAction);
    m_pMenuItem->addAction(sendBackAction);
}


//*************************************************************************************************************

void PluginGui::createToolbars()
{
    //Plugins Toolbar
    m_pActionGroupPlugins = new QActionGroup(this);
    m_pActionGroupPlugins->setExclusive(false);
    connect(m_pActionGroupPlugins, SIGNAL(triggered(QAction*)),
            this, SLOT(actionGroupTriggered(QAction*)));

    QToolButton *sensorToolButton = new QToolButton;
    QMenu *menuSensors = new QMenu;
    for(qint32 i = 0; i < m_pPluginManager->getSensorPlugins().size(); ++i)
        createItemAction(m_pPluginManager->getSensorPlugins()[i]->getName(), IPlugin::_ISensor, menuSensors);


//    createItemAction(QString("ECG Simulator"), PluginItem::Sensor, menuSensors);
//    createItemAction(QString("RT Client"), PluginItem::Sensor, menuSensors);
    sensorToolButton->setMenu(menuSensors);
    sensorToolButton->setPopupMode(QToolButton::InstantPopup);
    sensorToolButton->setIcon(QIcon(":/images/sensor.png"));
    sensorToolButton->setStatusTip(tr("Sensor Plugins"));
    sensorToolButton->setToolTip(tr("Sensor Plugins"));

    QToolButton *algorithmToolButton = new QToolButton;
    QMenu *menuAlgorithms = new QMenu;
    for(qint32 i = 0; i < m_pPluginManager->getAlgorithmPlugins().size(); ++i)
        createItemAction(m_pPluginManager->getAlgorithmPlugins()[i]->getName(), IPlugin::_IAlgorithm, menuAlgorithms);

    createItemAction(QString("SourceLab"), IPlugin::_IAlgorithm, menuAlgorithms);  //DEBUG
    createItemAction(QString("RTSSS"), IPlugin::_IAlgorithm, menuAlgorithms);       //DEBUG

    algorithmToolButton->setMenu(menuAlgorithms);
    algorithmToolButton->setPopupMode(QToolButton::InstantPopup);
    algorithmToolButton->setIcon(QIcon(":/images/algorithm.png"));
    algorithmToolButton->setStatusTip(tr("Algorithm Plugins"));
    algorithmToolButton->setToolTip(tr("Algorithm Plugins"));

    QToolButton *ioToolButton = new QToolButton;
    QMenu *menuIo = new QMenu;
    for(qint32 i = 0; i < m_pPluginManager->getIOPlugins().size(); ++i)
        createItemAction(m_pPluginManager->getIOPlugins()[i]->getName(), IPlugin::_IIO, menuIo);

    createItemAction(QString("FIFF"), IPlugin::_IIO, menuIo);  //DEBUG

    ioToolButton->setMenu(menuIo);
    ioToolButton->setPopupMode(QToolButton::InstantPopup);
    ioToolButton->setIcon(QIcon(":/images/io.png"));
    ioToolButton->setStatusTip(tr("I/O Plugins"));
    ioToolButton->setToolTip(tr("I/O Plugins"));

    m_pToolBarPlugins = new QToolBar(tr("Plugins"), this);
    m_pToolBarPlugins->addWidget(sensorToolButton);
    m_pToolBarPlugins->addWidget(algorithmToolButton);
    m_pToolBarPlugins->addWidget(ioToolButton);

    m_pToolBarPlugins->setAllowedAreas(Qt::LeftToolBarArea);
    m_pToolBarPlugins->setFloatable(false);
    m_pToolBarPlugins->setMovable(false);

    addToolBar(Qt::LeftToolBarArea, m_pToolBarPlugins);


    //Pointers Toolbar
    QToolButton *pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon(":/images/pointer.png"));
    pointerButton->setShortcut(tr("Ctrl+P"));
    pointerButton->setStatusTip(tr("Select/Place (Ctrl+P)"));
    pointerButton->setToolTip(tr("Select/Place"));

    QToolButton *linePointerButton = new QToolButton;
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon(":/images/linepointer.png"));
    linePointerButton->setShortcut(tr("Ctrl+L"));
    linePointerButton->setStatusTip(tr("Connection (Ctrl+L)"));
    linePointerButton->setToolTip(tr("Connection"));

    m_pButtonGroupPointers = new QButtonGroup(this);
    m_pButtonGroupPointers->addButton(pointerButton, int(PluginScene::MovePluginItem));
    m_pButtonGroupPointers->addButton(linePointerButton, int(PluginScene::InsertLine));

    connect(m_pButtonGroupPointers, SIGNAL(buttonClicked(int)),
            this, SLOT(pointerGroupClicked(int)));

    m_pToolBarPointer = new QToolBar(tr("Pointer type"), this);

    m_pToolBarPointer->addWidget(pointerButton);
    m_pToolBarPointer->addWidget(linePointerButton);

    m_pToolBarPointer->setAllowedAreas(Qt::LeftToolBarArea);
    m_pToolBarPointer->setFloatable(false);
    m_pToolBarPointer->setMovable(false);

    addToolBar(Qt::LeftToolBarArea, m_pToolBarPointer);


    //Item
    m_pToolBarItem = new QToolBar(tr("Item"), this);

    m_pToolBarItem->addAction(deleteAction);
    m_pToolBarItem->addAction(toFrontAction);
    m_pToolBarItem->addAction(sendBackAction);

    m_pToolBarItem->setAllowedAreas(Qt::LeftToolBarArea);
    m_pToolBarItem->setFloatable(false);
    m_pToolBarItem->setMovable(false);

    addToolBar(Qt::LeftToolBarArea, m_pToolBarItem);
}


//*************************************************************************************************************

QAction* PluginGui::createItemAction(QString name, IPlugin::PluginType type, QMenu* menu)
{

    QAction* action = menu->addAction(name);
    action->setData(type);
    m_pActionGroupPlugins->addAction(action);
    return action;
}
