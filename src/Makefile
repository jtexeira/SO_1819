############################# Makefile ##########################

SUBDIRS = agreg client manager server

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	(cd agreg && make clean)
	(cd client && make clean)
	(cd manager && make clean)
	(cd server && make clean)

cleandb:
	rm -rf artigos stocks strings vendas

.PHONY: $(SUBDIRS)
