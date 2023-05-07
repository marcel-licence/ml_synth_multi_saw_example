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
 * @file rp2040_adc.ino
 * @author Marcel Licence
 * @date 25.03.2023
 *
 * @brief Little analog input adoptation
 */


#ifdef ADC_ENABLED

#include <rp2040_adc.h>


#define CAPTURE_DEPTH 256
#ifdef ADC_CONTROL_NOTE
#define ADC_CONTROLLED_CH   15
#define ADC_CONTROLLED_NOTE  128
#endif

struct adc_link
{
    uint8_t ch;
    uint8_t control_number;
};

struct adc_link adcToMidiMap[4] =
{
    {0x00, 0x11},
    {0x01, 0x11},
#if 0
    {0x07, 0x11},
    {0x01, 0x12},
#else
    {0x03, 0x11}, /* cutoff */
    {0x01, 0x12},
#endif
};


uint16_t capture_buf[CAPTURE_DEPTH][4];
bool adcToNotePend = false;


static uint32_t adcRaws[4];
static uint32_t adcVals[4];
static uint32_t filVals[4];
static uint8_t outVals[4];

bool noteIsOn = false;


void AdcSetup()
{
    adc_setup(&capture_buf[0][0], CAPTURE_DEPTH);
}

#ifdef ADC_CONTROL_NOTE
void AdcToNote(uint32_t adc1Val, uint32_t adc2Val)
{
    if (noteIsOn == false)
    {
        if (adc2Val > 55000)
        {
            App_NoteOn(ADC_CONTROLLED_CH, ADC_CONTROLLED_NOTE, 127);
            noteIsOn = true;

            adc2Val -= 55000;
        }
    }
    else
    {
        if (adc2Val < 55000)
        {
            App_NoteOff(ADC_CONTROLLED_CH, ADC_CONTROLLED_NOTE);
            noteIsOn = false;
        }
    }

    adc1Val >>= 6;
    adc1Val += (1 << 13);
    adc2Val >>= 4;

    App_NoteSetPitch(ADC_CONTROLLED_CH, ADC_CONTROLLED_NOTE, adc1Val);
    App_NoteSetVolume(ADC_CONTROLLED_CH, ADC_CONTROLLED_NOTE, adc2Val);
}
#endif

void AdcCheck()
{
    if (adcToNotePend)
    {
        adcToNotePend = false;
#ifdef ADC_CONTROL_NOTE
        AdcToNote(adcRaws[2], adcRaws[3]);
#endif
    }
    else if (adc_done())
    {
        adc_stop();

        for (int c = 0; c < 4; ++c)
        {
            /* add all captured values */
            uint32_t adcVal = 0;
            for (int i = 0; i < CAPTURE_DEPTH; i++)
            {
                adcVal += capture_buf[i][c];
            }

            adcRaws[c] = adcVal ;
            adcVals[c] = adcVal / CAPTURE_DEPTH;

            if (adcVals[c] > filVals[c] + 8)
            {
                filVals[c] = adcVals[c] - 8;
            }
            if (adcVals[c] < filVals[c] - 8)
            {
                filVals[c] = adcVals[c] + 8;
            }

            uint8_t newVal;
            if (filVals[c] < 80)
            {
                newVal = 0;
            }
            else if (filVals[c] >= 4017)
            {
                newVal = 127;
            }
            else
            {
                newVal = (filVals[c] - 80) / 31;
            }
            if (outVals[c] != newVal)
            {
#ifdef ADC_CONTROL_NOTE
                if (c < 2)
#endif
                {
                    Midi_ControlChange(adcToMidiMap[c].ch, adcToMidiMap[c].control_number, newVal);
                }
                outVals[c] = newVal;
            }
        }

        adcToNotePend = true;

        restartAdc();
    }
}

#endif /* #ifdef ADC_ENABLED */
