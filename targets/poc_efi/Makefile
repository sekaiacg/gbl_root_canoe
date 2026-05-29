clean:
	rm -rf build || true
submodule_uefi:
	mkdir -p build
	cd ../../submodules/uefi && make build_poc
	cp ../../submodules/uefi/build/unlock_rollback.efi ./build/unlock_rollback.efi
build: submodule_uefi
	# do nothing, just build the submodules
