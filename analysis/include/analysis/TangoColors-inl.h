#ifndef analysis_TangoColors_inl_h
#define analysis_TangoColors_inl_h

#include "analysis/TangoColors.h"

inline std::map<std::string, TColor *> const & TangoColors::getColors()
{
    if (colors_.empty())
        createColors();
    return colors_;
}

inline TColor const & TangoColors::getTangoSkyBlueA()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoSkyBlueA"));
}

inline TColor const & TangoColors::getTangoChameleonA()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoChameleonA"));
}

inline TColor const & TangoColors::getTangoScarletRedA()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoScarletRedA"));
}

inline TColor const & TangoColors::getTangoPlumA()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoPlumA"));
}

inline TColor const & TangoColors::getTangoChocolateA()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoChocolateA"));
}

inline TColor const & TangoColors::getTangoOrangeA()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoOrangeA"));
}

inline TColor const & TangoColors::getTangoButterA()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoButterA"));
}

inline TColor const & TangoColors::getTangoAluminiumD()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoAluminiumD"));
}


inline TColor const & TangoColors::getTangoSkyBlueC()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoSkyBlueC"));
}

inline TColor const & TangoColors::getTangoChameleonC()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoChameleonC"));
}

inline TColor const & TangoColors::getTangoScarletRedC()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoScarletRedC"));
}

inline TColor const & TangoColors::getTangoPlumC()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoPlumC"));
}

inline TColor const & TangoColors::getTangoChocolateC()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoChocolateC"));
}

inline TColor const & TangoColors::getTangoOrangeC()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoOrangeC"));
}

inline TColor const & TangoColors::getTangoButterC()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoButterC"));
}

inline TColor const & TangoColors::getTangoAluminiumF()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoAluminiumF"));
}


inline TColor const & TangoColors::getTangoSkyBlueB()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoSkyBlueB"));
}

inline TColor const & TangoColors::getTangoChameleonB()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoChameleonB"));
}

inline TColor const & TangoColors::getTangoScarletRedB()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoScarletRedB"));
}

inline TColor const & TangoColors::getTangoPlumB()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoPlumB"));
}

inline TColor const & TangoColors::getTangoChocolateB()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoChocolateB"));
}

inline TColor const & TangoColors::getTangoOrangeB()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoOrangeB"));
}

inline TColor const & TangoColors::getTangoButterB()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoButterB"));
}

inline TColor const & TangoColors::getTangoAluminiumE()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoAluminiumE"));
}


inline TColor const & TangoColors::getTangoAluminiumC()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoAluminiumC"));
}

inline TColor const & TangoColors::getTangoAluminiumA()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoAluminiumA"));
}

inline TColor const & TangoColors::getTangoAluminiumB()
{
    if (colors_.empty())
        createColors();
    return *(colors_.at("TangoAluminiumB"));
}

#endif // analysis_TangoColors_inl_h
