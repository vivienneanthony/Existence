//
// Copyright (c) 2008-2014 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "RenderSurface.h"
#include "Ptr.h"
#include "Texture.h"

namespace Urho3D
{

class Image;

/// 2D texture resource.
class URHO3D_API Texture2D : public Texture
{
    OBJECT(Texture2D);
    
public:
    /// Construct.
    Texture2D(Context* context);
    /// Destruct.
    virtual ~Texture2D();
    /// Register object factory.
    static void RegisterObject(Context* context);
    
    /// Load resource. Return true if successful.
    virtual bool Load(Deserializer& source);
    /// Release default pool resources.
    virtual void OnDeviceLost();
    /// Recreate default pool resources.
    virtual void OnDeviceReset();
    /// Release texture.
    virtual void Release();
    
    /// Set size, format and usage. Zero size will follow application window size. Return true if successful.
    bool SetSize(int width, int height, unsigned format, TextureUsage usage = TEXTURE_STATIC);
    /// Set data either partially or fully on a mip level. Return true if successful.
    bool SetData(unsigned level, int x, int y, int width, int height, const void* data);
    /// Load from an image. Return true if successful. Optionally make a single channel image alpha-only.
    bool Load(SharedPtr<Image> image, bool useAlpha = false);
    
    /// Get data from a mip level. The destination buffer must be big enough. Return true if successful.
    bool GetData(unsigned level, void* dest) const;
    /// Return render surface.
    RenderSurface* GetRenderSurface() const { return renderSurface_; }
    
private:
    /// Create texture.
    bool Create();
    /// Handle render surface update event.
    void HandleRenderSurfaceUpdate(StringHash eventType, VariantMap& eventData);
    
    /// Render surface.
    SharedPtr<RenderSurface> renderSurface_;
};

}
