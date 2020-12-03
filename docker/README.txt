Build and push Titan Docker images to DockerHub

//----------------------------------------------titan-alpine image


docker build  --force-rm -f Dockerfile.alpine    -t eclipsetitan/titan-alpine:7.2.0  .

docker run --rm -it -v /home/<user>:/home/titan/data eclipsetitan/titan-alpine:7.2.0


~/titan.core $ printenv
HOSTNAME=de2bb469790c
SHLVL=3
LD_LIBRARY_PATH=/home/titan/titan.core/Install/lib:
HOME=/home/titan
_=/bin/sh
TERM=xterm
PATH=/home/titan/titan.core/Install/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
PWD=/home/titan/titan.core
TTCN3_DIR=/home/titan/titan.core/Install
~/titan.core $ gcc -v
Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/usr/libexec/gcc/x86_64-alpine-linux-musl/10.2.1/lto-wrapper
Target: x86_64-alpine-linux-musl
Configured with: /home/buildozer/aports/main/gcc/src/gcc-10.2.1/configure --prefix=/usr --mandir=/usr/share/man --infodir=/usr/share/info --build=x86_64-alpine-linux-musl --host=x86_64-alpine-linux-musl --target=x86_64-alpine-linux-musl --with-pkgversion='Alpine 10.2.1_pre0' --enable-checking=release --disable-fixed-point --disable-libstdcxx-pch --disable-multilib --disable-nls --disable-werror --disable-symvers --enable-__cxa_atexit --enable-default-pie --enable-default-ssp --enable-cloog-backend --enable-languages=c,c++,d,objc,go,fortran,ada --disable-libssp --disable-libmpx --disable-libmudflap --disable-libsanitizer --enable-shared --enable-threads --enable-tls --with-system-zlib --with-linker-hash-style=gnu
Thread model: posix
Supported LTO compression algorithms: zlib
gcc version 10.2.1 20201113 (Alpine 10.2.1_pre0) 

~/titan.core $ compiler -v
TTCN-3 and ASN.1 Compiler for the TTCN-3 Test Executor
Product number: 7/CAX 105 7730 R2A
Version: 7.2.pl0
Build date: Nov 29 2020 08:48:17
Compiled with: GCC 10.2.1
Using OpenSSL 1.1.1h  22 Sep 2020

Copyright (c) 2000-2020 Ericsson Telecom AB


//----------------------------------------------titan-ubuntu image

docker build   -f Dockerfile.ubuntu   -t eclpsetitan/titan-ubuntu:7.2.0  .

docker run --rm -it -v /home/<user>:/home/titan/data eclipsetitan/titan-ubuntu:7.2.0
TTCN-3 and ASN.1 Compiler for the TTCN-3 Test Executor
Product number: 7/CAX 105 7730 R2A
Version: 7.2.pl0
Build date: Nov 29 2020 08:21:02
Compiled with: GCC 9.3.0
Using OpenSSL 1.1.1f  31 Mar 2020

Copyright (c) 2000-2020 Ericsson Telecom AB

To run a command as administrator (user "root"), use "sudo <command>".
See "man sudo_root" for details.



titan@a5b1b797ed75:~$ printenv
HOSTNAME=a5b1b797ed75
PWD=/home/titan
HOME=/home/titan
LS_COLORS=rs=0:di=01;34:ln=01;36:mh=00:pi=40;33:so=01;35:do=01;35:bd=40;33;01:cd=40;33;01:or=40;31;01:mi=00:su=37;41:sg=30;43:ca=30;41:tw=30;42:ow=34;42:st=37;44:ex=01;32:*.tar=01;31:*.tgz=01;31:*.arc=01;31:*.arj=01;31:*.taz=01;31:*.lha=01;31:*.lz4=01;31:*.lzh=01;31:*.lzma=01;31:*.tlz=01;31:*.txz=01;31:*.tzo=01;31:*.t7z=01;31:*.zip=01;31:*.z=01;31:*.dz=01;31:*.gz=01;31:*.lrz=01;31:*.lz=01;31:*.lzo=01;31:*.xz=01;31:*.zst=01;31:*.tzst=01;31:*.bz2=01;31:*.bz=01;31:*.tbz=01;31:*.tbz2=01;31:*.tz=01;31:*.deb=01;31:*.rpm=01;31:*.jar=01;31:*.war=01;31:*.ear=01;31:*.sar=01;31:*.rar=01;31:*.alz=01;31:*.ace=01;31:*.zoo=01;31:*.cpio=01;31:*.7z=01;31:*.rz=01;31:*.cab=01;31:*.wim=01;31:*.swm=01;31:*.dwm=01;31:*.esd=01;31:*.jpg=01;35:*.jpeg=01;35:*.mjpg=01;35:*.mjpeg=01;35:*.gif=01;35:*.bmp=01;35:*.pbm=01;35:*.pgm=01;35:*.ppm=01;35:*.tga=01;35:*.xbm=01;35:*.xpm=01;35:*.tif=01;35:*.tiff=01;35:*.png=01;35:*.svg=01;35:*.svgz=01;35:*.mng=01;35:*.pcx=01;35:*.mov=01;35:*.mpg=01;35:*.mpeg=01;35:*.m2v=01;35:*.mkv=01;35:*.webm=01;35:*.ogm=01;35:*.mp4=01;35:*.m4v=01;35:*.mp4v=01;35:*.vob=01;35:*.qt=01;35:*.nuv=01;35:*.wmv=01;35:*.asf=01;35:*.rm=01;35:*.rmvb=01;35:*.flc=01;35:*.avi=01;35:*.fli=01;35:*.flv=01;35:*.gl=01;35:*.dl=01;35:*.xcf=01;35:*.xwd=01;35:*.yuv=01;35:*.cgm=01;35:*.emf=01;35:*.ogv=01;35:*.ogx=01;35:*.aac=00;36:*.au=00;36:*.flac=00;36:*.m4a=00;36:*.mid=00;36:*.midi=00;36:*.mka=00;36:*.mp3=00;36:*.mpc=00;36:*.ogg=00;36:*.ra=00;36:*.wav=00;36:*.oga=00;36:*.opus=00;36:*.spx=00;36:*.xspf=00;36:
LESSCLOSE=/usr/bin/lesspipe %s %s
TERM=xterm
LESSOPEN=| /usr/bin/lesspipe %s
SHLVL=1
TTCN3_DIR=/home/titan/titan.core/Install
LD_LIBRARY_PATH=/home/titan/titan.core/Install/lib:
PATH=/home/titan/titan.core/Install/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
DEBIAN_FRONTEND=noninteractive
_=/usr/bin/printenv
titan@a5b1b797ed75:~$ compiler -v
TTCN-3 and ASN.1 Compiler for the TTCN-3 Test Executor
Product number: 7/CAX 105 7730 R2A
Version: 7.2.pl0
Build date: Nov 29 2020 08:21:02
Compiled with: GCC 9.3.0
Using OpenSSL 1.1.1f  31 Mar 2020

Copyright (c) 2000-2020 Ericsson Telecom AB


titan@a5b1b797ed75:~$ gcc -v
Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-linux-gnu/9/lto-wrapper
OFFLOAD_TARGET_NAMES=nvptx-none:hsa
OFFLOAD_TARGET_DEFAULT=1
Target: x86_64-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu 9.3.0-17ubuntu1~20.04' --with-bugurl=file:///usr/share/doc/gcc-9/README.Bugs --enable-languages=c,ada,c++,go,brig,d,fortran,objc,obj-c++,gm2 --prefix=/usr --with-gcc-major-version-only --program-suffix=-9 --program-prefix=x86_64-linux-gnu- --enable-shared --enable-linker-build-id --libexecdir=/usr/lib --without-included-gettext --enable-threads=posix --libdir=/usr/lib --enable-nls --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes --with-default-libstdcxx-abi=new --enable-gnu-unique-object --disable-vtable-verify --enable-plugin --enable-default-pie --with-system-zlib --with-target-system-zlib=auto --enable-objc-gc=auto --enable-multiarch --disable-werror --with-arch-32=i686 --with-abi=m64 --with-multilib-list=m32,m64,mx32 --enable-multilib --with-tune=generic --enable-offload-targets=nvptx-none=/build/gcc-9-HskZEa/gcc-9-9.3.0/debian/tmp-nvptx/usr,hsa --without-cuda-driver --enable-checking=release --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu
Thread model: posix
gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04) 

titan@a5b1b797ed75:~$ openssl version
OpenSSL 1.1.1f  31 Mar 2020




docker images
REPOSITORY                  TAG                 IMAGE ID            CREATED             SIZE
eclipsetitan/titan-ubuntu   7.2.0               1ed223897e13        3 minutes ago       1.23GB
ubuntu                      20.04               9140108b62dc        2 months ago        72.9MB
alpine                      edge                003bcf045729        2 months ago        5.62MB



//----------------------pushing the images to dockerhub

docker login
Login with your Docker ID to push and pull images from Docker Hub. If you don't have a Docker ID, head over to https://hub.docker.com to create one.
Username: eclipsetitan
Password: 
Login Succeeded



<user>@Machine:~/DockerX$docker push eclipsetitan/titan-alpine:7.2.0
The push refers to repository [docker.io/eclipsetitan/titan-alpine]
dc357782968c: Pushed 
aeba4acc0743: Pushed 
d9f59572f736: Pushed 
527697a28b8d: Pushed 
1d94515fd5ea: Pushed 
89cf6a7784bb: Pushed 
696e214b57c7: Pushed 
e7962aa42bbc: Pushed 
ddbb49d265b9: Layer already exists 
7.2.0: digest: sha256:27e799b463c5aaa9ec7397d9fbf4b41979592a188b665a26b5cb8997e8715660 size: 2204







<user>@Machine:~/DockerX$ docker push eclipsetitan/titan-ubuntu:7.2.0
The push refers to repository [docker.io/eclipsetitan/titan-ubuntu]
9a48cc325c4e: Pushed 
2692e7b2c593: Pushed 
7833fb5bf157: Pushed 
624a17a45154: Pushed 
ba288b75668e: Pushed 
ccfba2ebc319: Pushed 
782f5f011dda: Mounted from library/ubuntu 
90ac32a0d9ab: Mounted from library/ubuntu 
d42a4fdf4b2a: Mounted from library/ubuntu 
7.1.1: digest: sha256:878c4ec4e161ef9ebf7cfb129aa3042499b9a4e90d79eacdaab00bfd5b05a6af size: 2207

docker logout


