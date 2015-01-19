# collect input
while true; do
	read -p "Do you wish to remove all generated files [y/n]? " yn
	case $yn in
		[Yy]* ) break;;
		[Nn]* ) exit;;
		* ) echo "Please answer y or n.";;
	esac
done

cd ./test/
make clean
cd ../
make clean

# dirs
rm -rf autom4te.cache/
rm -rf aux-dist/
rm -rf m4/
rm -rf src/.deps
rm -rf src/.libs
rm -f src/*.lo
rm -f src/*.la
rm -rf test/.deps
rm -rf test/.libs
rm -f test/*.lo
rm -f test/*.la


# files
if [ -f libtool ]; then
	rm libtool
fi
if [ -f include/Makefile ]; then
	rm include/Makefile
fi
if [ -f include/Makefile.in ]; then
	rm include/Makefile.in
fi
if [ -f src/Makefile ]; then
	rm src/Makefile
fi
if [ -f src/Makefile.in ]; then
	rm src/Makefile.in
fi
if [ -f test/Makefile ]; then
	rm test/Makefile
fi
if [ -f test/Makefile.in ]; then
	rm test/Makefile.in
fi
if [ -f aclocal.m4 ]; then
	rm aclocal.m4
fi
if [ -f config.status ]; then
	rm config.status
fi
if [ -f config.log ]; then
	rm config.log
fi
if [ -f configure ]; then
	rm configure
fi
if [ -f Makefile ]; then
	rm Makefile
fi
if [ -f Makefile.in ]; then
	rm Makefile.in
fi
echo "Cleaned files!"
