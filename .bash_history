ls
cd nn_noxim/
ls
git init
git add .
git status
git commit -m "add systemc2.2.0 and first run"
git remote add origin https://github.com/AbbaszadehMohammad/NeuNoxim.git
git push -u origin master
cd ..
ls
cd noxim-3d/
ls
cd bin/
ls
make clean
make
ls
cd ..
ls
cd systemc-2.2.0
ls
ls
mkdir objdir
ls
cd objdir/
ls
export CXX=g++
../configure
make
make install
cd ..
rm -rf objdir/
ls
cd ..
ls
cd bin/
ls
make clean
make
make clean
make
make clean
make
make clean
make
cls
exit
