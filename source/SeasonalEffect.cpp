// SeasonalEffect.cpp

#include "SeasonalEffect.h"
#include <iostream>

SeasonalEffect::SeasonalEffect()
    : currentSeason(Season::NONE),
    windowWidth(800.0f), windowHeight(600.0f) { }

void SeasonalEffect::initialize(Season initialSeason, float windowWidth, float windowHeight) {
    currentSeason = initialSeason;
    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;
}

void SeasonalEffect::setSeason(Season newSeason) {
    currentSeason = newSeason;
}

void SeasonalEffect::updateWindowSize(float windowWidth, float windowHeight) {
    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;
}
