/*
 * signal.h - declaration of class signal which is a bass synth attempting to
 *           emulate the Roland TB303 bass synth
 *
 * Copyright (c) 2006-2008 Paul Giblock <pgib/at/users.sourceforge.net>
 *
 * This file is part of LMMS - https://lmms.io
 *
 * signalFilterIIR2 is based on the gsyn filter code by Andy Sloane.
 *
 * signalFilter3Pole is based on the TB303 instrument written by
 *   Josep M Comajuncosas for the CSounds library
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */


#ifndef SIGNAL_H_
#define SIGNAL_H_

#include "DspEffectLibrary.h"
#include "Instrument.h"
#include "InstrumentView.h"
#include "LedCheckbox.h"
#include "Knob.h"
#include "NotePlayHandle.h"
#include <QMutex>

static const int NUM_FILTERS = 2;

class signalSynthView;
class NotePlayHandle;

class signalSynth : public Instrument
{
	Q_OBJECT
public:
	signalSynth( InstrumentTrack * _instrument_track );
	virtual ~signalSynth();

	virtual void play( sampleFrame * _working_buffer );
	virtual void playNote( NotePlayHandle * _n,
						sampleFrame * _working_buffer );
	virtual void deleteNotePluginData( NotePlayHandle * _n );


	virtual void saveSettings( QDomDocument & _doc, QDomElement & _parent );
	virtual void loadSettings( const QDomElement & _this );

	virtual QString nodeName() const;

	virtual Flags flags() const
	{
		return IsSingleStreamed;
	}

	virtual f_cnt_t desiredReleaseFrames() const
	{
		return 0; //4048;
	}

	virtual PluginView * instantiateView( QWidget * _parent );

private:
	void processNote( NotePlayHandle * n );

private:
	int release_frame;

	int process(sampleFrame *outbuf, const int size);

	friend class signalSynthView;

	NotePlayHandle * m_playingNote;
	NotePlayHandleList m_notes;
	QMutex m_notesMutex;
    float gate;
    float nn;
} ;


class signalSynthView : public InstrumentView
{
	Q_OBJECT
public:
	signalSynthView( Instrument * _instrument,
	                QWidget * _parent );
	virtual ~signalSynthView();

private:
	virtual void modelChanged();
} ;

#endif
