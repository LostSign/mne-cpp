//=============================================================================================================
/**
* @file     listener.cpp
* @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Jens Haueisen <jens.haueisen@tu-ilmenau.de>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     January, 2013
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
* @brief    Listener class implementation
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "listener.h"
#include <mne/mne_sourceestimate.h>
#include <iostream>


//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QTextStream>


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace MNELIB;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

Listener::Listener(QObject *parent)
: QThread(parent)
, m_bIsRunning(false)
{

}


//*************************************************************************************************************

Listener::~Listener()
{
    if(this->isRunning())
        this->stop();
}


//*************************************************************************************************************

bool Listener::start()
{
    QThread::start();
    m_bIsRunning = true;
    return true;
}


//*************************************************************************************************************

bool Listener::stop()
{
    m_bIsRunning = false;
    return true;
}

//*************************************************************************************************************

void Listener::run()
{
    QTextStream cin(stdin, QIODevice::ReadOnly);
    QString input;
    while(!cin.atEnd() && m_bIsRunning)
    {
        //QString line = cin.readLine();
        cin >> input;

        if(!input.compare("--stcstream") || !input.compare("-stcstream"))
        {
            QSharedPointer<MNELIB::MNESourceEstimate> stc(new MNELIB::MNESourceEstimate);
            MNESourceEstimate::readFromTxtStream(cin, *stc);
            emit sourceEstimateAvailable(stc);
            printf("stc stream received\n");
        }
        else
            printf("Unknown command %s\n", input.toLatin1().data());
    }

    printf("Listening ended\n");
}
