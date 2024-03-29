# JIT-компилятор
JIT-компилятор, транслирующий бинарный код, предназначенный для исполнения виртуальным процессором, в инструкции архитектуры x86.
Виртуальный процессор был написан в качестве учебной задачи в первом семестре (МФТИ ФРКТ)

Главная особенность JIT (Just-in-time) компилятора состоит в том, что результат трансляции - бинарный код, содержащий инструкции архитектуры, в которую происходит трансляция, исполняется сразу же после процесса трансляции. Для этого бинарный код сохраняется в массиве и вызывается в качестве функции. 

Главным результатом, которого мы хотим добиться при написании JIT-компилятора, это повышение быстродействия исполнения. Сравнение времени исполнения тестовой задачи будет заключающим пунктом работы.

## Архитектура виртуального процессора
Для того, чтобы быть более краткими в обсуждениях трансляции, будем называть архитектуру виртуального процессора, из которой происходит трансляция, r86.

Для начала опишем инструкции архитектуры r86.

Инструкции архитектуры r86 могут быть поделены на несколько групп: 
1) Инструкции, используемые для арифметических операций: эта группа включает в себя как инструкции, производящие простейшие арифметические операции - ADD, SUB, MUL, DIV, так и инструкции, вычисляющие значения некоторых математчиских функций - SIN, COS, TG, LN, ASIN, ATG, POW - и инструкции сравнения - MR, MRE, LS, LSE, EQ, NEQ.
2) Инструкции управления потоком исполнения - безусловный 'jump' - JMP, условные 'jump'ы - JA, JAE, JB, JBE, JE, JNE, а также вызов функции CALL, возврат из функции RET и прекращение работы процессора HLT. 
3) Инструкции перемещения данных - PUSH и POP
4) Инструкции ввода и вывода IN и OUT

Необходимыми для исполнения инструкций виртуальным процессоров являются:
- 16 регистров общего назначения, содержащие в себе значения с плавающей точкой (float)
- Оперативная память размера 1024 элемента, представляющая из себя массив типа float
-  Структура данных стек, использующийся для арифметики, построенной на PUSH и POP инструкциях, а также для хранения адреса возврата при вызове функций.

#### Арифметика 
Особенность арифметических операций в архитектуре r86 - все инструкции, использующиеся для вычисления арифметических выражений, принимает аргументы со стека. Это означает, что инструкции, требующие два аргумента, используют для вычислений два значения, которые сейчас находятся на вершине стека. После выполнения инструкции результат также находится на вершине стека.

ADD, SUB, MUL и DIV  исполняются простейшие арифметические действия - сложение, вычитание, умножение и деление. При этом если до выполнения инстукции в стеке лежало два числа - два аргумента, то после на вершине стека будет находиться только одно значение - результат вычисления.

Команды SIN, COS, TG, LN, ASIN, ATG вычисляют соответсвенно синус, косинус, тангенс, натуральный логарифм, арктангенс и арксинус от аргумента с вершины стека и там же возвращают результат. Фактически, аргумент, находящийся на вершине стека, заменяется результатом применения к нему соответствующей функции. Для вычисления результата используются функции из библеотеки <math.h>. 

Инструкции MR, MRE, LS, LSE, EQ, NEQ сравнивают два значения, находящиеся на вершине стека и в зависимости от результата, возвращают единицу или ноль на вершине стека. Если сравнение верно, то результатом является 1, иначе результат равен 0. По завершении выполнения инструкции сравнения аргументы удаляются из стека.

#### Инструкции управления потоком 

Инструкция HLT является обязательным условием завершения работы виртуального процессора. Как только виртуальный процессор встречает инструкцию HLT, вне зависимости от состояния стека исполнение дальнейших инструкций прекращается.  

Инструкция вызова функции CALL сохраняет адрес следующей после нее инструкции в стеке, а после этого исполнение инструкций продолжается с адреса, который является аргументом инструкции CALL в бинарном коде. Инструкция RET принимает значение с вершины стека, после чего исполнение инструкций происходит с адреса, равного этому значению. С помощью двух этих инструкций происходят вызов функции и возврат из нее в r86 архитектуре.

Безусловный 'jump' вне зависимости от состояния виртуального процессора изменяет текущую позицию в коде, инструкции продолжают исполнятся с адреса, являющегося аргументом инструкции в бинарном коде. В отличие от безусловного, условные 'jump'ы происходят только если результатом соответствующего сравнения двух аргументов - двух значений с вершины стека - является true. Иначе прыжок не происходит и исполнение инструкций продолжается со следующей после условного 'jump'а. При сравнении аргументы пропадают из стека. 

Аргументами условного и безусловных 'jump'ов являтся абсолютные адреса - позиции в бинарном коде относительно начала.

#### Перемещение данных

Для перемещения данных используются две инструкции - PUSH и POP.
Важно отметить, что в архитектуре r86 отсутствует возможность перемещения данных из регистра или оперативной памяти в регистр или оперативную память без использования стека. 

Инструкция PUSH добавляет значения в стек, а POP - удаляет его из стека.

В архитектуре r86 сущесвуют различные разновидности данных инструкций.

PUSH в стек:
- из регистра ( push ax )
- из оперативной памяти по индексу, равному значению в одном из регистров (  ``` push [ax] ``` )
- из оперативной памяти по индексу, равному аргументу инструкции из бинарного кода ( ``` push [4] ``` )
- из оперативной памяти по индексу, равному сумме значения в одном из регистров и значения из бинарного кода ( ``` push [ax+4] ``` )
- константного значения из бинарного кода ( ``` push 5 ``` )

 POP из стека в:
- в регистр ( pop ax )
- в оперативную памяти по индексу, равному значению в одном из регистров ( ```pop [ax] ``` )
- в оперативную памяти по индексу, равному аргументу инструкции из бинарного кода ( ``` pop [4] ``` )
- в оперативную памяти по индексу, равному сумме значения в одном из регистров и значения из бинарного кода (  ``` pop [ax+4] ``` )

#### Ввод и вывод
Инструкция IN принимает аргумент -float число - из ввода программы, считывая значение при помощи scanf. Далее считанное значение добавляется в стек.
Инструкция OUT принимает аргумент - значение на вершине стека и, используя printf, печатает число в выводе. Аргумент инструкции OUT удаляется из стека.
Вводом и выводом вирутального процессора являются две текстовых файла, определяемые конфигурацией процессора.

#### Неподдерживаемые инструкции

Инструкция DRAW выводит в терминале содержание определенной области оперативной памяти по строчкам длиной, заданной конфигурацией виртуального процессора. Данная область оперативной памяти является зарезервированной в качестве видеопамяти. Трансляция этой инструкции не поддерживается данной версией JIT-компилятора. При трансляции инструкция DRAW пропускается и не транслируется в какие-либо инструкции

## Трансляция инструкций

В соответствие 16-ти регистрам архитектуры r86 поставлены 16 XMM регистров архитектуры x86. Если в процессе трансляции устанавливается, что в коде присутствуют инструкции, обращающиеся к RAM, то аллоцируется массив типа float того же размера, что и RAM в архитектуре r86.

Целочисленные регистры x86 используются для сохранения значений XMM регистров при исполнении некоторых инструкций.

В качестве стека используется аппаратный стек архитектуры х86.

Начало трансляции сопровождается инициализацией инструкций, сохраняющих целочисленные регистры в стеке в соответствии с соглашением о вызовах System V AMD64 ABI и устанавливающих все XMM регистры в нулевое значение ( инструкции ``` pxor xmm(i), xmm(i) | 0 < i < 15 ``` ). 

Если транслируемая программа не завершается инструкцией HLT, то корректного завершения программы не будет произведено.

### HLT
Инструкция HLT транслируется в инструкции x86, которые восстанавливают значения целочисленных регистров из стека по значениям, которые были сохранены на входе. Завершающей инструкцией является ret (x86).

### Арифметика

Арифметика в оттранслированном коде функционирует аналогично арифметике в архитектуре r86. На момент произведения арифметического действия, на вершине стека ожидаются два аргумента. Одно из значений перемещается в XMM0 регистр, далее производится арифметическая операция со вторым значением, находящимся в стеке. После этого регистр RSP увеличивается на 8, тем самым удаляя один из аргументов из стека. Результат вычисления располагается на вершине стека, занимая место одного из аргументов. Так как все 16 XMM регистров содержат в себе значения, которые не должны изменяться, если не исполняется соотвествующая инструкция перемещения данных, значение XMM0 сохраняется в регистре r15d, а по завершении восстанавливается.

Пример: ADD

<code>  movd r15d, xmm15                  </code>

<code>  movss xmm15, dword [ rsp + 8 ]    </code>

<code>  adds xmm15,  dword [ rsp ]        </code>

<code>  add rsp, 8                        </code>
  
<code>  movss dword [ rsp ], xmm15        </code>

<code>  movd xmm15, r15d                  </code>

### Инструкции перемещения данных - PUSH и POP
В примерах трансляции ниже Start_addr - условное обозначение адреса начала float массива, который используется в качестве оперативной памяти. 

##### PUSH

1) PUSH в стек из оперативной памяти по индексу, равному значению одного из 16 регистров:
 Сначала определяется, значение какого из 16 регистров будет использоваться в команде. Для этого используется unsigned char значение из бинарного кода. Далее значение из выбранного XMM регистра конвертируется в один из целочисленных регистров в свое целочисленное представление. Далее происходит push из соответствующей ячейки оперативной памяти.
  
  <code> cvtss2si r13d, xmm(i) </code>
  
  <code> push qword [ Start_addr + r13d * 4] </code>

2) Push из регистра в стек:
 Аналогично предыдущему случаю, используемый в интсрукции регистр определяется по unsigned char значению из бинарного кода.
 
  <code> sub rsp, 8 </code>
  
  <code> movss dword [rsp], xmm(i) </code>
  
3) Push в стек из оперативной памяти по определенному индексу, определяемому float значением из бинарного кода:
  Float значение считывается из бинарного кода, приводится к целому числу (типу unsigned int) . Далее инициализируется инструкция, которая кладет полученное значение в целочисленный регистр r13d. После этого происходит push.
  
  <code> mov r13d, index </code>
  
  <code> push qword [ Start_addr + r13d * 4] </code>
  
4) Push константного значения в стек:
 В данном случае значение, которое будет добавлено в стек, определяется числом float в бинарном коде. На основе float значения генерируется unsigned int величина, побайтово равная представлению float числа. Результат добавляется в стек.
  
  <code> push Constant_value </code>
  
5) Push в стек из оперативной памяти по индексу, равному сумме значения одного из регистров и значения float из бинарного кода.
 Из бинарного кода считываются два значения, первое - unsigned char - определяет используемый в инструкции XMM регистр, его значение конвертируется в целочисленное представление в регистр r13d. Второе - float - приводится к целочисленному представлению и умножается на 4, т.к. sizeof(float) == 4. Полученное значение подставляется в инструкцию, перемещающую его в r14d. После этого происходит push.
 
  <code> cvtss2si r13d, xmm(i) </code>
  
  <code> mov r14d, Imm_value_from_binary_code </code>
  
  <code> push qword [ Start_addr + r13d * 4 + r14d ] </code>

##### POP

Трансляция инструкции POP происходит во многом схоже с трансляцией инструкции PUSH, поэтому ограничимся примерами инструкций без комментариев.

1) Pop из стека в оперативную память по индексу, равному значению одного из регистров:

  <code> cvtss2si r13d, xmm(i) </code>
  
  <code> pop r14 </code>
  
  <code> mov dword [ Start_addr + 4 * r13d ], r14d </code>

2) Pop из стека в регистр:

  <code> movss xmm(i), dword [ rsp ] </code>
  
  <code> add rsp, 8 </code>

3) Pop из стека в оперативную память по индексу, определяемому значением из бинарного кода:

  <code> mov r13d, Index_from_binary_code </code>
  
  <code> pop r14 </code>
  
  <code> mov dword [ Start_addr + r13d * 4 ], r14d </code>

4) Pop из стека в оперативную память по индексу, равному сумме значения одного из регистров и значения из бинарного кода
  
  <code> cvtss2si r13d, xmm(i) </code>
  
  <code> mov r15d, (unsigned int) (float value from binary) * 4 </code>

  <code> pop r14 </code>
  
  <code> mov dword [ Start_addr + r13d * 4 + r15d ], r14d </code>

### Инструкции сравнения: EQ, NEQ, MR, MRE, LS, LSE
Результатом выполнения данных инструкций должны быть ноль или единица ( их float представление ) на вершине стека в зависимости от сравнения двух аргументов с вершины стека.

Ниже приведен пример трансляции инструкции MR

1) Сохраняем значения XMM0 и  ХММ13 в целочисленных регистрах:
  
  <code> movd r15d, xmm0  </code>
  
  <code> movd r13d, xmm13 </code>

2) Обнуляем регистр XMM13 

  <code> pxor xmm13, xmm13 </code>
 
3) Сравниваем два значения с вершины стека, устанавливаются флаги x86
 
  <code> movss xmm0, dword [ rsp ] </code>
  
  <code> comiss xmm0, dword [ rsp + 8 ] </code>
 
4) Условный джамп, соответствующий условию, противоположному инструкции сравнения (тип условного 'jump'а зависит от типа инструкции сравнения:

  <code> jbe $ + sizeof (next instruction ) </code>

5) В случае, если условный джамп не произошел, а значит сравнение верно, устанавливаем значение регистра ХММ13 в единицу

  <code> movss xmm13, dword [ Address of 1 constant value ] </code>

6) Очищаем стек от аргументов инструкции сравнения
  
  <code> add rsp, 16 </code>
  
7) Push значения xmm13 регистра в стек

  <code> sub rsp, 8 </code>
  
  <code> movss dword [ rsp ], xmm13 </code>

8) Восстанавливаем значения ХММ0 и ХММ13 из целочисленных регистров

  <code> movd xmm0,  r15d </code>
  
  <code> movd xmm13, r13d </code>

### Условные и безусловный 'jump'ы, CALL и RET

1) При трансляции все абсолютные адреса, являющиеся аргументами 'jump'ов, пересчитываются в смещения от адреса инструкции 'jump'а до его места назначения. Эти величины являются аргументами для near relative 'jump'ов архитектуры x86. Максимальное смещение, на которое может происхожить 'jump' ограничивается размером одного сегмента. 
3) Результатами трансляции инструкция r86 JMP, CALL и RET являются соответсвенно near relative версии jmp, call и ret инструкций архитектуры x86.
4) Так как смещение, на которое может происходить условный 'jump' в архитектуре х86 меньше, чем размер сегмента кода, условные 'jump'ы транлируются особым образом. Ниже пример трансляции JA (r86) в архитектуру x86. 
  
  <code> add rsp, 16 </code>
  
  <code> movd r15d, xmm0 </code>

  <code> movss xmm0, dword [ rsp - 16 ] </code>
  
  <code> comiss xmm0, dword [ rsp - 8 ] </code>

  <code> movd xmm0, r15d </code>

  <code> jbe $ + sizeof (near relative jump) </code>
  
  <code> jmp (Offset to destination) </code>

### Математические функции, вывод и ввод

Инструкции, вычисляющие математические функции -  POW, SIN, COS и другие, а также IN и  OUT транслируются в вызовы соответсвутющих функций стандартной библеотеки функций, написанной специально для этих инструкций. Функии, вычисляющие синус, косинус и другие математисческие функции, используют библиотеку <math.h>. На первом этапе трансляции смещения в инструкциях call остаются равными нулю, на втором этапе - во время патчинга - в данные инструкции подставляются высчитанные смещения.

Для некоторых из таких функций требуется стек, выровненный на границу 16 байт, поэтому каждый вызов сопровождается выравниваем стека путём вычисления остатка от деления значения регистра RSP на 16. Полученное значение прибавляется к RSP и сохраняется в одном из целочисленных регистров, которые не изменяют своего значения при вызове функций по соглашению о вызовах. После возвращения из функции, величина, прибавленная к rsp, вычитается из этого регистра.

Кроме того, каждый вызов функции сопровождается сохранением всех ХММ регистров в стеке, так как по соглашению о вызовах ни один из ХММ регистров не сохраняется.

##### Пример трансляции SIN:
  - сохраняем значение ХММ0
 
  <code> movd r15d, xmm0 </code>
  
  - производим РОР из стека в ХММ0
   
  <code> movss xmm0, dword [ rsp ] </code>
  
  <code> add rsp, 8 </code>
  
  - сохраняем ХММ регистры с 1-го по 15-ый в стеке
  
  - выравниваем стек на границу 16
   
  <code> mov r14, rsp </code>
  
  <code> and r14, 0xF </code>
  
  <code> add rsp, r14 </code>
  
  - call соответствующей функции стандратной библеотеки
  
  - восстанавливаем значения ХММ регистров с 1-го по 15-ый из стека
  
  - производим PUSH из ХММ0 в стек

  <code> sub rsp, 8 </code>
  <code> movss dword [ rsp ], xmm0 </code>
  
  - восстанавливаем значением ХММ0 регистра, сохраненное в целочисленном регистре
  <code> movd xmm0, r15d </code >

### Патчинг адресов 'jump'ов и 'call'ов

Для того, чтобы правильно вычислить смещения всех условных и безусовных jump'ов и  'call'ов, производятся дествия в несколько шагов
- На этапе трансляции в динамическом массиве сохраняется соответствие между всеми адресами во входном бинарном коде и в уже оттранслированном коде.
- На этапе патчинга, происходящем уже после первого этапа, высчитываются необходимые смещения, используя полученную таблицу соответсвия адресов. Высчитанные смещения подставляются в 'jump'ы и 'call'ы, где это требовалось.

Таким образом, трансляция кода завершается в два прохода.

## Оптимизатор 

JIT-компилятор также предлагает возможность использовать оптимизации бинарного кода, в частности свёртку констант. 
Для проведения оптимизаций инициализируется список и при оптимизациях лишние элементы списка удаляются или заменяются на другие. После завершения оптимизаций на основе списка генерируется новый массив кодов инструкций архитектуры виртуального процессора, который используется в качестве входных данных транслятора.

Свертка констант вчключает в себя различные случаи. Примеры оптимизаций приведены ниже.

1) <code> PUSH 0; COS </code>              --> <code> PUSH 1 </code> 
2) <code> PUSH 2; PUSH 3; ADD </code>      --> <code> PUSH 5 </code>
3) <code> PUSH [ax+5]; PUSH 0; MUL </code> --> <code> PUSH 0 </code>
4) <code> PUSH [ax+5]; PUSH 1; MUL </code> --> <code> PUSH [ax+5] </code>
5) <code> PUSH [ax+5]; PUSH 0; ADD </code> --> <code> PUSH [ax+5] </code> 


### Тестирование оптимизаций

Для того, чтобы убедиться в уменьшении времени исполнения программы с применением оптимизаций, искуственнно создадим ситуацию, в которой оптимизация значительно сократит количество вычислений. Для этого запустим программу, вычисляющую 2^10 10,000,000 раз в цикле при помощи JIT-компилятора сначала без флага оптимизации, а затем с применением оптимизатора и сравним измеренные показатели времени исполнения в обоих случаях.

|           | no opt | -opt flag |
|-----------|--------|-----------|
| time, sec | 0,285  | 0,100     |

Можем увидеть, что в определенных ситуациях, свертка констант позволяет добиться более чем двухкратного уменьшения времени исполнения.

## Сравнение времени исполнения

Для сравнения времени исполнения виртуальным процессором и JIT-компилятором используем 
программу включающую в себя большой объем вычислений, чтобы время трансляции кода при исполнении JIT-компилятора было мало по сравнению со временем, приходящимся на непосредственно вычисления в самой программе.
Для проведения теста используем компилятор, написанный в качестве учебной задачи в первом семестре. Результатом работы компилятора является бинарный код, содержащий инстуркции архитектуры виртуального процессора. 
[Исходный текст тестовой программы на языке 'Harry Potter'](https://github.com/RustamSubkhankulov/BinaryTranslator/blob/main/lang/txt_files/testfactorial.txt "testfactorial.txt"). Она включает в себя вычисления факториала 34 в цикле 100,000 раз. 
Для измерения времени исполнения будем использовать built-in опцию командной строки 'time', в качестве времени будем принимать результат time 'user'. 
Каждое измерение проведем 5 раз и усредним полученные значения. Так же вычислим среднеквадратичное отклонение.


#### Сравнение времени

|           | CPU            | JIT           |
|-----------|----------------|---------------|
| time, sec | 26,154 ± 0,170 | 0,106 ± 0.014 |

Как можно увидеть по результатам измерений, время исполнения программы с использованием JIT-компилятора меньше времени исполнения программы виртуальным процессоров более чем в 240 раз. 
 
