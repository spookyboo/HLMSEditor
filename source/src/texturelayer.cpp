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

#include "constants.h"
#include "texturelayer.h"
#include "OgreHlmsPbs.h"
#include "OgreHlmsManager.h"
#include "OgreLogManager.h"
#include "OgreStringConverter.h"
#include <fstream>
#include <ctime>

//****************************************************************************/
TextureLayer::TextureLayer(void) :
    mTextureOnWhichIsPaintedWidth(0),
    mTextureOnWhichIsPaintedHeight(0),
    mTextureOnWhichIsPaintedHasAlpha(false),
    mNumMipMaps(0),
    mTextureTypeDefined(false),
    mMaxSequence(0)
{
    mTextureType = Ogre::PBSM_DIFFUSE;
    mDatablockId = "";
    mTextureFileName = "";
}

//****************************************************************************/
TextureLayer::~TextureLayer(void)
{
}

//****************************************************************************/
void TextureLayer::setDatablockIdAndTexture (const Ogre::IdString& datablockId,
                                             Ogre::PbsTextureTypes textureType,
                                             const Ogre::String& textureFileName)
{
    mDatablockId = datablockId;
    mTextureType = textureType;
    mTextureFileName = textureFileName;
    mTextureTypeDefined = true;

    // Load the texture as image; assume it can be loaded, because it was already loaded as part of the material
    setFirstTextureGeneration();

    // Create the pixelbox of the original texture; this MUST be a separate image
    mOriginalTexture.load(textureFileName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    mPixelboxOriginalTexture = mOriginalTexture.getPixelBox(0, 0);

    // Debug texture
    //Ogre::LogManager::getSingleton().logMessage("Debug texture: " + textureFileName);
    //Ogre::LogManager::getSingleton().logMessage("Depth: " + Ogre::StringConverter::toString(mOriginalTexture.getDepth()));
    //Ogre::LogManager::getSingleton().logMessage("Pixel format: " + Ogre::StringConverter::toString(mOriginalTexture.getFormat()));
    //Ogre::LogManager::getSingleton().logMessage("Alpha: " + Ogre::StringConverter::toString(mOriginalTexture.getHasAlpha()));
    //Ogre::LogManager::getSingleton().logMessage("Height: " + Ogre::StringConverter::toString(mOriginalTexture.getHeight()));
    //Ogre::LogManager::getSingleton().logMessage("Number of faces: " + Ogre::StringConverter::toString(mOriginalTexture.getNumFaces()));
    //Ogre::LogManager::getSingleton().logMessage("Number of mipmaps: " + Ogre::StringConverter::toString(mOriginalTexture.getNumMipmaps()));
    //Ogre::LogManager::getSingleton().logMessage("Width: " + Ogre::StringConverter::toString(mOriginalTexture.getWidth()));
}

//****************************************************************************/
void TextureLayer::blitTexture (void)
{
    /* Always get the actual pointers, because they may change. That is the reason why the datablock pointer cannot be cached.
     * The same seems to apply to the texture pointer.
     */
    Ogre::HlmsDatablock* datablock;
    Ogre::HlmsPbsDatablock* datablockPbs;
    Ogre::TexturePtr texture;
    Ogre::HlmsManager* hlmsManager = Ogre::Root::getSingletonPtr()->getHlmsManager();
    Ogre::HlmsPbs* hlmsPbs = static_cast<Ogre::HlmsPbs*>(hlmsManager->getHlms(Ogre::HLMS_PBS));
    datablock = hlmsPbs->getDatablock(mDatablockId);
    if (!datablock)
        return;

    datablockPbs = static_cast<Ogre::HlmsPbsDatablock*>(datablock);
    try
    {
        // Get texture on GPU
        if (!datablockPbs->getTexture(mTextureType).isNull())
        {
            texture = datablockPbs->getTexture(mTextureType); // TextureType MUST exist, otherwise the application crashes
            mNumMipMaps = texture->getNumMipmaps();
        }
    }
    catch (Ogre::Exception e){}

    if (texture.isNull())
        return;

    Ogre::uint8 maxMipMaps = mNumMipMaps + 1; // Increase with one, because there is always one image to blit
    maxMipMaps = maxMipMaps > PAINT_MAX_MIP_MAPS ? PAINT_MAX_MIP_MAPS : maxMipMaps; // Just paint a few mipmaps (not all)
    size_t w = mTextureOnWhichIsPaintedWidth;
    size_t h = mTextureOnWhichIsPaintedHeight;
    Ogre::Image textureOnWhichIsPaintedScaled = mTextureOnWhichIsPainted; // Define textureOnWhichIsPaintedScaled each time; reusing results in exception
    Ogre::v1::HardwarePixelBuffer* buffer;
    for (Ogre::uint8 i = 0; i < maxMipMaps; ++i)
    {
        buffer = texture->getBuffer(0, i).getPointer();
        buffer->blitFromMemory(textureOnWhichIsPaintedScaled.getPixelBox(0,0), Ogre::Box(0, 0, 0, w, h, 1));
        w*=0.5f; // Mipmaps always are half of the previous one
        h*=0.5f;
        if (w > 1.0f && h > 1.0f)
            textureOnWhichIsPaintedScaled.resize(w, h);
        else
            break; // Stop when the mipmaps are too small
    }
    textureOnWhichIsPaintedScaled.freeMemory();
}

//****************************************************************************/
void TextureLayer::setFirstTextureGeneration (void)
{
    // Don't check on existence mTextureFileName, because it does exist
    loadTextureGeneration(mTextureFileName);
}

//****************************************************************************/
void TextureLayer::setLastTextureGeneration (void)
{
    Ogre::String textureFileNameGeneration = getTextureFileNameGeneration (mMaxSequence); // returns full qualified filename
    if (textureFileExists(textureFileNameGeneration))
        loadTextureGeneration(textureFileNameGeneration);
}

//****************************************************************************/
void TextureLayer::loadTextureGeneration (Ogre::ushort sequence)
{
    Ogre::String textureFileNameGeneration = getTextureFileNameGeneration (sequence); // returns full qualified filename
    if (sequence == 0)
        loadTextureGeneration(textureFileNameGeneration); // Don't check the filename if sequence is 0, because it is without path
    else if (textureFileExists(textureFileNameGeneration))
        loadTextureGeneration(textureFileNameGeneration);
}

//****************************************************************************/
void TextureLayer::loadTextureGeneration (const Ogre::String& filename)
{
    // Assume the filename exists
    mTextureOnWhichIsPainted.load(filename, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    mPixelboxTextureOnWhichIsPainted = mTextureOnWhichIsPainted.getPixelBox(0, 0);
    mTextureOnWhichIsPaintedHasAlpha = mTextureOnWhichIsPainted.getHasAlpha();
    mTextureOnWhichIsPaintedWidth = mPixelboxTextureOnWhichIsPainted.getWidth();
    mTextureOnWhichIsPaintedHeight = mPixelboxTextureOnWhichIsPainted.getHeight();
    // In theory, createCarbonCopyTexture() of all related paintlayers should be called,
    // but the texture size doesn't change in practice.

    blitTexture();
}

//****************************************************************************/
void TextureLayer::saveTextureGeneration (void)
{
    // Increase the sequence
    ++mMaxSequence;
    Ogre::String textureFileNameGeneration = getTextureFileNameGeneration (mMaxSequence); // returns full qualified filename

    // Saving the Image must be done in the background, otherwise the painting stutters
    QThread* thread = new QThread;
    TextureSaveWorker* textureSaveWorker = new TextureSaveWorker (mTextureOnWhichIsPainted, textureFileNameGeneration);
    textureSaveWorker->moveToThread(thread);
    connect(thread, SIGNAL(started()), textureSaveWorker, SLOT(saveImage()));
    connect(textureSaveWorker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(textureSaveWorker, SIGNAL(finished()), textureSaveWorker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

//****************************************************************************/
const Ogre::String& TextureLayer::saveTextureWithTimeStampToImportDir (void)
{
    Ogre::String strippedTextureFileName = mTextureFileName;
    Ogre::String extension = mTextureFileName;
    strippedTextureFileName.erase(strippedTextureFileName.find_last_of("."), Ogre::String::npos); // Remove extension
    if (strippedTextureFileName.find("_hlms") != Ogre::String::npos)
        strippedTextureFileName.erase(strippedTextureFileName.find("_hlms"), Ogre::String::npos); // Remove earlier hlms editor additions
    extension.erase(0, extension.find_last_of("."));
    mHelperString = strippedTextureFileName +
            "_hlms" +
            Ogre::StringConverter::toString((size_t)time(0)) +
            extension;

    mTextureOnWhichIsPainted.save(DEFAULT_IMPORT_PATH.toStdString() + mHelperString); // Saving to the import dir doesn't have to be in the background
    return mHelperString; // Return the basename
}

//****************************************************************************/
const Ogre::String& TextureLayer::getTextureFileNameGeneration (int sequence, bool fullQualified)
{
    mHelperString = mTextureFileName;

    // Do not go beyond the max sequence number
    sequence = sequence > mMaxSequence ? mMaxSequence : sequence;
    if (sequence > 0)
    {
        // Do not go below sequence number 1 (otherwise the original mTextureFileName (without path) is returned)
        Ogre::String strippedTextureFileName = mTextureFileName;
        Ogre::String extension = mTextureFileName;
        strippedTextureFileName.erase(strippedTextureFileName.find_last_of("."), Ogre::String::npos);
        extension.erase(0, extension.find_last_of("."));
        mHelperString = strippedTextureFileName +
                Ogre::StringConverter::toString(sequence) +
                extension;

        if (fullQualified)
            mHelperString = TEMP_PATH + mHelperString;
    }

    return mHelperString;
}

//****************************************************************************/
bool TextureLayer::isTextureFileNameDefinedAsResource (const Ogre::String& filename)
{
    Ogre::String path;
    Ogre::FileInfoListPtr list = Ogre::ResourceGroupManager::getSingleton().listResourceFileInfo(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME) ;
    Ogre::FileInfoList::iterator it;
    Ogre::FileInfoList::iterator itStart = list->begin();
    Ogre::FileInfoList::iterator itEnd = list->end();
    for(it = itStart; it != itEnd; ++it)
    {
        Ogre::FileInfo& fileInfo = (*it);
        if (fileInfo.basename == filename || fileInfo.filename == filename)
            return true;
    }

    return false;
}

//****************************************************************************/
bool TextureLayer::textureFileExists (const Ogre::String& filename)
{
    std::ifstream infile(filename);
    return infile.good();
}
