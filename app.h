/*
 * Copyright (c) 2026 Marcel Licence
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
 * @file app.h
 * @author Marcel Licence
 * @date 18.03.2026
 *
 * @brief Declarations of the app
 */


#ifndef APP_H_
#define APP_H_


#include "config.h"

#include <stdint.h>


void App_Setup(void);
void App_Loop(void);

void App_Setup1(void);
void App_Loop1(void);


void App_ButtonA(uint8_t setting, uint8_t value);
void App_ButtonCh(uint8_t setting, uint8_t value);
void App_ButtonSlicer(uint8_t setting, uint8_t value);
void App_SliderSawCtrl(uint8_t id, uint8_t value);
void App_NoteOn(uint8_t ch, uint8_t note, uint8_t vel);
void App_NoteOff(uint8_t ch, uint8_t note);
void App_PitchBend(uint8_t ch, uint16_t bend);
void App_ModulationWheel(uint8_t ch, uint8_t value);


#endif /* APP_H_ */
