/*
 *  Aethyra
 *  Copyright (C) 2004  The Mana World Development Team
 *  Copyright (C) 2009  Aethyra Development Team
 *
 *  This file is part of Aethyra based on original code
 *  from The Mana World.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <physfs.h>
#include <SDL_image.h>

#ifdef WIN32
#include <SDL_syswm.h>
#else
#include <cerrno>
#include <sys/stat.h>
#endif

#ifdef __APPLE__
#include <CoreFoundation/CFBundle.h>
#endif

#include <libxml/parser.h>

#include <SDL/SDL_ttf.h>

#include "engine.h"
#include "main.h"
#include "options.h"

#include "bindings/guichan/graphics.h"
#include "bindings/guichan/gui.h"
#include "bindings/guichan/inputmanager.h"
#include "bindings/guichan/palette.h"

#include "bindings/guichan/dialogs/helpdialog.h"
#include "bindings/guichan/dialogs/setupdialog.h"

#ifdef USE_OPENGL
#include "bindings/guichan/opengl/openglgraphics.h"
#endif

#include "bindings/guichan/sdl/sdlgraphics.h"
#include "bindings/guichan/sdl/sdlinput.h"

#include "bindings/sdl/keyboardconfig.h"
#include "bindings/sdl/sound.h"

#include "core/configuration.h"
#include "core/log.h"
#include "core/resourcemanager.h"

#include "core/image/image.h"

#include "core/utils/gettext.h"
#include "core/utils/stringutils.h"

#include "eathena/gui/debugwindow.h"

#include "eathena/net/logindata.h"
#include "eathena/net/network.h"

Graphics *graphics;

DebugWindow *debugWindow;
HelpDialog *helpDialog;
Setup* setupWindow;

Configuration config;         /**< XML file configuration reader */
Logger *logger;               /**< Log object */

Palette *guiPalette;
KeyboardConfig keyboard;
InputManager *inputManager;
Sound sound;

extern "C" char const *_nl_locale_name_default(void);

Engine::Engine(const char *prog, const Options &gameOptions) : 
    options(gameOptions)
{
    // Initialize PhysicsFS
    PHYSFS_init(prog);

    logger = new Logger();

    initInternationalization();

    // Initialize libxml2 and check for potential ABI mismatches between
    // compiled version and the shared library actually used.
    xmlInitParser();
    LIBXML_TEST_VERSION;

    // Redirect libxml errors to /dev/null
    nullFile = fopen("/dev/null", "w");
    xmlSetGenericErrorFunc(nullFile, NULL);
#if defined __APPLE__
    // Use Application Directory instead of .aethyra
    homeDir = std::string(PHYSFS_getUserDir()) +
        "/Library/Application Support/Aethyra";
#else
    homeDir = std::string(PHYSFS_getUserDir()) + "/.aethyra";
#endif

    // Checking if home folder exists.
#if defined WIN32
    if (!CreateDirectory(homeDir.c_str(), 0) &&
            GetLastError() != ERROR_ALREADY_EXISTS)
#else
    if ((mkdir(homeDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) &&
            (errno != EEXIST))
#endif
    {
        std::cout << strprintf(_("%s can't be created, but it doesn't exist! "
                                 "Exiting."), homeDir.c_str()) << std::endl;
        exit(1);
    }

    // Set log file
    logger->setLogFile(homeDir + std::string("/aethyra.log"));

#ifdef PACKAGE_VERSION
    logger->log("Starting Aethyra Version %s.", PACKAGE_VERSION);
#else
    logger->log("Starting Aethyra - Version not defined.");
#endif

    // Initialize SDL
    logger->log("Initializing SDL...");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    {
        std::cerr << _("Could not initialize SDL: ") << SDL_GetError()
                  << std::endl;
        exit(1);
    }
    atexit(SDL_Quit);

    SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    ResourceManager *resman = ResourceManager::getInstance();

    if (!resman->setWriteDir(homeDir))
    {
        std::cout << strprintf(_("%s couldn't be set as home directory! "
                                 "Exiting."), homeDir.c_str()) << std::endl;
        exit(1);
    }

    // Add the user's homedir to PhysicsFS search path
    resman->addToSearchPath(homeDir, false);

    // Add the main data directories to our PhysicsFS search path
    if (!options.dataPath.empty())
        resman->addToSearchPath(options.dataPath, true);

    resman->addToSearchPath("data", true);
#if defined __APPLE__
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path,
                                          PATH_MAX))
    {
        fprintf(stderr, _("Can't find Resources directory!\n"));
    }
    CFRelease(resourcesURL);
    strncat(path, "/data", PATH_MAX - 1);
    resman->addToSearchPath(path, true);
#else
    resman->addToSearchPath(PKG_DATADIR "data", true);
#endif

    initConfig();
    initWindow();

    // Initialize sound engine
    try
    {
        if (config.getValue("sound", 0) == 1)
            sound.init();

        sound.setSfxVolume((int) config.getValue("sfxVolume", defaultSfxVolume));
        sound.setMusicVolume((int) config.getValue("musicVolume",
                                                   defaultMusicVolume));
    }
    catch (const char *err)
    {
        state = ERROR_STATE;
        errorMessage = err;
        logger->log("Warning: %s", err);
    }

    // Initialize keyboard
    keyboard.init();

    // Pallete colors are needed throughout the whole life of the program
    guiPalette = new Palette();

    setupWindow = new Setup();
    debugWindow = new DebugWindow();
    helpDialog = new HelpDialog();

    inputManager = new InputManager();

    SDLNet_Init();
    network = new Network();

    sound.playMusic("Magick - Real.ogg");
}

Engine::~Engine()
{
    // Before config.write() so that global windows can get their settings
    // written to the configuration file.
    delete debugWindow;
    delete helpDialog;
    delete setupWindow;

    setupWindow = NULL;

    config.write();

    delete gui;
    delete graphics;

    delete guiPalette;
    delete inputManager;

    // Shutdown libxml
    xmlCleanupParser();

    // Shutdown sound
    sound.close();

    ResourceManager::deleteInstance();
    delete logger;

    SDL_FreeSurface(icon);
    PHYSFS_deinit();

    delete network;
    SDLNet_Quit();

    if (nullFile)
        fclose(nullFile);
}

void Engine::initInternationalization()
{
#if ENABLE_NLS
#ifdef WIN32
    putenv(("LANG=" + std::string(_nl_locale_name_default())).c_str());
    // mingw doesn't like LOCALEDIR to be defined for some reason
    bindtextdomain("aethyra", "translations/");
#else
    bindtextdomain("aethyra", LOCALEDIR);
#endif
    setlocale(LC_MESSAGES, "");
    bind_textdomain_codeset("aethyra", "UTF-8");
    textdomain("aethyra");
#endif
}

void Engine::initConfig()
{
    // Fill configuration with defaults
    logger->log("Initializing configuration...");
    config.setValue("host", "www.aethyra.org");
    config.setValue("port", 21001);
    config.setValue("hwaccel", 0);
#if defined USE_OPENGL
    config.setValue("opengl", 1);
#else
    config.setValue("opengl", 0);
#endif
    config.setValue("screen", 0);
    config.setValue("sound", 1);
    config.setValue("guialpha", 0.8f);
    config.setValue("remember", 1);
    config.setValue("sfxVolume", 100);
    config.setValue("musicVolume", 60);
    config.setValue("fpslimit", 0);
    config.setValue("updatehost", "http://www.aethyra.org/updates");
    config.setValue("customcursor", 1);
    config.setValue("ChatLogLength", 128);

    // Checking if the configuration file exists... otherwise creates it with
    // default options !
    FILE *configFile = 0;
    std::string configPath = options.configPath;

    if (configPath.empty())
        configPath = homeDir + "/config.xml";

    configFile = fopen(configPath.c_str(), "r");

    // If we can't read it, it doesn't exist !
    if (configFile == NULL)
    {
        // We reopen the file in write mode and we create it
        configFile = fopen(configPath.c_str(), "wt");
    }

    if (configFile == NULL)
    {
        std::cout << strprintf("Can't create %s. Using Defaults.",
                               configPath.c_str()) << std::endl;
    }
    else
    {
        fclose(configFile);
        config.init(configPath);
    }
}

void Engine::initWindow()
{
    logger->log("Creating new SDL window...");
    SDL_WM_SetCaption("Aethyra", NULL);
#ifdef WIN32
    static SDL_SysWMinfo pInfo;
    SDL_GetWMInfo(&pInfo);
    HICON icon = LoadIcon(GetModuleHandle(NULL), "A");
    if (icon)
    {
        SetClassLong(pInfo.window, GCL_HICON, (LONG) icon);
    }
#else
    icon = IMG_Load(PKG_DATADIR "data/icons/aethyra.png");
    if (icon)
    {
        SDL_SetAlpha(icon, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);
        SDL_WM_SetIcon(icon, NULL);
    }
#endif

#ifdef USE_OPENGL
    bool useOpenGL = !options.noOpenGL && (config.getValue("opengl", 0) == 1);

    // Setup image loading for the right image format
    Image::setLoadAsOpenGL(useOpenGL);

    // Create the graphics context
    if (useOpenGL)
        graphics = new OpenGLGraphics();
    else
        graphics = new SDLGraphics();
#else
    // Create the graphics context
    graphics = new SDLGraphics();
#endif

    const int width = (int) config.getValue("screenwidth", defaultScreenWidth);
    const int height = (int) config.getValue("screenheight", defaultScreenHeight);
    const int bpp = 0;
    const bool fullscreen = ((int) config.getValue("screen", 0) == 1);
    const bool hwaccel = ((int) config.getValue("hwaccel", 0) == 1);

    // Try to set the desired video mode
    if (!graphics->setVideoMode(width, height, bpp, fullscreen, hwaccel))
    {
        std::cerr << _("Couldn't set ") << width << "x" << height << "x" 
                  << bpp << _(" video mode: ") << SDL_GetError() << std::endl;
        exit(1);
    }

    // Initialize for drawing
    graphics->_beginDraw();

    gui = new Gui(graphics);
}

void Engine::setUpdatesDir(std::string &updateHost, const LoginData &loginData)
{
    std::stringstream updates;

    // If updatesHost is currently empty, fill it from config file
    if (updateHost.empty())
    {
        updateHost =
            config.getValue("updatehost", "http://www.aethyra.org/updates");
    }

    // Remove any trailing slash at the end of the update host
    if (updateHost.at(updateHost.size() - 1) == '/')
        updateHost.resize(updateHost.size() - 1);

    // Parse out any "http://" or "ftp://", and set the updates directory
    size_t pos;
    pos = updateHost.find("://");
    if (pos != updateHost.npos)
    {
        if (pos + 3 < updateHost.length())
        {
            updates << "updates/" << updateHost.substr(pos + 3)
                    << "/" << loginData.port;
            updatesDir = updates.str();
        }
        else
        {
            logger->log("Error: Invalid update host: %s", updateHost.c_str());
            errorMessage = _("Invalid update host: ") + updateHost;
            state = ERROR_STATE;
        }
    }
    else
    {
        logger->log("Warning: no protocol was specified for the update host");
        updates << "updates/" << updateHost << "/" << loginData.port;
        updatesDir = updates.str();
    }

    ResourceManager *resman = ResourceManager::getInstance();

    // Verify that the updates directory exists. Create if necessary.
    if (!resman->isDirectory("/" + updatesDir))
    {
        if (!resman->mkdir("/" + updatesDir))
        {
#if defined WIN32
            std::string newDir = homeDir + "\\" + updatesDir;
            std::string::size_type loc = newDir.find("/", 0);

            while (loc != std::string::npos)
            {
                newDir.replace(loc, 1, "\\");
                loc = newDir.find("/", loc);
            }

            if (!CreateDirectory(newDir.c_str(), 0) &&
                GetLastError() != ERROR_ALREADY_EXISTS)
            {
                logger->log("Error: %s can't be made, but doesn't exist!",
                            newDir.c_str());
                errorMessage = _("Error creating updates directory!");
                state = ERROR_STATE;
            }
#else
            logger->log("Error: %s/%s can't be made, but doesn't exist!",
                        homeDir.c_str(), updatesDir.c_str());
            errorMessage = _("Error creating updates directory!");
            state = ERROR_STATE;
#endif
        }
    }
}
