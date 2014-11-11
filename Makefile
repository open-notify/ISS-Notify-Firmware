all:

%:
	$(MAKE) -C iss-notify $@
	$(MAKE) -C test-harness $@
