#pragma once
static const int SCALE_FACTOR = 10;


//const int SCREEN_WIDTH = 64 * SCALE_FACTOR;
//const int SCREEN_HEIGHT = 32 * SCALE_FACTOR;
static const int SCREEN_WIDTH = 1366;
static const int SCREEN_HEIGHT = 768;

extern bool quit;

void frontendInit();

void frontendUpdate();

void frontendClose();