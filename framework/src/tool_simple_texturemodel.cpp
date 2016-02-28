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

#include <QIcon>
#include <QMimeData>
#include <QMessageBox>
#include "tool_simple_texturemodel.h"

//****************************************************************************/
QtSimpleTextureModel::QtSimpleTextureModel(QObject *parent) : QAbstractListModel(parent)
{
    mMimeType = QString("texture/selection");
}

//****************************************************************************/
QtSimpleTextureModel::~QtSimpleTextureModel()
{
}

//****************************************************************************/
Qt::ItemFlags QtSimpleTextureModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return QAbstractListModel::flags(index) | Qt::ItemIsDragEnabled;

    return Qt::ItemIsDropEnabled;
}

//****************************************************************************/
int QtSimpleTextureModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return mPixmaps.size();
}

//****************************************************************************/
QVariant QtSimpleTextureModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DecorationRole)
    {
        return QIcon(mPixmaps.value(index.row()));
    }
    else if (role == Qt::UserRole)
    {
        return mPixmaps.value(index.row());
    }
    else if (role == Qt::UserRole + 1)
    {
        return mNames.value(index.row());
    }

    return QVariant();
}

//****************************************************************************/
void QtSimpleTextureModel::clear()
{
    beginResetModel();
    mPixmaps.clear();
    mNames.clear();
    endResetModel();
}

//****************************************************************************/
bool QtSimpleTextureModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;

    if (row >= mPixmaps.size() || row + count <= 0)
        return false;

    int beginRow = qMax(0, row);
    int endRow = qMin(row + count - 1, mPixmaps.size() - 1);

    beginRemoveRows(parent, beginRow, endRow);

    while (beginRow <= endRow)
    {
        mPixmaps.removeAt(beginRow);
        mNames.removeAt(beginRow);
        ++beginRow;
    }

    endRemoveRows();
    return true;
}

//****************************************************************************/
QStringList QtSimpleTextureModel::mimeTypes() const
{
    QStringList types;
    types << mMimeType;
    return types;
}

//****************************************************************************/
QMimeData* QtSimpleTextureModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData* mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            QPixmap pixmap = qvariant_cast<QPixmap>(data(index, Qt::UserRole));
            QString name = data(index, Qt::UserRole+1).toString();
            stream << pixmap << name;
        }
    }

    mimeData->setData(mMimeType, encodedData);
    return mimeData;
}

//****************************************************************************/
bool QtSimpleTextureModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                               int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    if (column > 0)
        return false;

    int endRow;

    if (!parent.isValid()) {
        if (row < 0)
            endRow = mPixmaps.size();
        else
            endRow = qMin(row, mPixmaps.size());
    } else {
        endRow = parent.row();
    }

    QByteArray encodedData;
    if (data->hasFormat(mMimeType))
        encodedData = data->data(mMimeType);

    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    while (!stream.atEnd()) {
        QPixmap pixmap;
        QString name;
        stream >> pixmap >> name;

        beginInsertRows(QModelIndex(), endRow, endRow);
        mPixmaps.insert(endRow, pixmap);
        mNames.insert(endRow, name);
        endInsertRows();

        ++endRow;
    }

    return true;
}

//****************************************************************************/
Qt::DropActions QtSimpleTextureModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

//****************************************************************************/
void QtSimpleTextureModel::addTexture(const QPixmap& pixmap, const QString& name, QSize size)
{
    int row = mPixmaps.size();
    beginInsertRows(QModelIndex(), row, row);
    pixmap.scaled(size.width(),
                  size.height(),
                  Qt::KeepAspectRatio,
                  Qt::SmoothTransformation);
    mPixmaps.insert(row, pixmap);
    mNames.insert(row, name);
    endInsertRows();
}

//****************************************************************************/
void QtSimpleTextureModel::addTexture(const QString& fileName, QSize size)
{
    int row = mPixmaps.size();
    beginInsertRows(QModelIndex(), row, row);
    QPixmap mTexturePixmap = QPixmap(fileName).scaled(size.width(),
                                                      size.height(),
                                                      Qt::KeepAspectRatio,
                                                      Qt::SmoothTransformation);


    mPixmaps.insert(row, mTexturePixmap);
    mNames.insert(row, fileName);
    endInsertRows();
}

//****************************************************************************/
void QtSimpleTextureModel::deleteTexture(const QString& name)
{
    int row = -1;
    foreach(QString textureName, mNames)
    {
        ++row;
        if (name == textureName)
        {
            removeRows(row, 1, QModelIndex());
            return;
        }
    }
}

//****************************************************************************/
const QString& QtSimpleTextureModel::getName(const QModelIndex &index) const
{
    return mNames.value(index.row());
}
