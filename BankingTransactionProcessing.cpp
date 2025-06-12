#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <list>

using namespace std;

#define NUM_ATMS 3
#define NUM_TELLERS 2
#define INITIAL_FUNDS 100000
#define MAX_TRANSACTIONS 30
#define SHM_KEY 0x1234
#define LOG_SIZE 1000
#define LOW_PRIORITY_LRU_SIZE 5

struct Transaction {
    int id;
    int amount;
    int priority;
    time_t timestamp;
    bool requires_verification;
};

// Task2: Shared Memory for Funds and Logs
struct SharedData {
    int available_funds;
    char transaction_log[LOG_SIZE][256];
    int log_index;
};

// Task4: ATM and Teller Semaphores
sem_t atm_sem;
sem_t teller_sem;
pthread_mutex_t queue_mutex;
pthread_mutex_t log_mutex;
pthread_mutex_t txn_count_mutex;

// Task1: Priority Queues for Critical, High, Medium
queue<Transaction> transaction_queues[3]; 
// Task5: LRU for Low Priority
list<Transaction> low_priority_lru;

int transactions_processed = 0;
bool terminate_system = false;

SharedData* shared_data = nullptr;
int shm_id;

void write_log(const string& log_entry) { // Task3
    pthread_mutex_lock(&log_mutex);
    if (shared_data->log_index < LOG_SIZE) {
        strncpy(shared_data->transaction_log[shared_data->log_index], log_entry.c_str(), 255);
        shared_data->log_index++;
    }
    pthread_mutex_unlock(&log_mutex);
}

string get_priority_name(int priority) {
    switch (priority) {
        case 0: return "Critical";
        case 1: return "High";
        case 2: return "Medium";
        case 3: return "Low";
        default: return "Unknown";
    }
}

Transaction generate_transaction(int id) {
    Transaction txn;
    txn.amount = (rand() % 900 + 100);
    txn.timestamp = time(nullptr);

    int r = rand() % 100;
    if (r < 5) txn.priority = 0;
    else if (r < 20) txn.priority = 1;
    else if (r < 60) txn.priority = 2;
    else txn.priority = 3;

    txn.requires_verification = (rand() % 10 == 0);
    txn.id = id;
    return txn;
}

void* transaction_generator(void*) { // Task6
    for (int i = 0; i < MAX_TRANSACTIONS; i++) {
        Transaction txn = generate_transaction(i);

        pthread_mutex_lock(&queue_mutex);
        if (txn.priority < 3)
            transaction_queues[txn.priority].push(txn);
        else {
            if (low_priority_lru.size() >= LOW_PRIORITY_LRU_SIZE) {
                low_priority_lru.pop_back(); // Task5: LRU Eviction
            }
            low_priority_lru.push_front(txn);
        }
        pthread_mutex_unlock(&queue_mutex);

        string log_entry = "[Generated] Transaction ID " + to_string(txn.id) +
                           " Priority: " + get_priority_name(txn.priority) +
                           " Amount: " + to_string(txn.amount) +
                           (txn.requires_verification ? " [Verification Required]" : "");
        write_log(log_entry);

        usleep(100000 + rand() % 400000);
    }

    terminate_system = true;
    return nullptr;
}

void simulate_io_delay(Transaction txn) { // Task7
    if (txn.requires_verification) {
        write_log("[Verification] Extra check for Transaction ID " + to_string(txn.id));
        sleep(1);
    }

    int r = rand() % 100;
    if (r < 10) {
        write_log("[I/O Delay] Network issue during Transaction ID " + to_string(txn.id));
        sleep(2);
    }
}

void* transaction_processor(void* arg) { // Task8
    bool is_atm = *((bool*)arg);
    sem_t* sem = is_atm ? &atm_sem : &teller_sem;

    while (!terminate_system || transactions_processed < MAX_TRANSACTIONS) {
        Transaction txn;
        bool txn_found = false;

        pthread_mutex_lock(&queue_mutex);
        for (int i = 0; i < 3; i++) {
            if (!transaction_queues[i].empty()) {
                txn = transaction_queues[i].front();
                transaction_queues[i].pop();
                txn_found = true;
                break;
            }
        }

        if (!txn_found && !low_priority_lru.empty()) {
            txn = low_priority_lru.back(); // Task5: LRU access
            low_priority_lru.pop_back();
            txn_found = true;
        }
        pthread_mutex_unlock(&queue_mutex);

        if (txn_found) {
            sem_wait(sem);

            simulate_io_delay(txn); // Task7

            pthread_mutex_lock(&txn_count_mutex);
            if (shared_data->available_funds >= txn.amount) {
                shared_data->available_funds -= txn.amount;

                string log_entry = (is_atm ? "[ATM] " : "[Teller] ") +
                                   string("Processed Transaction ID ") + to_string(txn.id) +
                                   " Amount: " + to_string(txn.amount) +
                                   " Priority: " + get_priority_name(txn.priority);
                write_log(log_entry);
            } else {
                write_log("[Error] Insufficient funds for Transaction ID " + to_string(txn.id));
                shared_data->available_funds += 50000; // Task9: Replenish
                write_log("[Funds] Replenished by 50000.");
            }

            transactions_processed++;
            pthread_mutex_unlock(&txn_count_mutex);

            sem_post(sem);
        } else {
            usleep(50000);
        }
    }

    return nullptr;
}

int main() { // Task10
    srand(time(nullptr));

    shm_id = shmget(SHM_KEY, sizeof(SharedData), IPC_CREAT | 0666);
    shared_data = (SharedData*)shmat(shm_id, nullptr, 0);
    shared_data->available_funds = INITIAL_FUNDS;
    shared_data->log_index = 0;

    sem_init(&atm_sem, 0, NUM_ATMS);
    sem_init(&teller_sem, 0, NUM_TELLERS);
    pthread_mutex_init(&queue_mutex, nullptr);
    pthread_mutex_init(&log_mutex, nullptr);
    pthread_mutex_init(&txn_count_mutex, nullptr);

    pthread_t generator_thread;
    pthread_create(&generator_thread, nullptr, transaction_generator, nullptr);

    bool atm = true, teller = false;
    pthread_t atm_threads[NUM_ATMS], teller_threads[NUM_TELLERS];

    for (int i = 0; i < NUM_ATMS; i++)
        pthread_create(&atm_threads[i], nullptr, transaction_processor, &atm);
    for (int i = 0; i < NUM_TELLERS; i++)
        pthread_create(&teller_threads[i], nullptr, transaction_processor, &teller);

    pthread_join(generator_thread, nullptr);
    for (int i = 0; i < NUM_ATMS; i++) pthread_join(atm_threads[i], nullptr);
    for (int i = 0; i < NUM_TELLERS; i++) pthread_join(teller_threads[i], nullptr);

    cout << "\n=== Transaction Log ===\n";
    for (int i = 0; i < shared_data->log_index; i++)
        cout << shared_data->transaction_log[i] << endl;

    cout << "\nFinal Available Funds: " << shared_data->available_funds << endl;

    sem_destroy(&atm_sem);
    sem_destroy(&teller_sem);
    pthread_mutex_destroy(&queue_mutex);
    pthread_mutex_destroy(&log_mutex);
    pthread_mutex_destroy(&txn_count_mutex);

    shmdt(shared_data);
    shmctl(shm_id, IPC_RMID, nullptr);

    return 0;
}

