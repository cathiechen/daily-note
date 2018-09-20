# Enviroment

1. find a big enough disk, and mount it in `sudo vi /etc/fstab`, and chown

2.  To build the webOS OSE image for QEMUx86.

```
$ git clone https://github.com/webosose/build-webos.git
$ cd build-webos
$ sudo scripts/prerequisites.sh
$ ./mcf -p 0 -b 0 qemux86
$ source oe-init-build-env
$ bitbake webos-image
```
Tips No.1: Restart the PC, if `./mcf -p 0 -b 0 qemux86` dosen't work.
Tips No.2: If you just wanna update the change of project, just run:

```
$ source oe-init-build-env
$ bitbake webos-image
```
Otherwise the settings might be changed.

E.g: 
- `IMAGE_ROOTFS_EXTRA_SPACE` in `/home/cathiechen/cathie/WebOS/source/build-webos/meta-webosose/meta-webos/recipes-core/images/webos-image-devel.bb`
- Add browser as default app:
add `com.webos.app.browser \` to `WEBOS_PACKAGESET_TESTAPPS` in
`/home/cathiechen/cathie/WebOS/source/build-webos/meta-webosose/meta-webos/recipes-core/packagegroups/packagegroup-webos-extended.bb`


3. clone the pre-built emulator package

`$ git clone https://github.com/webosose-emulator/prebuilt-emulator.git`


4. Run emulator

4.1 Create a JSON configuration file:

```
$ cd <pre-built emulator directory path>
$ mv webos-config-sample.json webos-config.json
$ vi webos-config.json
```
Basicly, this is to change `vmdk_file_path`, value the image you built, e.g: `~/build-webos/BUILD/deploy/images/qemux86/webos-image-qemux86.vmdk` 

4.2 run emulator: `./emulator webos-config.json`



### reference:
1. [QEMUx86 Emulator for Linux](http://webosose.org/develop/sdk-tools/emulator/qemux86-emulator-linux/)


## Install Command Line Interface

[download and install CLI](http://webosose.org/develop/sdk-tools/cli/download-and-install/)

1. Firstly, download it.
2. Unzip: `tar -xzvf xxx.tgz`
3. Add `ares-cli/bin` to PATH. How?
 
3.1. `vi ~/.profile`

3.2. 
```
# add CLI path
if [ -d "$HOME/ares-cli/bin" ]; then
  export PATH="$PATH:$HOME/ares-cli/bin"
fi
```
3.3. `$ source ~/.profile`

3.4. `$ echo $PATH`



## wechat process


https://chromium.googlesource.com/chromium/src.git/+/53.0.2744.1
