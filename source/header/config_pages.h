/****************************************************************************
**
** Copyright (C) 2016
**
** This file is part of the Magus toolkit
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

#ifndef CONFIG_PAGES_H
#define CONFIG_PAGES_H

#include <QWidget>
#include <QLineEdit>
#include <QRadioButton>
#include <QPushButton>
#include <QComboBox>
#include "config_dialog.h"

//****************************************************************************/
class GeneralPage : public QWidget
{
    Q_OBJECT

public:
    GeneralPage(ConfigDialog *parent = 0);

private slots:
    void doSetImportDir (void);
    void doResetAllSettings (void);

protected:
    void loadSettings(void);
    void saveSettings(void);

private:
    ConfigDialog* mParent;
    QString mImportPath;
    QLineEdit* mImportEdit;
};

//****************************************************************************/
class HlmsPage : public QWidget
{
    Q_OBJECT

public:
    HlmsPage(ConfigDialog *parent = 0);

private slots:
    void doFilterChanged(void);

private:
    ConfigDialog* mParent;
    QComboBox* mFilterList;
};

#endif
