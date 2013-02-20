-- test gpc library

local P=require"gpc"

print(P.version)

function output(f,...)
 for i=1,arg.n do
  f:write(arg[i]," ")
 end
 f:write("\n")
end

function plottri(f,p,r,g,b,w)
 if w=="stroke" then output(f,0,"setlinewidth") end
 output(f,r,g,b,"setrgbcolor")
 for c=1,p:get() do
  local n=p:get(c)
  local x1,y1=p:get(c,1)
  local x2,y2=p:get(c,2)
  for i=3,n do
   local x,y=p:get(c,i)
   output(f,x1,y1,"moveto")
   output(f,x2,y2,"lineto")
   output(f,x,y,"lineto")
   output(f,"closepath")
   x1,y1,x2,y2=x2,y2,x,y
  end
 end
 output(f,w)
end

function plot(f,p,r,g,b,w)
 output(f,r,g,b,"setrgbcolor")
 for c=1,p:get() do
  local n,h=p:get(c)
  local s="moveto"
  for i=1,n do
   local x,y=p:get(c,i)
   output(f,x,y,s)
   s="lineto"
  end
  output(f,"closepath")
 end
 output(f,w)
end

function show(n,f,p,r,g,b,w)
 output(f)
 output(f,"%%Page:",w,n)
 output(f,"setup")
 plot(f,p,r,g,b,"eofill")
 plot(f,p,0,0,0,"stroke")
 output(f,"showpage")
end

N=0
function page(f,p,w,r,g,b)
 N=N+1
 print(N,p:get(),w)
 show(N,f,p,r,g,b,w)
end

function bbox(p,xmin,xmax,ymin,ymax)
 local huge=1e30
 xmin=xmin or  huge
 xmax=xmax or -huge
 ymin=ymin or  huge
 ymax=ymax or -huge
 for c=1,p:get() do
  for i=1,p:get(c) do
   local x,y=p:get(c,i)
   if x<xmin then xmin=x elseif x>xmax then xmax=x end
   if y<ymin then ymin=y elseif y>ymax then ymax=y end
  end
 end
 return xmin,xmax,ymin,ymax
end

function test(file)
 local X0,Y0,DX,DY=50,50,500,500
 local xmin,xmax,ymin,ymax=bbox(b,bbox(a))
 print("bbox",xmin,xmax,ymin,ymax)
 local dx=xmax-xmin
 local dy=ymax-ymin
 local d=(dx-dy)/2
 if (d>0) then ymin=ymin-d ymax=ymax+d d=dx else xmin=xmin-d xmax=xmax+d d=dy end
 local f=assert(io.open(file,"w"))
 output(f,"%!PS-Adobe-2.0")
 output(f,"%%Title: gpc lua test")
 output(f,"%%%BoundingBox:",X0-5,Y0-5,X0+DX+5,Y0+DY+5) -- inactive
 output(f,"%%Creator:",P.version)
 output(f,"%%Pages:",9)
 output(f,"%%EndComments")
 output(f,"/setup {",X0,Y0,"translate",DX,DY,"scale")
 output(f,"1",d,"div dup scale",-xmin,-ymin,"translate")
 output(f,d/200,"setlinewidth} def")
 page(f,a,"a",1,0.8,0.8)
 page(f,b,"b",0.8,0.8,1)
 page(f,a+b,"union",0.8,1,0.8)
 page(f,a*b,"intersection",0.8,1,0.8)
 page(f,a-b,"diff",1,0.8,0.8)
 page(f,b-a,"diff",0.8,0.8,1)
 page(f,a^b,"xor",0.8,1,0.8)
 page(f,a,"a",1,0.8,0.8)
 plot=plottri
 page(f,a:strip(),"strip",1,0.8,0.8)
 output(f,"%%EOF")
 f:close()
end

------------------------------------------------------------------------------

a=P.new():add{
1315 , 1282 ,
1315 , 1329 ,
1300 , 1314 ,
1284 , 1345 ,
1253 , 1312 ,
1284 , 1298 ,
1268 , 1282 ,
}

b=P.new():add{
1290 , 1270 ,
1335 , 1315 ,
1250 , 1340 ,
}

test"test.ps"

print(P.version)
