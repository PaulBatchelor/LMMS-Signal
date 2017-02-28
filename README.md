# LMMS-Signal

LMMS-Signal is a simple utility instrument designed to work with 
[LMMS-Sporth](http://www.github.com/paulbatchelor/lmms-sporth) to build
monophonic synthesizers. 

## How it works

When a note a pressed in LMMS-signal, it generates a special stereo signal. In
the left channel is the midi note number, while on the right channel is a gate
signal. These signals can be processed by sporth ugens to make monophonic 
synthesizer sounds. 

