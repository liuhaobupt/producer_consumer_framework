#include <pthread.h>

#define MAXNITEMS 1000000
#define MAXPRODUCERS 100
#define MAXCONSUMERS 1 

int nitems;
int buff[MAXNITEMS];
struct {
    pthread_mutex_t mutex;
    int nput;
    int nval;
} put = {
    PTHREAD_MUTEX_INITIALIZER, 0, 0
};

struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int nready;
} nready = {
    PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0
};

void *producer(void *arg) {
    for(; ;) {
        pthread_mutex_lock(&put.mutex);
        if (put.nput >= nitems) {
            pthread_mutex_unlock(&put.mutex);
            return NULL;
        }
        buff[put.nput] = put.nval;
        put.nput++;
        put.nval++;
        pthread_mutex_unlock(&put.mutex);

        pthread_mutex_lock(&nready.mutex);
        if (nready.nready == 0)
            pthread_cond_signal(&nready.cond);
        nready.nready++;
        pthread_mutex_unlock(&nready.mutex);
        //do sth
        sleep(1);
    }

}

void *consumer(void *arg) {
    int i;
    for (i = 0; i < nitems; i++) {
        pthread_mutex_lock(&nready.mutex);
        while (nready.nready == 0)
            pthread_cond_wait(&nready.cond, &nready.mutex);
        nready.nready--;
        pthread_mutex_unlock(&nready.mutex);
        
        printf("%d %d:%d\n",nready.nready,(int)arg,buff[i]);
    }
}

int main(int argc, const char **argv) {
    int i;
    pthread_t tid_producer[MAXPRODUCERS], tid_consumer[MAXCONSUMERS];

    nitems = MAXNITEMS;
    for (i = 0; i < MAXPRODUCERS; i++) {
        pthread_create(&tid_producer[i], NULL,producer, NULL);
    }
    for (i = 0; i < MAXCONSUMERS; i++) {
        pthread_create(&tid_consumer[i], NULL, consumer, (void *)i);
    }
    for (i = 0; i < MAXPRODUCERS; i++) {
        pthread_join(tid_producer[i], NULL);
    }
    for (i = 0; i < MAXCONSUMERS; i++) {
        pthread_join(tid_consumer[i], NULL);
    }

    return 0; 
}
