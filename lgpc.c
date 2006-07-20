/*
* lgpc.c
* polygon library for Lua 5.0 based on gpc
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 20 Jul 2006 09:15:01
* This code is hereby placed in the public domain.
*/

#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gpc.h"

#include "lua.h"
#include "lauxlib.h"

#define MYNAME		"gpc"
#define MYVERSION	MYNAME " library for " LUA_VERSION " / Jul 2006 / "\
			"using gpc " GPC_VERSION
#define MYTYPE		MYNAME " handle"

static gpc_polygon *Pget(lua_State *L, int i)
{
 if (luaL_checkudata(L,i,MYTYPE)==NULL) luaL_typerror(L,i,MYTYPE);
 return lua_touserdata(L,i);
}

static gpc_polygon *Pnew(lua_State *L)
{
 gpc_polygon *p=lua_newuserdata(L,sizeof(gpc_polygon));
 luaL_getmetatable(L,MYTYPE);
 lua_setmetatable(L,-2);
 p->num_contours=0;
 p->hole=NULL;
 p->contour=NULL;
 return p;
}

static int Pclip(lua_State *L, int op)
{
 gpc_polygon *p=Pget(L,1);
 gpc_polygon *q=Pget(L,2);
 gpc_polygon *r=Pnew(L);
 gpc_polygon_clip(op,p,q,r);
 return 1;
}

static int Lnew(lua_State *L)			/** new() */
{
 Pnew(L);
 return 1;
}

static int Lclip(lua_State *L)			/** clip(p,q,op) */
{
 static const int Iop[]= { GPC_DIFF, GPC_INT, GPC_XOR, GPC_UNION, };
 static const char *const Sop[] = { "-", "*", "^", "+", NULL};
#ifndef LUA_VERSION_NUM
 int i=luaL_findstring(luaL_optstring(L,3,"*"),Sop);
 luaL_argcheck(L, i!=-1, 3, "invalid mode");
#else
 int i=luaL_checkoption(L,3,"*",Sop);
#endif
 return Pclip(L,Iop[i]);
}

static int Ldiff(lua_State *L)
{
 return Pclip(L,GPC_DIFF);
}

static int Lintersect(lua_State *L)
{
 return Pclip(L,GPC_INT);
}

static int Lxor(lua_State *L)
{
 return Pclip(L,GPC_XOR);
}

static int Lunion(lua_State *L)
{
 return Pclip(L,GPC_UNION);
}

static int Ladd(lua_State *L)			/** add(p,c,[hole]) */
{
 int i,n;
 gpc_vertex_list c;
 gpc_polygon *p=Pget(L,1);
 luaL_checktype(L,2,LUA_TTABLE); 
 n=luaL_getn(L,2)/2;
 c.num_vertices=n;
 c.vertex=malloc(c.num_vertices*sizeof(*c.vertex));
 for (i=0; i<n; i++)
 {
  lua_rawgeti(L,2,2*i+1); c.vertex[i].x=lua_tonumber(L,-1);
  lua_rawgeti(L,2,2*i+2); c.vertex[i].y=lua_tonumber(L,-1);
  lua_pop(L,2);
 }
 gpc_add_contour(p,&c,lua_toboolean(L,3));
 free(c.vertex);
 return 0;
}

static int Lget(lua_State *L)			/** get(p,[c,[i]]) */
{
 int m=lua_gettop(L);
 gpc_polygon *p=Pget(L,1);
 int n=p->num_contours;
 int c,i;
 if (m==1)
 {
  lua_pushnumber(L,n);
  return 1;
 }
 c=luaL_checkint(L,2);
 if (c<1 || c>n) return 0;
 n=p->contour[--c].num_vertices;
 if (m==2)
 {
  lua_pushnumber(L,n);
  lua_pushboolean(L,p->hole[c]);
  return 2;
 }
 i=luaL_checkint(L,3);
 if (i<=n)
 {
  --i;
  lua_pushnumber(L,p->contour[c].vertex[i].x);
  lua_pushnumber(L,p->contour[c].vertex[i].y);
  return 2;
 }
 else
  return 0;
}

static int Pio(lua_State *L, const char *mode)
{
 gpc_polygon *p=Pget(L,1);
 const char *file=lua_tostring(L,2);
 int holes=lua_toboolean(L,3);
 int reading=(*mode=='r');
 FILE *f= (file==NULL) ? (reading ? stdin : stdout) : fopen(file,mode);
 if (f==NULL)
 {
  lua_pushnil(L); 
  lua_pushstring(L,strerror(errno));
  return 2;
 }
 (reading ? gpc_read_polygon : gpc_write_polygon)(f,holes,p);
 if (file!=NULL) fclose(f);
 lua_pushboolean(L,1); 
 return 1;
}

static int Lread(lua_State *L)			/** read(p,file,holes) */
{
 return Pio(L,"r");
}

static int Lwrite(lua_State *L)			/** write(p,file,holes) */
{
 return Pio(L,"w");
}

static int Ltostring(lua_State *L)              /** tostring(p) */
{
 gpc_polygon *p=Pget(L,1);
 char s[64];
 sprintf(s,"%s %p",MYTYPE,(void*)p);
 lua_pushstring(L,s);
 return 1;
}

static int Lgc(lua_State *L)
{
 gpc_polygon *p=Pget(L,1);
 gpc_free_polygon(p);
 return 0;
}

static const luaL_reg R[] =
{
	{ "__add",	Lunion		},
	{ "__gc",	Lgc		},
	{ "__mul",	Lintersect	},
	{ "__pow",	Lxor		},
	{ "__sub",	Ldiff		},
	{ "__tostring",	Ltostring	},
	{ "add",	Ladd		},
	{ "clip",	Lclip		},
	{ "get",	Lget		},
	{ "new",	Lnew		},
	{ "read",	Lread		},
	{ "tostring",	Ltostring	},
	{ "write",	Lwrite		},
	{ NULL,		NULL		}
};

LUALIB_API int luaopen_gpc(lua_State *L)
{
 lua_pushliteral(L,MYNAME);
 luaL_newmetatable(L,MYTYPE);
 luaL_openlib(L,NULL,R,0);
 lua_pushliteral(L,"version");			/** version */
 lua_pushliteral(L,MYVERSION);
 lua_settable(L,-3);
 lua_pushliteral(L,"__index");
 lua_pushvalue(L,-2);
 lua_settable(L,-3);
 lua_rawset(L,LUA_GLOBALSINDEX);
 return 1;
}
