# Демков Михаил Кириллович. БПИ212. Вариант 22
## Описание задания
**Задача об инвентаризации по рядам.** 

После нового года в библиотеке университета обнаружилась пропажа каталога. После поиска и наказания виноватых, ректор дал указание восстановить каталог силами студентов. Фонд библиотека представляет собой прямоугольное помещение, в котором находится M рядов по N шкафов по K книг в каждом шкафу. Требуется создать приложение, составляющее каталог. В качестве отдельного процесса
задается составление подкаталога одним студентом–процессом для
одного ряда. После этого студент передает информацию процессу–
библиотекарю, который сортирует ее учитывая подкаталоги, переданные другими студентами. Примечание. Каталог — это список
книг, упорядоченный по их инвентарному номеру или по алфавиту (на выбор разработчика). Каждая строка каталога содержит
идентифицирующее значение (номер или название), номер ряда,
номер шкафа, номер книги в шкафу.

## 4 балла
**Все материалы в каталоге mark_4**

Каталог упорядочен по инвентарным номерам.

- Родительский процесс создает дочерние процессы, каждый из которых отвечает за создание подкаталога для одного ряда.
- Дочерние процессы записывают данные о книгах в разделяемую память.
- Родительский процесс ожидает завершения всех дочерних процессов, сортирует каталог и выводит его на экран.
- Приложение корректно завершается по прерыванию с клавиатуры (Ctrl+C) благодаря обработчику сигнала SIGINT.
- По завершении работы программы семафоры и разделяемая память удаляются.

**Реализация функции main:**

* Создание разделяемой памяти и именованных POSIX семафоров.
* Создание дочерних процессов и запуск функции inventory_process для каждого ряда.
* Ожидание завершения всех дочерних процессов.
* Сортировка и вывод каталога на экран.
* Освобождение ресурсов: закрытие семафоров и разделяемой памяти.

**Результаты работы программы:**
* Программа успешно реализует задачу инвентаризации каталога библиотеки. 
* Каталог упорядочен по инвентарным номерам и выводится на экран.
* Программа корректно завершается при получении сигнала прерывания с клавиатуры (Ctrl+C). 
* При завершении программы удаляются семафоры и разделяемая память.

Результат запуска программы можно посмотреть в файле _**console_screenshot.jpg**_

## 5 баллов
**Все материалы в каталоге mark_5**

* Реализована функция inventory_process, которая выполняется дочерними процессами для составления подкаталогов. Каждый дочерний процесс создает подкаталог для своего ряда, после чего передает его процессу-библиотекарю через разделяемую память.
* Функция handle_sigint обрабатывает сигнал прерывания с клавиатуры (Ctrl+C), что позволяет корректно завершить программу.
* После завершения работы программы, семафоры и разделяемая память удаляются с использованием функций sem_destroy, munmap.

**Вывод:** В ходе данной работы было разработано консольное приложение на языке C для инвентаризации каталога библиотеки. Программа использует разделяемую память и семафоры для обмена данными между процессами. Результат работы программы соответствует поставленной задаче, а корректное завершение программы и удаление разделяемой памяти и семафоров гарантирует стабильность работы системы.

Результат запуска программы можно посмотреть в файле _**console_screenshot.jpg**_

## 6 баллов
**Все материалы в каталоге mark_6**

**Результаты работы:**

* Приложение использует множество дочерних процессов, разделяемую память и семафоры стандарта UNIX System V для координации работы процессов.
* Ввод количества рядов (M), шкафов (N) и книг в каждом шкафу (K) осуществляется через командную строку.
* Программа корректно завершает работу при прерывании с клавиатуры и удаляет семафоры и разделяемую память.
* Результаты работы программы отображаются на экране на русском языке, а также комментарии в коде представлены на русском языке.

**Выводы:**

* Разработано приложение на языке C, решающее задачу инвентаризации книг в библиотеке с использованием множества процессов, разделяемой памяти и семафоров стандарта UNIX System V.
* Приложение успешно выполняет поставленные задачи и обеспечивает корректное взаимодействие процессов при создании каталога книг.
* Работа с разделяемой памятью и семафорами стандарта UNIX System V позволяет обеспечить эффективное взаимодействие процессов при выполнении задачи инвентаризации книг в библиотеке.

Результат запуска программы можно посмотреть в файле _**console_screenshot.jpg**_