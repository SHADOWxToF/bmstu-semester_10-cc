CFLAGS := -std=c99 -Wall -pedantic -Wvla -Wfloat-conversion -Wfloat-equal -Iinc -g3 -Wextra

OUT := out/main.o out/grammar.o out/menu.o out/regexp.o out/system.o out/fsm.o out/set.o

TEST := out/grammar.o out/menu.o

ifeq ($(strip $(wildcard out/*.exe) $(wildcard out/*.o)),)
	CLEAN := @echo Nothing to be cleaned
else
	CLEAN := rm $(wildcard out/*.exe) $(wildcard out/*.o)
endif

app : out/app.exe

test : out/test.exe

out/app.exe : $(OUT)
	gcc $^ -o $@

out/test.exe : $(TEST)
	gcc $^ -o $@

out/%.o : src/%.c inc/*.h
	gcc $(CFLAGS) $< -c -o $@

.PHONY: clean, app

clear :
	$(CLEAN)