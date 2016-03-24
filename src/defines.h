#ifndef DEFINES
#define DEFINES

#if defined(LIBFACE_STATIC)
#define LIBFACE_DLL_EXPORTED
#elif defined(_MSC_VER)
#if defined(BUILDING_LIBFACE)
#define LIBFACE_DLL_EXPORTED __declspec(dllexport)
#else
#define LIBFACE_DLL_EXPORTED __declspec(dllimport)
#endif
#elif defined(BUILDING_LIBFACE) && 1
#  define LIBFACE_DLL_EXPORTED __attribute__((__visibility__("default")))
#else
#  define LIBFACE_DLL_EXPORTED
#endif

#endif // DEFINES
