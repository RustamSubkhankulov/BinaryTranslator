CFLAGS = -lubsan -D NDEBUG -g -std=c++14 -fmax-errors=1 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++0x-compat -Wc++11-compat -Wc++14-compat  -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion  -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline  -Wlogical-op -Wmissing-declarations  -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstack-usage=8192  -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types  -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef  -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -fcheck-new -fsized-deallocation  -fstack-protector -fstrict-overflow   -flto-odr-type-merging -fno-omit-frame-pointer -fPIE -fsanitize=address  -fsanitize=bool -fsanitize=bounds -fsanitize=enum  -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=leak -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=shift -fsanitize=signed-integer-overflow -fsanitize=undefined -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=vptr -lm -pie

all:
	g++ syntax/*.cpp trans/*.cpp tree_simp/*.cpp tree/*.cpp general/*.cpp text_processing/*.cpp main.cpp logs/*.cpp lang_common/*.cpp lexic/*.cpp -O0 -o temp.exe -lubsan -D DEBUG -g -std=c++14 -fmax-errors=1 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++0x-compat -Wc++11-compat -Wc++14-compat -Wcast-align  -Wchar-subscripts -Wconditionally-supported -Wconversion  -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline  -Wlogical-op -Wmissing-declarations  -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstack-usage=8192  -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types  -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef  -Wunreachable-code -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -fcheck-new -fsized-deallocation  -fstack-protector -fstrict-overflow   -flto-odr-type-merging -fno-omit-frame-pointer -fPIE -fsanitize=address  -fsanitize=alignment -fsanitize=bool -fsanitize=bounds -fsanitize=enum  -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=leak -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=shift -fsanitize=signed-integer-overflow -fsanitize=undefined -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=vptr -lm -pie -Wno-unused-parameter -std=c++17

logfile:
	>> /tmp/calc_logs.html

.PHONY: asm proc imagesdir procexecute asmexecute noflags

exec: 
	./temp.exe calc_logs.html txt_files/circle.txt txt_files/trans.txt 

#################################################

langtest:
	./temp.exe calc_logs.html txt_files/testfactorial.txt txt_files/trans.txt

proctest:
	./proc/cpu.exe  txt_files/trans.code txt_files/in.txt txt_files/out.txt proc_logs.html

jittest:
	.././bintrans -i txt_files/trans.code 

#################################################

noflags:
	g++ syntax/*.cpp trans/*.cpp tree_simp/*.cpp tree/*.cpp general/*.cpp text_processing/*.cpp main.cpp logs/*.cpp lang_common/*.cpp lexic/*.cpp -O0 -o temp.exe 

asm:
	g++ proc/assembler/*.cpp text_processing/*.cpp general/*.cpp logs/*.cpp -o proc/asm.exe -lubsan -D NDEBUG -g -std=c++14 -fmax-errors=1 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++0x-compat -Wc++11-compat -Wc++14-compat  -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion  -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline  -Wlogical-op -Wmissing-declarations  -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstack-usage=8192  -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types  -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef  -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -fcheck-new -fsized-deallocation  -fstack-protector -fstrict-overflow   -flto-odr-type-merging -fno-omit-frame-pointer -fPIE -fsanitize=address  -fsanitize=bool -fsanitize=bounds -fsanitize=enum  -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=leak -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=shift -fsanitize=signed-integer-overflow -fsanitize=unreachable -lm -pie

proc:
	g++ proc/cpu/*.cpp proc/stack/*.cpp logs/*.cpp general/*.cpp text_processing/*.cpp proc/assembler/assembler.cpp -o proc/cpu.exe -lubsan -D NDEBUG -g -std=c++14 -fmax-errors=1 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++0x-compat -Wc++11-compat -Wc++14-compat  -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion  -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline  -Wlogical-op -Wmissing-declarations  -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstack-usage=8192  -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types  -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef  -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -fcheck-new -fsized-deallocation  -fstack-protector -fstrict-overflow   -flto-odr-type-merging -fno-omit-frame-pointer -fPIE -fsanitize=address  -fsanitize=bool -fsanitize=bounds -fsanitize=enum  -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=leak -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=shift -fsanitize=signed-integer-overflow  -fsanitize=unreachable  -lm -pie

imagesdir:
	mkdir /tmp/diff_images 

procexecute:
	./proc/cpu.exe proc/text_files/quadratic.code proc/text_files/proc_input.txt proc/text_files/proc_output.txt  proc_logs.html

asmexecute:
	./proc/asm.exe proc/text_files/quadratic.txt proc/text_files/quadratic.code asm_logs.html proc/text_files/asm_listing.txt

bin:
	./proc/asm.exe proc/text_files/test.txt ../binary/test.code asm_logs.html proc/text_files/test_listing.txt

binexec:
	./proc/cpu.exe ../binary/test.code proc/text_files/proc_input.txt proc/text_files/proc_output.txt  proc_logs.html

factexec:
	./proc/cpu.exe ../binary/fact.code proc/text_files/proc_input.txt proc/text_files/proc_output.txt  proc_logs.html

fact:
	./proc/asm.exe proc/text_files/factorial.txt ../binary/fact.code asm_logs.html proc/text_files/fact_listing.txt

quad:
	./proc/asm.exe proc/text_files/quadratic.txt ../binary/quad.code asm_logs.html proc/text_files/quad_listing.txt
