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
    mRegisteredDatablockStruct.datablock = 0;
    mRegisteredDatablockStruct.datablockFullName = "";
    mRegisteredDatablockStruct.datablockId = "";
    mRegisteredDatablockStruct.jsonFileName = "";
    mRegisteredDatablockStruct.type = HLMS_NONE;
    mRegisteredDatablockStruct.textureMap.clear();

    helperDatablockStruct.datablock = 0;
    helperDatablockStruct.datablockFullName = "";
    helperDatablockStruct.datablockId = "";
    helperDatablockStruct.jsonFileName = "";
    helperDatablockStruct.type = HLMS_NONE;
    helperDatablockStruct.textureMap.clear();
    mSnapshot.clear();
    mRegisteredDatablocks.clear();
    helperString = "";
}

//****************************************************************************/
HlmsUtilsManager::~HlmsUtilsManager(void)
{
}

//****************************************************************************/
HlmsUtilsManager::DatablockStruct HlmsUtilsManager::loadDatablock(const QString& jsonFileName,
                                                                  bool makeSnaphot)
{
    mRegisteredDatablockStruct.datablock = 0;
    mRegisteredDatablockStruct.datablockFullName = "";
    mRegisteredDatablockStruct.datablockId = "";
    mRegisteredDatablockStruct.jsonFileName = jsonFileName.toStdString();
    mRegisteredDatablockStruct.type = HLMS_NONE;
    mRegisteredDatablockStruct.textureMap.clear();

    if (jsonFileName.isEmpty())
    {
        Ogre::LogManager::getSingleton().logMessage("HlmsUtilsManager::loadDatablock: jsonFileName " + jsonFileName.toStdString() + " is empty");
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
            if (makeSnaphot)
            {
                // Make snapshot of the already registered datablocks
                makeSnapshotDatablocks();
            }

            // Load the json file and create the datablock(s)
            hlmsJson.loadMaterials(fname, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, jsonChar);
        }

        catch (Ogre::Exception e)
        {
        }

        file.close();

        if (makeSnaphot)
        {
            // Compare the snapshot with the registered datablocks vector that is managed by 'this'  object
            // The returned datablock is the newly created one (no exception) or the one that
            // was already registered (catched exception)
            mRegisteredDatablockStruct = compareSnapshotWithRegisteredDatablocksAndAdminister(jsonFileName, jsonChar);

            if (!mRegisteredDatablockStruct.datablock)
            {
                Ogre::LogManager::getSingleton().logMessage("HlmsUtilsManager::loadDatablock -> Could not load the materialfile\n");
            }
        }

    }

    return mRegisteredDatablockStruct;
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
        helperDatablockStruct.type = HLMS_PBS;
        helperDatablockStruct.textureMap.clear();   // It is not a problem that the texture names are empty, because
                                                    // the names are not required in the snapshot. The snapshot is
                                                    // only for comparing an actual situation (the actual datablocks)
                                                    // with a situation after a json file was loaded.
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
        helperDatablockStruct.type = HLMS_UNLIT;
        helperDatablockStruct.textureMap.clear();
        mSnapshot.append(helperDatablockStruct);
        ++itorUnlit;
    }
}

//****************************************************************************/
void HlmsUtilsManager::reloadNonSpecialDatablocks(void)
{
    // Although mRegisteredDatablocks suggests that it concerns loaded datablocks this is not
    // always true. In some cases mRegisteredDatablocks contains a list of previously loaded
    // datablocks, but these datablocks are not actually in (Ogre) memory.
    // mRegisteredDatablocks also refers to datablocks that were loaded before a certain
    // action took place.
    QVectorIterator<DatablockStruct> itRegisteredDatablocks(mRegisteredDatablocks);
    while (itRegisteredDatablocks.hasNext())
    {
        helperDatablockStruct = itRegisteredDatablocks.next();
        loadDatablock(helperDatablockStruct.jsonFileName.c_str(), false);
    }
}

//****************************************************************************/
HlmsUtilsManager::DatablockStruct HlmsUtilsManager::compareSnapshotWithRegisteredDatablocksAndAdminister(const QString& jsonFileName, const char* jsonChar)
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
        ogreDatablockStruct.type = HLMS_PBS;
        ogreDatablockStruct.textureMap.clear();

        // Check whether it is available in the snapshot
        QVectorIterator<DatablockStruct> itSnapshot(mSnapshot);
        found = false;
        while (itSnapshot.hasNext())
        {
            helperDatablockStruct = itSnapshot.next();
            if (helperDatablockStruct.datablockId == ogreDatablockStruct.datablockId)
            {
                found = true;
                //Ogre::LogManager::getSingleton().logMessage("HlmsUtilsManager::loadDatablock: " + helperDatablockStruct.datablockFullName + " available in snapshot"); //DEBUG
                break;
            }
        }

        // If the Pbs datablock in Ogres' HlmsManager is not in the snapshot, it is appended in the diff
        if (!found)
        {
            diff.append(ogreDatablockStruct);
            //Ogre::LogManager::getSingleton().logMessage("HlmsUtilsManager::loadDatablock: " + ogreDatablockStruct.datablockFullName + " not available in snapshot"); // DEBUG
        }

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
        ogreDatablockStruct.type = HLMS_UNLIT;

        // Check whether it is available in the snapshot
        QVectorIterator<DatablockStruct> itSnapshot(mSnapshot);
        found = false;
        while (itSnapshot.hasNext())
        {
            helperDatablockStruct = itSnapshot.next();
            if (helperDatablockStruct.datablockId == ogreDatablockStruct.datablockId)
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
    // - If 'diff' is empty, the datablock was already registered, so the only way to get that datablock is to
    //   search for jsonFileName in mRegisteredDatablocks.
    // - If 'diff' contains one entry, use it and add it to mRegisteredDatablocks, because that is the one that is newly loaded.
    // - If 'diff' contains more than one entry, take the first, add it to mRegisteredDatablocks and delete the other datablocks
    //   from Ogres' HlmsManager. Why?
    //       That is because we only allow to have one datablock per json file. Ogre allows multiple datablocks in one json file,
    //       but that way it becomes difficult to track the relation between datablock and json file (not impossible, but then we
    //       must parse the json file in the HLMS Editor and interpret it).
    if (diff.isEmpty())
    {
        // There is no new loaded datablock; search the json filename in mRegisteredDatablocks
        QVectorIterator<DatablockStruct> itRegisteredDatablocks(mRegisteredDatablocks);
        while (itRegisteredDatablocks.hasNext())
        {
            // If the filename of the datablock is the same as the filename argument, return the datablock info
            // After it has been enriched with the texture names
            helperDatablockStruct = itRegisteredDatablocks.next();
            if (helperDatablockStruct.jsonFileName == jsonFileName.toStdString())
            {
                // Enrich the helperDatablockStruct with texture names
                parseJsonAndRetrieveDetails(&helperDatablockStruct, jsonChar);
                return helperDatablockStruct;
            }
        }

        // Not found
        helperDatablockStruct.datablock = 0;
        helperDatablockStruct.datablockFullName = "";
        helperDatablockStruct.datablockId = "";
        helperDatablockStruct.jsonFileName = jsonFileName.toStdString();
        helperDatablockStruct.type = HLMS_NONE;
        helperDatablockStruct.textureMap.clear();
        return helperDatablockStruct;
    }
    else
    {
        // There is at least one entry in 'diff'
        helperDatablockStruct.datablock = diff[0].datablock;
        helperDatablockStruct.datablockFullName = diff[0].datablockFullName;
        helperDatablockStruct.datablockId = diff[0].datablockId;
        helperDatablockStruct.jsonFileName = jsonFileName.toStdString();
        helperDatablockStruct.type = diff[0].type;
        helperDatablockStruct.textureMap.clear();

        if (!isInRegisteredDatablocksVec(helperDatablockStruct.datablockFullName))
        {
            // Enrich the helperDatablockStruct with texture names
            parseJsonAndRetrieveDetails(&helperDatablockStruct, jsonChar);

            // Add it to the vector with registered datablocks
            mRegisteredDatablocks.append(helperDatablockStruct); // Add it to the vector
            Ogre::LogManager::getSingleton().logMessage("HlmsUtilsManager::loadDatablock: " + helperDatablockStruct.datablockFullName + " added to snapshot"); // DEBUG
        }

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
                    if (    diffStruct.type == HLMS_PBS &&
                            diffStruct.datablock != 0 &&
                            diffStruct.datablock->getLinkedRenderables().size() == 0)
                    {
                        try
                        {
                            hlmsPbs->destroyDatablock(diffStruct.datablockId);
                        }
                        catch (Ogre::Exception e) {}
                    }
                    else if (   diffStruct.type == HLMS_UNLIT &&
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
    }

    return helperDatablockStruct;
}

//****************************************************************************/
void HlmsUtilsManager::destroyDatablocks(bool excludeSpecialDatablocks,
                                         bool keepVecRegisteredDatablock,
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
    excludeDatablockFullNameStruct.type = HLMS_NONE;
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
                excludeDatablockFullNameStruct.type = HLMS_PBS;
            }
        }

        // If not excluded, mark it to be destroyed
        if (!exclude)
        {
            //Ogre::LogManager::getSingleton().logMessage("To be destroyed: " + fullName); // DEBUG
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
                excludeDatablockFullNameStruct.type = HLMS_UNLIT;
            }
        }

        // If not excluded, mark it to be destroyed
        if (!exclude)
        {
            //Ogre::LogManager::getSingleton().logMessage("To be destroyed: " + fullName); // DEBUG
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
            //Ogre::LogManager::getSingleton().logMessage("Destroying Pbs: " + *pbsDatablock->getFullName()); // DEBUG
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
            //Ogre::LogManager::getSingleton().logMessage("Destroying Unlit: " + *unlitDatablock->getFullName()); // DEBUG
            hlmsUnlit->destroyDatablock(unlitDatablock->getName());
        }
    }

    // Do not forget to clear the vector with registered datablocks
    // Note: Do not clear mRegisteredDatablocks completely in case excludeDatablockFullName has a value and is available in mRegisteredDatablocks
    bool available =    excludeDatablockFullNameStruct.datablock &&
                        isInRegisteredDatablocksVec(excludeDatablockFullNameStruct.datablockFullName);
    if (!keepVecRegisteredDatablock)
    {
        mRegisteredDatablocks.clear();
        if (available)
            mRegisteredDatablocks.append(excludeDatablockFullNameStruct);
    }
}

//****************************************************************************/
void HlmsUtilsManager::destroyDatablock(const Ogre::IdString& datablockId)
{
    Ogre::HlmsManager* hlmsManager = Ogre::Root::getSingletonPtr()->getHlmsManager();
    Ogre::HlmsPbs* hlmsPbs = static_cast<Ogre::HlmsPbs*>( hlmsManager->getHlms(Ogre::HLMS_PBS));
    Ogre::HlmsUnlit* hlmsUnlit = static_cast<Ogre::HlmsUnlit*>( hlmsManager->getHlms(Ogre::HLMS_UNLIT));
    Ogre::HlmsDatablock* datablock;
    bool destroyed = false;

    // Try to delete the datablock when it is a pbs
    try
    {
        datablock = hlmsPbs->getDatablock(datablockId);
        if (datablock)
        {
            hlmsPbs->destroyDatablock(datablockId);
            destroyed = true;
        }
    }
    catch (Ogre::Exception e) {}

    // Try to delete the datablock when it is an unlit
    try
    {
        datablock = hlmsUnlit->getDatablock(datablockId);
        if (datablock)
        {
            hlmsUnlit->destroyDatablock(datablockId);
            destroyed = true;
        }
    }
    catch (Ogre::Exception e) {}

    // Also delete it from mRegisteredDatablocks (if present)
    QVector<DatablockStruct>::iterator itRegisteredDatablocks = mRegisteredDatablocks.begin();
    QVector<DatablockStruct>::iterator itRegisteredDatablocksEnd = mRegisteredDatablocks.end();
    DatablockStruct datablockStruct;
    while (itRegisteredDatablocks != itRegisteredDatablocksEnd)
    {
        datablockStruct = *itRegisteredDatablocks;
        if (datablockStruct.datablockId == datablockId)
            mRegisteredDatablocks.erase(itRegisteredDatablocks);
        ++itRegisteredDatablocks;
    }
}


//****************************************************************************/
bool HlmsUtilsManager::isInRegisteredDatablocksVec (const Ogre::String& datablockFullName)
{
    QVectorIterator<DatablockStruct> itRegisteredDatablocks(mRegisteredDatablocks);
    while (itRegisteredDatablocks.hasNext())
    {
        if (itRegisteredDatablocks.next().datablockFullName == datablockFullName)
            return true;
    }
    return false;
}

//****************************************************************************/
HlmsUtilsManager::DatablockStruct HlmsUtilsManager::getDatablockStructOfFullName (const Ogre::String& datablockFullName)
{
    helperDatablockStruct.datablock = 0;
    helperDatablockStruct.datablockFullName = "";
    helperDatablockStruct.datablockId = "";
    helperDatablockStruct.jsonFileName = "";
    helperDatablockStruct.type = HLMS_NONE;

    QVectorIterator<DatablockStruct> itRegisteredDatablocks(mRegisteredDatablocks);
    DatablockStruct datablockStruct;
    while (itRegisteredDatablocks.hasNext())
    {
        datablockStruct = itRegisteredDatablocks.next();
        if (datablockStruct.datablockFullName == datablockFullName)
        {
            helperDatablockStruct = datablockStruct;
            return helperDatablockStruct;
        }
    }

    return helperDatablockStruct;
}

//****************************************************************************/
bool HlmsUtilsManager::parseJsonAndRetrieveDetails (HlmsUtilsManager::DatablockStruct* datablockStruct, const char* jsonChar)
{
    rapidjson::Document d;
    d.Parse( jsonChar );
    if( d.HasParseError() )
    {
        Ogre::LogManager::getSingleton().logMessage("HlmsUtilsManager::parseJsonAndRetrieveDetails: Cannot parse " + Ogre::String(jsonChar));
        return false;
    }

    // Pbs: Retrieve the texture names and set them in the datablockStruct
    rapidjson::Value::ConstMemberIterator itorPbs = d.FindMember("pbs");
    if( itorPbs != d.MemberEnd() && itorPbs->value.IsObject() )
    {
        // Only get the first datablock; ignore the rest (this IS an editor, so I am in control).
        rapidjson::Value::ConstMemberIterator itorDatablock = itorPbs->value.MemberBegin();
        if (itorDatablock != itorPbs->value.MemberEnd() && itorDatablock->value.IsObject() )
        {
            // Parse all texture types
            parsePbsTextureType(datablockStruct, itorDatablock->value, "diffuse", Ogre::PBSM_DIFFUSE);
            parsePbsTextureType(datablockStruct, itorDatablock->value, "normal", Ogre::PBSM_NORMAL);
            parsePbsTextureType(datablockStruct, itorDatablock->value, "specular", Ogre::PBSM_SPECULAR);
            parsePbsTextureType(datablockStruct, itorDatablock->value, "metallness", Ogre::PBSM_METALLIC);
            parsePbsTextureType(datablockStruct, itorDatablock->value, "roughness", Ogre::PBSM_ROUGHNESS);
            parsePbsTextureType(datablockStruct, itorDatablock->value, "detail_weight", Ogre::PBSM_DETAIL_WEIGHT);
            parsePbsTextureType(datablockStruct, itorDatablock->value, "detail_diffuse0", Ogre::PBSM_DETAIL0);
            parsePbsTextureType(datablockStruct, itorDatablock->value, "detail_diffuse1", Ogre::PBSM_DETAIL1);
            parsePbsTextureType(datablockStruct, itorDatablock->value, "detail_diffuse2", Ogre::PBSM_DETAIL2);
            parsePbsTextureType(datablockStruct, itorDatablock->value, "detail_diffuse3", Ogre::PBSM_DETAIL3);
            parsePbsTextureType(datablockStruct, itorDatablock->value, "detail_normal0", Ogre::PBSM_DETAIL0_NM);
            parsePbsTextureType(datablockStruct, itorDatablock->value, "detail_normal1", Ogre::PBSM_DETAIL1_NM);
            parsePbsTextureType(datablockStruct, itorDatablock->value, "detail_normal2", Ogre::PBSM_DETAIL2_NM);
            parsePbsTextureType(datablockStruct, itorDatablock->value, "detail_normal3", Ogre::PBSM_DETAIL3);
            parsePbsTextureType(datablockStruct, itorDatablock->value, "reflection", Ogre::PBSM_REFLECTION);
        }
    }

    // Unlit: Retrieve the texture names and set them in the datablockStruct
    rapidjson::Value::ConstMemberIterator itorUnlit = d.FindMember("unlit");
    if( itorUnlit != d.MemberEnd() && itorUnlit->value.IsObject() )
    {
        // Only get the fist datablock
        rapidjson::Value::ConstMemberIterator itorDatablock = itorUnlit->value.MemberBegin();
        if (itorDatablock != itorUnlit->value.MemberEnd() && itorDatablock->value.IsObject() )
        {
            // Parse all textures
            parseUnlitTexture (datablockStruct, itorDatablock->value);
        }
    }

    return true;
}

//****************************************************************************/
void HlmsUtilsManager::parsePbsTextureType (HlmsUtilsManager::DatablockStruct* datablockStruct,
                                            const rapidjson::Value& textureTypeJson,
                                            const char* textureType,
                                            unsigned short index)
{
    rapidjson::Value::ConstMemberIterator itorTextureType = textureTypeJson.FindMember(textureType);
    const rapidjson::Value& itorTextureTypeStruct = itorTextureType->value;
    if (itorTextureType != textureTypeJson.MemberEnd() && itorTextureTypeStruct.IsObject() )
    {
        rapidjson::Value::ConstMemberIterator itorTexture = itorTextureTypeStruct.FindMember("texture");
        if (itorTexture != itorTextureTypeStruct.MemberEnd() && itorTexture->value.IsString() )
        {
            Ogre::String textureName(itorTexture->value.GetString());
            datablockStruct->textureMap[index] = textureName;
        }
    }
}

//****************************************************************************/
void HlmsUtilsManager::parseUnlitTexture (HlmsUtilsManager::DatablockStruct* datablockStruct,
                                         const rapidjson::Value& textureJson)
{
    Ogre::String diffuseMap;
    for (unsigned short index = 0; index < Ogre::UnlitTextureTypes::NUM_UNLIT_TEXTURE_TYPES; ++index)
    {
        diffuseMap = "diffuse_map" + Ogre::StringConverter::toString(index);
        rapidjson::Value::ConstMemberIterator itorTextureType = textureJson.FindMember(diffuseMap.c_str());
        const rapidjson::Value& itorTextureTypeStruct = itorTextureType->value;
        if (itorTextureType != textureJson.MemberEnd() && itorTextureTypeStruct.IsObject() )
        {
            rapidjson::Value::ConstMemberIterator itorTexture = itorTextureTypeStruct.FindMember("texture");
            if (itorTexture != itorTextureTypeStruct.MemberEnd() && itorTexture->value.IsString() )
            {
                Ogre::String textureName(itorTexture->value.GetString());
                datablockStruct->textureMap[index] = textureName;
            }
        }
    }
}

//****************************************************************************/
void HlmsUtilsManager::getTexturesFromRegisteredPbsDatablocks(std::vector<Ogre::String>* v)
{
    Ogre::String textureName;
    unsigned short index;
    QVectorIterator<DatablockStruct> itRegisteredDatablocks(mRegisteredDatablocks);
    while (itRegisteredDatablocks.hasNext())
    {
        // If the type is pbs, append the texturenames to the vector
        helperDatablockStruct = itRegisteredDatablocks.next();
        if (helperDatablockStruct.type == HLMS_PBS)
        {
            QMap <unsigned short, Ogre::String>::iterator itTextures = helperDatablockStruct.textureMap.begin();
            QMap <unsigned short, Ogre::String>::iterator itTexturesEnd = helperDatablockStruct.textureMap.end();
            while (itTextures != itTexturesEnd)
            {
                index = itTextures.key();
                textureName = itTextures.value();
                if (!textureName.empty())
                    v->push_back(textureName);

                ++itTextures;
            }
        }
    }
}

//****************************************************************************/
void HlmsUtilsManager::getTexturesFromRegisteredUnlitDatablocks(std::vector<Ogre::String>* v)
{
    Ogre::String textureName;
    unsigned short index;
    QVectorIterator<DatablockStruct> itRegisteredDatablocks(mRegisteredDatablocks);
    while (itRegisteredDatablocks.hasNext())
    {
        // If the type is unlit, append the texturenames to the vector
        helperDatablockStruct = itRegisteredDatablocks.next();
        if (helperDatablockStruct.type == HLMS_UNLIT)
        {
            QMap <unsigned short, Ogre::String>::iterator itTextures = helperDatablockStruct.textureMap.begin();
            QMap <unsigned short, Ogre::String>::iterator itTexturesEnd = helperDatablockStruct.textureMap.end();
            while (itTextures != itTexturesEnd)
            {
                index = itTextures.key();
                textureName = itTextures.value();
                if (!textureName.empty())
                    v->push_back(textureName);

                ++itTextures;
            }
        }
    }
}

//****************************************************************************/
const Ogre::String& HlmsUtilsManager::searchJsonFileName (const Ogre::IdString& datablockId)
{
    QVectorIterator<DatablockStruct> itRegisteredDatablocks(mRegisteredDatablocks);
    while (itRegisteredDatablocks.hasNext())
    {
        helperDatablockStruct = itRegisteredDatablocks.next();
        if (helperDatablockStruct.datablockId == datablockId)
        {
            return helperDatablockStruct.jsonFileName;
        }
    }

    return helperString;
}

//****************************************************************************/
void HlmsUtilsManager::addNewDatablockToRegisteredDatablocks (const Ogre::IdString& datablockId, const Ogre::String jsonFileName)
{
    helperDatablockStruct = getDatablock(datablockId);
    helperDatablockStruct.jsonFileName = jsonFileName;
    mRegisteredDatablocks.append(helperDatablockStruct);
}

//****************************************************************************/
const Ogre::String& HlmsUtilsManager::getTextureFileNameOfPbs (const Ogre::IdString& datablockId, Ogre::PbsTextureTypes textureType)
{
    helperString = "";

    // Find the datablock
    Ogre::HlmsManager* hlmsManager = Ogre::Root::getSingletonPtr()->getHlmsManager();
    Ogre::HlmsPbs* hlmsPbs = static_cast<Ogre::HlmsPbs*>( hlmsManager->getHlms(Ogre::HLMS_PBS));
    Ogre::HlmsDatablock* datablock = 0;
    try
    {
        datablock = hlmsPbs->getDatablock(datablockId);
        if (datablock)
        {
            // Get the texture
            Ogre::HlmsPbsDatablock* pbsDatablock = static_cast<Ogre::HlmsPbsDatablock*>(datablock);
            if (!pbsDatablock->getTexture(textureType).isNull())
            {
                Ogre::HlmsTextureManager::TextureLocation texLocation;
                Ogre::HlmsManager* hlmsManager = Ogre::Root::getSingletonPtr()->getHlmsManager();
                texLocation.texture = pbsDatablock->getTexture(textureType);
                if( !texLocation.texture.isNull() )
                {
                    texLocation.xIdx = pbsDatablock->_getTextureIdx(textureType);
                    texLocation.yIdx = 0;
                    texLocation.divisor = 1;
                    const Ogre::String *texName = hlmsManager->getTextureManager()->findAliasName(texLocation);

                    if(texName)
                    {
                        helperString = *texName;
                    }
                }
            }
        }
    }
    catch (Ogre::Exception e) {}

    return helperString;
}

//****************************************************************************/
HlmsUtilsManager::DatablockStruct HlmsUtilsManager::getDatablock(const Ogre::IdString& datablockId)
{
    QVectorIterator<DatablockStruct> itRegisteredDatablocks(mRegisteredDatablocks);
    while (itRegisteredDatablocks.hasNext())
    {
        helperDatablockStruct = itRegisteredDatablocks.next();
        if (helperDatablockStruct.datablockId == datablockId)
        {
            // Found it, return the struct
            return helperDatablockStruct;
        }
    }

    // Not found in the registered datablocks; this is possible in case of a datablock constructed by means of the node editor
    // So, search it as a real datablock instead (by means of Ogre functions)
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
        if (pbsDatablock->getName() == datablockId)
        {
            helperDatablockStruct.datablock = pbsDatablock;
            helperDatablockStruct.datablockFullName = *pbsDatablock->getFullName();
            helperDatablockStruct.datablockId = pbsDatablock->getName();
            helperDatablockStruct.jsonFileName = "";
            helperDatablockStruct.type = HLMS_PBS;
            enrichTextureMapFromPbs(&helperDatablockStruct);
            return helperDatablockStruct;
        }
        ++itorPbs;
    }

    // Iterate through all unlit datablocks
    Ogre::Hlms::HlmsDatablockMap::const_iterator itorUnlit = hlmsUnlit->getDatablockMap().begin();
    Ogre::Hlms::HlmsDatablockMap::const_iterator endUnlit = hlmsUnlit->getDatablockMap().end();
    Ogre::HlmsUnlitDatablock* unlitDatablock;
    while( itorUnlit != endUnlit)
    {
        unlitDatablock = static_cast<Ogre::HlmsUnlitDatablock*>(itorUnlit->second.datablock);
        if (unlitDatablock->getName() == datablockId)
        {
            helperDatablockStruct.datablock = unlitDatablock;
            helperDatablockStruct.datablockFullName = *unlitDatablock->getFullName();
            helperDatablockStruct.datablockId = unlitDatablock->getName();
            helperDatablockStruct.jsonFileName = "";
            helperDatablockStruct.type = HLMS_UNLIT;
            enrichTextureMapFromUnlit(&helperDatablockStruct);
            return helperDatablockStruct;
        }
        ++itorUnlit;
    }
}

//****************************************************************************/
void HlmsUtilsManager::enrichTextureMapFromPbs(DatablockStruct* datablockStruct)
{
    helperString = "";
    if (!datablockStruct->datablock)
        return;

    Ogre::HlmsPbsDatablock* pbsDatablock = static_cast<Ogre::HlmsPbsDatablock*>(datablockStruct->datablock);
    datablockStruct->textureMap.clear();
    for (size_t index = 0; index < Ogre::PbsTextureTypes::NUM_PBSM_TEXTURE_TYPES; ++index)
    {
        if (!pbsDatablock->getTexture(index).isNull())
        {
            Ogre::HlmsTextureManager::TextureLocation texLocation;
            Ogre::HlmsManager* hlmsManager = Ogre::Root::getSingletonPtr()->getHlmsManager();
            texLocation.texture = pbsDatablock->getTexture(index);
            if( !texLocation.texture.isNull() )
            {
                texLocation.xIdx = pbsDatablock->_getTextureIdx((Ogre::PbsTextureTypes)index);
                texLocation.yIdx = 0;
                texLocation.divisor = 1;
                const Ogre::String *texName = hlmsManager->getTextureManager()->findAliasName(texLocation);

                if(texName)
                {
                    helperString = *texName;
                    datablockStruct->textureMap[index] = helperString;
                }
            }
        }
    }
}

//****************************************************************************/
void HlmsUtilsManager::enrichTextureMapFromUnlit(DatablockStruct* datablockStruct)
{
    helperString = "";
    if (!datablockStruct->datablock)
        return;

    Ogre::HlmsUnlitDatablock* unlitDatablock = static_cast<Ogre::HlmsUnlitDatablock*>(datablockStruct->datablock);
    datablockStruct->textureMap.clear();
    for (size_t index = 0; index < Ogre::UnlitTextureTypes::NUM_UNLIT_TEXTURE_TYPES; ++index)
    {
        if (!unlitDatablock->getTexture(index).isNull())
        {
            Ogre::HlmsTextureManager::TextureLocation texLocation;
            Ogre::HlmsManager* hlmsManager = Ogre::Root::getSingletonPtr()->getHlmsManager();
            texLocation.texture = unlitDatablock->getTexture(index);
            if( !texLocation.texture.isNull() )
            {
                texLocation.xIdx = unlitDatablock->_getTextureIdx(index);
                texLocation.yIdx = 0;
                texLocation.divisor = 1;
                const Ogre::String *texName = hlmsManager->getTextureManager()->findAliasName(texLocation);

                if(texName)
                {
                    helperString = *texName;
                    datablockStruct->textureMap[index] = helperString;
                }
            }
        }
    }
}

