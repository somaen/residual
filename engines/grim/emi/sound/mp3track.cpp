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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "common/mutex.h"
#include "common/textconsole.h"
#include "common/substream.h"
#include "audio/mixer.h"
#include "audio/audiostream.h"
#include "audio/decoders/mp3.h"
#include "engines/grim/textsplit.h"
#include "engines/grim/resource.h"
#include "engines/grim/emi/sound/mp3track.h"

namespace Grim {

void MP3Track::parseRIFFHeader(Common::SeekableReadStream *data) {
	uint32 tag = data->readUint32BE();
	if (tag == MKTAG('R','I','F','F')) {
		_endFlag = false;
		data->seek(18, SEEK_CUR);
		_channels = data->readByte();
		data->readByte();
		_freq = data->readUint32LE();
		data->seek(6, SEEK_CUR);
		_bits = data->readByte();
		data->seek(5, SEEK_CUR);
		_regionLength = data->readUint32LE();
		_headerSize = 44;
	} else {
		error("Unknown file header");
	}
}

MP3Track::MP3Track(Audio::Mixer::SoundType soundType) {
	_soundType = soundType;
	_headerSize = 0;
	_regionLength = 0;
	_freq = 0;
	_bits = 0,
	_channels = 0;
	_endFlag = false;
}

MP3Track::~MP3Track() {
	stop();
	delete _handle;
}

bool MP3Track::parseJMM(const Common::String &filepath) {
	Common::SeekableReadStream *jmmStream = g_resourceloader->openNewStreamFile(filepath);
	if (jmmStream) {
		TextSplitter ts(filepath, jmmStream);
		float start, end;
		int intVal1, intVal2, intVal3, intVal4;
		float loopEnd, loopStart, floatVal3, floatVal4;
		ts.scanString(".start %f", 1, &start);
		bool jump = false;
		if (ts.checkString(".jump")) {
			jump = true;
			ts.scanString(".jump %f %f %d %f %f %d %d %d", 8, &loopEnd, &loopStart, &intVal1, &floatVal3, &floatVal4, &intVal2, &intVal3, &intVal4);
		} else {
			ts.nextLine();
		}
		ts.scanString(".end %f", 1, &end);
		warning("JMM-data: Start: %f End: %f", start, end);
		if (jump) {
			warning(".jump %f %f %d %f %f %d %d %d", &loopEnd, &loopStart, &intVal1, &floatVal3, &floatVal4, &intVal2, &intVal3, &intVal4);
			_loopStart = loopStart;
			_loopEnd = loopEnd;
		}
	}
}

bool MP3Track::openSound(const Common::String &path, const Common::String &soundName, Common::SeekableReadStream *file) {
	Common::String jmmFilename(soundName);
	jmmFilename.erase(jmmFilename.size() - 3);
	jmmFilename = path + jmmFilename + "jmm";
	warning("JMM: %s", jmmFilename.c_str());
	parseJMM(jmmFilename);
	return openSound(soundName, file);
}

bool MP3Track::openSound(const Common::String &soundName, Common::SeekableReadStream *file) {
	if (!file) {
		warning("Stream for %s not open", soundName.c_str());
		return false;
	}
	_soundName = soundName;
#ifndef USE_MAD
	warning("Cannot open %s, MP3 support not enabled", soundName.c_str());
	return true;
#else
	parseRIFFHeader(file);
	Common::SeekableReadStream *stream = new Common::SeekableSubReadStream(file, file->pos(), file->size());
	_stream = new Audio::SubLoopingAudioStream(Audio::makeMP3Stream(stream, DisposeAfterUse::YES), 0, Audio::Timestamp(_loopStart), Audio::Timestamp(_loopEnd), DisposeAfterUse::YES);
	warning("Loop start: %f Loop end: %f", _loopStart, _loopEnd);
	Audio::Timestamp ts = Audio::Timestamp(_loopStart);
	Audio::Timestamp ts2 = Audio::Timestamp(_loopEnd);
	warning("%d %d", ts.msecs(), ts2.msecs());
//	_stream = Audio::makeLoopingAudioStream(Audio::makeMP3Stream(file, DisposeAfterUse::YES), 0);
	_handle = new Audio::SoundHandle();
	return true;
#endif
}

} // end of namespace Grim
