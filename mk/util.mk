# http://stackoverflow.com/questions/664601/in-gnu-make-how-do-i-convert-a-variable-to-lower-case
#
#  python code for generating these statements:
#
#  LTU = [ chr(i) for i in range ( 0x61, 0x7b ) ]
#  UTL = [ c.upper() for c in LTU ]
#
#  f = lambda v: '$(1)' if not v else '$(subst {},{},{})'.format (v[0],v[0].swapcase(),f(v[1:]))
#
## >>> f(UTL) => lc
## >>> f(LTU) => uc

lc = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$(1)))))))))))))))))))))))))))
uc = $(subst a,A,$(subst b,B,$(subst c,C,$(subst d,D,$(subst e,E,$(subst f,F,$(subst g,G,$(subst h,H,$(subst i,I,$(subst j,J,$(subst k,K,$(subst l,L,$(subst m,M,$(subst n,N,$(subst o,O,$(subst p,P,$(subst q,Q,$(subst r,R,$(subst s,S,$(subst t,T,$(subst u,U,$(subst v,V,$(subst w,W,$(subst x,X,$(subst y,Y,$(subst z,Z,$(1)))))))))))))))))))))))))))

f_convert_name = $(subst -,_,$(subst /,_,$(call uc,$(1))))
# compat name
_f_convert_name = $(f_convert_name)
