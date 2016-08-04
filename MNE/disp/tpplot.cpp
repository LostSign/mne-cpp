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

Tpplot::Tpplot()
{

}

MatrixXd Tpplot::createMapGrid(MatrixXd signal, QStringList chn_names , QMap<QString,QPointF> layoutMap, QSize topo_matrix_size)
{
    qreal minXCoor = std::numeric_limits<int>::max();
    qreal maxXCoor = std::numeric_limits<int>::min();
    qreal minYCoor = std::numeric_limits<int>::max();
    qreal maxYCoor = std::numeric_limits<int>::min();
    qreal factorXCoor = 0;
    qreal factorYCoor = 0;
    MatrixXd tp_map =  MatrixXd::Zero(topo_matrix_size.height(), topo_matrix_size.width());
    QMapIterator<QString, QPointF> coor(layoutMap);

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
    coor.toFront();

    factorXCoor = (maxXCoor - minXCoor) / topo_matrix_size.width();
    factorYCoor = (maxYCoor - minYCoor) / topo_matrix_size.height();


    qint32 index = 0;
    while (coor.hasNext())
    {
        coor.next();
        layoutMap[chn_names.at(index)] = QPointF(((coor.value().x()) - minXCoor) / factorXCoor, ((coor.value().y() - minYCoor)) / factorYCoor);
        if(round(layoutMap[chn_names.at(index)].x()) < topo_matrix_size.width() && round(layoutMap[chn_names.at(index)].y()) < topo_matrix_size.height())
            tp_map(round(layoutMap[chn_names.at(index)].x()), round(layoutMap[chn_names.at(index)].y())) = 1;//signal(0, index);
        index++;
    }

    return  tp_map;
}
