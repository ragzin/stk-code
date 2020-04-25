//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2020 SuperTuxKart-Team
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef HEADER_SDL_CONTROLLER_HPP
#define HEADER_SDL_CONTROLLER_HPP

#ifndef SERVER_ONLY

#include <SDL.h>
#include <IEventReceiver.h>
#include <bitset>
#include <vector>
#include "utils/types.hpp"

class GamePadDevice;

class SDLController
{
private:
    SDL_GameController* m_game_controller;

    SDL_Joystick* m_joystick;

    GamePadDevice* m_gamepad;

    int m_buttons;

    int m_axes;

    int m_hats;

    SDL_JoystickID m_id;

    irr::SEvent m_irr_event;

    int16_t m_prev_axes[irr::SEvent::SJoystickEvent::NUMBER_OF_AXES];
#ifdef ANDROID
    void handleDirectScanCode(const SDL_Event& event);
#endif
public:
    // ------------------------------------------------------------------------
    SDLController(int device_id);
    // ------------------------------------------------------------------------
    ~SDLController();
    // ------------------------------------------------------------------------
    const irr::SEvent& getEvent() const                 { return m_irr_event; }
    // ------------------------------------------------------------------------
    SDL_JoystickID getInstanceID() const                       { return m_id; }
    // ------------------------------------------------------------------------
    void handleAxisInputSense(const SDL_Event& event);
    // ------------------------------------------------------------------------
    bool handleAxis(const SDL_Event& event)
    {
        int axis_idx = event.jaxis.axis;
        if (axis_idx > m_axes)
            return false;
        m_irr_event.JoystickEvent.Axis[axis_idx] = event.jaxis.value;
        m_prev_axes[axis_idx] = event.jaxis.value;
        return true;
    }   // handleAxis
    // ------------------------------------------------------------------------
    bool handleHat(const SDL_Event& event,
                   std::bitset<irr::SEvent::SJoystickEvent::NUMBER_OF_BUTTONS>&
                   button_changed)
    {
        if (event.jhat.hat > m_hats)
            return false;
        std::bitset<4> status;
        // Up, right, down and left (4 buttons)
        switch (event.jhat.value)
        {
        case SDL_HAT_UP:
            status[0] = true;
            break;
        case SDL_HAT_RIGHTUP:
            status[0] = true;
            status[1] = true;
            break;
        case SDL_HAT_RIGHT:
            status[1] = true;
            break;
        case SDL_HAT_RIGHTDOWN:
            status[1] = true;
            status[2] = true;
            break;
        case SDL_HAT_DOWN:
            status[2] = true;
            break;
        case SDL_HAT_LEFTDOWN:
            status[2] = true;
            status[3] = true;
            break;
        case SDL_HAT_LEFT:
            status[3] = true;
            break;
        case SDL_HAT_LEFTUP:
            status[3] = true;
            status[0] = true;
            break;
        case SDL_HAT_CENTERED:
        default:
            break;
        }
        std::bitset<4> prev_status;
        int hat_start = m_buttons - (m_hats * 4) + (event.jhat.hat * 4);
        for (unsigned i = 0; i < 4; i++)
        {
            int hat_button_id = i + hat_start;
            prev_status[i] =
                m_irr_event.JoystickEvent.IsButtonPressed(hat_button_id);
            uint32_t value = 1 << hat_button_id;
            if (status[i])
                m_irr_event.JoystickEvent.ButtonStates |= value;
            else
                m_irr_event.JoystickEvent.ButtonStates &= (uint32_t)~value;
            if (prev_status[i] != status[i])
                button_changed[hat_button_id] = true;
        }
        return true;
    }   // handleHat
    // ------------------------------------------------------------------------
    bool handleButton(const SDL_Event& event)
    {
        if (event.jbutton.button > m_buttons)
        {
#ifdef ANDROID
            handleDirectScanCode(event);
#endif
            return false;
        }
        bool pressed = event.jbutton.state == SDL_PRESSED;
        uint32_t value = 1 << event.jbutton.button;
        if (pressed)
            m_irr_event.JoystickEvent.ButtonStates |= value;
        else
            m_irr_event.JoystickEvent.ButtonStates &= (uint32_t)~value;
        return true;
    }   // handleButton
    // ------------------------------------------------------------------------
    SDL_GameController* getGameController() const { return m_game_controller; }
};

#endif

#endif
