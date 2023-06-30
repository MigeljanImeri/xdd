#!/usr/bin/env bash 

set -e
# taken from https://github.com/mar-file-system/GUFI/blob/master/contrib/CI/centos8.sh
# Point to appropriate repos, current is no longer supported
sed -i 's/mirrorlist/#mirrorlist/g' /etc/yum.repos.d/CentOS-*
sed -i 's|#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' /etc/yum.repos.d/CentOS-*
 
yum -y install cmake gcc rdma-core-devel numactl-devel pkgconf-pkg-config rpm-build bc time strace sudo
