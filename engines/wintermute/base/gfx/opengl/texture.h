/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef WINTERMUTE_TEXTURE_H
#define WINTERMUTE_TEXTURE_H

#include "graphics/surface.h"
#include "common/ptr.h"

namespace Wintermute {
class TextureManager;
class Texture {
public:
	Texture(const Graphics::Surface &surface);
	Texture(const Texture &);
	Texture &operator=(const Texture &rhs);
	~Texture();

	void bind() const;

	uint32 getWidth() const {
		return _width;
	}
	uint32 getHeight() const {
		return _height;
	}
	bool hasAlpha() const { return _hasAlpha; }
	uint32 getTexId() const;
private:
	// Wrapper to allow for copying and still retaining the texture
	// for the correct lifetime
	struct TexId {
		uint32 _texId;
		TexId();
		~TexId();
	};
	void init(const Graphics::Surface &surface);
	void copyFrom(const Texture &other);
	void cleanup();
	bool _hasAlpha;
	Common::SharedPtr<TexId> _texIdPtr;
	uint32 _width;
	uint32 _height;
};

} // End of namespace Wintermute

#endif
