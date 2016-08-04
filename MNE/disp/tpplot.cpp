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

void Tpplot::createMapGrid(MatrixXd signal, QStringList chn_names , QMap<QString,QPointF> layoutMap)
{
    qint32 minXCoor = std::numeric_limits<int>::max();
    qint32 maxXCoor = std::numeric_limits<int>::min();
    qint32 minYCoor = std::numeric_limits<int>::max();
    qint32 maxYCoor = std::numeric_limits<int>::min();
    MatrixXd tp_map =  MatrixXd::Zero(64, 64);
    QMapIterator<QString, QPointF> coor(layoutMap);



    while (coor.hasNext())
    {
        channel.next();
        if(coor.value().x() < minXCoor)
            minXCoor = coor.value().x();
        if(coor.value().x > maxXCoor)
            maxXCoor = coor.value().x();
        if(coor.value().y() < minYCoor)
            minYCoor = coor.value().y();
        if(coor.value().y > maxYCoor)
            maxYCoor = coor.value().y();
    }

}
