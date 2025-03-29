#! /bin/bash
# pa_1_helper.sh
#
# Zips up your assignment when finished.
#
# These variables need to be changed to fit your system.
KERNEL_DIR="/home/kernel/linux-hwe-4.15.0/"
TEST_PGM_DIR="/home/os/OS_Programs/test_helloworld.c"

# zip command zips a file with this syntax:
#   zipi {arguments} FILE.zip FILES_TO_ZIP
#
# the -j option throws away the paths for the files in the zip folder
sudo zip -j assignment.zip "$KERNEL_DIR/arch/x86/kernel/helloworld.c" "$TEST_PGM_DIR" "/var/log/syslog"

# look inside zip file.
unzip -l assignment.zip



