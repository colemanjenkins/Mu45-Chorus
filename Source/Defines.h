/*
  ==============================================================================

    Defines.h
    Created: 2 Nov 2021 4:02:36pm
    Author:  Coleman Jenkins

  ==============================================================================
*/

#pragma once

// text suffixes for certain type of parameter
#define _PCT            " %"
#define _HZ             " Hz"
#define _MS             " ms"

// interval and skew if not defined
#define DEFAULT_INTERVAL 0.1
#define NO_SKEW          1

#define DEPTH_MIN       0  // %
#define DEPTH_DEFAULT   10
#define DEPTH_MAX       100
#define DEPTH_SUFFIX    _PCT

#define RATE_MIN        0 // Hz
#define RATE_DEFAULT    2
#define RATE_MAX        20
#define RATE_SKEW       0.4
#define RATE_INTERVAL   0.01
#define RATE_SUFFIX     _HZ

#define FOCUS_MIN       20 // Hz
#define FOCUS_DEFAULT   20
#define FOCUS_MAX       10000
#define FOCUS_SKEW      0.3
#define FOCUS_INTERVAL  0.1
#define FOCUS_SUFFIX    _HZ

#define WET_MIN         0 // %
#define WET_DEFAULT     80
#define WET_MAX         100

#define DRY_MIN         0 // %
#define DRY_DEFAULT     100
#define DRY_MAX         100

#define STEREO_MIN      0 // degrees
#define STEREO_DEFAULT  30
#define STEREO_MAX      90
#define STEREO_SUFFIX   ""
#define STERO_INTERVAL  1

#define DELAY_MIN       2 // midpoint of delay LFO in ms
#define DELAY_DEFAULT   15
#define DELAY_MAX       20
#define DELAY_SUFFIX    _MS

#define MAX_TOTAL_DELAY (DELAY_MAX + (DELAY_MAX - INST_DELAY_MIN))

#define INST_DELAY_MIN  1 // instantaneous delay minimum in ms, bottom of delay LFO

// GUI

#define UNIT_LENGTH_X       30
#define UNIT_LENGTH_Y       30

#define KNOB_HEIGHT         4*UNIT_LENGTH_Y
#define KNOB_WIDTH          4*UNIT_LENGTH_X

#define SLIDER_HEIGHT       6*UNIT_LENGTH_Y
#define SLIDER_WIDTH        2*UNIT_LENGTH_X

#define CONTAINER_WIDTH     24*UNIT_LENGTH_X
#define CONTAINER_HEIGHT    12*UNIT_LENGTH_Y
