# This is needed by the linker in order to find libopenal.so, if
# its location is not in the standard linker path.
# (Set the TMP variable to the correct path).

TMP=/usr/local/lib
case :$LD_LIBRARY_PATH: in
 *$TMP*) ;; # already in
 *) export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$TMP:;;
esac
