/* This file is part of Clementine.
   Copyright 2010, David Sansome <me@davidsansome.com>

   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
*/

//Copyright: (C) 2003 Mark Kretschmann
//           (C) 2004,2005 Max Howell, <max.howell@methylblue.com>
//License:   See COPYING

#include "enginebase.h"

#include <cmath>

#include <QSettings>

const char* Engine::Base::kSettingsGroup = "Player";

Engine::Base::Base()
  : volume_(50),
    beginning_nanosec_(0),
    end_nanosec_(0),
    scope_(kScopeSize),
    fadeout_enabled_(true),
    fadeout_duration_nanosec_(2000 * 1e6), // 2s
    crossfade_enabled_(true),
    next_background_stream_id_(0),
    about_to_end_emitted_(false)
{
}

Engine::Base::~Base() {
}

bool Engine::Base::Load(const QUrl& url, TrackChangeType,
                        quint64 beginning_nanosec, qint64 end_nanosec) {
  url_ = url;
  beginning_nanosec_ = beginning_nanosec;
  end_nanosec_ = end_nanosec;

  about_to_end_emitted_ = false;
  return true;
}

void Engine::Base::SetVolume(uint value) {
  volume_ = value;

  SetVolumeSW(MakeVolumeLogarithmic(value));
}

uint Engine::Base::MakeVolumeLogarithmic(uint volume) {
  // We're using a logarithmic function to make the volume ramp more natural.
  return static_cast<uint>( 100 - 100.0 * std::log10( ( 100 - volume ) * 0.09 + 1.0 ) );
}

void Engine::Base::ReloadSettings() {
  QSettings s;
  s.beginGroup(kSettingsGroup);

  fadeout_enabled_ = s.value("FadeoutEnabled", true).toBool();
  fadeout_duration_nanosec_ = s.value("FadeoutDuration", 2000).toLongLong() * 1e6;
  crossfade_enabled_ = s.value("CrossfadeEnabled", true).toBool();
  autocrossfade_enabled_ = s.value("AutoCrossfadeEnabled", false).toBool();
}

void Engine::Base::EmitAboutToEnd() {
  if (about_to_end_emitted_)
    return;

  about_to_end_emitted_ = true;
  emit TrackAboutToEnd();
}

int Engine::Base::AddBackgroundStream(const QUrl& url) {
  return -1;
}

bool Engine::Base::Play(const QUrl& u, TrackChangeType c,
                        quint64 beginning_nanosec, qint64 end_nanosec) {
  if (!Load(u, c, beginning_nanosec, end_nanosec))
    return false;

  return Play(0);
}
