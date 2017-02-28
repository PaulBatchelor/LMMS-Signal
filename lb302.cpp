/*
 * lb302.cpp - implementation of class lb302 which is a bass synth attempting
 *             to emulate the Roland TB303 bass synth
 *
 * Copyright (c) 2006-2008 Paul Giblock <pgib/at/users.sourceforge.net>
 *
 * This file is part of LMMS - https://lmms.io
 *
 * lb302FilterIIR2 is based on the gsyn filter code by Andy Sloane.
 *
 * lb302Filter3Pole is based on the TB303 instrument written by
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

// Need to include this first to ensure we get M_PI in MinGW with C++11
#define _USE_MATH_DEFINES
#include <math.h>

#include "lb302.h"
#include "AutomatableButton.h"
#include "Engine.h"
#include "InstrumentPlayHandle.h"
#include "InstrumentTrack.h"
#include "Knob.h"
#include "NotePlayHandle.h"
#include "Oscillator.h"
#include "PixmapButton.h"
#include "templates.h"
#include "ToolTip.h"
#include "BandLimitedWave.h"

#include "embed.cpp"


// Envelope Recalculation period
#define ENVINC 64

//
// New config
//
#define LB_24_IGNORE_ENVELOPE
#define LB_FILTERED
//#define LB_DECAY
//#define LB_24_RES_TRICK

#define LB_DIST_RATIO    4.0
#define LB_24_VOL_ADJUST 3.0
//#define LB_DECAY_NOTES

//
// Old config
//


//#define engine::mixer()->processingSampleRate() 44100.0f


extern "C"
{

Plugin::Descriptor PLUGIN_EXPORT signal_plugin_descriptor =
{
	STRINGIFY( PLUGIN_NAME ),
	"Signal",
	QT_TRANSLATE_NOOP( "pluginBrowser",
			"Control Voltage Signal Generator for Sporth" ),
	"Paul Batchelor",
	0x0100,
	Plugin::Instrument,
	new PluginPixmapLoader( "logo" ),
	NULL,
	NULL
};

}

lb302Synth::lb302Synth( InstrumentTrack * _instrumentTrack ) :
	Instrument( _instrumentTrack, &signal_plugin_descriptor )

{

	InstrumentPlayHandle * iph = new InstrumentPlayHandle( this, _instrumentTrack );
	Engine::mixer()->addPlayHandle( iph );
    gate = 0;
    nn = 0;
	release_frame = 0;
}


lb302Synth::~lb302Synth()
{
	//for (int i=0; i<NUM_FILTERS; ++i) {
	//	delete vcfs[i];
	//}
}


void lb302Synth::saveSettings( QDomDocument & _doc,
	                             QDomElement & _this )
{
}

void lb302Synth::loadSettings( const QDomElement & _this )
{
}

QString lb302Synth::nodeName() const
{
	return( signal_plugin_descriptor.name );
}

int lb302Synth::process(sampleFrame *outbuf, const int size)
{
	if( release_frame == 0 || ! m_playingNote ) 
	{
        gate = 0;
	}

	for( int i=0; i<size; i++ ) 
	{
		if( i >= release_frame )
		{
            gate = 0;
		}

        outbuf[i][0] = nn;
        outbuf[i][1] = gate;

	}
	return 1;
}

void lb302Synth::playNote( NotePlayHandle * _n, sampleFrame * _working_buffer )
{
	if( _n->isMasterNote() || ( _n->hasParent() && _n->isReleased() ) )
	{
		return;
	}

	// sort notes: new notes to the end
	m_notesMutex.lock();
	if( _n->totalFramesPlayed() == 0 )
	{
		m_notes.append( _n );
	}
	else
	{
		m_notes.prepend( _n );
	}
	m_notesMutex.unlock();
	
	release_frame = qMax( release_frame, _n->framesLeft() + _n->offset() );
}



void lb302Synth::processNote( NotePlayHandle * _n )
{
		/// Start a new note.
		if( _n->m_pluginData != this ) 
		{
			m_playingNote = _n;
			//new_freq = true;
			_n->m_pluginData = this;
            gate = 1;
		}
		
		if( ! m_playingNote && ! _n->isReleased() && release_frame > 0 )
		{
			m_playingNote = _n;
			//if ( slideToggle.value() ) 
			//{
			//	vco_slideinc = GET_INC( _n->frequency() );
			//}
		}

		// Check for slide
		if( m_playingNote == _n ) 
		{
			//true_freq = _n->frequency();
            nn = _n->midiKey();

			//if( slideToggle.value() ) {
			//	vco_slidebase = GET_INC( true_freq );			// The REAL frequency
			//}
			//else {
			//	vco_inc = GET_INC( true_freq );
			//}
		}
}



void lb302Synth::play( sampleFrame * _working_buffer )
{
	m_notesMutex.lock();
	while( ! m_notes.isEmpty() )
	{
		processNote( m_notes.takeFirst() );
	};
	m_notesMutex.unlock();
	
	const fpp_t frames = Engine::mixer()->framesPerPeriod();

	process( _working_buffer, frames );
	instrumentTrack()->processAudioBuffer( _working_buffer, frames, NULL );
//	release_frame = 0; //removed for issue # 1432
}



void lb302Synth::deleteNotePluginData( NotePlayHandle * _n )
{
	//printf("GONE\n");
	if( m_playingNote == _n )
	{
		m_playingNote = NULL;
	}
}


PluginView * lb302Synth::instantiateView( QWidget * _parent )
{
	return( new lb302SynthView( this, _parent ) );
}


lb302SynthView::lb302SynthView( Instrument * _instrument, QWidget * _parent ) :
	InstrumentView( _instrument, _parent )
{
}


lb302SynthView::~lb302SynthView()
{
}


void lb302SynthView::modelChanged()
{
}



extern "C"
{

// necessary for getting instance out of shared lib
Plugin * PLUGIN_EXPORT lmms_plugin_main( Model *, void * _data )
{

	return( new lb302Synth(
	        static_cast<InstrumentTrack *>( _data ) ) );
}


}


