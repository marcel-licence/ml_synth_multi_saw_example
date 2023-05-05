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
 * @file ml_synth_organ_example.ino
 * @author Marcel Licence
 * @date 26.11.2021
 *
 * @brief   This is the main project file to test the ML_SynthLibrary (multi saw module)
 *          It should be compatible with the Raspberry Pi Pico and all other RP2040 platforms
 *
 * @see     https://youtu.be/kcf597op8o4
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#include "config.h"


#include <Arduino.h>
#include <Wire.h>


/*
 * Library can be found on https://github.com/marcel-licence/ML_SynthTools
 */
#include <ml_types.h>
#include <ml_alg.h>
#include <ml_waveform.h>
#include <ml_filter.h>

#include <ml_multi_saw.h>
#include <ml_slicer.h>

#ifdef ARP_MODULE_ENABLED
#include <ml_arp.h>
#endif

#ifdef REVERB_ENABLED
#include <ml_reverb.h>
#endif
#include <ml_delay.h>
#ifdef OLED_OSC_DISP_ENABLED
#include <ml_scope.h>
#endif


/* centralized modules */
#define ML_SYNTH_INLINE_DECLARATION
#include <ml_inline.h>
#undef ML_SYNTH_INLINE_DECLARATION


#if (defined ARDUINO_GENERIC_F407VGTX) || (defined ARDUINO_DISCO_F407VG)
#include <Wire.h> /* todo remove, just for scanning */
#endif




char shortName[] = "ML_MultiSaw";

float static_sine[WAVEFORM_CNT];
float *sine = static_sine;


struct filterQCoeffT mainFiltC;
struct filterQProcT mainFiltL;
struct filterQProcT mainFiltR;


void setup()
{
    for (int i = 0; i < WAVEFORM_CNT; i++)
    {
        float val = (float)sin(i * 2.0 * PI / WAVEFORM_CNT);
        sine[i] = val;
    }

    Filter_Init(&mainFiltL, &mainFiltC);
    Filter_Init(&mainFiltR, &mainFiltC);

    /*
     * this code runs once
     */
#ifdef MIDI_USB_ENABLED
    Midi_Usb_Setup();
#endif

#ifdef BLINK_LED_PIN
    Blink_Setup();
    Blink_Fast(3);
#endif

#ifdef ARDUINO_DAISY_SEED
    DaisySeed_Setup();
#endif

    delay(500);

#ifdef SWAP_SERIAL
    /* only one hw serial use this for ESP */
    Serial.begin(115200);
    delay(500);
#else
    Serial.begin(SERIAL_BAUDRATE);
#endif

    Serial.println();


    Serial.printf("Loading data\n");


    Serial.printf("Multi Saw Synth Example");


#ifdef ESP8266
    Midi_Setup();
#endif

    Serial.printf("Initialize Audio Interface\n");
    Audio_Setup();

#ifdef TEENSYDUINO
    Teensy_Setup();
#else

#ifdef ARDUINO_SEEED_XIAO_M0
    pinMode(7, INPUT_PULLUP);
    Midi_Setup();
    pinMode(LED_BUILTIN, OUTPUT);
#else

#ifndef ESP8266 /* otherwise it will break audio output */
    Midi_Setup();
#endif
#endif

#endif


#ifdef REVERB_ENABLED
    /*
     * Initialize reverb
     * The buffer shall be static to ensure that
     * the memory will be exclusive available for the reverb module
     */

    static float *revBuffer = (float *)malloc(sizeof(float) * REV_BUFF_SIZE);
    Reverb_Setup(revBuffer);
#endif

#ifdef MAX_DELAY
    /*
     * Prepare a buffer which can be used for the delay
     */
#ifdef ESP32
    static int16_t *delBuffer1 = (int16_t *)malloc(sizeof(int16_t) * MAX_DELAY);
    static int16_t *delBuffer2 = (int16_t *)malloc(sizeof(int16_t) * MAX_DELAY);
#else
    static int16_t delBuffer1[MAX_DELAY];
    static int16_t delBuffer2[MAX_DELAY];
#endif
    DelayQ_Init2(delBuffer1, delBuffer2, MAX_DELAY);
#endif

#ifdef MIDI_BLE_ENABLED
    midi_ble_setup();
#endif

#ifdef USB_HOST_ENABLED
    Usb_Host_Midi_setup();
#endif

#ifdef ESP32
    Serial.printf("ESP.getFreeHeap() %d\n", ESP.getFreeHeap());
    Serial.printf("ESP.getMinFreeHeap() %d\n", ESP.getMinFreeHeap());
    Serial.printf("ESP.getHeapSize() %d\n", ESP.getHeapSize());
    Serial.printf("ESP.getMaxAllocHeap() %d\n", ESP.getMaxAllocHeap());

    /* PSRAM will be fully used by the looper */
    Serial.printf("Total PSRAM: %d\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());
#endif

#if (defined OLED_OSC_DISP_ENABLED) && (defined TEENSYDUINO)
    ScopeOled_Setup();
#endif

#ifdef USB_HOST_ENABLED
    Usb_Host_Midi_setup();
#endif

#ifdef ARP_MODULE_ENABLED
    Arp_Init(1750); /* some quick tempo */
    Arp_SelectSequence(1, 1);
    Arp_Active();
#endif

    Slicer_SetType(0, 0);
    Slicer_SetDecay(0, 127);
    Slicer_SetDepth(0, 0);

    Serial.printf("Wait for multi saw module to be initialized\n");
    MultiSawSynth_Init(SAMPLE_RATE);
    Serial.printf("Multi saw module ready\n");
    MultiSawSynth_SetDetune(32);
    MultiSawSynth_SetCount(127);

#ifdef NOTE_ON_AFTER_SETUP
    App_NoteOn(0, 64, 127);
#endif


#ifdef MIDI_STREAM_PLAYER_ENABLED
    MidiStreamPlayer_Init();

    char midiFile[] = "/song.mid";
    MidiStreamPlayer_PlayMidiFile_fromLittleFS(midiFile, 1);
#endif

#if (defined MIDI_VIA_USB_ENABLED) || (defined OLED_OSC_DISP_ENABLED)
#ifdef ESP32
    Core0TaskInit();
#else
    //#error only supported by ESP32 platform
#endif
#endif

#ifdef ADC_ENABLED
    AdcSetup();
#endif
}

#ifdef ESP32
/*
 * Core 0
 */
/* this is used to add a task to core 0 */
TaskHandle_t Core0TaskHnd;

inline
void Core0TaskInit()
{
    /* we need a second task for the terminal output */
    xTaskCreatePinnedToCore(Core0Task, "CoreTask0", 8000, NULL, 999, &Core0TaskHnd, 0);
}

void Core0TaskSetup()
{
    /*
     * init your stuff for core0 here
     */

#ifdef OLED_OSC_DISP_ENABLED
    ScopeOled_Setup();
#endif
#ifdef MIDI_VIA_USB_ENABLED
    UsbMidi_Setup();
#endif
}

void Core0TaskLoop()
{
    /*
     * put your loop stuff for core0 here
     */
#ifdef MIDI_VIA_USB_ENABLED
    UsbMidi_Loop();
#endif

#ifdef OLED_OSC_DISP_ENABLED
    ScopeOled_Process();
#endif
}

void Core0Task(void *parameter)
{
    Core0TaskSetup();

    while (true)
    {
        Core0TaskLoop();

        /* this seems necessary to trigger the watchdog */
        delay(1);
        yield();
    }
}
#endif /* ESP32 */

void loop_1Hz()
{
#ifdef CYCLE_MODULE_ENABLED
    CyclePrint();
#endif
#ifdef BLINK_LED_PIN
    Blink_Process();
#endif
}


void loop()
{
    static int loop_cnt_1hz = 0; /*!< counter to allow 1Hz loop cycle */

#ifdef SAMPLE_BUFFER_SIZE
    loop_cnt_1hz += SAMPLE_BUFFER_SIZE;
#else
    loop_cnt_1hz += 1; /* in case only one sample will be processed per loop cycle */
#endif
    if (loop_cnt_1hz >= SAMPLE_RATE)
    {
        loop_cnt_1hz -= SAMPLE_RATE;
        loop_1Hz();
    }

#ifdef ADC_ENABLED
    AdcCheck();
#endif

#ifdef ARP_MODULE_ENABLED
    Arp_Process(1);
#endif

    /*
     * MIDI processing
     */
    Midi_Process();

#ifdef MIDI_VIA_USB_ENABLED
    UsbMidi_ProcessSync();
#endif
#ifdef MIDI_STREAM_PLAYER_ENABLED
    MidiStreamPlayer_Tick(SAMPLE_BUFFER_SIZE);
#endif

#ifdef MIDI_BLE_ENABLED
    midi_ble_loop();
#endif

#ifdef USB_HOST_ENABLED
    Usb_Host_Midi_loop();
#endif

#ifdef MIDI_USB_ENABLED
    Midi_Usb_Loop();
#endif

    /*
     * And finally the audio stuff
     */
    Q1_14 left[SAMPLE_BUFFER_SIZE];
    Q1_14 right[SAMPLE_BUFFER_SIZE];

    memset(left, 0, sizeof(left));
    memset(right, 0, sizeof(left));

    MultiSawSynth_Process(left, right, SAMPLE_BUFFER_SIZE);

    Filter_Process_Buffer(left, &mainFiltL, SAMPLE_BUFFER_SIZE);
    Filter_Process_Buffer(right, &mainFiltR, SAMPLE_BUFFER_SIZE);

    Slicer_Process(left, right, SAMPLE_BUFFER_SIZE);

#ifdef MAX_DELAY
    DelayQ_Process_Buff(&left[0].s16, &right[0].s16, &left[0].s16, &right[0].s16, SAMPLE_BUFFER_SIZE);
#endif

    /*
     * Output the audio
     */
    Audio_Output(left, right);
}

void Status_ValueChangedFloat(const char *descr, float value)
{
    Serial.printf("%s: %0.3f\n", descr, value);
}

void Status_ValueChangedInt(const char *descr, int value)
{
    Serial.printf("%s: %d\n", descr, value);
}

void App_NoteOn(uint8_t ch, uint8_t note, uint8_t vel)
{
#ifdef ARP_MODULE_ENABLED
    Arp_NoteOn(ch, note, vel);
#else
    MultiSawSynth_NoteOn(ch, note, vel);
#endif
}

void App_NoteOff(uint8_t ch, uint8_t note)
{
#ifdef ARP_MODULE_ENABLED
    Arp_NoteOff(ch, note);
#else
    MultiSawSynth_NoteOff(ch, note);
#endif
}

void App_NoteSetPitch(uint8_t ch, uint8_t note, uint32_t pitch)
{
    MultiSawSynth_NoteSetPitch(ch, note, pitch);
}

void App_NoteSetVolume(uint8_t ch, uint8_t note, uint16_t volume)
{
    MultiSawSynth_NoteSetVolume(ch, note, volume);
}

void App_PitchBend(uint8_t ch, uint16_t bend)
{
    MultiSawSynth_PitchBend(ch, bend);
}

void App_ModulationWheel(uint8_t ch, uint8_t value)
{
    /* not implemented yet */
}

/*
 * MIDI via USB Host Module
 */
#ifdef MIDI_VIA_USB_ENABLED
void App_UsbMidiShortMsgReceived(uint8_t *msg)
{
#ifdef MIDI_TX2_PIN
    Midi_SendShortMessage(msg);
#endif
    Midi_HandleShortMsg(msg, 8);
}
#endif

/*
 * MIDI callbacks
 */
inline void App_ButtonA(uint8_t setting, uint8_t value)
{
    if (value > 0)
    {
        MultiSawSynth_SetProfile(setting);
    }
}

inline void App_ButtonCh(uint8_t setting, uint8_t value)
{
    if (value > 0)
    {
        MultiSawSynth_SetCurrentCh(setting);
    }
}

inline void App_ButtonSlicer(uint8_t setting, uint8_t value)
{
    if (value > 0)
    {
        Slicer_SetType(0, setting);
    }
}

inline void App_SliderSawCtrl(uint8_t id, uint8_t value)
{
    switch (id)
    {
    case 0:
        MultiSawSynth_SetDetune(value);
        break;
    case 1:
        MultiSawSynth_SetCount(value);
        break;
    case 2:
        MultiSawSynth_SetProfile(value);
        break;
    case 3:
        {
            if (value < 30)
            {
                value = 30;
            }
            if (value > 126)
            {
                value = 126;
            }
            float c = value;
            c *= 1.0f / 127.0f;
            Filter_Calculate(c, 0.5f, &mainFiltC);
        }
        break;
    }
}

/*
 * Arp Callbacks
 */
void Arp_Cb_NoteOn(uint8_t ch, uint8_t note, float vel)
{
    MultiSawSynth_NoteOn(ch, note, vel);
}

void Arp_Cb_NoteOff(uint8_t ch, uint8_t note)
{
    MultiSawSynth_NoteOff(ch, note);
}

void Arp_Status_ValueChangedInt(const char *msg, int value)
{
    // Status_ValueChangedInt(msg, value);
}

void Arp_Status_LogMessage(const char *msg)
{
    //Status_LogMessage(msg);
}

void Arp_Status_ValueChangedFloat(const char *msg, float value)
{
    // Status_ValueChangedFloat(msg, value);
}

void Arp_Cb_Step(uint8_t step)
{
    /* ignore */
}

#if defined(I2C_SCL) && defined(I2C_SDA)
void  ScanI2C(void)
{
#ifdef ARDUINO_GENERIC_F407VGTX
    Wire.setSDA(I2C_SDA);
    Wire.setSCL(I2C_SCL);
    Wire.begin();//I2C_SDA, I2C_SCL);
#else
    Wire.begin();
#endif

    byte address;
    int nDevices;

    Serial.println("Scanning...");

    nDevices = 0;
    for (address = 1; address < 127; address++)
    {
        byte r_error;
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        r_error = Wire.endTransmission();

        if (r_error == 0)
        {
            Serial.print("I2C device found at address 0x");
            if (address < 16)
            {
                Serial.print("0");
            }
            Serial.print(address, HEX);
            Serial.println("  !");

            nDevices++;
        }
        else if (r_error == 4)
        {
            Serial.print("Unknown error at address 0x");
            if (address < 16)
            {
                Serial.print("0");
            }
            Serial.println(address, HEX);
        }
    }
    if (nDevices == 0)
    {
        Serial.println("No I2C devices found\n");
    }
    else
    {
        Serial.println("done\n");
    }
}
#endif /* (defined ARDUINO_GENERIC_F407VGTX) || (defined ARDUINO_DISCO_F407VG) */

