all:

gdb:
	$(GDB) $(TARGET) ./samples/main.jkl

jackal:
	$(MAKE) -e DEBUG="-DVERBOSE -DENABLE_COLOR" -C lib

run: CFLAGS += -DVERBOSE -DENABLE_COLOR -fsanitize=address -fsanitize=undefined -g
run: all
	$(TARGET) ./samples/main.jkl

test:
	$(MAKE) -C tests

format:
	$(ASTYLE) --style=linux --indent=spaces=2 --indent-switches --indent-preproc-define --indent-preproc-cond --indent-col1-comments --pad-oper --pad-header --unpad-paren --align-pointer=name --align-reference=name --add-brackets --convert-tabs --close-templates --max-code-length=80 --suffix=none --mode=c lib/*.c include/**/*.h

defs:
	gperf -t -L ANSI-C -C -E -k '1,3,$$' -H hash -N keyword_lookup -p -t -j1 -o -i 1 -g -G -s 2 ./defs/keywords.gperf > ./include/jackal/jackal_keywords.h

check:
	$(CPPCHECK) -I include lib/*.c

install_deps: install_astyle install_cppcheck

install_astyle:
	@mkdir -p tools
	@cd ./tools && wget https://sinalbr.dl.sourceforge.net/project/astyle/astyle/astyle%203.1/astyle_$(ASTYLE_VERSION)_linux.tar.gz
	@cd ./tools && tar xvf astyle_$(ASTYLE_VERSION)_linux.tar.gz
	@cd ./tools/astyle/build && cmake .. && cmake build .. && make

install_cppcheck:
	@mkdir -p tools
	wget -O ./tools/cppcheck-$(CPPCHECK_VERSION).tar.gz https://github.com/danmar/cppcheck/archive/$(CPPCHECK_VERSION).tar.gz
	@cd ./tools && tar xvf cppcheck-$(CPPCHECK_VERSION).tar.gz
	@cd ./tools/cppcheck-$(CPPCHECK_VERSION) && make -j$(nproc)


.PHONE: all gdb jackal run test format defs check install_deps install_astyle install_cppcheck
