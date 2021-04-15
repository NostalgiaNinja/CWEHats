#include "pch.h"
#include "SA2ModLoader.h"
#include <string>
#include "ModelInfo.h"

//Non SA2Modloader structs
struct BlackMarketItemAttributes
{
    int PurchasePrice;
    int SalePrice;
    __int16 RequiredEmblems;
    __int16 Name;
    __int16 Descriptions;
    __int16 Unknown;
};

//EAccessory Type defaults 0 to head, other to other parts of the body.
enum EAccessoryType : unsigned char
{
    Head = 0,
    Face,
    Chest,
    Feet,
    AccessoryTypeCount
};

extern "C"
{

    //function prototypes for Chao Registration functions in init()
    int (*RegisterChaoHat)(NJS_OBJECT *model, NJS_TEXLIST *texlist, BlackMarketItemAttributes *attrib, const char *name, const char *description, bool hideHead);
    int (*RegisterChaoAccessory)(EAccessoryType type, NJS_OBJECT* model, NJS_TEXLIST* texlist, BlackMarketItemAttributes* attrib, const char* name, const char* description);
    void (*RegisterChaoTexlistLoad)(const char *name, NJS_TEXLIST *load);
    void (*RegisterDataFunc)(void *ptr);

    //Load Textures - Add textures to the RegisterChaoTexlistLoad "filename".PAK file, and put in gd_PC/PRS
    //Increase the array size when close to reaching the amount of textures specified, or your game will crash!
    NJS_TEXNAME texname_HATS[9];
    NJS_TEXLIST texlist_HATS = {arrayptrandlength(texname_HATS)};

    std::string pathStr;

    //Easy Register for hats - Docu v1.2 adds bool hideHead and defaults to false
    void EasyRegisterHat(const char *path, int rings, int sellPrice, const char *name, const char *desc, bool hideHead = false)
    {
        BlackMarketItemAttributes attrib = {rings, sellPrice, 0, -1, -1, 0};
        std::string finalPath = pathStr + "\\" + path;
        ModelInfo *file = new ModelInfo(finalPath);
        RegisterChaoHat(file->getmodel(), &texlist_HATS, &attrib, name, desc, false);
    }

    //Easy Register for Accessories - Docu v1.2 adds new EasyRegister for people to play with.
    void EasyRegisterAccessory(const char* path, int rings, int sellprice, const char* name, const char* desc, EAccessoryType type)
    {
        BlackMarketItemAttributes attrib = { rings, sellprice, 0, -1, -1, 0 };
        std::string finalPath = pathStr + "\\" + path;
        ModelInfo* file = new ModelInfo(finalPath);
        RegisterChaoAccessory(type, file->getmodel(), &texlist_HATS, &attrib, name, desc);
    }

    void CWELoad()
    {

        //ChaoTexlistLoad - "Filename" - Do NOT conflict with other hat or fruit mods.
        RegisterChaoTexlistLoad("ExampleHats", &texlist_HATS);

        //EasyRegisterHat - "Filename", Price, Sale, "Name", "Description", Hide Head (Defaults to false if omitted) - Place hat models in the root folder of the mod folder.
        //Note to whoever copies this code - this is sample code and needs to be changed as appropriate, look at the function calls for what needs to be added.
        EasyRegisterHat("filename.sa2mdl", 100, 100, "Name", "Description", false);

        //EasyRegisterAccessory - EAccessoryType (see enum above), "Filename", buy, sell, "Name", "Description" - Adding example for documentation sake
        EasyRegisterAccessory("filename.sa2mdl",100,100,"Name","Description",Head);

        
    }

    __declspec(dllexport) void Init(const char *path, HelperFunctions &func)
    {
        //pathStr defined as per the easy register functions - Do not remove or game will crash!
        pathStr = std::string(path);

        //load CWE Module
        HMODULE h = GetModuleHandle(L"CWE");
        RegisterChaoHat = (int (*)(NJS_OBJECT * model, NJS_TEXLIST * texlist, BlackMarketItemAttributes * attrib, const char *name, const char *description, bool hideHead)) GetProcAddress(h, "RegisterChaoHat");
        RegisterChaoAccessory = (int (*)(EAccessoryType type, NJS_OBJECT * model, NJS_TEXLIST * texlist, BlackMarketItemAttributes * attrib, const char* name, const char* description)) GetProcAddress(h, "RegisterChaoAccessory");
        RegisterDataFunc = (void (*)(void *ptr))GetProcAddress(h, "RegisterDataFunc");
        RegisterChaoTexlistLoad = (void (*)(const char *name, NJS_TEXLIST *load))GetProcAddress(h, "RegisterChaoTexlistLoad");

        RegisterDataFunc(CWELoad);
    }
    __declspec(dllexport) ModInfo SA2ModInfo = {ModLoaderVer};
}