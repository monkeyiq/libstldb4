echo "This is the monkeyiq config invoker. please don't use it."
#export CFLAGS=" -fPIC "
export CXXFLAGS=" -fPIC -fdiagnostics-all-candidates"
/Develop/libstldb4/configure \
--enable-wrapdebug --with-uniquename=stldb4 --enable-hiddensymbols \
--disable-stlport --with-sigcxx-2x=yes \
--prefix=/usr/local --libdir=/usr/local/lib64

# --enable-rpc
#--prefix=/usr --libdir=/usr/lib 
