#!/usr/bin/env bash 

set -e

#https://grigorkh.medium.com/fix-tzdata-hangs-docker-image-build-cdb52cc3360d
#Added because github runner would get stuck on interactive option when installing 
#tzdata
TZ="America/Denver"
ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

apt-get update
apt install -y libnuma-dev libnuma1 cmake gcc pkg-config dpkg dpkg-dev sudo \
	strace bc time libibverbs-dev 

