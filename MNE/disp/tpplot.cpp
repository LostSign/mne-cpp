//=============================================================================================================
/**
* @file     tpplot.cpp
* @author   Martin Henfling <martin.henfling@tu-ilmenau.de>;
*           Daniel Knobl <daniel.knobl@tu-ilmenau.de>;
* @version  1.0
* @date     September, 2015
*
* @section  LICENSE
*
* Copyright (C) 2014, Martin Henfling and Daniel Knobl. All rights reserved.
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
* @brief    Implementation of topo plot class.
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "tpplot.h"
#include "math.h"
#include <limits>


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace DISPLIB;

Tpplot::Tpplot() {}

QMap<QString,QPointF> Tpplot::createMapGrid(QMap<QString,QPointF> layoutMap, QSize topo_matrix_size)
{    
    qreal minXCoor = std::numeric_limits<int>::max();
    qreal maxXCoor = std::numeric_limits<int>::min();
    qreal minYCoor = std::numeric_limits<int>::max();
    qreal maxYCoor = std::numeric_limits<int>::min();
    qreal factorXCoor = 0;
    qreal factorYCoor = 0;
    QMapIterator<QString, QPointF> coor(layoutMap);

    // find min and max values
    while (coor.hasNext())
    {
        coor.next();
        if(coor.value().x() < minXCoor)
            minXCoor = coor.value().x();
        if(coor.value().x() > maxXCoor)
            maxXCoor = coor.value().x();
        if(coor.value().y() < minYCoor)
            minYCoor = coor.value().y();
        if(coor.value().y() > maxYCoor)
            maxYCoor = coor.value().y();
    }

    // 2px at the left edge
    minXCoor -= 2;
    minYCoor -= 2;
                                                                // 2px at the right edge
    factorXCoor = (maxXCoor - minXCoor) / (qreal)(topo_matrix_size.width() - 2);
    factorYCoor = (maxYCoor - minYCoor) / (qreal)(topo_matrix_size.height() - 2);

    // reset iterator
    coor.toFront();
    while (coor.hasNext())
    {
        coor.next();
        layoutMap[coor.key()] = QPointF(round((coor.value().x() - minXCoor) / factorXCoor), round((coor.value().y() - minYCoor) / factorYCoor));

        // just safty :)
        if(layoutMap[coor.key()].x() >= topo_matrix_size.width())
            layoutMap[coor.key()].setX(topo_matrix_size.width() - 1);
        if(layoutMap[coor.key()].y() >= topo_matrix_size.height())
            layoutMap[coor.key()].setY(topo_matrix_size.height() - 1);
        if(layoutMap[coor.key()].x() < 0)
            layoutMap[coor.key()].setX(0);
        if(layoutMap[coor.key()].y() < 0)
            layoutMap[coor.key()].setY(0);
    }
    return  layoutMap;
}

MatrixXd Tpplot::normSignal(MatrixXd signalMatrix)
{
    //normalisation foreach channel
    for(qint32 chnI = 0; chnI < signalMatrix.cols(); chnI++)   // over all Channels
    {
        VectorXd channel = signalMatrix.col(chnI);

        qreal max = channel.maxCoeff();
        qreal min = channel.minCoeff();
        for(qint32 i = 0; i < channel.rows(); i++)
            channel(i) -= min;
        channel /= ((max - min) / 1.0);
        signalMatrix.col(chnI) = channel;
    }
    return signalMatrix;
}

MatrixXd Tpplot::createTopoMatrix(MatrixXd signal, QMap<QString,QPointF> mapGrid, QSize topo_matrix_size, qreal timeSample)
{
    qint32 channel = 0;
    MatrixXd tp_map =  MatrixXd::Zero(topo_matrix_size.height(), topo_matrix_size.width());

    // count of elektrods != count auf channels
    if(mapGrid.count() != signal.cols())
        return tp_map;

    QMapIterator<QString, QPointF> coor(mapGrid);
    while (coor.hasNext())
    {
        coor.next();
        qreal value = signal(timeSample, channel);
        tp_map(mapGrid[coor.key()].y(), mapGrid[coor.key()].x()) = value;
        channel++;
    }
    return tp_map;
}


















