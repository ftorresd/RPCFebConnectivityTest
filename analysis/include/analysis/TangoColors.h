#ifndef analysis_TangoColors_h
#define analysis_TangoColors_h

#include <map>
#include <string>
#include "TColor.h"

class TangoColors
{
public:
    TangoColors(int _offset = 20000);
    ~TangoColors();

    static void createColors(int _offset = 20000, bool _force = false);
    static std::map<std::string, TColor *> const & getColors();

    static TColor const & getTangoSkyBlueA();
    static TColor const & getTangoChameleonA();
    static TColor const & getTangoScarletRedA();
    static TColor const & getTangoPlumA();
    static TColor const & getTangoChocolateA();
    static TColor const & getTangoOrangeA();
    static TColor const & getTangoButterA();
    static TColor const & getTangoAluminiumD();

    static TColor const & getTangoSkyBlueC();
    static TColor const & getTangoChameleonC();
    static TColor const & getTangoScarletRedC();
    static TColor const & getTangoPlumC();
    static TColor const & getTangoChocolateC();
    static TColor const & getTangoOrangeC();
    static TColor const & getTangoButterC();
    static TColor const & getTangoAluminiumF();

    static TColor const & getTangoSkyBlueB();
    static TColor const & getTangoChameleonB();
    static TColor const & getTangoScarletRedB();
    static TColor const & getTangoPlumB();
    static TColor const & getTangoChocolateB();
    static TColor const & getTangoOrangeB();
    static TColor const & getTangoButterB();
    static TColor const & getTangoAluminiumE();

    static TColor const & getTangoAluminiumC();
    static TColor const & getTangoAluminiumA();
    static TColor const & getTangoAluminiumB();

protected:
    static std::map<std::string, TColor *> colors_;
};

#include "analysis/TangoColors-inl.h"

#endif // analysis_TangoColors_h
