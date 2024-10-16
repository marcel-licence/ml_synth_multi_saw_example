/*
 * Copyright (c) 2023 Marcel Licence
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Dieses Programm ist Freie Software: Sie können es unter den Bedingungen
 * der GNU General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
 * veröffentlichten Version, weiter verteilen und/oder modifizieren.
 *
 * Dieses Programm wird in der Hoffnung bereitgestellt, dass es nützlich sein wird, jedoch
 * OHNE JEDE GEWÄHR,; sogar ohne die implizite
 * Gewähr der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU General Public License für weitere Einzelheiten.
 *
 * Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 * Programm erhalten haben. Wenn nicht, siehe <https://www.gnu.org/licenses/>.
 */

/**
 * @file z_config.ino
 * @author Marcel Licence
 * @date 12.05.2021
 *
 * @brief This file contains the mapping configuration
 * Put all your project configuration here (no defines etc)
 * This file will be included at the and can access all
 * declarations and type definitions
 *
 * @see ESP32 Arduino DIY Synthesizer Projects - Little startup guide to get your MIDI synth working - https://youtu.be/ZNxGCB-d68g
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#ifdef AUDIO_KIT_BUTTON_ANALOG
audioKitButtonCb audioKitButtonCallback = NULL;
#endif

/*
 * this mapping is used for the edirol pcr-800
 * this should be changed when using another controller
 */
struct midiControllerMapping edirolMapping[] =
{
    /* general MIDI */
    { 0x0, 0x40, "sustain", NULL, NULL, 0},

    /* transport buttons */
    { 0x8, 0x52, "back", NULL, NULL, 0},
    { 0xD, 0x52, "stop", NULL, NULL, 0},
    { 0xe, 0x52, "start", NULL, NULL, 0},
    { 0xe, 0x52, "start", NULL, NULL, 0},
    { 0xa, 0x52, "rec", NULL, NULL, 0},

    /* upper row of buttons */
    { 0x0, 0x50, "A1", NULL, App_ButtonA, 0},
    { 0x1, 0x50, "A2", NULL, App_ButtonA, 1},
    { 0x2, 0x50, "A3", NULL, App_ButtonA, 2},
    { 0x3, 0x50, "A4", NULL, App_ButtonA, 3},

#ifdef MIDI_STREAM_PLAYER_ENABLED
    { 0x4, 0x50, "A5", NULL, MidiStreamPlayerCtrl, MIDI_STREAM_PLAYER_CTRL_PAUSE},
    { 0x5, 0x50, "A6", NULL, MidiStreamPlayerCtrl, MIDI_STREAM_PLAYER_CTRL_STOP},
    { 0x6, 0x50, "A7", NULL, MidiStreamPlayerCtrl, MIDI_STREAM_PLAYER_CTRL_PLAY},
    { 0x7, 0x50, "A8", NULL, MidiStreamPlayerCtrl, MIDI_STREAM_PLAYER_CTRL_SKIP},
#else
    { 0x4, 0x50, "A5", NULL, NULL, 0},
    { 0x5, 0x50, "A6", NULL, NULL, 1},
    { 0x6, 0x50, "A7", NULL, NULL, 2},
    { 0x7, 0x50, "A8", NULL, NULL, 0},
#endif

    { 0x0, 0x53, "A9", NULL, App_ButtonCh, 0},

    /* lower row of buttons */
    { 0x0, 0x51, "B1", NULL, App_ButtonSlicer, 0},
    { 0x1, 0x51, "B2", NULL, App_ButtonSlicer, 1},
    { 0x2, 0x51, "B3", NULL, App_ButtonSlicer, 2},
    { 0x3, 0x51, "B4", NULL, App_ButtonSlicer, 3},

    { 0x4, 0x51, "B5", NULL, App_ButtonSlicer, 4},
    { 0x5, 0x51, "B6", NULL, App_ButtonSlicer, 5},
    { 0x6, 0x51, "B7", NULL, App_ButtonSlicer, 6},
    { 0x7, 0x51, "B8", NULL, App_ButtonSlicer, 7},

    { 0x1, 0x53, "B9", NULL, App_ButtonCh, 1},

    /* pedal */
    { 0x0, 0x0b, "VolumePedal", NULL, NULL, 0},

    /* slider */
    { 0x0, 0x11, "S1", NULL, App_SliderSawCtrl, 0},
    { 0x1, 0x11, "S2", NULL, App_SliderSawCtrl, 1},
    { 0x2, 0x11, "S3", NULL, App_SliderSawCtrl, 2},
    { 0x3, 0x11, "S4", NULL, App_SliderSawCtrl, 3},

    { 0x4, 0x11, "S5", NULL, Slicer_SetDecay, 4},
    { 0x5, 0x11, "S6", NULL, Slicer_SetDepth, 5},
    { 0x6, 0x11, "S7", NULL, Slicer_SetType, 6},
#ifdef MAX_DELAY_Q
    { 0x7, 0x11, "S8", NULL, DelayQ_SetFeedback, 7},

    { 0x1, 0x12, "S9", NULL, DelayQ_SetOutputLevel, 8},
#endif

    /* rotary */
#ifdef MIDI_STREAM_PLAYER_ENABLED
    { 0x0, 0x10, "R1", NULL, MidiStreamPlayerTempo, 0},
#else
    { 0x0, 0x10, "R1", NULL, NULL, 0},
#endif
    { 0x1, 0x10, "R2", NULL, NULL, 1},
    { 0x2, 0x10, "R3", NULL, NULL, 2},
#if 1
    { 0x3, 0x10, "R4", NULL, NULL, 3},

    { 0x4, 0x10, "R5", NULL, NULL, 4},
#else
#ifdef MAX_DELAY
    { 0x3, 0x10, "R4", NULL, DelayQ_SetOutputLevelInt, 3},

    { 0x4, 0x10, "R5", NULL, DelayQ_SetFeedbackInt, 4},
#endif
#endif
    { 0x5, 0x10, "R6", NULL, NULL, 5},
    { 0x6, 0x10, "R7", NULL, NULL, 6},
    { 0x7, 0x10, "R8", NULL, NULL, 7},
#ifdef REVERB_ENABLED
    { 0x0, 0x12, "R9", NULL, Reverb_SetLevelInt, 8},
#endif
    /* Central slider */
    { 0x0, 0x13, "H1", NULL, NULL, 0},


    /* MIDI defaults */
    { 0x0, 7, "Volume", NULL, NULL, 0},
#ifdef REVERB_ENABLED
    { 0x0, 91, "Reverb", NULL, Reverb_SetLevelInt, 8},
#endif
    { 0x0, 93, "Chorus", NULL, NULL, 0},

};

struct midiMapping_s midiMapping =
{
    NULL,
    App_NoteOn,
    App_NoteOff,
    App_PitchBend,
    App_ModulationWheel,
    NULL, /* assign program change callback here! */
    NULL,
    NULL,
    edirolMapping,
    sizeof(edirolMapping) / sizeof(edirolMapping[0]),
};

#ifdef MIDI_VIA_USB_ENABLED
struct usbMidiMappingEntry_s usbMidiMappingEntries[] =
{
    {
        NULL,
        App_UsbMidiShortMsgReceived,
        NULL,
        NULL,
        0xFF,
    },
};

struct usbMidiMapping_s usbMidiMapping =
{
    NULL,
    NULL,
    usbMidiMappingEntries,
    sizeof(usbMidiMappingEntries) / sizeof(usbMidiMappingEntries[0]),
};
#endif /* MIDI_VIA_USB_ENABLED */
