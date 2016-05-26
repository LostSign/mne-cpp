//=============================================================================================================
/**
* @file     custommesh.cpp
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
* @brief    CustomMesh class definition.
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "custommesh.h"


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace DISP3DLIB;
using namespace Eigen;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

CustomMesh::CustomMesh()
: Qt3DRender::QGeometryRenderer()
, m_pVertexDataBuffer(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer))
, m_pNormalDataBuffer(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer))
, m_pColorDataBuffer(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer))
, m_pIndexDataBuffer(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer))
, m_pColorAttribute(new Qt3DRender::QAttribute())
, m_iNumVert(0)
{

}


//*************************************************************************************************************

CustomMesh::CustomMesh(const MatrixX3f& tMatVert, const MatrixX3f& tMatNorm, const MatrixX3i& tMatTris, const Vector3f& tVecOffset, const QByteArray& tArrayColors)
: Qt3DRender::QGeometryRenderer()
, m_iNumVert(tMatVert.rows())
{
    this->createCustomMesh(tMatVert, tMatNorm, tMatTris, tVecOffset, tArrayColors);
}


//*************************************************************************************************************

CustomMesh::~CustomMesh()
{
}


//*************************************************************************************************************

bool CustomMesh::setVertColor(const QByteArray& tArrayColors)
{
    //Check dimensions
    if(tArrayColors.size() != m_iNumVert * 3 * (int)sizeof(float)) {
        qDebug() << "CustomMesh::updateVertColors - new color and vertices dimensions do not match or mesh data was not set yet!";
        return false;
    }

    //Update color
    m_pColorAttribute->buffer()->setData(tArrayColors);

    return true;
}


//*************************************************************************************************************

bool CustomMesh::setMeshData(const MatrixX3f& tMatVert, const MatrixX3f& tMatNorm, const MatrixX3i& tMatTris, const Vector3f& tVecOffset, const QByteArray& tArrayColors)
{
    m_iNumVert = tMatVert.rows();

    return createCustomMesh(tMatVert, tMatNorm, tMatTris, tVecOffset, tArrayColors);
}

//*************************************************************************************************************

bool CustomMesh::createCustomMesh(const MatrixX3f& tMatVert, const MatrixX3f& tMatNorm, const MatrixX3i& tMatTris, const Vector3f& tVecOffset, const QByteArray& tArrayColors)
{
    Qt3DRender::QGeometry* customGeometry = new Qt3DRender::QGeometry(this);

    m_pVertexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, customGeometry);//QSharedPointer<Qt3DRender::QBuffer>(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, customGeometry));
    m_pNormalDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, customGeometry);//QSharedPointer<Qt3DRender::QBuffer>(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, customGeometry));
    m_pColorDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, customGeometry);//QSharedPointer<Qt3DRender::QBuffer>(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, customGeometry));
    m_pIndexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, customGeometry);//QSharedPointer<Qt3DRender::QBuffer>(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, customGeometry));

    //Fill vertexBuffer with data which hold the vertices, normals and colors
    QByteArray vertexBufferData;
    vertexBufferData.resize(tMatVert.rows() * 3 * (int)sizeof(float));
    float *rawVertexArray = reinterpret_cast<float *>(vertexBufferData.data());

    QByteArray normalBufferData;
    normalBufferData.resize(tMatVert.rows() * 3 * (int)sizeof(float));
    float *rawNormalArray = reinterpret_cast<float *>(normalBufferData.data());

    QByteArray colorBufferData;
    colorBufferData.resize(tMatVert.rows() * 3 * (int)sizeof(float));
    float *rawColorArray = reinterpret_cast<float *>(colorBufferData.data());

    int idxVert = 0;
    int idxNorm = 0;
    int idxColor = 0;

    for(int i = 0; i<tMatVert.rows(); i++) {
        //Vertex
        rawVertexArray[idxVert++] = (tMatVert(i,0) + tVecOffset(0));
        rawVertexArray[idxVert++] = (tMatVert(i,1) + tVecOffset(1));
        rawVertexArray[idxVert++] = (tMatVert(i,2) + tVecOffset(2));

        //Normal
        rawNormalArray[idxNorm++] = tMatNorm(i,0);
        rawNormalArray[idxNorm++] = tMatNorm(i,1);
        rawNormalArray[idxNorm++] = tMatNorm(i,2);

        //Color (this is the default color and will be used until the updateVertColor function was called)
        if(tArrayColors.size() == 0 || tArrayColors.size() != colorBufferData.size()) {
            rawColorArray[idxColor++] = 0.5f;
            rawColorArray[idxColor++] = 0.2f;
            rawColorArray[idxColor++] = 0.2f;
        } else {
            rawColorArray[idxColor++] = tArrayColors[idxColor];
            rawColorArray[idxColor++] = tArrayColors[idxColor];
            rawColorArray[idxColor++] = tArrayColors[idxColor];
        }
    }

    //Fill indexBufferData with data which holds the triangulation information (faces/tris)
    QByteArray indexBufferData;
    indexBufferData.resize(tMatTris.rows() * 3 * (int)sizeof(uint));
    uint *rawIndexArray = reinterpret_cast<uint *>(indexBufferData.data());
    int idxTris = 0;

    for(int i = 0; i<tMatTris.rows(); i++) {
        //Faces/Tris
        rawIndexArray[idxTris++] = tMatTris(i,0);
        rawIndexArray[idxTris++] = tMatTris(i,1);
        rawIndexArray[idxTris++] = tMatTris(i,2);
    }

    //Set data to buffers
    m_pVertexDataBuffer->setData(vertexBufferData);
    m_pNormalDataBuffer->setData(normalBufferData);
    m_pColorDataBuffer->setData(colorBufferData);
    m_pIndexDataBuffer->setData(indexBufferData);

    // Attributes
    Qt3DRender::QAttribute *positionAttribute = new Qt3DRender::QAttribute();
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(m_pVertexDataBuffer);
    positionAttribute->setDataType(Qt3DRender::QAttribute::Float);
    positionAttribute->setDataSize(3);
    positionAttribute->setByteOffset(0);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(tMatVert.rows());
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    Qt3DRender::QAttribute *normalAttribute = new Qt3DRender::QAttribute();
    normalAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    normalAttribute->setBuffer(m_pNormalDataBuffer);
    normalAttribute->setDataType(Qt3DRender::QAttribute::Float);
    normalAttribute->setDataSize(3);
    normalAttribute->setByteOffset(0);
    normalAttribute->setByteStride(3 * sizeof(float));
    normalAttribute->setCount(tMatVert.rows());
    normalAttribute->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());

    m_pColorAttribute = new Qt3DRender::QAttribute();
    m_pColorAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    m_pColorAttribute->setBuffer(m_pColorDataBuffer);
    m_pColorAttribute->setDataType(Qt3DRender::QAttribute::Float);
    m_pColorAttribute->setDataSize(3);
    m_pColorAttribute->setByteOffset(0);
    m_pColorAttribute->setByteStride(3 * sizeof(float));
    m_pColorAttribute->setCount(tMatVert.rows());
    m_pColorAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());

    Qt3DRender::QAttribute *indexAttribute = new Qt3DRender::QAttribute();
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(m_pIndexDataBuffer);
    indexAttribute->setDataType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setDataSize(1);
    indexAttribute->setByteOffset(0);
    indexAttribute->setByteStride(0);
    indexAttribute->setCount(tMatTris.rows());

    customGeometry->addAttribute(positionAttribute);
    customGeometry->addAttribute(normalAttribute);
    customGeometry->addAttribute(m_pColorAttribute);
    customGeometry->addAttribute(indexAttribute);

    this->setInstanceCount(1);
    this->setBaseVertex(0);
    this->setBaseInstance(0);
    this->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    this->setGeometry(customGeometry);

    this->setPrimitiveCount(tMatTris.rows()*3);
    return true;
}
