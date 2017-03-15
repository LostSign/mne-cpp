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
#include <iostream>


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

//*************************************************************************************************************

QImage * Tpplot::creatPlotImage(MatrixXd tf_matrix, QSize imageSize, ColorMaps cmap)
{
    //normalisation of the tf-matrix
    qreal norm1 = tf_matrix.maxCoeff();
    qreal mnorm = tf_matrix.minCoeff();
    if(abs(mnorm) > norm1) norm1 = mnorm;
    tf_matrix /= norm1;

    qint32 y_factor =  tf_matrix.rows() / imageSize.height();
    qint32 x_factor =  tf_matrix.cols() / imageSize.width();
    if(y_factor == 0) y_factor = 1;
    if(x_factor == 0) x_factor = 1;

    //setup image
    QImage * image_to_tf_plot = new QImage(imageSize, QImage::Format_RGB32);

    //setup pixelcolors in image
    QColor color;
    qint32 ximage = 0;
    qint32 yimage = 0;

    for ( qint32 y = 0; y < tf_matrix.rows(); y = y + y_factor)
    {
        ximage = 0;
        for ( qint32 x = 0; x < tf_matrix.cols(); x = x + x_factor )
        {
            switch  (cmap)
            {
            case Jet:
                color.setRgb(ColorMap::valueToJet(abs(tf_matrix(y, x))));
                break;
            case Hot:
                color.setRgb(ColorMap::valueToHot(abs(tf_matrix(y, x))));
                break;
            case HotNeg1:
                color.setRgb(ColorMap::valueToHotNegative1(abs(tf_matrix(y, x))));
                break;
            case HotNeg2:
                color.setRgb(ColorMap::valueToHotNegative2(abs(tf_matrix(y, x))));
                break;
            case Bone:
                color.setRgb(ColorMap::valueToBone(abs(tf_matrix(y, x))));
                break;
            case RedBlue:
                color.setRgb(ColorMap::valueToRedBlue(abs(tf_matrix(y, x))));
                break;
            }
            if(ximage < image_to_tf_plot->width() && yimage < image_to_tf_plot->height())
                image_to_tf_plot->setPixel(ximage, image_to_tf_plot->height() - 1 - yimage,  color.rgb());
            ximage++;
        }
        yimage++;
    }
    return image_to_tf_plot;
}

MatrixXd Tpplot::calcSplineInterpolation(MatrixXd topoMatrix)
{
    typedef Spline<double,2> Spline2d;
    typedef Spline2d::PointType PointType;
    typedef Spline2d::ControlPointVectorType ControlPointVectorType;
    MatrixXd splineResultMatrix(topoMatrix.cols(), topoMatrix.rows());

    for(qint32 j = 0; j < topoMatrix.rows(); j++)
    {
        qint32 gridPoints = 0;
        for(qint32 i = 0; i < topoMatrix.cols(); i++)
        {

            if(topoMatrix(i, j) != 0)
                gridPoints++;
        }

        if(gridPoints != 0)
        {            
            VectorXd xvals(4);
            VectorXd yvals(4);

            if(gridPoints == 1)
            {
                for(qint32 i = 0; i < topoMatrix.cols(); i++)
                    if(topoMatrix(i, j) != 0)
                    {
                        xvals[2] = i;
                        yvals[2] = topoMatrix(i, j);
                    }

                xvals[0] = 0;
                yvals[0] = 0;
                xvals[1] = 1;
                yvals[1] = 0;
                xvals[3] = topoMatrix.cols() - 1;
                yvals[3] = 0;
            }
            else if(gridPoints == 2)
            {
                qint32 valsIndex = 1;
                for(qint32 i = 0; i < topoMatrix.cols(); i++)
                    if(topoMatrix(i, j) != 0)
                    {
                        xvals[valsIndex] = i;
                        yvals[valsIndex] = topoMatrix(i, j);
                        valsIndex++;
                    }

                xvals[0] = 0;
                yvals[0] = 0;
                xvals[3] = topoMatrix.cols() - 1;
                yvals[3] = 0;
            }
            else if(gridPoints == 3)
            {
                qint32 valsIndex = 1;
                for(qint32 i = 0; i < topoMatrix.cols(); i++)
                    if(topoMatrix(i, j) != 0)
                    {
                        xvals[valsIndex] = i;
                        yvals[valsIndex] = topoMatrix(i, j);
                        valsIndex++;
                    }

                xvals[0] = 0;
                yvals[0] = 0;
            }
            else
            {
                xvals.resize(gridPoints);
                yvals.resize(gridPoints);

                qint32 valsIndex = 0;
                for(qint32 i = 0; i < topoMatrix.cols(); i++)
                    if(topoMatrix(i, j) != 0)
                    {
                        xvals[valsIndex] = i;
                        yvals[valsIndex] = topoMatrix(i, j);
                        valsIndex++;
                    }
            }

            std::cout << "xvals=" << xvals << " --- " << std::endl;
            std::cout << "yvals=" << yvals << std::endl;
            //std::cout << yvals[1] << "---";
            //std::cout << xvals.rows() << "---";

            ControlPointVectorType points(2, xvals.rows());
            points.row(0) = xvals;
            points.row(1) = yvals;
            const Spline2d spline = SplineFitting<Spline2d>::Interpolate(points, 3);


            qint32 index = 0;
            for(qreal u = 0; u <= 1; u += 1.0 /(topoMatrix.cols() - 1))
            {
                PointType y = spline(u);
                // std::cout << "(" << u << ":" << y(0,0) << "," << y(1,0) << ") " << std::endl;
                splineResultMatrix(index, j) = y(1, 0);
                index++;
            }            
        }
        else
        {
            std::cout << "Nullzeile" << std::endl;
            for(int i = 0; i < splineResultMatrix.cols(); i++)
                splineResultMatrix(i, j) = 0;
        }
    }
    return splineResultMatrix;
}


















