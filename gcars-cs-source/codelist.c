#define FUZZIQER_TYPES
#include "types.h"
#include "gcars.h"

void AddGame(GCARS_DATABLOCK* db,GCARS_GAME* game)
{
    u32 x;
    GCARS_GAME** t = datablock->games;
    t = (GCARS_GAME**)malloc(sizeof(GCARS_GAME*) * (db->numgames + 1));
    for (x = 0; x < db->numgames; x++) t[x] = db->games[x];
    t[db->numgames] = game;
    free(db->games);
    db->games = t;
    db->numgames++;
}

void RemoveGame(GCARS_DATABLOCK* db,GCARS_GAME* game)
{
    u32 x;
    for (x = 0; x < db->numgames; x++)
    {
        if (db->games[x] == game) RemoveGameID(db,x);
    }
}

void RemoveGameID(GCARS_DATABLOCK* db,u32 id)
{
    u32 x;
    GCARS_GAME** t = (GCARS_GAME**)malloc(sizeof(GCARS_GAME*) * (datablock->numgames));
    GCARS_GAME** s = db->games;
    db->numgames--;
    for (x = 0; x < id; x++) t[x] = s[x];
    for (x = id; x < datablock->numgames; x++) t[x] = s[x + 1];
    free(datablock->games);
    datablock->games = t;
}

void AddCode(GCARS_GAME* game,GCARS_CODE* code)
{
    u32 x;
    GCARS_CODE** t = game->codes;
    t = (GCARS_CODE**)malloc(sizeof(GCARS_CODE*) * (game->numcodes + 1));
    for (x = 0; x < game->numcodes; x++) t[x] = game->codes[x];
    t[game->numcodes] = code;
    free(game->codes);
    game->codes = t;
    game->numcodes++;
}

void RemoveCode(GCARS_GAME* game,GCARS_CODE* code)
{
    u32 x;
    for (x = 0; x < game->numcodes; x++)
    {
        if (game->codes[x] == code) RemoveCodeID(game,x);
    }
}

void RemoveCodeID(GCARS_GAME* game,u32 id)
{
    u32 x;
    GCARS_CODE** t = (GCARS_CODE**)malloc(sizeof(GCARS_CODE*) * (game->numcodes));
    GCARS_CODE** s = game->codes;
    game->numcodes--;
    for (x = 0; x < id; x++) t[x] = s[x];
    for (x = id; x < game->numcodes; x++) t[x] = s[x + 1];
    free(game->codes);
    game->codes = t;
}

