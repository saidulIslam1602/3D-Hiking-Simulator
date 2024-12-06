// SeasonalEffect.h

#ifndef SEASONAL_EFFECT_H
#define SEASONAL_EFFECT_H

class SeasonalEffect {
public:
    enum class Season {
        NONE,
        RAIN
    };

    SeasonalEffect();

    void initialize(Season initialSeason, float windowWidth, float windowHeight);
    void render();
    void cleanup();
    void setSeason(Season newSeason);
    void updateWindowSize(float windowWidth, float windowHeight);

private:
    Season currentSeason;
    float windowWidth;
    float windowHeight;
};

#endif // SEASONAL_EFFECT_H
