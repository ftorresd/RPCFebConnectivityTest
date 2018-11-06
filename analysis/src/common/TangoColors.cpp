#include "analysis/TangoColors.h"

std::map<std::string, TColor *> TangoColors::colors_;

TangoColors::TangoColors(int _offset)
{
    createColors(_offset);
}

TangoColors::~TangoColors()
{
    // right, ROOT?
}

void TangoColors::createColors(int _offset, bool _force)
{
    if (colors_.empty() || _force) {
        colors_.clear();

        colors_.insert(std::pair<std::string, TColor *>("TangoSkyBlueA"
                                                        , new TColor(++_offset, 114./255., 159./255., 207./255., "TangoSkyBlueA")));
        colors_.insert(std::pair<std::string, TColor *>("TangoChameleonA"
                                                        , new TColor(++_offset, 138./255., 226./255., 52./255., "TangoChameleonA")));
        colors_.insert(std::pair<std::string, TColor *>("TangoScarletRedA"
                                                        , new TColor(++_offset, 239./255., 41./255., 41./255., "TangoScarletRedA")));
        colors_.insert(std::pair<std::string, TColor *>("TangoOrangeA"
                                                        , new TColor(++_offset, 252./255., 175./255., 62./255., "TangoOrangeA")));
        colors_.insert(std::pair<std::string, TColor *>("TangoPlumA"
                                                        , new TColor(++_offset, 173./255., 127./255., 168./255., "TangoPlumA")));
        colors_.insert(std::pair<std::string, TColor *>("TangoButterA"
                                                        , new TColor(++_offset, 252./255., 233./255., 79./255., "TangoButterA")));
        colors_.insert(std::pair<std::string, TColor *>("TangoAluminiumD"
                                                        , new TColor(++_offset, 136./255., 138./255., 133./255., "TangoAluminiumD")));
        colors_.insert(std::pair<std::string, TColor *>("TangoChocolateA"
                                                        , new TColor(++_offset, 233./255., 185./255., 110./255., "TangoChocolateA")));

        colors_.insert(std::pair<std::string, TColor *>("TangoSkyBlueC"
                                                        , new TColor(++_offset, 32./255., 74./255., 135./255., "TangoSkyBlueC")));
        colors_.insert(std::pair<std::string, TColor *>("TangoChameleonC"
                                                        , new TColor(++_offset, 78./255., 154./255., 6./255., "TangoChameleonC")));
        colors_.insert(std::pair<std::string, TColor *>("TangoScarletRedC"
                                                        , new TColor(++_offset, 164./255., 0./255., 0./255., "TangoScarletRedC")));
        colors_.insert(std::pair<std::string, TColor *>("TangoOrangeC"
                                                        , new TColor(++_offset, 206./255., 92./255., 0./255., "TangoOrangeC")));
        colors_.insert(std::pair<std::string, TColor *>("TangoPlumC"
                                                        , new TColor(++_offset, 92./255., 53./255., 102./255., "TangoPlumC")));
        colors_.insert(std::pair<std::string, TColor *>("TangoButterC"
                                                        , new TColor(++_offset, 196./255., 160./255., 0./255., "TangoButterC")));
        colors_.insert(std::pair<std::string, TColor *>("TangoAluminiumF"
                                                        , new TColor(++_offset, 46./255., 52./255., 54./255., "TangoAluminiumF")));
        colors_.insert(std::pair<std::string, TColor *>("TangoChocolateC"
                                                        , new TColor(++_offset, 143./255., 89./255., 2./255., "TangoChocolateC")));

        colors_.insert(std::pair<std::string, TColor *>("TangoSkyBlueB"
                                                        , new TColor(++_offset, 52./255., 101./255., 164./255., "TangoSkyBlueB")));
        colors_.insert(std::pair<std::string, TColor *>("TangoChameleonB"
                                                        , new TColor(++_offset, 115./255., 210./255., 22./255., "TangoChameleonB")));
        colors_.insert(std::pair<std::string, TColor *>("TangoScarletRedB"
                                                        , new TColor(++_offset, 204./255., 0./255., 0./255., "TangoScarletRedB")));
        colors_.insert(std::pair<std::string, TColor *>("TangoOrangeB"
                                                        , new TColor(++_offset, 245./255., 121./255., 0./255., "TangoOrangeB")));
        colors_.insert(std::pair<std::string, TColor *>("TangoPlumB"
                                                        , new TColor(++_offset, 117./255., 80./255., 123./255., "TangoPlumB")));
        colors_.insert(std::pair<std::string, TColor *>("TangoButterB"
                                                        , new TColor(++_offset, 237./255., 212./255., 0./255., "TangoButterB")));
        colors_.insert(std::pair<std::string, TColor *>("TangoAluminiumE"
                                                        , new TColor(++_offset, 85./255., 87./255., 83./255., "TangoAluminiumE")));
        colors_.insert(std::pair<std::string, TColor *>("TangoChocolateB"
                                                        , new TColor(++_offset, 193./255., 125./255., 17./255., "TangoChocolateB")));

        colors_.insert(std::pair<std::string, TColor *>("TangoAluminiumC"
                                                        , new TColor(++_offset, 186./255., 189./255., 182./255., "TangoAluminiumC")));
        colors_.insert(std::pair<std::string, TColor *>("TangoAluminiumA"
                                                        , new TColor(++_offset, 238./255., 238./255., 236./255., "TangoAluminiumA")));
        colors_.insert(std::pair<std::string, TColor *>("TangoAluminiumB"
                                                        , new TColor(++_offset, 211./255., 215./255., 207./255., "TangoAluminiumB")));
    }
}
