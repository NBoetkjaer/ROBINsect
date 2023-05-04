#pragma once

static const int NUM_LEGS = 6;
static const int LEG_FRONT_LEFT = 0;
static const int LEG_FRONT_RIGHT = 1;
static const int LEG_MID_LEFT = 2;
static const int LEG_MID_RIGHT = 3;
static const int LEG_BACK_LEFT = 4;
static const int LEG_BACK_RIGHT = 5;

#define IS_LEFT_LEG(legID)  ((legID & 1) == 0)
#define IS_RIGHT_LEG(legID) ((legID & 1) == 1)
#define IS_FRONT_LEG(legID) (legID < 2)
#define IS_MID_LEG(legID) (legID == 2 || legID == 3)
#define IS_BACK_LEG(legID) (legID > 3)
