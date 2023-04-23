#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>

typedef struct {
int id;
char title[256];
int row;
int shelf;
int book_number;
} Book;

union semun {
int val;
struct semid_ds *buf;
unsigned short *array;
};

void inventory_process(int row, int sem_id, int shm_id, int N, int K) {
struct sembuf sem_op;
sem_op.sem_num = row;
sem_op.sem_op = -1;
sem_op.sem_flg = 0;

// Ожидание семафора
if (semop(sem_id, &sem_op, 1) == -1) {
perror("Ошибка ожидания семафора");
exit(EXIT_FAILURE);
}

// Подключение разделяемой памяти
Book *books = (Book *)shmat(shm_id, NULL, 0);
if (books == (Book *)-1) {
perror("Ошибка подключения разделяемой памяти");
exit(EXIT_FAILURE);
}

// Инвентаризация
for (int i = 0; i < N; ++i) {
for (int j = 0; j < K; ++j) {
int index = row * N * K + i * K + j;
books[index].id = index;
snprintf(books[index].title, sizeof(books[index].title), "Книга %d", index);
books[index].row = row;
books[index].shelf = i;
books[index].book_number = j;
}
}

// Отключение разделяемой памяти
if (shmdt(books) == -1) {
perror("Ошибка отключения разделяемой памяти");
exit(EXIT_FAILURE);
}

// Сигнал семафора
sem_op.sem_op = 1;
if (semop(sem_id, &sem_op, 1) == -1) {
perror("Ошибка сигнала семафора");
exit(EXIT_FAILURE);
}
}

void sort_books(Book *books, int size) {
for (int i = 0; i < size - 1; ++i) {
for (int j = 0; j < size - 1 - i; ++j) {
if (books[j].id > books[j + 1].id) {
Book temp = books[j];
books[j] = books[j + 1];
books[j + 1] = temp;
}
}
}
}

void print_catalog(Book *books, int size) {
printf("Каталог:\n");
printf("%-8s%-30s%-8s%-8s%-8s\n", "ID", "Название", "Ряд", "Шкаф", "Книга");
for (int i = 0; i < size; ++i) {
printf("%-8d%-30s%-8d%-8d%-8d\n", books[i].id, books[i
].title, books[i].row, books[i].shelf, books[i].book_number);
}
}

void handle_sigint(int sig) {
if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
perror("Ошибка удаления разделяемой памяти при прерывании");
}
union semun sem_union;
if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1) {
perror("Ошибка удаления семафора при прерывании");
}

exit(EXIT_FAILURE);
}

// Глобальные переменные
int sem_id;
int shm_id;

int main(int argc, char *argv[]) {
if (argc != 4) {
printf("Использование: %s <M (ряды)> <N (шкафы)> <K (книги в шкафу)>\n", argv[0]);
exit(EXIT_FAILURE);
}

int M = atoi(argv[1]);
int N = atoi(argv[2]);
int K = atoi(argv[3]);

key_t key = IPC_PRIVATE;
sem_id = semget(key, M, IPC_CREAT | 0666);
shm_id = shmget(key, M * N * K * sizeof(Book), IPC_CREAT | 0666);

if (sem_id == -1 || shm_id == -1) {
perror("Ошибка создания семафора или разделяемой памяти");
exit(EXIT_FAILURE);
}

// Инициализация значений семафоров
union semun sem_union;
sem_union.val = 1;
for (int i = 0; i < M; ++i) {
if (semctl(sem_id, i, SETVAL, sem_union) == -1) {
perror("Ошибка инициализации семафора");
exit(EXIT_FAILURE);
}
}

// Обработка SIGINT
signal(SIGINT, handle_sigint);

// Создание дочерних процессов
for (int i = 0; i < M; ++i) {
pid_t pid = fork();
if (pid == 0) { // Дочерний процесс
inventory_process(i, sem_id, shm_id, N, K);
exit(EXIT_SUCCESS);
} else if (pid < 0) {
perror("Ошибка создания дочернего процесса");
exit(EXIT_FAILURE);
}
}

// Ожидание завершения дочерних процессов
for (int i = 0; i < M; ++i) {
wait(NULL);
}

// Объединение и сортировка подкаталогов
Book *catalog = (Book *)shmat(shm_id, NULL, 0);
if (catalog == (Book *)-1) {
perror("Ошибка подключения разделяемой памяти");
exit(EXIT_FAILURE);
}
sort_books(catalog, M * N * K);

// Вывод каталога
print_catalog(catalog, M * N * K);

// Отключение и удаление
if (shmdt(catalog) == -1) {
perror("Ошибка отключения разделяемой памяти");
exit(EXIT_FAILURE);
}

if (shmctl(shm_id, IPC_RMID, NULL) == -1 || semctl(sem_id, 0, IPC_RMID, sem_union) == -1) {
perror("Ошибка удаления разделяемой памяти или семафора");
exit(EXIT_FAILURE);
}

return 0;
}
