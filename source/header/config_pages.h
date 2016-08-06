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

//****************************************************************************/
class GeneralPage : public QWidget
{
    Q_OBJECT

public:
    GeneralPage(QWidget *parent = 0);
    //const QString& getIconDir(void) const;
    //const QString& getTemplateDir(void) const;
    //const QString& getLayoutDir(void) const;

//private slots:
    //void textEdited(void);
    //void restorePushed(void);

//private:
    //void restoreValues(const QString& configFileName);
    //QString mIconDir;
    //QString mTemplateDir;
    //QString mLayoutDir;
    //QLineEdit* mIconDirEdit;
    //QLineEdit* mTemplateDirEdit;
    //QLineEdit* mLayoutDirEdit;
    //QPushButton* mRestoreButton;
};

//****************************************************************************/
class HlmsPage : public QWidget
{
    Q_OBJECT

public:
    HlmsPage(QWidget *parent = 0);
    //const QString& getOutputDir(void) const;

//private slots:
    //void textEdited(void);
    //void restorePushed(void);

//private:
    //void restoreValues(const QString& configFileName);
    //QString mOutputDir;
    //QLineEdit* mOutputDirEdit;
    //QPushButton* mRestoreButton;
};

#endif
