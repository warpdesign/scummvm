/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
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

#include "common/scummsys.h"
#include "common/config-manager.h"
#include "common/debug-channels.h"
#include "common/events.h"
#include "common/file.h"
#include "engines/util.h"
#include "graphics/scaler.h"
#include "graphics/thumbnail.h"
#include "gargoyle/gargoyle.h"
#include "gargoyle/conf.h"
#include "gargoyle/events.h"
#include "gargoyle/picture.h"
#include "gargoyle/screen.h"
#include "gargoyle/selection.h"
#include "gargoyle/streams.h"
#include "gargoyle/windows.h"

namespace Gargoyle {

GargoyleEngine *g_vm;

GargoyleEngine::GargoyleEngine(OSystem *syst, const GargoyleGameDescription *gameDesc) :
	_gameDescription(gameDesc), Engine(syst), _random("Gargoyle"), _clipboard(nullptr),
	_conf(nullptr), _events(nullptr), _picList(nullptr), _screen(nullptr),
	_selection(nullptr), _windows(nullptr), _copySelect(false), _terminated(false),
	gli_unregister_obj(nullptr), gli_register_arr(nullptr), gli_unregister_arr(nullptr) {
	g_vm = this;
}

GargoyleEngine::~GargoyleEngine() {
	delete _clipboard;
	delete _conf;
	delete _events;
	delete _picList;
	delete _screen;
	delete _selection;
	delete _streams;
	delete _windows;
}

void GargoyleEngine::initialize() {
	// Set up debug channels
	DebugMan.addDebugChannel(kDebugCore, "core", "Core engine debug level");
	DebugMan.addDebugChannel(kDebugScripts, "scripts", "Game scripts");
	DebugMan.addDebugChannel(kDebugGraphics, "graphics", "Graphics handling");
	DebugMan.addDebugChannel(kDebugSound, "sound", "Sound and Music handling");

	initGraphicsMode();
	_conf = new Conf();
	_screen = new Screen();

	_clipboard = new Clipboard();
	_events = new Events();
	_picList = new PicList();
	_selection = new Selection();
	_streams = new Streams();
	_windows = new Windows(_screen);
}

void GargoyleEngine::initGraphicsMode() {
	uint width = ConfMan.hasKey("width") ? ConfMan.getInt("width") : 640;
	uint height = ConfMan.hasKey("height") ? ConfMan.getInt("height") : 480;
	Common::List<Graphics::PixelFormat> formats = g_system->getSupportedFormats();
	Graphics::PixelFormat format = formats.front();

	for (Common::List<Graphics::PixelFormat>::iterator i = formats.begin(); i != formats.end(); ++i) {
		if ((*i).bytesPerPixel > 1) {
			format = *i;
			break;
		}
	}

	initGraphics(width, height, &format);
}

Common::Error GargoyleEngine::run() {
	initialize();

	Common::File f;
	if (f.open(getFilename()))
		runGame(&f);

	return Common::kNoError;
}

void GargoyleEngine::GUIError(const char *msg, ...) {
	char buffer[STRINGBUFLEN];
	va_list va;

	// Generate the full error message
	va_start(va, msg);
	vsnprintf(buffer, STRINGBUFLEN, msg, va);
	va_end(va);

	GUIErrorMessage(buffer);
}

} // End of namespace Gargoyle