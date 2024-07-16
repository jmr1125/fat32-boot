if [ "$1" -eq "qemu" ] ; then
	qemu-system-x86_64 -hdc res.img -debugcon stdio
fi
rm res.img.lock
bochs -q
