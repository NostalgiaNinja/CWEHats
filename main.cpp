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

extern "C"
{

    //Chao Registration functionality.
    int (*RegisterChaoHat)(NJS_OBJECT *model, NJS_TEXLIST *texlist, BlackMarketItemAttributes *attrib, const char *name, const char *description, bool hideHead);
    void (*RegisterChaoTexlistLoad)(const char *name, NJS_TEXLIST *load);
    void (*RegisterDataFunc)(void *ptr);

    //Load Textures - Add textures to the RegisterChaoTexlistLoad "filename".PAK file, and put in gd_PC/PRS
    NJS_TEXNAME texname_HATS[9];
    NJS_TEXLIST texlist_HATS = {arrayptrandlength(texname_HATS)};

    std::string pathStr;

    //Easy Register system - Uses sa2mdl exports from Blender
    void EasyRegisterHat(const char *path, int rings, int sellPrice, const char *name, const char *desc)
    {
        BlackMarketItemAttributes attrib = {rings, sellPrice, 0, -1, -1, 0};
        std::string finalPath = pathStr + "\\" + path;
        ModelInfo *file = new ModelInfo(finalPath);
        RegisterChaoHat(file->getmodel(), &texlist_HATS, &attrib, name, desc, false);
    }

    void CWELoad()
    {

        //ChaoTexlistLoad - "Filename" - Do NOT conflict with other hat or fruit mods.
        RegisterChaoTexlistLoad("CustomHats", &texlist_HATS);

        //EasyRegisterHat - "Filename", Price, Sale, "Name", "Description", Hide Head (Defaults to false if omitted) - Place hat models in the root folder of the mod folder.
        //Note to whoever copies this code - this is sample code and needs to be changed as appropriate, look at the function calls for what needs to be added.
        EasyRegisterHat("filename.sa2mdl", 100, 100, "Name", "Description");
    }

    __declspec(dllexport) void Init(const char *path, HelperFunctions &func)
    {
        //IMPORTANT - REGISTER THIS OR EASY REGISTERS WON'T WORK!
        pathStr = std::string(path);

        //load CWE Module
        HMODULE h = GetModuleHandle(L"CWE");
        RegisterChaoHat = (int (*)(NJS_OBJECT * model, NJS_TEXLIST * texlist, BlackMarketItemAttributes * attrib, const char *name, const char *description, bool hideHead)) GetProcAddress(h, "RegisterChaoHat");
        RegisterDataFunc = (void (*)(void *ptr))GetProcAddress(h, "RegisterDataFunc");
        RegisterChaoTexlistLoad = (void (*)(const char *name, NJS_TEXLIST *load))GetProcAddress(h, "RegisterChaoTexlistLoad");

        RegisterDataFunc(CWELoad);
    }
    __declspec(dllexport) ModInfo SA2ModInfo = {ModLoaderVer};
}