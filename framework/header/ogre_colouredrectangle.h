#ifndef QOGREQColouredRectangle2D_H
#define QOGREQColouredRectangle2D_H

#include "OgreRectangle2D.h"
#include "OgreHardwareBufferManager.h"
#define COLOUR_BINDING 3

namespace QOgre
{

	class ColouredRectangle2D : public Ogre::Rectangle2D
	{
	public:
		ColouredRectangle2D(bool includeTextureCoordinates = false) : Ogre::Rectangle2D (includeTextureCoordinates)
		{
			Ogre::VertexDeclaration* decl = mRenderOp.vertexData->vertexDeclaration;

			decl->addElement(COLOUR_BINDING, 0, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
			Ogre::VertexBufferBinding* bind = mRenderOp.vertexData->vertexBufferBinding;

			Ogre::HardwareVertexBufferSharedPtr vbuf = 
			Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			decl->getVertexSize(COLOUR_BINDING),
			mRenderOp.vertexData->vertexCount,
			Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

			// Bind buffer
			bind->setBinding(COLOUR_BINDING, vbuf);
		}
		~ColouredRectangle2D() {}

		/// Sets the colour of the 2D rectangle.
		/// Format: Top Left, Bottom Left, Top Right, Bottom Right
		void setColours(const Ogre::ColourValue &topLeft, const Ogre::ColourValue &bottomLeft, const Ogre::ColourValue &topRight, const Ogre::ColourValue &bottomRight)
		{
			Ogre::HardwareVertexBufferSharedPtr vbuf = 
				mRenderOp.vertexData->vertexBufferBinding->getBuffer(COLOUR_BINDING);
			unsigned int* pUint32 = static_cast<unsigned int*>(vbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

			const Ogre::VertexElementType srcType = Ogre::VertexElement::getBestColourVertexElementType();

			*pUint32++ = Ogre::VertexElement::convertColourValue( topLeft, srcType );

			*pUint32++ = Ogre::VertexElement::convertColourValue( bottomLeft, srcType );

			*pUint32++ = Ogre::VertexElement::convertColourValue( topRight, srcType );

			*pUint32++ = Ogre::VertexElement::convertColourValue( bottomRight, srcType );

			vbuf->unlock();
		}
	};
}
#endif // QOGREQColouredRectangle2D_H
