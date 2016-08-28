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

#include "QFile"
#include "QTextStream"
#include "constants.h"
#include "hlms_utils_manager.h"
#include "OgreHlmsPbs.h"
#include "OgreHlmsUnlit.h"
#include "OgreHlmsPbsDatablock.h"
#include "OgreHlmsUnlitDatablock.h"
#include "OgreLogManager.h"

//****************************************************************************/
HlmsUtilsManager::HlmsUtilsManager(void)
{
    mLoadedDatablockStruct.datablock = 0;
    mLoadedDatablockStruct.datablockFullName = "";
    mLoadedDatablockStruct.datablockId = "";
    mLoadedDatablockStruct.jsonFileName = "";
    mLoadedDatablockStruct.type = EditorHlmsTypes::HLMS_NONE;

    helperDatablockStruct.datablock = 0;
    helperDatablockStruct.datablockFullName = "";
    helperDatablockStruct.datablockId = "";
    helperDatablockStruct.jsonFileName = "";
    helperDatablockStruct.type = EditorHlmsTypes::HLMS_NONE;
}

//****************************************************************************/
HlmsUtilsManager::~HlmsUtilsManager(void)
{
}

//****************************************************************************/
HlmsUtilsManager::DatablockStruct HlmsUtilsManager::loadDatablock(const QString& jsonFileName)
{
    mLoadedDatablockStruct.datablock = 0;
    mLoadedDatablockStruct.datablockFullName = "";
    mLoadedDatablockStruct.datablockId = "";
    mLoadedDatablockStruct.jsonFileName = jsonFileName.toStdString();
    mLoadedDatablockStruct.type = EditorHlmsTypes::HLMS_NONE;

    if (jsonFileName.isEmpty())
    {
        Ogre::LogManager::getSingleton().logMessage("HlmsUtilsManager::loadDatablock -> jsonFileName is empty\n");
    }
    else
    {
        // Read the json file as text file and feed it to the HlmsManager::loadMaterials() function
        // Note, that the resources (textures, etc.) must be present
        Ogre::HlmsManager* hlmsManager = Ogre::Root::getSingletonPtr()->getHlmsManager();
        QFile file(jsonFileName);
        file.open(QFile::ReadOnly | QFile::Text);
        QTextStream readFile(&file);
        QString jsonString = readFile.readAll();
        QByteArray ba = jsonString.toLatin1();
        char* jsonChar = ba.data();
        Ogre::String fname = jsonFileName.toStdString();
        Ogre::HlmsJson hlmsJson(hlmsManager);
        try
        {
            // Make snapshot of the already loaded datablocks
            makeSnapshotDatablocks();

            // Load the json file and crete the datablock(s)
            hlmsJson.loadMaterials(fname, jsonChar);
        }

        catch (Ogre::Exception e) {}

        file.close();

        // Compare the snapshot with the loaded datablocks vector that is managed by 'this'  object
        // The returned datablock is the newly created one (no exception) or the one that
        // was already loaded (catched exception)
        mLoadedDatablockStruct = compareSnapshotWithLoadedDatablocksAndAdminister(jsonFileName);

        if (!mLoadedDatablockStruct.datablock)
        {
            Ogre::LogManager::getSingleton().logMessage("HlmsUtilsManager::loadDatablock -> Could not load the materialfile\n");
        }

    }

    return mLoadedDatablockStruct;
}

//****************************************************************************/
void HlmsUtilsManager::makeSnapshotDatablocks(void)
{
    mSnapshot.clear();
    Ogre::HlmsManager* hlmsManager = Ogre::Root::getSingletonPtr()->getHlmsManager();
    Ogre::HlmsPbs* hlmsPbs = static_cast<Ogre::HlmsPbs*>( hlmsManager->getHlms(Ogre::HLMS_PBS));
    Ogre::HlmsUnlit* hlmsUnlit = static_cast<Ogre::HlmsUnlit*>( hlmsManager->getHlms(Ogre::HLMS_UNLIT));

    // Iterate through all pbs datablocks
    Ogre::Hlms::HlmsDatablockMap::const_iterator itorPbs = hlmsPbs->getDatablockMap().begin();
    Ogre::Hlms::HlmsDatablockMap::const_iterator endPbs = hlmsPbs->getDatablockMap().end();
    Ogre::HlmsPbsDatablock* pbsDatablock;
    while( itorPbs != endPbs)
    {
        pbsDatablock = static_cast<Ogre::HlmsPbsDatablock*>(itorPbs->second.datablock);
        helperDatablockStruct.datablock = pbsDatablock;
        helperDatablockStruct.datablockFullName = *pbsDatablock->getFullName();
        helperDatablockStruct.datablockId = pbsDatablock->getName();
        helperDatablockStruct.jsonFileName = "";
        helperDatablockStruct.type = EditorHlmsTypes::HLMS_PBS;
        mSnapshot.append(helperDatablockStruct);
        ++itorPbs;
    }

    // Iterate through all unlit datablocks
    Ogre::Hlms::HlmsDatablockMap::const_iterator itorUnlit = hlmsUnlit->getDatablockMap().begin();
    Ogre::Hlms::HlmsDatablockMap::const_iterator endUnlit = hlmsUnlit->getDatablockMap().end();
    Ogre::HlmsUnlitDatablock* unlitDatablock;
    while( itorUnlit != endUnlit)
    {
        unlitDatablock = static_cast<Ogre::HlmsUnlitDatablock*>(itorUnlit->second.datablock);
        helperDatablockStruct.datablock = unlitDatablock;
        helperDatablockStruct.datablockFullName = *unlitDatablock->getFullName();
        helperDatablockStruct.datablockId = unlitDatablock->getName();
        helperDatablockStruct.jsonFileName = "";
        helperDatablockStruct.type = EditorHlmsTypes::HLMS_UNLIT;
        mSnapshot.append(helperDatablockStruct);
        ++itorUnlit;
    }
}

//****************************************************************************/
void HlmsUtilsManager::reloadNonSpecialDatablocks(void)
{
    Ogre::HlmsManager* hlmsManager = Ogre::Root::getSingletonPtr()->getHlmsManager();
    QVectorIterator<DatablockStruct> itLoadedDatablocks(mLoadedDatablocks);
    while (itLoadedDatablocks.hasNext())
    {
        helperDatablockStruct = itLoadedDatablocks.next();
        if (!hlmsManager->getDatablock(helperDatablockStruct.datablockId))
        {
            // The datablock does not exist anymore; reload it
            loadDatablock(helperDatablockStruct.jsonFileName.c_str());
        }
    }
}

//****************************************************************************/
HlmsUtilsManager::DatablockStruct HlmsUtilsManager::compareSnapshotWithLoadedDatablocksAndAdminister(const QString& jsonFileName)
{
    Ogre::HlmsManager* hlmsManager = Ogre::Root::getSingletonPtr()->getHlmsManager();
    Ogre::HlmsPbs* hlmsPbs = static_cast<Ogre::HlmsPbs*>( hlmsManager->getHlms(Ogre::HLMS_PBS));
    Ogre::HlmsUnlit* hlmsUnlit = static_cast<Ogre::HlmsUnlit*>( hlmsManager->getHlms(Ogre::HLMS_UNLIT));

    // Iterate through all pbs datablocks to check whether it is also in the snapshot
    Ogre::Hlms::HlmsDatablockMap::const_iterator itorPbs = hlmsPbs->getDatablockMap().begin();
    Ogre::Hlms::HlmsDatablockMap::const_iterator endPbs = hlmsPbs->getDatablockMap().end();
    Ogre::HlmsPbsDatablock* pbsDatablock;
    DatablockStruct ogreDatablockStruct;
    QVector<DatablockStruct> diff;
    bool found = false;
    while( itorPbs != endPbs)
    {
        pbsDatablock = static_cast<Ogre::HlmsPbsDatablock*>(itorPbs->second.datablock);
        ogreDatablockStruct.datablock = pbsDatablock;
        ogreDatablockStruct.datablockFullName = *pbsDatablock->getFullName();
        ogreDatablockStruct.datablockId = pbsDatablock->getName();
        ogreDatablockStruct.jsonFileName = "";
        ogreDatablockStruct.type = EditorHlmsTypes::HLMS_PBS;

        // Check whether it is available in the snapshot
        QVectorIterator<DatablockStruct> itSnapshot(mSnapshot);
        found = false;
        while (itSnapshot.hasNext())
        {
            helperDatablockStruct = itSnapshot.next();
            if (helperDatablockStruct.datablock == ogreDatablockStruct.datablock)
            {
                found = true;
                break;
            }
        }

        // If the Pbs datablock in Ogres' HlmsManager is not in the snapshot, it is appended in the diff
        if (!found)
            diff.append(ogreDatablockStruct);

        ++itorPbs;
    }

    // Iterate through all unlit datablocks to check whether it is also in the snapshot
    Ogre::Hlms::HlmsDatablockMap::const_iterator itorUnlit = hlmsUnlit->getDatablockMap().begin();
    Ogre::Hlms::HlmsDatablockMap::const_iterator endUnlit = hlmsUnlit->getDatablockMap().end();
    Ogre::HlmsUnlitDatablock* unlitDatablock;
    while( itorUnlit != endUnlit)
    {
        unlitDatablock = static_cast<Ogre::HlmsUnlitDatablock*>(itorUnlit->second.datablock);
        ogreDatablockStruct.datablock = unlitDatablock;
        ogreDatablockStruct.datablockFullName = *unlitDatablock->getFullName();
        ogreDatablockStruct.datablockId = unlitDatablock->getName();
        ogreDatablockStruct.jsonFileName = "";
        ogreDatablockStruct.type = EditorHlmsTypes::HLMS_UNLIT;

        // Check whether it is available in the snapshot
        QVectorIterator<DatablockStruct> itSnapshot(mSnapshot);
        found = false;
        while (itSnapshot.hasNext())
        {
            helperDatablockStruct = itSnapshot.next();
            if (helperDatablockStruct.datablock == ogreDatablockStruct.datablock)
            {
                found = true;
                break;
            }
        }

        // If the Unlit datablock in Ogre's HlmsManager is not in the snapshot, it is appended in the diff
        if (!found)
            diff.append(ogreDatablockStruct);

        ++itorUnlit;
    }

    // So, 'diff'  contains the differences
    // - If 'diff' is empty, the datablock was already loaded, so the only way to get that datablock is to
    //   search for jsonFileName in mLoadedDatablocks.
    // - If 'diff' contains one entry, use it and add it to mLoadedDatablocks, because that is the one that is loaded.
    // - If 'diff' contains more than one entry, take the first, add it to mLoadedDatablocks and delete the other datablocks
    //   from Ogres' HlmsManager. Why?
    //       That is because we only allow to have one datablock per json file. Ogre allowed multiple datablocks in one json file,
    //       but that way it becomes difficult to track the relation between datablock and json file (not impossible, but then we
    //       must parse the json file in the HLMS Editor and interpret it).
    if (diff.isEmpty())
    {
        // There is no new loaded datablock; search the json filename in mLoadedDatablocks
        QVectorIterator<DatablockStruct> itLoadedDatablocks(mLoadedDatablocks);
        while (itLoadedDatablocks.hasNext())
        {
            // If the filename of the datablock is the same as the filename argument, return the datablock info
            helperDatablockStruct = itLoadedDatablocks.next();
            if (helperDatablockStruct.jsonFileName == jsonFileName.toStdString())
                return helperDatablockStruct;
        }

        // Not found
        helperDatablockStruct.datablock = 0;
        helperDatablockStruct.datablockFullName = "";
        helperDatablockStruct.datablockId = "";
        helperDatablockStruct.jsonFileName = jsonFileName.toStdString();
        helperDatablockStruct.type = EditorHlmsTypes::HLMS_NONE;
        return helperDatablockStruct;
    }

    // There is at least one entry in 'diff'
    helperDatablockStruct.datablock = diff[0].datablock;
    helperDatablockStruct.datablockFullName = diff[0].datablockFullName;
    helperDatablockStruct.datablockId = diff[0].datablockId;
    helperDatablockStruct.jsonFileName = jsonFileName.toStdString();
    helperDatablockStruct.type = diff[0].type;
    if (!isInLoadedDatablocksVec(helperDatablockStruct.datablockFullName))
        mLoadedDatablocks.append(helperDatablockStruct); // Add it to the vector

    // Destroy any datablock when there is more than one entry in 'diff'
    QVectorIterator<DatablockStruct> itDiff(diff);
    bool first = true;
    DatablockStruct diffStruct;
    while (itDiff.hasNext())
    {
        diffStruct = itDiff.next();
        if (!true)
        {
            if (hlmsManager->getDatablock(diffStruct.datablockId))
            {
                // Destroy the datablock
                if (    diffStruct.type == EditorHlmsTypes::HLMS_PBS &&
                        diffStruct.datablock != 0 &&
                        diffStruct.datablock->getLinkedRenderables().size() == 0)
                {
                    try
                    {
                        hlmsPbs->destroyDatablock(diffStruct.datablockId);
                    }
                    catch (Ogre::Exception e) {}
                }
                else if (   diffStruct.type == EditorHlmsTypes::HLMS_UNLIT &&
                            diffStruct.datablock != 0 &&
                            diffStruct.datablock->getLinkedRenderables().size() == 0)
                {
                    try
                    {
                        hlmsUnlit->destroyDatablock(diffStruct.datablockId);
                    }
                    catch (Ogre::Exception e) {}
                }
            }
        }

        // The first one is skipped, but all subsequent datablocks are destroyed
        first = false;
    }

    return helperDatablockStruct;
}

//****************************************************************************/
void HlmsUtilsManager::destroyDatablocks(bool excludeSpecialDatablocks,
                                         bool keepVecLoadedDatablock,
                                         const Ogre::String& excludeDatablockFullName)
{
    // If excludeSpecialDatablocks == true
    // - Exclude the default datablocks
    // - Exclude Axis and Highlight materials
    // - Exclude the unlit materials associated with the 'render-texture'  item (defined by 0, 1, 2, 3, ...)
    Ogre::HlmsManager* hlmsManager = Ogre::Root::getSingletonPtr()->getHlmsManager();
    Ogre::HlmsPbs* hlmsPbs = static_cast<Ogre::HlmsPbs*>( hlmsManager->getHlms(Ogre::HLMS_PBS));
    Ogre::HlmsUnlit* hlmsUnlit = static_cast<Ogre::HlmsUnlit*>( hlmsManager->getHlms(Ogre::HLMS_UNLIT));
    QString excludeName = excludeDatablockFullName.c_str();
    DatablockStruct excludeDatablockFullNameStruct;
    excludeDatablockFullNameStruct.datablock = 0;
    excludeDatablockFullNameStruct.datablockFullName = "";
    excludeDatablockFullNameStruct.datablockId = "";
    excludeDatablockFullNameStruct.jsonFileName = "";
    excludeDatablockFullNameStruct.type = EditorHlmsTypes::HLMS_NONE;
    QString numericString;
    Ogre::String fullName;
    int value;
    bool nameIsNumber = false;
    bool exclude = false;
    QVector<Ogre::HlmsPbsDatablock*> pbsDatablocksToBeDestroyed;
    QVector<Ogre::HlmsUnlitDatablock*> unlitDatablocksToBeDestroyed;

    // Iterate through all pbs datablocks and mark it to be destroyed
    Ogre::Hlms::HlmsDatablockMap::const_iterator itorPbs = hlmsPbs->getDatablockMap().begin();
    Ogre::Hlms::HlmsDatablockMap::const_iterator endPbs  = hlmsPbs->getDatablockMap().end();
    Ogre::HlmsPbsDatablock* pbsDatablock;
    while( itorPbs != endPbs)
    {
        pbsDatablock = static_cast<Ogre::HlmsPbsDatablock*>(itorPbs->second.datablock);
        fullName = *pbsDatablock->getFullName();

        // Default datablocks are NEVER detroyed
        exclude = pbsDatablock == hlmsPbs->getDefaultDatablock() || pbsDatablock == hlmsUnlit->getDefaultDatablock();

        // Check to see whether special datablocks are excluded
        if (!exclude)
            exclude = excludeSpecialDatablocks && (fullName == Magus::AXIS_MATERIAL_NAME || fullName == Magus::HIGHLIGHT_MATERIAL_NAME);

        // If not excluded because of special datablocks, check whether it must be excluded because of the excludeDatablockName
        if (!exclude)
        {
            exclude = !excludeName.isEmpty() && fullName == excludeDatablockFullName;
            if (exclude)
            {
                excludeDatablockFullNameStruct.datablock = pbsDatablock;
                excludeDatablockFullNameStruct.datablockFullName = excludeDatablockFullName;
                excludeDatablockFullNameStruct.datablockId = pbsDatablock->getName();
                excludeDatablockFullNameStruct.jsonFileName = "";
                excludeDatablockFullNameStruct.type = EditorHlmsTypes::HLMS_PBS;
            }
        }

        // If not excluded, mark it to be destroyed
        if (!exclude)
        {
            Ogre::LogManager::getSingleton().logMessage("To be destroyed: " + fullName); // DEBUG
            pbsDatablocksToBeDestroyed.append(pbsDatablock);
        }

        ++itorPbs;
    }

    // Iterate through all unlit datablocks and mark it to be destroyed
    Ogre::Hlms::HlmsDatablockMap::const_iterator itorUnlit = hlmsUnlit->getDatablockMap().begin();
    Ogre::Hlms::HlmsDatablockMap::const_iterator endUnlit  = hlmsUnlit->getDatablockMap().end();
    Ogre::HlmsUnlitDatablock* unlitDatablock;
    while( itorUnlit != endUnlit)
    {
        unlitDatablock = static_cast<Ogre::HlmsUnlitDatablock*>(itorUnlit->second.datablock);
        fullName = *unlitDatablock->getFullName();

        // Default datablocks are NEVER detroyed
        exclude = unlitDatablock == hlmsPbs->getDefaultDatablock() || unlitDatablock == hlmsUnlit->getDefaultDatablock();

        // Check to see whether special datablocks are excluded
        if (!exclude)
            exclude = excludeSpecialDatablocks && (fullName == Magus::AXIS_MATERIAL_NAME || fullName == Magus::HIGHLIGHT_MATERIAL_NAME);

        // If not excluded because of generic special datablocks, check whether it must be excluded because of
        // the other special datablocks. These are the ones with a numberic value as name and used for the
        // render-texture
        if (!exclude)
        {
            numericString = fullName.c_str();
            value = numericString.toInt(&nameIsNumber);
            exclude = nameIsNumber;
        }

        // If not excluded because of special datablocks, check whether it must be excluded because of the excludeDatablockName
        if (!exclude)
        {
            exclude = !excludeName.isEmpty() && fullName == excludeDatablockFullName;
            if (exclude)
            {
                excludeDatablockFullNameStruct.datablock = unlitDatablock;
                excludeDatablockFullNameStruct.datablockFullName = excludeDatablockFullName;
                excludeDatablockFullNameStruct.datablockId = unlitDatablock->getName();
                excludeDatablockFullNameStruct.jsonFileName = "";
                excludeDatablockFullNameStruct.type = EditorHlmsTypes::HLMS_UNLIT;
            }
        }

        // If not excluded, mark it to be destroyed
        if (!exclude)
        {
            Ogre::LogManager::getSingleton().logMessage("To be destroyed: " + fullName); // DEBUG
            unlitDatablocksToBeDestroyed.append(unlitDatablock);
        }

            ++itorUnlit;
    }

    // Destroy the datablocks (Pbs)
    QVectorIterator<Ogre::HlmsPbsDatablock*> itPbsToBeDestroyed(pbsDatablocksToBeDestroyed);
    while (itPbsToBeDestroyed.hasNext())
    {
        pbsDatablock = itPbsToBeDestroyed.next();
        if (pbsDatablock)
        {
            Ogre::LogManager::getSingleton().logMessage("Destroying Pbs: " + *pbsDatablock->getFullName()); // DEBUG
            hlmsPbs->destroyDatablock(pbsDatablock->getName());
        }
    }

    // Destroy the datablocks (Unlit)
    QVectorIterator<Ogre::HlmsUnlitDatablock*> itUnlitToBeDestroyed(unlitDatablocksToBeDestroyed);
    while (itUnlitToBeDestroyed.hasNext())
    {
        unlitDatablock = itUnlitToBeDestroyed.next();
        if (unlitDatablock)
        {
            Ogre::LogManager::getSingleton().logMessage("Destroying Unlit: " + *unlitDatablock->getFullName()); // DEBUG
            hlmsUnlit->destroyDatablock(unlitDatablock->getName());
        }
    }

    // Do not forget to clear the vector with loaded datablocks
    // Note: Do not clear mLoadedDatablocks completely in case excludeDatablockFullName has a value and is available in mLoadedDatablocks
    bool available =    excludeDatablockFullNameStruct.datablock &&
                        isInLoadedDatablocksVec(excludeDatablockFullNameStruct.datablockFullName);
    if (!keepVecLoadedDatablock)
    {
        mLoadedDatablocks.clear();
        if (available)
            mLoadedDatablocks.append(excludeDatablockFullNameStruct);
    }
}

//****************************************************************************/
bool HlmsUtilsManager::isInLoadedDatablocksVec (const Ogre::String& datablockFullName)
{
    QVectorIterator<DatablockStruct> itLoadedDatablocks(mLoadedDatablocks);
    Ogre::String fullName;
    while (itLoadedDatablocks.hasNext())
    {
        if (itLoadedDatablocks.next().datablockFullName == datablockFullName)
            return true;
    }
    return false;
}
