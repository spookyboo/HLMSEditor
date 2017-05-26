/****************************************************************************
**
** Copyright (C) 2016 - 2017
**
** This file is generated by the Magus toolkit
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/

#ifndef MAGUS_NODE_PORT_TYPE_H
#define MAGUS_NODE_PORT_TYPE_H

#include <QVector>
#include "node_constants.h"

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

namespace Magus
{
    /****************************************************************************
    QtPortType defines the type of QtPort and is used to determine the policy
    of a QtPort. This means that there is a restriction to which other porttype
    it can be connected
    ***************************************************************************/
    class QtPortType
    {
        public:
            unsigned int mPortType;
            QtPortType(void);
            virtual ~QtPortType(void);

            // Determines which types of ports are allowed.
            void addPortTypeToConnectionPolicy(const QtPortType& portType);

            // Check whether it is allowed to establish a connection with another port
            bool isConnectionAllowed(const QtPortType& portType);

            // Clear the allowed porttypes
            void clearConnectionPolicy (void);

        private:
            QVector<unsigned int> mConnectionPolicy;
    };


    /****************************************************************************
    The QtInputPort is a standard port
    ***************************************************************************/
    class QtInputPortType : public QtPortType
    {
        public:
            QtInputPortType(void);
            ~QtInputPortType(void){}
    };

    /****************************************************************************
    The QtOutputPort is a standard port
    ***************************************************************************/
    class QtOutputPortType : public QtPortType
    {
        public:
            QtOutputPortType(void);
            ~QtOutputPortType(void){}
    };
}

#endif
