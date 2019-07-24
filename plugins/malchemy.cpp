#include <iostream>
#include <iomanip>
#include <map>
#include <algorithm>
#include <vector>
#include <math.h>

#include "Core.h"
#include "Console.h"
#include "Export.h"
#include "PluginManager.h"
#include "modules/MapCache.h"
#include "modules/Random.h"
#include "modules/World.h"

#include "MiscUtils.h"

#include "DataDefs.h"
#include "df/reaction.h"
#include "df/world.h"

#ifdef LINUX_BUILD
#include <tr1/memory>
using std::tr1::shared_ptr;
#else
#include <memory>
using std::shared_ptr;
#endif

using namespace df::enums;

using namespace DFHack;
using namespace MapExtras;
using namespace DFHack::Random;

DFHACK_PLUGIN("malchemy");
REQUIRE_GLOBAL(world);
REQUIRE_GLOBAL(gametype);

command_result cmd_malchemy(color_ostream &out, std::vector <std::string> & parameters);

DFhackCExport command_result plugin_init ( color_ostream &out, std::vector <PluginCommand> &commands)
{
    commands.push_back(PluginCommand(
        "malchemy", "Adds seed-random alchemy recipes to a new alchemist's workstation.",
        cmd_malchemy, false,
        "  Run this after embark to create random alchemical recipes at\n"
        "  the new alchemist's workstation.\n"
    ));
    return CR_OK;
}

DFhackCExport command_result plugin_shutdown ( color_ostream &out )
{
    return CR_OK;
}

namespace malchemy {

    MersenneRNG rng;

    void init_seeds()
    {
        static bool bIsInitialized = false;

        if (!bIsInitialized)
        {
            std::string seed = world->worldgen.worldgen_parms.seed;
            seed.resize((seed.size() + 3)&~3);
            rng.init((uint32_t*)seed.data(), seed.size() / 4, 10);
            bIsInitialized = true;
        }
    }

    void get_rng(color_ostream &con)
    {
        con.print("Random number is: %u\n", rng.random());
    }

    void test_malchemy(color_ostream &out)
    {
        for (auto it = world->raws.reactions.reactions.begin(); it != world->raws.reactions.reactions.end(); ++it)
        {
            df::reaction* reaction = (*it);
            out.print("Reaction: %s\n", reaction->name);
        }

        FOR_ENUM_ITEMS(item_type, it)
        {
            out.print("Item: %s\n", ENUM_KEY_STR(item_type, it));
        }
    }

}

command_result cmd_malchemy(color_ostream &con, std::vector<std::string> & parameters)
{
    bool verbose = false;

    for (size_t i = 0; i < parameters.size(); i++)
    {
        if (parameters[i] == "verbose")
            verbose = true;
        else
            return CR_WRONG_USAGE;
    }

    CoreSuspender suspend;

    if (!Maps::IsValid())
    {
        con.printerr("Map is not available!\n");
        return CR_FAILURE;
    }

    if (!World::isFortressMode())
    {
        con.printerr("Must be used in fortress mode!\n");
        return CR_FAILURE;
    }

    malchemy::init_seeds();
    malchemy::get_rng(con);
    malchemy::test_malchemy(con);

    return CR_OK;
}
