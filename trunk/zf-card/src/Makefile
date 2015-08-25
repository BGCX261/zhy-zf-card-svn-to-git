SUBDIRS=common statistics/c server client #test

define make_subdir
@for subdir in $(SUBDIRS); do \
	(cd $$subdir && make $1) \
	done;
endef 

all:
	$(call make_subdir, all)

install:
	$(call make_subdir, install)
	
.PHONY: test clean

clean:
	$(call make_subdir, clean)

test:
	cd test && make test

