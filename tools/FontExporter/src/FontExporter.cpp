#include <freetype2/freetype/freetype.h>
#include <freetype2/ft2build.h>
#include <stdlib.h>

#include FT_FREETYPE_H
#include <cstdio>

int main(int arc, char** argv)
{
    printf("Initializing Freetype library");

    // We start by initializing FT
    auto e = new FT_Library();

    if(FT_Init_FreeType((FT_Library*)e))
    {
        printf("Error : We could not initialize the freetype library");
        return 0;
    }
}
