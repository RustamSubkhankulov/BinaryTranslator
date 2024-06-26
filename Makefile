CC    = gcc 

OBJ   = obj/main.o      \
	    obj/logs.o 	    \
		obj/general.o   \
		obj/bintrans.o  \
		obj/standard.o  \
		obj/patch.o     \
		obj/trans.o	    \
		obj/optimizer.o \
		obj/list.o

FLAGS = 
#-lubsan -D NDEBUG -g -std=c++14 -fmax-errors=1 				\
		-Wall -Wextra -Weffc++ -Waggressive-loop-optimizations  	\
		-Wc++0x-compat -Wc++11-compat -Wc++14-compat  				\
		-Wcast-qual -Wchar-subscripts -Wconditionally-supported 	\
		-Wconversion  -Wctor-dtor-privacy -Wempty-body 				\
		-Wfloat-equal -Wformat-nonliteral -Wformat-security 		\
		-Wformat-signedness -Wformat=2 -Winline  -Wlogical-op 		\
		-Wmissing-declarations  -Wnon-virtual-dtor -Wopenmp-simd	\
		-Woverloaded-virtual -Wpacked -Wpointer-arith 				\
		-Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo	\
		-Wstack-usage=8192  -Wstrict-null-sentinel 					\
		-Wstrict-overflow=2 -Wsuggest-attribute=noreturn 			\
		-Wsuggest-final-methods -Wsuggest-final-types  				\
		-Wsuggest-override -Wswitch-default -Wswitch-enum 			\
		-Wsync-nand -Wundef  -Wunreachable-code -Wunused 			\
		-Wuseless-cast -Wvariadic-macros -Wno-literal-suffix 		\
		-Wno-missing-field-initializers -Wno-narrowing				\
		-Wno-old-style-cast -Wno-varargs -fcheck-new 				\
		-fsized-deallocation  -fstack-protector 					\
		-fstrict-overflow   -flto-odr-type-merging 					\
		-fno-omit-frame-pointer -fPIE -fsanitize=address  			\
		-fsanitize=bool -fsanitize=bounds -fsanitize=enum  			\
		-fsanitize=float-cast-overflow 								\
		-fsanitize=float-divide-by-zero 							\
		-fsanitize=integer-divide-by-zero -fsanitize=leak 			\
		-fsanitize=nonnull-attribute -fsanitize=null 				\
		-fsanitize=object-size -fsanitize=return 					\
		-fsanitize=returns-nonnull-attribute -fsanitize=shift 		\
		-fsanitize=signed-integer-overflow -fsanitize=undefined 	\
		-fsanitize=unreachable -fsanitize=vla-bound 				\
		-fsanitize=vptr -lm -pie

#================================================

GLOBAL_DEP 		= src/global_conf.h 				\
				  text_files/commands.txt			\
				  text_files/instr.txt				\
				  text_files/jumps.txt				\
				  text_files/std_func.txt

#================================================

GENERAL_DEP  	= src/general/general.cpp 			\
				  src/general/general.h 			\
				  src/general/general_conf.h 

MAIN_DEP 	 	= src/main.cpp

LOG_DEP 	 	= src/logs/errors_and_logs.cpp		\
				  src/logs/errors_and_logs.h 		\
				  src/logs/errors.h 				\
				  src/logs/log_definitions.h 		\
				  src/include/errors.txt

BINTRANS_DEP 	= src/bintrans/bintrans.cpp 		\
				  src/bintrans/bintrans.h 			\
				  src/bintrans/bintrans_conf.h 		\
				  src/bintrans/instr.h				\
				  src/bintrans/patch.h				\
				  src/bintrans/patch_conf.h						

LIST_DEP        = src/list/list_config.h			\
				  src/list/list.cpp					\
				  src/list/list.h	

STANDARD_DEP    = src/bintrans/standard.cpp			\
				  src/bintrans/standard.h			\
				  src/bintrans/standard_conf.h		\

PATCH_DEP       = src/bintrans/patch.h				\
				  src/bintrans/patch.cpp			\
				  src/bintrans/patch_conf.h			\
				  src/bintrans/bintrans.h			\
				  src/bintrans/bintrans_conf.h		\
				  src/bintrans/instr.h

TRANS_DEP       = src/bintrans/trans.cpp			\
				  src/bintrans/trans.h				\
				  src/bintrans/instr.h				\
				  src/bintrans/bintrans.h			\
				  src/bintrans/bintrans.cpp			\
				  src/bintrans/bintrans_conf.h

OPTIMIZER_DEP   = src/bintrans/optimizer.cpp		\
				  src/bintrans/optimizer.h			\
				  src/bintrans/optimizer_conf.h		\
				  src/bintrans/bintrans_conf.h		\

#================================================

all: global

global: $(OBJ) 
	$(CC) $(OBJ) -o bintrans $(FLAGS) -lm -no-pie 
#-fsanitize=address -fsanitize=bounds

#================================================

obj/main.o: 	 $(GLOBAL_DEP) $(MAIN_DEP)
	$(CC) src/main.cpp 					-c -o obj/main.o 	  $(FLAGS)

obj/logs.o: 	 $(GLOBAL_DEP) $(LOG_DEP)
	$(CC) src/logs/errors_and_logs.cpp  -c -o obj/logs.o      $(FLAGS)

obj/general.o: 	 $(GLOBAL_DEP) $(GLOBAL_DEP)
	$(CC) src/general/general.cpp 		-c -o obj/general.o   $(FLAGS) -mavx -mavx2 -msse4

obj/bintrans.o:	 $(GLOBAL_DEP) $(BINTRANS_DEP) 
	$(CC) src/bintrans/bintrans.cpp 	-c -o obj/bintrans.o  $(FLAGS)

obj/list.o:		 $(GLOBAL_DEP) $(LIST_DEP)
	$(CC) src/list/list.cpp 		    -c -o obj/list.o 	  $(FLAGS)

obj/standard.o:  $(GLOBAL_DEP) $(STANDARD_DEP)
	$(CC) src/bintrans/standard.cpp     -c -o obj/standard.o  $(FLAGS)
	$(FLAGS)

obj/patch.o:	 $(GLOBAL_DEP) $(PATCH_DEP)
	$(CC) src/bintrans/patch.cpp 		-c -o obj/patch.o     $(FLAGS)

obj/trans.o:	 $(GLOBAL_DEP) $(TRANS_DEP)
	$(CC) src/bintrans/trans.cpp 		-c -o obj/trans.o     $(FLAGS)

obj/optimizer.o: $(GLOBAL_DEP) $(OPTIMIZER_DEP)
	$(CC) src/bintrans/optimizer.cpp	-c -o obj/optimizer.o $(FLAGS)

#================================================

.PNONY: cleanup listimages

listimages:
	mkdir /tmp/list_images

cleanup:
	rm obj/*.o 
	rm bintrans 

#================================================
