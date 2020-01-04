git add pidGrowbox.atsln
git add pidGrowbox/*.h
git add pidGrowbox/*.c
git add gitAddFiles.bat
git add checkOriginMaster.bat
git add pidGrowbox/pidGrowbox.cproj
git add tools/*
git add pidGrowbox/pidGrowbox.mdl
echo " "
echo "STATUS:"
git status
echo " "
echo "git diff --name-status master origin/master :"
git diff --name-status master origin/master 