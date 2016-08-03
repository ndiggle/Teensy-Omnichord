//////////////////////////////////////////////////////////////////////
/*
	Title:
	Teensy3.2-Based Omnichord MIDI Controller
	
	Description:
	- This project is designed to mimic the functionality of a Suzuki
	Omnichord, through the use of a Teensy 3.2 dev. board and a few
	push-buttons.
	- If you press any of the 12 note keys, single notes will play
	- If you hold any of the 3 chord-select buttons (major, minor,
	or major-seventh), and then press any note key, the chord
	coresponding to that root note will be played

	Note:
	Use of the Bounce libary and associated objects is optional.
*/
//////////////////////////////////////////////////////////////////////

/* Buttons to USB MIDI Example

   You must select MIDI from the "Tools > USB Type" menu

   To view the raw MIDI data on Linux: aseqdump -p "Teensy MIDI"

   This example code is in the public domain.
*/

#include <Bounce.h>

// User-Modifiable Values
#define MIDI_CH           1
#define MIDI_VEL          80
#define DEBOUNCE_DELAY_MS 5

unsigned char chordType = 0;	// Designates chord type of most the recent chord-select command
									// 0: Single note
									// 1: Major Chord
									// 2: Minor Chord
									// 3: Major 7th Chord
unsigned char transpose = 0;	// Octave-shift value (semitones)

// Initialize all note inputs as Bounce objects (simplifies debouncing issues)
Bounce button0  = Bounce( 0, DEBOUNCE_DELAY_MS);	// C note
Bounce button1  = Bounce( 1, DEBOUNCE_DELAY_MS);	// C# note
Bounce button2  = Bounce( 2, DEBOUNCE_DELAY_MS);	// D note
Bounce button3  = Bounce( 3, DEBOUNCE_DELAY_MS);	// Eb note
Bounce button4  = Bounce( 4, DEBOUNCE_DELAY_MS);	// E note
Bounce button5  = Bounce( 5, DEBOUNCE_DELAY_MS);	// F note
Bounce button6  = Bounce( 6, DEBOUNCE_DELAY_MS);	// Gb note
Bounce button7  = Bounce( 7, DEBOUNCE_DELAY_MS);	// G note
Bounce button8  = Bounce( 8, DEBOUNCE_DELAY_MS);	// Ab note
Bounce button9  = Bounce( 9, DEBOUNCE_DELAY_MS);	// A note
Bounce button10 = Bounce(10, DEBOUNCE_DELAY_MS);	// Bb note
Bounce button11 = Bounce(11, DEBOUNCE_DELAY_MS);	// B note

Bounce octaveUp   = Bounce(16, DEBOUNCE_DELAY_MS);	// Octave-Up
Bounce octaveDown = Bounce(17, DEBOUNCE_DELAY_MS);	// Octave-Down

void setup() {
	// Set up inputs for notes 1-12/C-B
	pinMode( 0, INPUT_PULLUP);
	pinMode( 1, INPUT_PULLUP);
	pinMode( 2, INPUT_PULLUP);
	pinMode( 3, INPUT_PULLUP);
	pinMode( 4, INPUT_PULLUP);
	pinMode( 5, INPUT_PULLUP);
	pinMode( 6, INPUT_PULLUP);
	pinMode( 7, INPUT_PULLUP);
	pinMode( 8, INPUT_PULLUP);
	pinMode( 9, INPUT_PULLUP);
	pinMode(10, INPUT_PULLUP);
	pinMode(11, INPUT_PULLUP);

	// Set Up button inputs for chord-select buttons
	pinMode(12, INPUT_PULLUP);
	pinMode(14, INPUT_PULLUP);
	pinMode(15, INPUT_PULLUP);

	// Set up button inputs for octave-shift buttons
	pinMode(16, INPUT_PULLUP);
	pinMode(17, INPUT_PULLUP);
}

void playMaj(unsigned char note) {
	usbMIDI.sendNoteOn(note, MIDI_VEL, MIDI_CH);
	usbMIDI.sendNoteOn(note + 4, MIDI_VEL, MIDI_CH);
	usbMIDI.sendNoteOn(note + 7, MIDI_VEL, MIDI_CH);
}

void playMin(unsigned char note) {
	usbMIDI.sendNoteOn(note, MIDI_VEL, MIDI_CH);
	usbMIDI.sendNoteOn(note + 3, MIDI_VEL, MIDI_CH);
	usbMIDI.sendNoteOn(note + 7, MIDI_VEL, MIDI_CH);
}

void playSev(unsigned char note) {
	usbMIDI.sendNoteOn(note, MIDI_VEL, MIDI_CH);
	usbMIDI.sendNoteOn(note + 4, MIDI_VEL, MIDI_CH);
	usbMIDI.sendNoteOn(note + 7, MIDI_VEL, MIDI_CH);
	usbMIDI.sendNoteOn(note + 11, MIDI_VEL, MIDI_CH);
}

void releaseMajor(unsigned char note) {
	usbMIDI.sendNoteOff(note, MIDI_VEL, MIDI_CH);
	usbMIDI.sendNoteOff(note + 4, MIDI_VEL, MIDI_CH);
	usbMIDI.sendNoteOff(note + 7, MIDI_VEL, MIDI_CH);
}

void releaseMinor(unsigned char note) {
	usbMIDI.sendNoteOff(note, MIDI_VEL, MIDI_CH);
	usbMIDI.sendNoteOff(note + 3, MIDI_VEL, MIDI_CH);
	usbMIDI.sendNoteOff(note + 7, MIDI_VEL, MIDI_CH);
}

void releaseSeventh(unsigned char note) {
	usbMIDI.sendNoteOff(note, MIDI_VEL, MIDI_CH);
	usbMIDI.sendNoteOff(note + 4, MIDI_VEL, MIDI_CH);
	usbMIDI.sendNoteOff(note + 7, MIDI_VEL, MIDI_CH);
	usbMIDI.sendNoteOff(note + 11, MIDI_VEL, MIDI_CH);
}

// 
void noteOn(unsigned char note) {
	note += transpose;

	if (digitalRead(12) == LOW) {          // Major Chord
		playMaj(note);
		chordType = 1;
	} else if (digitalRead(14) == LOW) {   // Minor Chord
		playMin(note);
		chordType = 2;
	} else if (digitalRead(15) == LOW) {   // Seventh Chord
		playSev(note);
		chordType = 3;
	} else {                               // Single Note Pressed
		usbMIDI.sendNoteOn(note, MIDI_VEL, MIDI_CH);
		chordType = 0;
	}
}

void noteOff(unsigned char note) {
	note += transpose;

	if (chordType == 0)
		usbMIDI.sendNoteOff(note, MIDI_VEL, MIDI_CH);
	if (chordType == 1)
		releaseMajor(note);
	if (chordType == 2)
		releaseMinor(note);
	if (chordType == 3)
		releaseSeventh(note);
}

void loop() {
	// Scan each button input for notes 1-12
	button0.update();
	button1.update();
	button2.update();
	button3.update();
	button4.update();
	button5.update();
	button6.update();
	button7.update();
	button8.update();
	button9.update();
	button10.update();
	button11.update();

	// Scan for octave-shift commands
	octaveUp.update();
	octaveDown.update();

	// Read and interpret note triggers
	if (button0.fallingEdge())
		noteOn(60);
	if (button1.fallingEdge())
		noteOn(61);
	if (button2.fallingEdge())
		noteOn(62);
	if (button3.fallingEdge())
		noteOn(63);
	if (button4.fallingEdge())
		noteOn(64);
	if (button5.fallingEdge())
		noteOn(65);
	if (button6.fallingEdge())
		noteOn(66);
	if (button7.fallingEdge())
		noteOn(67);
	if (button8.fallingEdge())
		noteOn(68);
	if (button9.fallingEdge())
		noteOn(69);
	if (button10.fallingEdge())
		noteOn(70);
	if (button11.fallingEdge())
		noteOn(71);

	// Read and interpret octave-shift commands
	if (octaveUp.fallingEdge())
		transpose += 12;
	if (octaveDown.fallingEdge())
		transpose -= 12;

	// Read and interpret note releases
	if (button0.risingEdge())
		noteOff(60);
	if (button1.risingEdge())
		noteOff(61);
	if (button2.risingEdge())
		noteOff(62);
	if (button3.risingEdge())
		noteOff(63);
	if (button4.risingEdge())
		noteOff(64);
	if (button5.risingEdge())
		noteOff(65);
	if (button6.risingEdge())
		noteOff(66);
	if (button7.risingEdge())
		noteOff(67);
	if (button8.risingEdge())
		noteOff(68);
	if (button9.risingEdge())
		noteOff(69);
	if (button10.risingEdge())
		noteOff(70);
	if (button11.risingEdge())
		noteOff(71);

	// Disregard MIDI input data
	while (usbMIDI.read());
}
