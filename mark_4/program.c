#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <signal.h>

typedef struct {
int inventory_number;
char title[50];
int row;
int shelf;
int book_number;
} Book;

int compare_books(const void *a, const void *b) {
const Book *book_a = (Book *)a;
const Book *book_b = (Book *)b;
return book_a->inventory_number - book_b->inventory_number;
}

void sort_books(Book *books, int total_books) {
qsort(books, total_books, sizeof(Book), compare_books);
}

void print_catalog(Book *books, int total_books) {
printf("Каталог:\n");
for (int i = 0; i < total_books; i++) {
printf("%d: %s [ряд %d, шкаф %d, книга %d]\n", books[i].inventory_number, books[i].title, books[i].row, books[i].shelf, books[i].book_number);
}
}

void inventory_process(int row, int shelves, int books_per_shelf, Book *shared_memory, sem_t *semaphore) {
for (int shelf = 0; shelf < shelves; shelf++) {
for (int book_number = 0; book_number < books_per_shelf; book_number++) {
Book book;
book.inventory_number = row * 100000 + shelf * 1000 + book_number;
snprintf(book.title, sizeof(book.title), "Книга %d", book.inventory_number);
book.row = row;
book.shelf = shelf;
book.book_number = book_number;

sem_wait(semaphore);
shared_memory[row * shelves * books_per_shelf + shelf * books_per_shelf + book_number] = book;
sem_post(semaphore);
}
}
exit(0);
}

void handle_sigint(int signal) {
printf("\nЗавершение программы\n");
exit(0);
}

int main(int argc, char *argv[]) {
if (argc != 4) {
printf("Использование: ./inventory M N K\n");
exit(1);
}

signal(SIGINT, handle_sigint);

int rows = atoi(argv[1]);
int shelves = atoi(argv[2]);
int books_per_shelf = atoi(argv[3]);
int total_books = rows * shelves * books_per_shelf;

Book *shared_memory = mmap(NULL, total_books * sizeof(Book), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
sem_t *semaphore = sem_open("/inventory_semaphore", O_CREAT | O_EXCL, 0644, 1);
sem_unlink("/inventory_semaphore");

for (int row = 0; row < rows; row++) {
pid_t pid = fork();
if (pid == 0) {
inventory_process(row, shelves, books_per_shelf, shared_memory, semaphore);
} else if (pid < 0) {
perror("Ошибка fork");
exit(1);
}
}

for (int row = 0; row < rows; row++) {
wait(NULL);
}

sem_wait(semaphore);
sort_books(shared_memory, total_books);
print_catalog(shared_memory, total_books);
sem_post(semaphore);

sem_close(semaphore);
munmap(shared_memory, total_books * sizeof(Book));

printf("Каталог успешно восстановлен!\n");
return 0;
}
